#include <itpp/itbase.h>
#include <iomanip>
#include <fstream>

using namespace std;
using namespace itpp;

double phi(double x)
{
    if (x < 10)
        return exp(-0.4527*pow(x, 0.86)+0.0218);
    else
        return sqrt(pi/x)*exp(-x/4)*(1-10/7/x);
}

double phi_1(double y)
{
    if (y > 0.0414)
        return pow((0.0218 - log(y))/0.4527, 1/0.86);
    double x1 = 10.0;
    double x2, y1;
    do {
        y1 = phi(x1);
        double k = 1000 * (phi(x1 + 0.001) - y1);
        x2 = x1;
        x1 += (y - y1) / k;
    } while (abs(x2 - x1) > 0.01);
    return x1;
}

int main(int argc, char *argv[])
{
    unsigned char t = 0;
    if (argc >= 2)
        t = atoi(argv[1]);
    if (t == 0) {  // Plot phi function
        ofstream ofs("test.dat", ofstream::out);
        cout << "Phi(0)=" << phi(0) << endl;
        cout << "Phi(10)=" << phi(10) << endl;
        for (int i = 0; i < 200; ++i)
            ofs << (double)i / 10 << setw(14) << phi((double)i / 10) << endl;
        ofs.close();
        return 0;
    }
    vec a[2];
    a[0] = "3.5565"; // 2 / sigma^2
    int n = 1;
    int level = 3; // code length will be 2^level
    for (int i = 0; i < level; ++i) {
        n *= 2;
        int j = i + 1;
        a[j % 2].set_size(n);
        for (int k = 0; k < n / 2; ++k) {
            double tmp = a[i % 2][k];
            a[j % 2][k * 2] = phi_1(1 - pow(1 - phi(tmp), 2));
            a[j % 2][k * 2 + 1] = 2 * tmp;
        }
    }
    cout << a[level % 2] << endl;
    return 1;
}
