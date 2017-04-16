#include <itpp/itsignal.h>
#include <fstream>

using namespace std;
using namespace itpp;

const double PI=3.1415926;
const int fs = 500;
const int T = 1;

int main()
{
    vec b;
    int f1 = 17;
    int f2 = 50;
    int N = T * fs;  // frequency resolution is fs/N=1/T
    b.set_size(N);

    for (int i = 0; i < N; ++i) {
        b[i] = 0.5 + sin(2 * PI * (f1 + 0.3)/ fs *i) + sin(2 * PI * f2 / fs * i);
    }
    ofstream ofs ("test.dat", ofstream::out);
    cvec c = fft_real(b);
    for (int i = 0; i < N; ++i) {
        ofs << abs(c[i]) / N << endl;
    }
    ofs.close();
}
