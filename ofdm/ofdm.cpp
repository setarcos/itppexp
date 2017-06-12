#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include <iomanip>
#include <iostream>
#include <fstream>

using namespace itpp;
using std::endl;
using std::setw;
using std::cout;

const int N = 512;
const int Ncp = 23;
const int Number_of_bits = 100;
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
        freq = fft(concat(zeros_c(N * u_sample/ 2 - Ncp * u_sample), transmitted_symbols, zeros_c(N  * u_sample/ 2)));
        if (t == 0) {
            for (int j = 0; j < N * 2; ++j)
                stat[j] = 20 * log10(abs(freq[j]));
        //    if (abs(freq[j]) > stat[j]) stat[j] = abs(freq[j]);
        }
        if (t == 1) {
            stat.set_size((N + Ncp) * u_sample);
            for (int j = 0; j < stat.size(); ++j)
                stat[j] = abs(transmitted_symbols[j]);
        }
    }
    std::ofstream f("test.dat");
    for (int j = 0; j < stat.size(); ++j)
        f << stat[j] << endl;
    f.close();
}

