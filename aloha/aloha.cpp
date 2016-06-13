#include <iomanip>
#include <fstream>
#include <itpp/itbase.h>
#include <itpp/base/random.h>

const int SLOTS = 100000;
const double MAX_LAMDA = 4.0;
const double STEP = 0.1;

using namespace itpp;
using namespace std;

int main(int argc, char * argv[])
{
    vec lamda;
    ivec slot(SLOTS);
    Exponential_RNG erng(0.1);
    RNG_randomize();
    lamda = linspace(0.1, MAX_LAMDA, MAX_LAMDA / STEP);
    ofstream ofs ("test.dat", ofstream::out);
    for (int k; k < lamda.length(); ++k) {
        erng.setup(lamda(k));
        for (int i = 0; i < SLOTS; ++i) slot[i] = 0;
        double t = 0;
        int idx = 0;
        while (t < SLOTS) {
            t += erng();
            while (t > idx + 1) idx++;
            if (idx < SLOTS) slot[idx]++;
        }
        int s_count = 0;
        int f_count = 0;
        for (int i = 0; i < SLOTS; ++i) {
            if (slot[i] == 1) s_count++;  // success
            if (slot[i] >= 2) f_count++;  // fail
        }
        ofs << setw(4) << lamda(k) << setw(12) << (double)s_count / SLOTS << endl;
    }
    ofs.close();
    return 0;
}
