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
#define RUN_SIZE 1000

int main(int argc, char *argv[])
{
    Polar p(BLK_LEN, BLK_LEN / 2);
    p.gen_frozen_ga(0.79433);
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
    int runsize = BLK_LEN / 2 * RUN_SIZE;
    if (m == 0) {
        p.set_polar_decoder(POLAR_SC);
        bler.set_blocksize(BLK_LEN / 2);
        f.open("sc.dat");
    }
    if (m == 1) {
        p.set_polar_decoder(POLAR_SCL);
        p.set_scl_size(LIST_SIZE);
        bler.set_blocksize(BLK_LEN / 2);
        f.open("scl.dat");
    }
    if (m == 2) {
        p.set_polar_decoder(POLAR_CASCL);
        p.set_crc_code(CRC, CRC_LEN);
        p.set_scl_size(LIST_SIZE);
        bler.set_blocksize(BLK_LEN / 2 - CRC_LEN);
        f.open("cascl.dat");
        runsize = (BLK_LEN / 2 - CRC_LEN) * RUN_SIZE;
    }
    ber.clear();
    bler.clear();
    for (int i = 0; i < length;) {
        bitsin =randb(runsize);
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
        ber.count(bitsin, output);
        bler.count(bitsin, output);
        if ((bler.get_errors() > 100) || (bler.get_total_blocks() > 50000)) {
            bit_erate[i] = ber.get_errorrate();
            blk_erate[i] = bler.get_errorrate();
            i++;
            ber.clear();
            bler.clear();
        }
    }
    for (int i = 0; i < length; ++i) {
        f << setw(4) << EbN0db(i) << setw(12) << bit_erate(i) << setw(12) << blk_erate(i) << endl;
    }
    f.close();
    return 0;
}
