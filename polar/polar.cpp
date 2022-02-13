#include "polar.h"
#include <itpp/itbase.h>
#include <algorithm>
#include <iostream>
#include <stack>

namespace itpp {

Polar::Polar(int in_n, int in_k):
    n(in_n), k(in_k), scl_size(4), crc_size(0), method(POLAR_SCL)
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
    //std::cout << a[layers % 2] << std::endl;
    ivec index = sort_index(a[layers % 2]);
    fbit.set_size(n);
    fbit.zeros();
    for (int i = 0; i < k; ++i)
        fbit[index[i]] = 1;
    //std::cout << "[Info]frozen bits are: " << fbit << std::endl;
    gen_unfrozen_idx();
}

void Polar::gen_frozen_ga(double sigma)
{
    vec a[2];
    a[0].set_size(1);
    a[0][0] = 2 / ::pow(sigma, 2); // 2 / sigma^2
    int len = 1;
    for (int i = 0; i < layers; ++i) {
        len *= 2;
        int j = i + 1;
        a[j % 2].set_size(len);
        for (int k = 0; k < len / 2; ++k) {
            double tmp = a[i % 2][k];
            a[j % 2][k * 2] = phi_1(1 - ::pow(1 - phi(tmp), 2));
            a[j % 2][k * 2 + 1] = 2 * tmp;
        }
    }
    //std::cout << a[layers % 2] << std::endl;
    ivec index = sort_index(a[layers % 2]);
    fbit.set_size(n);
    fbit.zeros();
    for (int i = 0; i < k; ++i)
        fbit[index[i]] = 1;
    //std::cout << "[Info]frozen bits are: " << fbit << std::endl;
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
    int iter = length / (k - crc_size);
    coded_bits.set_size(iter * n);
    coded_bits.zeros();
    for (int i = 0; i < iter; ++i) {
        if (crc_size == 0) {
            for (int j = 0; j < k; ++j)
                coded_bits[i * n + ufbit[j]] = uncoded_bits[i * k + j];
        } else {
            bvec cabit;
            crc.encode(uncoded_bits.mid(i * (k - crc_size), k - crc_size), cabit);
            for (int j = 0; j < k; ++j)
                coded_bits[i * n + ufbit[j]] = cabit[j];
            // std::cout << cabit << std::endl;;
        }
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
        if (method == POLAR_SC)
            decode_frame_sc(llr_in.mid(i * n, n), o);
        if ((method == POLAR_SCL) || (method == POLAR_CASCL))
            decode_frame_scl(llr_in.mid(i * n, n), o, scl_size);
        if (method == POLAR_SCR) {
            bvec o2;
            bvec c;
            decode_frame_sc_r(llr_in.mid(i * n, n), o2, c, 0, n);
            o.set_size(k);
            for (int i = 0; i < k; ++i) {
                o[i] = o2[ufbit[i]];
            }
        }
        if (crc_size)
            output.replace_mid(i * (k - crc_size), o.mid(0, k - crc_size));
        else
            output.replace_mid(i * k, o);
    }
}

