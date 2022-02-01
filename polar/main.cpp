#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include <iostream>
#include "polar.h"

using namespace itpp;
using std::endl;
using std::cout;

int main(int argc, char *argv[])
{
    Polar p(8, 4);
    p.gen_frozen_bec(0.7499);

    vec EbN0db = "1:0.5:5";
    double N0 = pow(10.0, -5 / 10.0) / p.get_rate();
    AWGN_Channel chan(N0 / 2);
    BPSK mod;
    bvec bitsin = randb(128);
    bvec coded;
    p.encode(bitsin, coded);
    vec s = mod.modulate_bits(coded);
    vec x = chan(s);
    vec llr = mod.demodulate_soft_bits(x, N0);
    bvec output;
    p.decode(llr, output);
    cout << bitsin << endl;
    cout << output << endl;
    return 0;
}
