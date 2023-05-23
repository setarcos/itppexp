#include <itpp/itsignal.h>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace itpp;

const double PI=3.1415926;
const int fs = 500;
const int T = 1;

int main(int argc, char * argv[])
{
    vec b;
    int f1 = 17;
    int f2 = 50;
    int N = T * fs;  // frequency resolution is fs/N=1/T
    b.set_size(N);
    unsigned char t = 0;
    if (argc >= 2)
        t = atoi(argv[1]);

    for (int i = 0; i < N; ++i) {
        b[i] = 0.5 + sin(2 * PI * (f1 + 0.3)/ fs *i) + sin(2 * PI * f2 / fs * i);
    }
    vec d;
    switch (t) {
        case 1:
            cout << "upsampling by a factor of 4." << endl;
            d = upsample(b, 4);
            N *= 4;
            break;
        case 2:
            cout << "zero padding to both ends." << endl;
            d = zeros(N * 4);
            d.set_subvector(N * 2 - N / 2, b);
            N *= 4;
            break;
        case 3:
            cout << "repeat each point 4 times." << endl;
            d = repeat(b, 4);
            N *= 4;
            break;
        case 4:
            cout << "repeat the signal 4 times" << endl;
            d = zeros(N * 4);
            for (int i = 0; i < 4; ++i)
                d.set_subvector(i * N, b);
            N *= 4;
            break;
        default:
            cout << "original fft." << endl;
            d = b;
    }
    cvec c = fft_real(d);
    ofstream ofs ("test.dat", ofstream::out);
    for (int i = 0; i < N; ++i) {
        ofs << setw(16) << abs(c[i]) / N << setw(16) << d[i] <<  endl;
    }
    ofs.close();
}
