#include <itpp/itbase.h>
#include <itpp/itcomm.h>
#include <iomanip>
#include <iostream>
#include <fstream>

using namespace itpp;
using std::endl;
using std::setw;
using std::cout;

int main()
{
    int i, Number_of_bits;
    double Eb;
    vec EbN0dB, EbN0, N0, noise_variance, bit_error_rate;
    bvec transmitted_bits, received_bits;
    vec mod_symbols, demod_symbols;
    cvec transmitted_symbols, received_symbols;

    BPSK bpsk;                     //The BPSK modulator class
    AWGN_Channel awgn_channel;     //The AWGN channel class
    BERC berc;                     //Used to count the bit errors
    Real_Timer tt;                 //The timer used to measure the execution time

    //Reset and start the timer:
    tt.tic();

    Eb = 1.0;                      //The transmitted energy per BPSK symbol is 1.
    EbN0dB = linspace(-5, 20.0, 13); //Simulate for 10 Eb/N0 values from 0 to 9 dB.
    EbN0 = inv_dB(EbN0dB);         //Calculate Eb/N0 in a linear scale instead of dB.
    N0 = Eb * pow(EbN0, -1.0);     //N0 is the variance of the (complex valued) noise.
    Number_of_bits = 100000;       //One hundred thousand bits is transmitted for each Eb/N0 value

    //Allocate storage space for the result vector.
    //The "false" argument means "Do not copy the old content of the vector to the new storage area."
    bit_error_rate.set_size(EbN0dB.length(), false);
    Independent_Fading_Generator fad1;
    transmitted_symbols.set_size(Number_of_bits);
    demod_symbols.set_size(Number_of_bits);
    fad1.set_LOS_power(0);  // no LOS, so it's rayleigh
    cvec tc;
    fad1.generate(Number_of_bits, tc);

    RNG_randomize();

    //Iterate over all EbN0dB values:
    for (i = 0; i < EbN0dB.length(); i++) {

        //Show how the simulation progresses:
        cout << "Now simulating Eb/N0 value number " << i + 1 << " of " << EbN0dB.length() << endl;

        //Generate a vector of random bits to transmit:
        transmitted_bits = randb(Number_of_bits);

        //Modulate the bits to BPSK symbols:
        mod_symbols = bpsk.modulate_bits(transmitted_bits);
        for (int j = 0; j < Number_of_bits; ++j)
            transmitted_symbols(j) = mod_symbols(j) * tc(j);

        //Set the noise variance of the AWGN channel:
        awgn_channel.set_noise(N0(i));

        //Run the transmited symbols through the channel using the () operator:
        received_symbols = awgn_channel(transmitted_symbols);
        for (int j = 0; j < Number_of_bits; ++j)
            demod_symbols(j) = real(received_symbols(j) / tc(j));

        //Demodulate the received BPSK symbols into received bits:
        received_bits = bpsk.demodulate_bits(demod_symbols);

        //Calculate the bit error rate:
        berc.clear();                               //Clear the bit error rate counter
        berc.count(transmitted_bits, received_bits); //Count the bit errors
        bit_error_rate(i) = berc.get_errorrate();   //Save the estimated BER in the result vector

    }
    tt.toc();
    std::ofstream f("test.dat");
    for (int i = 0; i < bit_error_rate.length(); ++i) {
        f << setw(4) << EbN0dB(i) << setw(12) << bit_error_rate(i)
          << setw(12) << 0.5 *(1-sqrt(EbN0(i)/(1+EbN0(i)))) << endl;
    }
    f.close();

    return 0;
}