void Polar::decode_frame_sc(const vec &llr_in, bvec &output)
{
    static int initialized;
    vec llr = llr_in;
    llr.set_size(n * 2 - 1, true);
    static bvec dec;  // decisions of info bits;
    static bvec ibit[2];  // Internal bits, C array in some literature
    static ivec level;
    static ivec froms;
    if (not initialized) {
        initialized = 1;
        dec.set_size(k);
        ibit[0].set_size(n * 2 - 1);
        ibit[1].set_size(n * 2 - 1);
        level.set_size(n);
        for (int i = 1; i <= n; ++i)
            level[i - 1] = floor_i(::log2(i));
        froms.set_size(layers + 1);
        froms[0] = 0;
        for (int i = 1; i <= layers; ++i)
            froms[i] = froms[i - 1] + ::pow(2, layers - i + 1);
    }
    // cout << froms << endl;
    std::stack<int> st;
    int lastVisit = 0;
    int node = 0;
    int decidx = 0;
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
                if (fbit[node - n + 1])
                    ibit[node % 2][n * 2 - 2] = 0;
                else {
                    dec[decidx] = (llr[n * 2 - 2] < 0);
                    ibit[node % 2][n * 2 - 2] = dec[decidx];
                    decidx++;
                }
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
    output = dec;
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

void Polar::decode_frame_scl(const vec &llr_in, bvec &output, int list_size)
{
    static int initialized = 0;
    static vec llr;
    static bvec dec;  // decisions of info bits;
    static bvec ibit[2];  // Internal bits, C array in some literature
    static ivec level;
    static ivec froms;
    static vec pm;  // path matrics
    static ivec pm2;
    static ivec lc; // lazy copy array
    static bvec lactive; // active list vector
    if (not initialized) {
        initialized = 1;
        llr.set_size((n - 1) * list_size); // does not copy llr_in
        dec.set_size(k * list_size);  //save only info bits;
        ibit[0].set_size((n  - 1) * list_size);  // agree with llr
        ibit[1].set_size((n  - 1) * list_size);
        ibit[0].zeros();
        ibit[1].zeros();
        pm.set_size(list_size * 2);
        pm2.set_size(list_size);
        lc.set_size(layers * list_size);
        lactive.set_size(list_size * 2);
        level.set_size(n);
        for (int i = 1; i <= n; ++i)
            level[i - 1] = floor_i(::log2(i));
        froms.set_size(layers); // unlike sc decoder, llr does not duplicate llr_in, so layers - 1
        froms[0] = 0;
        for (int i = 1; i < layers; ++i)
            froms[i] = froms[i - 1] + ::pow(2, layers - i);
    }
    lc.zeros();
    lactive.zeros();
    lactive[0] = 1;
    int active_size = 1;
    pm.zeros();
    std::stack<int> st;
    int lastVisit = 0;
    int node = 0;
    int decidx = 0;
    while (!st.empty() || node < n * 2 - 1) {
        while (node < n * 2 - 1) {
            st.push(node);
            if (node < n - 1) {// F Function
               // std::cout << "F" << node << std::endl;
                int step = n / (2 << level[node]);
                for (int ll = 0; ll < active_size; ++ll) {
                    for (int i = 0; i < step; ++i) {
                        double v1, v2;
                        if (node == 0) {  // level[node] == 0
                            v1 = llr_in[i];
                            v2 = llr_in[i + step];
                        } else {
                            int idx = froms[level[node] - 1] + i + (n - 1) * ll;
                            v1 = llr[idx];
                            v2 = llr[idx + step];
                        }
                        llr[(n - 1) * ll + froms[level[node]] + i] =
                            sign(v1) * sign(v2) * ((::abs(v1) < ::abs(v2)) ? ::abs(v1) : ::abs(v2));
                    }
                }
                //std::cout << llr << std::endl;
            } else { // decision
                if (fbit[node - n + 1]) { // frozen bit
                    for (int ll = 0; ll < active_size; ++ll) {
                        //int idx = (n - 1) * lc[ll * layers + layers - 1] + n - 2;
                        int idx = (n - 1) * ll + n - 2;
                        ibit[node % 2][idx] = 0;
                        if (llr[idx] < 0) pm[ll] += ::abs(llr[idx]); // update pm
                    }
                } else { // clone list
                    for (int ll = 0; ll < active_size; ++ll) {
                        //int idx = (n - 1) * lc[ll * layers + layers - 1] + n - 2;
                        int idx = (n - 1) * ll + n - 2;
                        dec[k * ll + decidx] = (llr[idx] < 0);
                        ibit[node % 2][idx] = (llr[idx] < 0);
                        pm[ll + list_size] = pm[ll] + ::abs(llr[idx]);
                        if (active_size < list_size)
                            pm2[ll + active_size] = ll;
                    }
                    if (active_size == list_size) {
                       // std::cout << pm << std::endl;
                        //for (int i = 0; i < list_size; ++i)
                         //   std::cout << llr.mid((n-1)*i, n-1) << std::endl;
                        ivec index = sort_index(pm);
                        lactive.zeros();
                        for (int i = 0; i < list_size; ++i)
                            lactive[index[i]] = 1;
                        int i = 0;
                        int j = 0;
                        while(i < list_size) {
                            if (lactive[i] == 0) {
                                while(lactive[j + list_size] == 0) j++;
                                pm2[i++] = j++;
                            }
                            else
                                i++;
                        }
                    } else active_size *= 2;
                    for (int i = 0; i < active_size; ++i) {
                        if (lactive[i] == 0) {
                            // clone pm2[i](th) to i(th) position
                            for (int j = 0; j < layers; ++j)
                                lc[i * layers + j] = lc[pm2[i] * layers + j];
                            for (int j = 0; j < decidx; ++j)
                                dec[i * k + j] = dec[pm2[i] * k + j];
                            dec[i * k + decidx] = !dec[pm2[i] * k + decidx];
                            ibit[node % 2][i * (n - 1) + n - 2] = dec[i * k + decidx];
                            if (node % 2) lc[i * layers + layers - 1] = i;
                            pm[i] = pm[pm2[i] + list_size];
                            lactive[i] = 1;
                        }
                    }
                    if (decidx == k - 1) {
                        if (crc_size) { // CASCL
                            int best = -1;
                            double pmbest;
                            for (int i = 0; i < list_size; ++i) {
                                if (crc.check_parity(dec.mid(i * k, k))) {
                                    if (best < 0) {
                                        pmbest = pm[i];
                                        best = i;
                                    } else if (pm[i] <= pmbest) {
                                        best = i;
                                        pmbest = pm[i];
                                    }
                                }
                            }
                            if (best >= 0) {
                                //std::cout << "Best is" << best << std::endl;
                                output = dec.mid(best * k, k);
                                return;
                            } // if no valid crc found, fall back to choose the smallest pm index
                            //std::cout << "frame error" << std::endl;
                        }
                        ivec index = sort_index(pm.mid(0, list_size));
                        output = dec.mid(index[0]* k, k);
                        //std::cout << pm << std::endl;
                        //std::cout << dec << std::endl;
                        return;
                    }
                    decidx++;
                }
                //std::cout << "D" << node << std::endl;
                //std::cout << llr << std::endl;
                //std::cout << ibit[1] << std::endl;
                //std::cout << ibit[0] << std::endl;
                //std::cout << pm << std::endl;
                //std::cout << lc << std::endl;
            }
            node = node * 2 + 1; // Left child
        }
        node = st.top();
        if ((lastVisit == node * 2 + 2) || (node * 2 + 2 > n * 2 - 2)) {
            st.pop();
            lastVisit = node;
            if ((node > 0) && (node < n - 1)) {
                // Caculate internal bits
                int step = n / (2 << level[node]);
                for (int ll = 0; ll < active_size; ++ll) {
                    for (int i = 0; i < step; ++i) {
                        int idx = (n - 1) * lc[ll * layers + level[node]] + froms[level[node]] + i;
                        int idx1 = (n - 1) * ll + froms[level[node]] + i;
                        int idx2 = (n - 1) * ll + froms[level[node] - 1] + i;
                        ibit[node % 2][idx2] = ibit[0][idx1] + ibit[1][idx];
                        ibit[node % 2][idx2 + step] = ibit[0][idx1];
                    }
                    lc[ll * layers + level[node]] = ll;
                    if (node % 2) lc[ll * layers + level[node] - 1] = ll; // final write
                }
                // std::cout << "C" << node << "  " << active_size << std::endl;
            }
            node = 2 * n + 1; // out of range
        } else {
            if (node < n - 1) { // Not a leaf node
                int step = n / (2 << level[node]);
                // std::cout << ibit << std::endl;
                // G Function
                for (int ll = 0; ll < active_size; ++ll) {
                    for (int i = 0; i < step; ++i) {
                        double v1, v2;
                        if (node == 0) {  // level[node] == 0
                            v1 = llr_in[i];
                            v2 = llr_in[i + step];
                        } else {
                            int idx = froms[level[node] - 1] + i + (n - 1) * lc[ll * layers + level[node] - 1];
                            v1 = llr[idx];
                            v2 = llr[idx + step];
                        }
                        int idx = (n - 1) * ll + froms[level[node]] + i;
                        llr[idx] =(1 - 2 * (ibit[1][idx] ? 1 : 0)) * v1 + v2;
                    }
                }
                //std::cout << "G" << node << std::endl;
                //std::cout << llr << std::endl;
                //std::cout << ibit[1] << std::endl;
                //std::cout << ibit[0] << std::endl;
                //std::cout << lc << std::endl;
            }
            node = node * 2 + 2;
        }
    }
}

bvec Polar::decode(const vec &llr_in)
{
    bvec decoded_bits;
    decode(llr_in, decoded_bits);
    return decoded_bits;
}

double Polar::phi(double x)
{
    if (x < 10)
        return ::exp(-0.4527*::pow(x, 0.86)+0.0218);
    else
        return ::sqrt(pi/x)*::exp(-x/4)*(1-10/7/x);
}

double Polar::phi_1(double y)
{
    if (y > 0.0414)
        return ::pow((0.0218 - ::log(y))/0.4527, 1/0.86);
    double x1 = 10.0;
    double x2, y1;
    do {
        y1 = phi(x1);
        double k = 1000 * (phi(x1 + 0.001) - y1);
        x2 = x1;
        x1 += (y - y1) / k;
    } while (::abs(x2 - x1) > 0.01);
    return x1;
}

} // namespace itpp
