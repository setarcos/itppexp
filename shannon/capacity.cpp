#include <itpp/itbase.h>
#include <unistd.h>
#include <math.h>
#include <iomanip>
#include <iostream>
#include <fstream>

using namespace std;
using namespace itpp;

double h2p(double p)
{
    return (p * log2(1/p)+(1-p)*log2(1/(1-p)));
}

double rho(double snr)
{
    return (log2(1+snr));
}

int main(int argc, char* argv[])
{
    unsigned char t = 0;
    if (argc >= 2)
        t = atoi(argv[1]);
    vec snrdb = linspace(-20, 20, 30);
    vec snr = inv_dB(snrdb);
    ofstream f("test.dat");
    if (t < 2) {
        for (int i = 0; i < snr.length(); ++i) {
            if (t == 0)
                f << setw(4) << snrdb(i) << setw(12) << rho(snr[i]) << endl;
            if (t == 1)
                f << setw(4) << snrdb(i) << setw(12) << 10 * log10(snr[i] / rho(snr[i])) << endl;
        }
    }
    if (t >= 2) {
        vec pe = linspace(-7, -1, 13);
        vec pel = pow(10, pe);
        vec r = "0.01, 0.5, 0.75, 2.0";
        for (int i = 0; i < pe.length(); ++i) {
            for (int j = 0; j < r.length(); ++j) {
                double ebnol = (pow(2, (r[j] * (1 - h2p(pel[i])))) - 1); // Actually SNR
                if (t == 2) ebnol /= r[j];
                f << setw(12) << 10 * log10(ebnol);
            }
            f << setw(12) << pe[i] << endl;
        }
    }
    f.close();
    f.open("test.gp");
    f << "set grid" << endl;
    if ((t < 2) || (t == 3))
        f << "set xlabel \"SNR[db]\"" << endl;
    else
        f << "set xlabel \"EbN0[db]\"" << endl;
    if (t == 0)
        f << "set ylabel \"Rho\"" << endl;
    if (t == 1)
        f << "set ylabel \"EbN0[db]\"" << endl;
    if (t >= 2)
        f << "set ylabel \"Pe[log10]\"" << endl;
    if (t < 2) {
        f << "set xrange [-20:20]" << endl;
        f << "plot \"test.dat\" using 1:2 w lp pt 5 ";
        if (t == 0) f << "title \"Rho vs SNR\"" << endl;
        else f << "title \"EbN0 vs SNR\"" << endl;
    } else {
        if (t == 2)
            f << "set xrange [-5:3]" << endl;
        else
            f << "set xrange [-25:5]" << endl;
        f << "plot \"test.dat\" using 1:5 w lp pt 5 title \"R=0.01\",\\" << endl;
        f << "    \"test.dat\" using 2:5 w lp pt 4 title \"R=0.5\",\\" << endl;
        f << "    \"test.dat\" using 3:5 w lp pt 3 title \"R=0.75\",\\" << endl;
        f << "    \"test.dat\" using 4:5 w lp pt 2 title \"R=2.0\"" << endl;
    }
    f << "pause mouse any \"Hit any key to exit\\r\\n\"" << endl;
    f.close();
}
