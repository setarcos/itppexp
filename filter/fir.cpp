#include <itpp/itsignal.h>
#include <fstream>
#include <iomanip>

using namespace std;
using namespace itpp;

int main(int argc, char * argv[])
{
    int N = 500;
    MA_Filter<double, double, double> fir;
    fir.set_coeffs(fir1(80, 0.5)); // Normalized frenquency is f/fs
    vec a = zeros(N);
    a[0] = 1;
    vec b = fir(a);
    cvec h = freqz(b, a, N / 2);
    ofstream ofs ("test.dat", ofstream::out);
    for (int i = 0; i < N / 2; ++i) {
        ofs << setw(16) << abs(h[i]) << setw(16) << arg(h[i]) << endl;
    }
    ofs.close();
}
