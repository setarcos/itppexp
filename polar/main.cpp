#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include <iostream>
#include <iomanip>
#include "polar.h"

using namespace itpp;
using std::endl;
using std::cout;
using std::setw;

int main(int argc, char *argv[])
{
    Polar p(64, 32);
    BERC ber;
    p.gen_frozen_bec(0.5);

    vec EbN0db = "1:0.5:5";
    vec bit_error_rate;
    bit_error_rate.set_size(EbN0db.length());
    RNG_randomize();
    for (int i = 0; i < EbN0db.length(); ++i) {
        double N0 = pow(10.0, -EbN0db[i] / 10.0) / p.get_rate();
        AWGN_Channel chan(N0 / 2);
        BPSK mod;
        bvec bitsin = randb(512000);
        bvec coded;
        p.encode(bitsin, coded);
        vec s = mod.modulate_bits(coded);
        vec x = chan(s);
        vec llr = mod.demodulate_soft_bits(x, N0);
        bvec output;
        p.decode(llr, output);
        ber.clear();
        ber.count(bitsin, output);
        bit_error_rate[i] = ber.get_errorrate();
    }
    std::ofstream f("test.dat");
    for (int i = 0; i < bit_error_rate.length(); ++i) {
        f << setw(4) << EbN0db(i) << setw(12) << bit_error_rate(i) << endl;
    }
    f.close();
    return 0;
}
