#include <itpp/comm/channel.h>
#include <itpp/itbase.h>
#include <iomanip>  // for setw
#include <unistd.h>
#include <fstream>

using namespace std;
using namespace itpp;
const int LEN = 100;

int main(int argc, char* argv[])
{
    unsigned char t = 0;
    if (argc >= 2)
        t = atoi(argv[1]);
    cvec tc;
    RNG_randomize();
    Fading_Generator *fad;
    switch (t) {
        case 1:
            fad = new Static_Fading_Generator();
            break;
        case 2:
            fad = new FIR_Fading_Generator(0.02, 500);
        default:
            fad = new Independent_Fading_Generator();
            fad->set_LOS_power(0);  // no LOS, so it's rayleigh
            break;
    }
    fad->generate(LEN,tc);
    delete fad;
    ofstream f("test.dat");
    for (int i = 0; i < LEN; ++i) {
        f << abs(tc[i]) << endl;
    }
    f.close();
    
    return 0;
}

