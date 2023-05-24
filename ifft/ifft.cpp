#include <itpp/itsignal.h>
#include <fstream>

using namespace std;
using namespace itpp;

const double PI=3.1415926;
const int fs = 800;
const int T = 1;  // one second

void filter(cvec &f)
{
    int offset = 15;
    for (int i = 0 - offset; i < offset; i++) {
        f[i + 17] = 0;
        f[i + fs - 18] = 0;
    }
}

int main(int argc, char * argv[])
{
    vec b, buf;
    int f1 = 17;
    int f2 = 50;
    int N = T * fs;  // frequency resolution is fs/N=1/T
    b.set_size(N * 2);
    buf.set_size(N);
    unsigned char t = 0;
    if (argc >= 2)
        t = atoi(argv[1]);

    for (int i = 0; i < N * 2; ++i) {
        b[i] = sin(2 * PI * (f1 + 0.3)/ fs *i) + sin(2 * PI * f2 / fs * i);
    }
    for (int i = 0; i < N; i++) {
        buf[i] = b[i + N / 2];
    }
    vec d;
    vec win;
    switch (t) {
        case 1:
            cout << "Hamming window." << endl;
            win = hamming(N);
            d = elem_mult(buf, win);
            break;
        case 2:
            cout << "Hanning window." << endl;
            win = hanning(N);
            d = elem_mult(buf, win);
            break;
        case 3:
            cout << "Hann window." << endl;
            win = hann(N);
            d = elem_mult(buf, win);
            break;
        default:
            cout << "Original signal." << endl;
            d = buf;
    }
    cvec c = fft_real(d);
    filter(c);
    vec ic = ifft_real(c);
    if (t == 2) { // Overlap add
        for (int i = 0; i < N; ++i) {
            buf[i] = b[i];
        }
        d = elem_mult(buf, win);
        c = fft_real(d);
        filter(c);
        vec tmp = ifft_real(c);
        for (int i = 0; i < fs / 2; i++) {
            ic[i] += tmp[i + fs / 2];
        }

        for (int i = 0; i < N; ++i) {
            buf[i] = b[i + N];
        }
        d = elem_mult(buf, win);
        c = fft_real(d);
        filter(c);
        tmp = ifft_real(c);
        for (int i = fs / 2; i < fs; i++) {
            ic[i] += tmp[i - fs / 2];
        }
    }
    ofstream ofs ("test.dat", ofstream::out);
    for (int i = 0; i < N; ++i) {
        ofs << ic[i] / N << "    " << d[i] << "     " << abs(c[i]) << endl;
    }
    ofs.close();
}
