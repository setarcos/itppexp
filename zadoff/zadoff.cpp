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

void cross_correlation(const cvec &a, cvec &b, cvec &x)
{
    int k = a.size();
    x.set_size(k);
    for (int i = 0; i < k; ++i) {
        x[i] = 0;
        for (int j = 0; j < k - i; ++j)
            x[i] += a[j] * conj(b[j + i]);
        for (int j = k - i; j < k; ++j)
            x[i] += a[j] * conj(b[j - k + i]);
    }
}

const double PI = 3.1415926536;

void orig_zadoff(int n, int m)
{
    cvec a;
    a.set_size(n);
    for (int i = 0; i < n; ++i)
        a[i] = polar(1.0, m * PI * i * i / n);
    cvec x;
    auto_correlation(a, x);
    ofstream ofs ("test.dat", ofstream::out);
    //cout << a << endl;
    for (int i = 0; i < n; ++i)
         ofs << abs(x[i]) << endl;
}

const int N_zc = 837;

void lte_zadoff(int u, cvec &a)
{
    a.set_size(N_zc);
    for (int i = 0; i < N_zc; ++i)
        a[i] = polar(1.0, (-1) * PI * u * i * (i + 1) / N_zc);
}

void zadoff_p1(int u1, int u2)
{
    cvec a;
    cvec b;
    lte_zadoff(u1, a);
    lte_zadoff(u2, b);
    cvec x;
    cross_correlation(a, b, x);
    ofstream ofs ("test.dat", ofstream::out);
    for (int i = 0; i < N_zc; ++i)
         ofs << abs(x[i]) << endl;
}

int main(int argc, char * argv[])
{
    unsigned char t = 0;
    cvec a;
    cvec b;
    if (argc >= 2)
        t = atoi(argv[1]);
    switch (t) {
        case 0:
            orig_zadoff(16, 5);
            break;
        case 1:
            zadoff_p1(1, 1);
            break;
        case 2:
            zadoff_p1(1, 129);
            break;
    }
    return 0;
}
