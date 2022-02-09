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
    Polar p(512, 256);
    BERC ber;
    p.gen_frozen_ga(0.75);

    vec EbN0db = "0:0.5:4";
    vec bit_error_rate;
    int length = EbN0db.length();
    bit_error_rate.set_size(length * 2);
    RNG_randomize();
    for (int m = 0; m < 2; ++m) {
        if (m == 0)
            p.set_polar_decoder(POLAR_SC);
        if (m == 1) {
            p.set_polar_decoder(POLAR_CASCL);
            p.set_crc_code("CRC-8", 8);
            p.set_scl_size(16);
        }
        for (int i = 0; i < length; ++i) {
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
            bit_error_rate[m * length + i] = ber.get_errorrate();
        }
    }
    std::ofstream f("test.dat");
    for (int i = 0; i < length; ++i) {
        f << setw(4) << EbN0db(i) << setw(12) << bit_error_rate(i) << setw(12) << bit_error_rate(i + length) << endl;
    }
    f.close();
    return 0;
}
