#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include <iostream>
#include <iomanip>
#include "polar.h"

using namespace itpp;
using std::endl;
using std::cout;
using std::setw;

#define BLK_LEN 2048
#define CRC "CCITT-16"
#define CRC_LEN 16
#define LIST_SIZE 16

int main(int argc, char *argv[])
{
    Polar p(BLK_LEN, BLK_LEN / 2);
    p.gen_frozen_bec(0.32);
    BERC ber;
    BLERC bler;
    vec EbN0db = "1:0.25:3";
    vec bit_erate;
    vec blk_erate;
    int length = EbN0db.length();
    bit_erate.set_size(length);
    blk_erate.set_size(length);
    RNG_randomize();
    unsigned char m = 0;
    if (argc >= 2)
        m = atoi(argv[1]);
    bvec bitsin;
    std::ofstream f;
    if (m == 0) {
        p.set_polar_decoder(POLAR_SC);
        bitsin = randb(BLK_LEN * 1000);
        bler.set_blocksize(BLK_LEN);
        f.open("sc.dat");
    }
    if (m == 1) {
        p.set_polar_decoder(POLAR_SCL);
        bitsin = randb(BLK_LEN * 1000);
        p.set_scl_size(LIST_SIZE);
        bler.set_blocksize(BLK_LEN);
        f.open("scl.dat");
    }
    if (m == 2) {
        p.set_polar_decoder(POLAR_CASCL);
        p.set_crc_code(CRC, CRC_LEN);
        p.set_scl_size(LIST_SIZE);
        bitsin = randb((BLK_LEN / 2 - CRC_LEN) * 2000);
        bler.set_blocksize(BLK_LEN - CRC_LEN);
        f.open("cascl.dat");
    }
    for (int i = 0; i < length; ++i) {
        double N0 = pow(10.0, -EbN0db[i] / 10.0) / p.get_rate();
        AWGN_Channel chan(N0 / 2);
        BPSK mod;
        bvec coded;
        p.encode(bitsin, coded);
        vec s = mod.modulate_bits(coded);
        vec x = chan(s);
        vec llr = mod.demodulate_soft_bits(x, N0);
        bvec output;
        p.decode(llr, output);
        ber.clear();
        ber.count(bitsin, output);
        bit_erate[i] = ber.get_errorrate();
        bler.clear();
        bler.count(bitsin, output);
        blk_erate[i] = bler.get_errorrate();
    }
    for (int i = 0; i < length; ++i) {
        f << setw(4) << EbN0db(i) << setw(12) << bit_erate(i) << setw(12) << blk_erate(i) << endl;
    }
    f.close();
    return 0;
}
