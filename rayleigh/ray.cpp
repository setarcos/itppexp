#include <itpp/comm/channel.h>
#include <itpp/itbase.h>
#include <itpp/stat/histogram.h>
#include <iomanip>  // for setw
#include <fstream>

using namespace std;
using namespace itpp;
const int LEN = 100000;
const double step = 0.1;

int main(int argc, char* argv[])
{
    Independent_Fading_Generator fad1;
    fad1.set_LOS_power(0);  // no LOS, so it's rayleigh
    cvec tc;
    RNG_randomize();
    fad1.generate(LEN,tc);
    Histogram<double> hist(step / 2, step * 30 - step / 2, 30);
    hist.update(abs(tc));
    vec pdf = hist.get_pdf();
    pdf = pdf / step;
    ofstream f("test.dat");
    for (int i = 0; i < pdf.length(); ++i) {
        f << setw(4) << hist.get_bin_right(i) << setw(12) << pdf(i) << endl;
    }
    f.close();
    return 0;
}

