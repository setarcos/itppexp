#ifndef POLAR_H
#define POLAR_H

#include <itpp/comm/channel_code.h>
#include <itpp/comm/crc.h>
#include <itpp/itexports.h>

namespace itpp
{

enum POLAR_DECODER {POLAR_SC, POLAR_SCR, POLAR_SCL, POLAR_CASCL, PAC_SCL};

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
    return static_cast<double>(k - crc_size) / n;
  }

  //! choose decoder
  void set_polar_decoder(enum POLAR_DECODER d) {
      method = d;
      if (d == POLAR_SCL)
          crc_size = 0;
  }

  //! SC decoder
  void decode_frame_sc(const vec &llr_in, bvec &output);

  //! SCL decoder
  void decode_frame_scl(const vec &llr_in, bvec &output, int list_size);

  //! Set SCL decoder list size
  void set_scl_size(int size) {
      scl_size = size;
  }

  //! Set CRC code
  void set_crc_code(const std::string &code, int size) {
      crc.set_code(code);
      crc_size = size;
  }

  //! Recursive SC decoder for debug reference
  void decode_frame_sc_r(const vec &llr_in, bvec &output, bvec &code, int s, int l);

  //! Generate frozen bits
  void gen_frozen_bec(double epsilon);
  void gen_frozen_ga(double sigma);
  void gen_frozen_rm();

  //! Generate information bits position index
  void gen_unfrozen_idx();

  //! Set the function generator
  void set_pac_generator(const bvec &b) {
      generator = b;
      glen = b.size();
  }

  bin conv1b_trans(bin v, bvec &state, bvec &new_state, int start = 0);
  inline bin llr2bin(double llr) {
      if (llr < 0) return 1;
      else return 0;
  }

private:
  int n, k;
  int layers; // log2(n)
  bvec fbit;  // frozen bit definition
  ivec ufbit; // store the position of the information bits
  bvec generator; // function generator for the PAC convolutional code
  int scl_size;
  CRC_Code crc;
  int crc_size;
  int glen;
  enum POLAR_DECODER method;

  // functions used in GA
  double phi(double x);
  double phi_1(double y);
};

} // namespace itpp

#endif  // POLAR_H
