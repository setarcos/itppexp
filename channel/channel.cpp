#include <iomanip>  // for setw
#include <fstream>  // for ofstream
#include <itpp/comm/channel.h>
#include <itpp/itsignal.h>

using namespace std;
using namespace itpp;

const double PI=3.1415926;

int main(int argc, char * argv[])
{
    double Ts = 200.0e-9;
    const int LEN = 128;
    Channel_Specification channel_spec(ITU_Vehicular_A);
    TDL_Channel channel(channel_spec, Ts);
    //fc = 2.6GHz v = 30km/h
    const double doppler = 70.0;
    RNG_randomize();
    channel.set_norm_doppler(doppler * Ts);
    cvec tsig;
    cvec rsig;
    tsig.set_size(LEN);
    for (int i = 0; i < LEN; ++i) tsig[i] = 0;
    tsig[5] = 1;
    rsig = channel(tsig);
    ofstream ofs ("test.dat", ofstream::out);
    vec ic = dB(abs(fft_real(abs(rsig))));
    for (int i = 0; i < LEN; ++i) {
        ofs << setw(16) << abs(rsig[i]) << setw(16) << ic[i] << endl;
    }
    ofs.close();
}
