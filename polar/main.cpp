#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include <iostream>
#include <iomanip>
#include <getopt.h>
#include <string.h>
#include "polar.h"

using namespace itpp;
using std::endl;
using std::cout;
using std::setw;
using std::cerr;

#define BLK_LEN 2048
#define LIST_SIZE 16
#define RUN_SIZE 1000

int main(int argc, char *argv[])
{
    const char *optstr = "hn:k:c:p:l:";
    int n = BLK_LEN;
    int k = 0;
    int c = 0;
    char *crc = nullptr; // initialize later
    char *pac = nullptr;
    int l = LIST_SIZE;
    int f = 0;
    int o;
    char *filename = nullptr;
    while ((o = getopt(argc, argv, "hn:k:c:C:p:l:f:o:")) != -1) {
        switch (o) {
            case 'h':
                cout << argv[0] << ": polar(pac) code simulation." << endl
                    << "\t-n [block_length]" << endl
                    << "\t-k [info_length]" << endl
                    << "\t-c [CRC_length]" << endl
                    << "\t-C [CRC standard polynomials]" << endl
                    << "\t-f [0 for bec, 1 for ga, 2 for rm]" << endl
                    << "\t-p [PAC polynomial]" << endl
                    << "\t-l [list size]" << endl;
                return 0;
            case 'n':
                n = atoi(optarg);
                break;
            case 'k':
                k = atoi(optarg);
                break;
            case 'c':
                c = atoi(optarg);
                break;
            case 'C':
                crc = (char*) malloc(strlen(optarg) + 1);
                strcpy(crc, optarg);
                break;
            case 'p':
                pac = (char*) malloc(strlen(optarg) + 1);
                strcpy(pac, optarg);
                break;
            case 'o':
                filename = (char*) malloc(strlen(optarg) + 1);
                strcpy(filename, optarg);
                break;
            case 'l':
                l = atoi(optarg);
                break;
            case 'f':
                f = atoi(optarg);
                break;
        }
    }

    if (k == 0) k = n / 2;
    if ((c > 0) && (!crc)) {
        cerr << "CRC polynomials needed." << endl;
        return 1;
    }
    if (!filename) {
        cerr << "Output filename needed." << endl;
        return 1;
    }
    cout << "Block length " << n << "; Info length " << k << "; CRC length " << c << "; List size " << l <<  endl;
    Polar p(n, k);
    switch (f) {
        case 1:
            p.gen_frozen_ga(0.79433);
            break;
        case 2:
            p.gen_frozen_rm();
            break;
        default:
            p.gen_frozen_bec(0.32);
    }
    BERC ber;
    BLERC bler;
    Real_Timer tt;
    vec EbN0db = "1:0.25:3";
    vec bit_erate;
    vec blk_erate;
    int length = EbN0db.length();
    bit_erate.set_size(length);
    blk_erate.set_size(length);
    bler.set_blocksize(k - c);
    RNG_randomize();
    bvec bitsin;
    std::ofstream fn;
    int runsize = (k - c) * RUN_SIZE;
    if (l == 0) // SC Decorder
        p.set_polar_decoder(POLAR_SC);
    else
        p.set_scl_size(l);

    if (pac) {
        p.set_polar_decoder(PAC_SCL);
        p.set_pac_generator(pac);
    } else {
        if (c > 0)
            p.set_polar_decoder(POLAR_CASCL);
        else
            if (l > 0) p.set_polar_decoder(POLAR_SCL);
    }

    if (c > 0)
        p.set_crc_code(crc, c);

    ber.clear();
    bler.clear();
    tt.tic();
    cout << " EbN0   ErrorRate   ElapsedTime" << endl;
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
            cout << setw(5) << EbN0db(i) << setw(12) << bit_erate[i] << setw(12) << tt.toc() << " seconds" << endl;
            tt.tic();
            i++;
            ber.clear();
            bler.clear();
        }
    }
    fn.open(filename);
    if (pac) fn << "#PAC(" << pac << "),";
    else fn << "#Polar,";
    fn << "Block length " << n << "; Info length " << k << "; CRC length " << c << "; List size " << l <<  endl;
    for (int i = 0; i < length; ++i) {
        fn << setw(4) << EbN0db(i) << setw(12) << bit_erate(i) << setw(12) << blk_erate(i) << endl;
    }
    fn.close();
    return 0;
}
