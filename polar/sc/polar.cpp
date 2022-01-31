#include <itpp/itbase.h>
#include <iostream>
#include <stack>
using namespace itpp;
using std::endl;
using std::cout;

int main(int argc, char *argv[])
{
    int flen = 8;
    RNG_randomize();
    bvec info = randb(flen);
    bvec fbit = "1 1 1 0 1 0 0 0"; // "1" from frozen bits
    for (int i = 0; i < flen; ++i) {
        if (fbit[i]) info[i] = 0;
    }
    bvec code = info;

    // Encode info
    for (int step = 1; step < flen; step *= 2) {
        for (int j = 0; j < step; ++j) {
            for (int i = 0; i < flen; i += 2 * step) {
                code[i + j] += code[i + j + step];
            }
        }
    }
    cout << "info bits: " << info << endl;
    cout << "code word: " << code << endl;

    double N0 = 1; // 0db
    vec tsym;
    tsym.set_size(flen);
    for (int i = 0; i < flen; ++i) {
        tsym[i] = (code[i] ? -1.0 : 1.0);
    }
    vec rec = tsym + sqrt(N0/2) * randn(flen);

    vec llr = rec / (N0 / 4);
    llr.set_size(flen * 2 - 1, true);
    int maxlevel = log2(flen);
    bvec dec;  // decisions of info bits;
    dec.set_size(flen);
    bvec ibit;  // Internal bits
    ibit.set_size(flen * 2);
    ivec level;
    level.set_size(flen);
    for (int i = 1; i <= flen; ++i)
        level[i - 1] = floor(log2(i));
    // cout << level << endl;
    ivec froms;
    froms.set_size(maxlevel + 1);
    froms[0] = 0;
    for (int i = 1; i <= maxlevel; ++i)
        froms[i] = froms[i - 1] + pow(2, maxlevel - i + 1);
    // cout << froms << endl;
    std::stack<int> st;
    int node = 0;
    while (!st.empty() || node < flen * 2 - 1) {
        while (node < flen * 2 - 1) {
            st.push(node);
            if (node < flen - 1) {// F Function
                int step = flen / (2 << level[node]);
                for (int i = 0; i < step; ++i) {
                    double v1 = llr[froms[level[node]] + i];
                    double v2 = llr[froms[level[node]] + i + step];
                    llr[froms[level[node] + 1] + i] = sign(v1) * sign(v2)
                        * ((abs(v1) < abs(v2)) ? abs(v1) : abs(v2));
                }
            } else { // decision
                int idx = node - flen + 1;
                if (fbit[idx]) dec[idx] = 0;
                else if (llr[flen * 2 - 2] > 0) dec[idx] = 0;
                    else dec[idx] = 1;
                if (node % 2 == 1) // left child
                    ibit[flen * 2 - 2] = dec[idx];
                else
                    ibit[flen * 2 - 1] = dec[idx]; // save for future use
            }
            node = node * 2 + 1; // Left child
        }
        node = st.top();
        if (node < flen - 1) { // Not a leaf node
            int step = flen / (2 << level[node]);
            // Encode internal bits
            if (level[node] < maxlevel - 1) {
                int ml = level[node] + 1;
                ibit[froms[ml]] = ibit[flen * 2 - 1];
                for (int l = maxlevel; l > ml; --l) {
                    int s = flen / (1 << l);
                    for (int i = 0; i < s; ++i) {
                        ibit[froms[ml] + i + s] = ibit[froms[ml + 1] + i] + ibit[froms[ml] + i]; 
                    }
                }
            }
            // G Funcction
            for (int i = 0; i < step; ++i) {
                double v1 = llr[froms[level[node]] + i];
                double v2 = llr[froms[level[node]] + i + step];
                int idx = froms[level[node] + 1] + i;
                llr[idx] =(1 - 2 * (ibit[idx] ? 1 : 0)) * v1 + v2;
            }
        }
        node = node * 2 + 2;
        st.pop();
    }
    cout << "decoded:   " << dec << endl;
    if (dec == info) cout << "right" << endl;
    else cout << "error" << endl;
    return 0;
}
