#include "itpp/itbase.h"
#include <iomanip>
#include <iostream>
#include <fstream>

using namespace itpp;
using namespace std;
double sigma;

double h2p(double p)
{
    return (p * log2(1/p)+(1-p)*log2(1/(1-p)));
}

struct Integrand_Functor
{
    double operator()(const double x) const
    {
        double p =(1.0 / sqrt(8 * pi * sigma)) * (exp(-(x-1)*(x-1)/2/sigma)
                 + exp(-(x+1)*(x+1)/2/sigma));
        return p * log2(p);
    }
};

int main(int argc, char* argv[])
{
    vec EsN0dB = linspace(-10, 10, 41);
    unsigned char t = 0;
    if (argc >= 2)
        t = atoi(argv[1]);
    if (t == 1) EsN0dB = linspace(-10, 30, 81);
 /*   if (t == 4) {
        sigma = 10;
        for (int i = 0; i < 2000; ++i) {
            double j = 1.0 / 50 * i - 20;
            cout << setw(14) << j << setw(14) << Integrand_Functor()(j) << endl;
        }
        return 0;
    } */
    vec EsN0 = inv_dB(EsN0dB);
    vec cap(EsN0.length());
    for (int i = 0; i < EsN0.length(); ++i) {
        sigma = 1.0 / EsN0[i];
        double interval;
        if (i < 5) interval = 20;
        else if (i < 45) interval = 10;
        if (i < 45) {
            double res = -quadl(Integrand_Functor(), -interval, interval) - 0.5 * log2(2 * pi * exp(1) * sigma);
            cap[i] = res;
        } else
            cap[i] = cap[44];
    }
    ofstream f("test.dat");
    if (t >= 2) {
        vec r = "0.25, 0.333, 0.5, 0.666, 0.75";
        vec pe = linspace(-7, -1, 13);
        vec pel = pow(10, pe);
        double rate = EsN0dB[1] - EsN0dB[0];
        for (int i = 0; i < pe.length(); ++i) {
            f << setw(14) << pel[i];
            for (int j = 0; j < r.length(); ++j) {
                double c = r[j] * (1 - h2p(pel[i]));
                int idx = 0;  // linear interpolation
                while (idx < cap.length()) {
                    if (cap[idx] >= c) break;
                    idx++;
                }
                if (idx == 0) idx = 1;
                double snr = (c - cap[idx - 1]) / (cap[idx] - cap[idx - 1]) * rate + EsN0dB[idx - 1];
                if (t == 2) // SNR
                    f << setw(14) << snr;
                if (t == 3)
                    f << setw(14) << snr - 10 * log10(2 * r[j]); // EbN0
            }
            f << endl;
        }
        if (t == 2)
            system("cp test2.gp test.gp -f");
        if (t == 3)
            system("sed 's/SNR/EbN0/g;s/8:5/3:3/g' test2.gp > test.gp");
    } else {
        if (t == 0) { // SNR vs cap
            for (int i = 0; i < EsN0.length(); ++i) {
                f << setw(14) << EsN0dB[i] << setw(14) << cap[i]
                  << setw(14) << 1 - h2p(Qfunc(sqrt(EsN0[i])))
                  << setw(14) << 0.5 * log2(1 + EsN0[i]) << endl;
            }
            system("cp test1.gp test.gp -f");
        } else { // t == 1, EbN0 vs cap
            for (int i = 0; i < EsN0.length(); ++i) {
                double C = log2(1 + EsN0[i]);
                f << setw(14) << EsN0dB[i] - 10 * log10(cap[i] * 2) << setw(14) << cap[i] * 2
                  << setw(14) << EsN0dB[i] - 10 * log10(C) << setw(14) << C << endl;
            }
            system("cp test3.gp test.gp -f");
        }
    }
    f.close();
    return 0;
}

