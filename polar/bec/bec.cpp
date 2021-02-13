#include <itpp/itsignal.h>
#include <fstream>

using namespace std;
using namespace itpp;

int main()
{
    vec a[2];
    a[0] = "0.5";
    int n = 1;
    for (int i = 0; i < 10; ++i) {
        n *= 2;
        int j = i + 1;
        a[j % 2].set_size(n);
        for (int k = 0; k < n / 2; ++k) {
            a[j % 2][k * 2] = a[i % 2][k] * a[i % 2][k];
            a[j % 2][k * 2 + 1] = 2 * a[i % 2][k] - a[i % 2][k] * a[i % 2][k];
        }
    }
    ofstream ofs("test.dat", ofstream::out);
    for (int i = 0; i < n; ++i) {
        ofs << a[0][i] << endl;
    }
    ofs.close();
}
