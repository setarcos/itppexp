#include <itpp/itsignal.h>
#include <fstream>

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
    vec win;
    switch (t) {
        case 1:
            cout << "Hamming window." << endl;
            win = hamming(N);
            d = elem_mult(b, win);
            break;
        case 2:
            cout << "Hanning window." << endl;
            win = hanning(N);
            d = elem_mult(b, win);
            break;
        case 3:
            cout << "Hann window." << endl;
            win = hann(N);
            d = elem_mult(b, win);
            break;
        default:
            cout << "Original signal." << endl;
            d = b;
    }
    cvec c = fft_real(d);
    ofstream ofs ("test.dat", ofstream::out);
    for (int i = 0; i < N; ++i) {
        ofs << abs(c[i]) / N << "    " << d[i] << endl;
    }
    ofs.close();
}
