#include <itpp/itsignal.h>
#include <fstream>

using namespace std;
using namespace itpp;

const double PI=3.1415926;
const int L = 500;

int main()
{
    vec b;
    b.set_size(L);

    for (int i = 0; i < L; ++i) {
        b[i] = sin(20*PI/(L + 25) *i) + sin(100 * PI / L * i);
    }
    ofstream ofs ("test.dat", ofstream::out);
    cvec c = fft_real(b);
    for (int i = 0; i < L; ++i) {
        ofs << abs(c[i]) / L << endl;
    }
    ofs.close();
}
