#include <itpp/itbase.h>
#include <complex>

using namespace itpp;
using namespace std;

void auto_correlation(const cvec &a, cvec &x)
{
    int k = a.size();
    x.set_size(k);
    for (int i = 0; i < k; ++i) {
        x[i] = 0;
        for (int j = 0; j < k - i; ++j)
            x[i] += a[j] * conj(a[j + i]);
        for (int j = k - i; j < k; ++j)
            x[i] += a[j] * conj(a[j - k + i]);
    }
}

const int N = 16;
const int M = 5;
const double PI = 3.1415926536;

int main(int argc, char * argv[])
{
    cvec a;
    a.set_size(N);
    for (int i = 0; i < N; ++i)
        a[i] = polar(1.0, M * PI * i * i / N);
    cvec x;
    auto_correlation(a, x);
    ofstream ofs ("test.dat", ofstream::out);
    //cout << a << endl;
    for (int i = 0; i < N; ++i)
         ofs << abs(x[i]) << endl;
    return 0;
}
