#ifndef POLAR_H
#define POLAR_H

#include <itpp/comm/channel_code.h>
#include <itpp/itexports.h>

namespace itpp
{

class ITPP_EXPORT Polar: public Channel_Code
{
public:
  Polar(int in_n, int in_k);

  //! Destructor
  virtual ~Polar() { }

  //! Encode a bvec of indata
  virtual void encode(const bvec &uncoded_bits, bvec &coded_bits);
  //! Encode a bvec of indata
  virtual bvec encode(const bvec &uncoded_bits);

  //! Inherited from the base class - not implemented here
  virtual void decode(const bvec &coded_bits, bvec &decoded_bits) {
      it_error("Polar::decode(): Hard input decoding not implemented");
  }

  //! Inherited from the base class - not implemented here
  virtual bvec decode(const bvec &coded_bits) {
      it_error("Polar::decode(): Hard input decoding not implemented");
      return bvec();
  }

  virtual void decode(const vec &llr_in, bvec &output);
  virtual bvec decode(const vec &llr_in);

  //! Get the code rate
  virtual double get_rate() const {
    return static_cast<double>(k) / n;
  }

  //! SC decoder
  void decode_frame_sc(const vec &llr_in, bvec &output);

  //! SCL decoder
  void decode_frame_scl(const vec &llr_in, bvec &output, int list_size);

  //! Recursive SC decoder for debug reference
  void decode_frame_sc_r(const vec &llr_in, bvec &output, bvec &code, int s, int l);

  //! Generate frozen bits
  void gen_frozen_bec(double epsilon);
  void gen_frozen_ga(double sigma);
  
  //! Generate information bits position index
  void gen_unfrozen_idx();

private:
  int n, k;
  int layers; // log2(n)
  bvec fbit;  // frozen bit definition
  ivec ufbit; // store the position of the information bits
  double phi(double x);
  double phi_1(double y);
};

} // namespace itpp

#endif  // POLAR_H
