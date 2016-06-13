#include <itpp/comm/channel.h>
#include <itpp/itbase.h>
#include <itpp/stat/histogram.h>
#include <iomanip>  // for setw
#include <fstream>

using namespace std;
using namespace itpp;
const int LEN = 10000;

int main(int argc, char* argv[])
{
    Independent_Fading_Generator fad1;
    fad1.set_LOS_power(0);  // no LOS, so it's rayleigh
    cvec tc;
    fad1.generate(LEN,tc);
    Histogram<double> hist(0, 3, 21);
    hist.update(abs(tc));
    vec pdf = hist.get_pdf();
    ofstream f("test.dat");
    for (int i = 0; i < pdf.length(); ++i) {
        f << setw(4) << hist.get_bin_center(i) << setw(12) << pdf(i) << endl;
    }
    f.close();
    return 0;
}

