#include <cmath>    // for log10
#include <iomanip>  // for setw
#include <fstream>  // for ofstream

using namespace std;

const double PI=3.1415926;
const double light = 3e+8;
const int L = 16;
const double fc = 1.5e+9;  // 1.5G

int main(int argc, char * argv[])
{
    ofstream ofs ("test.dat", ofstream::out);
    double lamda = light/fc;
    double d0 = 100;
// PL = -20*log10(lamda/(4*pi*d0))+10*n*log10(d/d0);
    for (int i = 0; i < L; ++i) {
        double d = (i * 2 + 1) * (i * 2 + 1);
        ofs << setw(4) << d << setw(12) << 20 * log10(4 * PI * d0 / lamda) + 10 * 2 * log10(d / d0)
            << setw(12) << 20 * log10(4 * PI * d0 / lamda) + 10 * 3 * log10(d / d0)
            << setw(12) << 20 * log10(4 * PI * d0 / lamda) + 10 * 6 * log10(d / d0) << endl;
    }
    ofs.close();
}
