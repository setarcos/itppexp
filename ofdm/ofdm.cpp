#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include <iomanip>
#include <complex>
#include <iostream>
#include <fstream>

using namespace itpp;
using std::endl;
using std::setw;
using std::cout;

const int N = 512;
const int Ncp = 23;
const int Number_of_bits = 1000;
const int Carriers = 100;
const int u_sample = 8;

int main(int argc, char* argv[])
{
    bvec transmitted_bits;
    cvec mod_symbols;
    cvec transmitted_symbols;

    QPSK qpsk;
    OFDM ofdm(N, Ncp, u_sample);
    cvec StoP;
    cvec freq;
    cvec t_os;
    vec stat;
    int t = 0;

    if (argc == 2)
        t = atoi(argv[1]);

    RNG_randomize();

    transmitted_bits = randb(Number_of_bits * 2);
    //Modulate the bits to BPSK symbols:
    mod_symbols = qpsk.modulate_bits(transmitted_bits);
    stat = zeros(N * 2);
    for (int i = 0; i < Number_of_bits / Carriers; ++i) {
        StoP = zeros_c(N);
        StoP.set_subvector(50, mod_symbols.mid(i * Carriers, Carriers));
        transmitted_symbols = ofdm.modulate(StoP);
        t_os = concat(zeros_c(N * u_sample/ 2 - Ncp * u_sample), transmitted_symbols, zeros_c(N  * u_sample/ 2));
        if ((t == 3) || (t == 4)) { // no oversample
            t_os = transmitted_symbols;
        }
        freq = fft(t_os);
        if ((t == 0) || (t == 4)) {
            for (int j = 0; j < N * 2; ++j)
                stat[j] += 20 * log10(abs(freq[j])); // PSD
        //    if (abs(freq[j]) > stat[j]) stat[j] = abs(freq[j]);
        }
        if (t == 1) {
            stat.set_size((N + Ncp) * u_sample);
            for (int j = 0; j < stat.size(); ++j)
                stat[j] = abs(transmitted_symbols[j]);
        }
        if ((t == 2) || (t == 3)) {
            const double PI = 3.141592653589793238463;
            cvec carrier;
            int k = t_os.size();
            carrier.set_size(k);
            // carrier is e^{jwt}, wt=2\pi*fc/(fs*up_sample)*t
            // fs = 1e6, fc = 2e6,
            for (int j = 0; j < k; ++j) carrier[j] = std::polar(1.0, 4 * PI / u_sample * j);
            freq = fft_real(real(elem_mult(t_os, carrier)));
            stat.set_size(freq.size());
            stat += abs(freq);
        }
    }
    std::ofstream f("test.dat");
    for (int j = 0; j < stat.size(); ++j)
        f << stat[j] << endl;
    f.close();
}

