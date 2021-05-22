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
    vec EsN0 = inv_dB(EsN0dB);
    vec cap(EsN0.length());
    unsigned char t = 0;
    if (argc >= 2)
        t = atoi(argv[1]);
    for (int i = 0; i < EsN0.length(); ++i) {
        sigma = 1.0 / EsN0[i];
        double res = -quadl(Integrand_Functor(), -10.0, 10.0) - 0.5 * log2(2 * pi * exp(1) * sigma);
        cap[i] = res;
    }
    ofstream f("test.dat");
    if (t == 1) {
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
                double ebno = (c - cap[idx - 1]) / (cap[idx] - cap[idx - 1]) * rate + EsN0dB[idx - 1];
                f << setw(14) << ebno; // dB(inv_dB(ebno)/r[j]);
            }
            f << endl;
        }
        system("cp test2.gp test.gp -f");
    } else {
        for (int i = 0; i < EsN0.length(); ++i) {
            f << setw(14) << EsN0dB[i] << setw(14) << cap[i]
              << setw(14) << 1 - h2p(Qfunc(sqrt(EsN0[i])))
              << setw(14) << 0.5 * log2(1 + EsN0[i]) << endl;
        }
        system("cp test1.gp test.gp -f");
    }
    f.close();
    return 0;
}

