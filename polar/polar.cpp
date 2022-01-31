#include "polar.h"
#include <itpp/itbase.h>
#include <algorithm>
#include <iostream>
#include <stack>

namespace itpp {

Polar::Polar(int in_n, int in_k):
    n(in_n), k(in_k)
{
    layers = ::log2(n);
}

void Polar::gen_frozen_bec(double epsilon)
{
    vec a[2];

    a[0].set_size(1);
    a[0][0] = epsilon;
    int len = 1;
    for (int i = 0; i < layers; ++i) {
        len *= 2;
        int j = i + 1;
        a[j % 2].set_size(len);
        for (int k = 0; k < len / 2; ++k) {
            a[j % 2][k * 2] = a[i % 2][k] * a[i % 2][k];
            a[j % 2][k * 2 + 1] = 2 * a[i % 2][k] - a[i % 2][k] * a[i % 2][k];
        }
    }
  //  std::cout << a[layers % 2] << std::endl;
    ivec index = sort_index(a[layers % 2]);
    fbit.set_size(n);
    fbit.zeros();
    for (int i = 0; i < k; ++i)
        fbit[index[i]] = 1;
    std::cout << "[Info]frozen bits are: " << fbit << std::endl;
    gen_unfrozen_idx();
}

void Polar::gen_unfrozen_idx()
{
    ufbit.set_size(k);
    int i = 0;
    int j = 0;
    while (i < n) {
        if (fbit[i] == 0) ufbit[j++] = i;
        i++;
    }
}

void Polar::encode(const bvec &uncoded_bits, bvec &coded_bits)
{
    int length = uncoded_bits.length();
    int iter = length / k;
    coded_bits.set_size(iter * n);
    coded_bits.zeros();
    for (int i = 0; i < iter; ++i) {
        for (int j = 0; j < k; ++j)
            coded_bits[i * n + ufbit[j]] = uncoded_bits[i * k + j];
        for (int step = 1; step < n; step *= 2) {
            for (int j = 0; j < step; ++j) {
                for (int l = 0; l < n; l += 2 * step) {
                    coded_bits[n * i + l + j] += coded_bits[n * i + l + j + step];
                }
            }
        }
    }
}

bvec Polar::encode(const bvec &uncoded_bits)
{
    bvec coded_bits;
    encode(uncoded_bits, coded_bits);
    return coded_bits;
}

void Polar::decode(const vec &llr_in, bvec &output)
{
    int length = llr_in.length();
    int iter = length / n;
    output.set_size(iter * k);
    for (int i = 0; i < iter; ++i) {
        bvec o;
        decode_frame_sc(llr_in.mid(i * n, n), o);
    /*    bvec o2;
        bvec c;
        decode_frame_sc_r(llr_in.mid(i * n, n), o2, c, 0, n);
        o.set_size(k);
        for (int i = 0; i < k; ++i) {
            o[i] = o2[ufbit[i]];
        } */
        output.replace_mid(i * k, o);
    }
}

void Polar::decode_frame_sc(const vec &llr_in, bvec &output)
{
    vec llr = llr_in;
    llr.set_size(n * 2 - 1, true);
    for (int i = n; i < n * 2 - 1; ++i) llr[i] = 0;
    bvec dec;  // decisions of info bits;
    dec.set_size(n);
    bvec ibit[2];  // Internal bits, C array in some literature
    ibit[0].set_size(n * 2 - 1);
    ibit[1].set_size(n * 2 - 1);
    ibit[0].zeros();
    ibit[1].zeros();
    ivec level;
    level.set_size(n);
    for (int i = 1; i <= n; ++i)
        level[i - 1] = floor_i(::log2(i));
    ivec froms;
    froms.set_size(layers + 1);
    froms[0] = 0;
    for (int i = 1; i <= layers; ++i)
        froms[i] = froms[i - 1] + ::pow(2, layers - i + 1);
    // cout << froms << endl;
    std::stack<int> st;
    int lastVisit = 0;
    int node = 0;
    while (!st.empty() || node < n * 2 - 1) {
        while (node < n * 2 - 1) {
            st.push(node);
            if (node < n - 1) {// F Function
               // std::cout << "F" << node << std::endl;
                int step = n / (2 << level[node]);
                for (int i = 0; i < step; ++i) {
                    double v1 = llr[froms[level[node]] + i];
                    double v2 = llr[froms[level[node]] + i + step];
                    llr[froms[level[node] + 1] + i] = sign(v1) * sign(v2)
                        * ((::abs(v1) < ::abs(v2)) ? ::abs(v1) : ::abs(v2));
                }
                //std::cout << llr << std::endl;
            } else { // decision
                int idx = node - n + 1;
                if (fbit[idx]) dec[idx] = 0;
                else if (llr[n * 2 - 2] > 0) dec[idx] = 0;
                else dec[idx] = 1;
                ibit[node % 2][n * 2 - 2] = dec[idx];
                //std::cout << "D" << node << std::endl;
            }
            node = node * 2 + 1; // Left child
        }
        node = st.top();
        if ((lastVisit == node * 2 + 2) || (node * 2 + 2 > n * 2 - 2)) {
            st.pop();
            lastVisit = node;
            if (node < n - 1) {
                // Caculate internal bits
                int step = n / (2 << level[node]);
                for (int i = 0; i < step; ++i) {
                    int idx = froms[level[node] + 1] + i;
                    ibit[node % 2][froms[level[node]] + i] = ibit[0][idx] + ibit[1][idx];
                    ibit[node % 2][froms[level[node]] + i + step] = ibit[0][idx];
                }
                //std::cout << "C" << node << std::endl;
                //std::cout << ibit[1] << std::endl << ibit[0] << std::endl;
            }
            node = 2 * n + 1; // out of range
        } else {
            if (node < n - 1) { // Not a leaf node
                int step = n / (2 << level[node]);
                // std::cout << ibit << std::endl;
                // G Function
                for (int i = 0; i < step; ++i) {
                    double v1 = llr[froms[level[node]] + i];
                    double v2 = llr[froms[level[node]] + i + step];
                    int idx = froms[level[node] + 1] + i;
                    llr[idx] =(1 - 2 * (ibit[1][idx] ? 1 : 0)) * v1 + v2;
                }
                //std::cout << "G" << node << std::endl;
                //std::cout << llr << std::endl;
                //std::cout << ibit[1] << std::endl;
            }
            node = node * 2 + 2;
        }
    }
    output.set_size(k);
    for (int i = 0; i < k; ++i) {
        output[i] = dec[ufbit[i]];
    }
}

void Polar::decode_frame_sc_r(const vec &llr_in, bvec &output, bvec &code, int s, int l)
{
    output.set_size(l);
    code.set_size(l);
    if (l == 1) {
        if (fbit[s])
            output[0] = 0;
        else {
            if (llr_in[0] < 0) output[0] = 1;
            else output[0] = 0;
        }
        code[0] = output[0];
    } else {
        vec llrl, llrr;
        bvec codel, coder;
        bvec outl, outr;
        llrl.set_size(l / 2);
        llrr.set_size(l / 2);
        codel.set_size(l / 2);
        coder.set_size(l / 2);
        outl.set_size(l / 2);
        outr.set_size(l / 2);
        // F function
        for (int i = 0; i < l / 2; ++i) {
            double v1 = llr_in[i];
            double v2 = llr_in[i + l / 2];
            llrl[i] = sign(v1) * sign(v2) * ((::abs(v1) < ::abs(v2)) ? ::abs(v1) : ::abs(v2));
        }
        decode_frame_sc_r(llrl, outl, codel, s, l / 2);
        // G function
        for (int i = 0; i < l / 2; ++i) {
            llrr[i] = (1 - 2 * (codel[i] ? 1 : 0)) * llr_in[i] + llr_in[i + l / 2];
        }
        decode_frame_sc_r(llrr, outr, coder, s + l / 2, l / 2);
        for (int i = 0; i < l / 2; ++i) {
            code[i] = codel[i] + coder[i];
            code[i + l / 2] = coder[i];
            output[i] = outl[i];
            output[i + l / 2] = outr[i];
        }
        std::cout << s <<" "<< l << output << std::endl;
        std::cout << llr_in << std::endl;
        std::cout << code << std::endl;
        std::cout << output << std::endl;
    }
}

bvec Polar::decode(const vec &llr_in)
{
    bvec decoded_bits;
    decode(llr_in, decoded_bits);
    return decoded_bits;
}

} // namespace itpp
