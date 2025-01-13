// By AlejandroSPosada
// Code about: Suffix Array.
//   PROBLEMS
//     HACKERRANK
//       1) 004. Morgan and a String
#include<cstdio>
#include<iostream>
#include<vector>
#include<algorithm>
#include<string>
#include<cstring>
using namespace std;
enum { STYPE, LTYPE };
struct SAIS {
    string str;
    vector<int>S, SA, lcp, rank;
    int A_SIZE;
    int N;
    vector<int>t, B;
    bool is_lms(int i) {
        return i>0 && t[i] == STYPE && t[i-1] == LTYPE;
    }
    void bucket() {
        B = vector<int>(A_SIZE);
        for (int i=0; i<N; i++) B[S[i]]++;
        for (int i=0; i<A_SIZE-1; i++) B[i+1] += B[i];
    }

    void induced_sort() {
        // STEP 2
        bucket();
        for (int i=0; i<N; i++) {
            int j = SA[i]-1;
            if (j >= 0 && t[j] == LTYPE) SA[B[S[j]-1]++] = j;
        }
        // STEP 3
        bucket();
        for (int i=N; i--; ) {
            int j = SA[i]-1;
            if (j >= 0 && t[j] == STYPE) SA[--B[S[j]]] = j;
        }
    }
    SAIS(const vector<int>&S, int A_SIZE) : S(S), A_SIZE(A_SIZE) {
        // N is the size of the recursivity input, at the beggining 
        // represents the string input size
        N = S.size();
        // starts with -1 in order to make step 1
        SA = vector<int>(N, -1);
        // base case of the recursion
        if (N==1) {
            SA[0] = 0;
            return;
        }
        // t is where we are going to store the L and S type information
        t = vector<int>(N, STYPE);
        // fill t
        for (int i=N-1; i--;)
            if (S[i] > S[i+1] || (S[i] == S[i+1] && t[i+1] == LTYPE))
            t[i] = LTYPE;
        // preparing bucket
        bucket();
        // step 1
        for (int i=N; i--;)
            if (is_lms(i)) SA[--B[S[i]]] = i;
        // step 2 and 3
        induced_sort(); 
        // SA is going to store only the LMS type, i.e., is gonna cumply
        // the fuction of P1 (definition 2.4)
        int N1 = 0;
        for (int i=0; i<N; i++) if (is_lms(SA[i])) SA[N1++] = SA[i];   

        fill(SA.begin()+N1, SA.end(), -1);
        int name = 0, prev = -1;

        // We are going to store the LMS names in SA in the right part of SA, 
        // in SA+N1 and in such a way that they will be stored in their relative order in S. 
        // This is possible thanks to the property of lemma 2.1
        // WARN: Here we are not gonna compare completely the substrings, just partially.
        // For example, if prev = 1234, but cur = 12, this is NOT gonna mark them like diff = true.
        // It is gonna take them like being the same. We do this because in futures
        // recursions, the difference will be clarified anyway.
        for (int i=0; i<N1; i++) {
            int cur = SA[i];
            bool diff = (prev == -1);
            for (int j=0; !diff; j++) {
                if (j>0 && is_lms(cur+j)) break;
                if (S[cur+j] != S[prev+j]) diff = true;
            }
            if (diff) name++;   
            SA[N1+cur/2] = name-1;
            prev = cur;
        }
        // Now let's form S1, keeping in mind that
        // they are already in SA+N1 in their original positional order in S
        vector<int> S1, SA1(N1);
        for (int i=N1; i<N; i++) if (SA[i]>=0) S1.push_back(SA[i]);

        // if each character in S1 is unique, then S1[x] = z
        // is gonna be store in SA1[z] = x
        // where x is the relative order in S of the name z in S1
        if (name == N1) for (int i=0; i<N1; i++) SA1[S1[i]] = i;
        // else, apply recursion
        else SA1 = SAIS(S1, name).SA;

        // S1 is gonna comply the fuction of P1 (see definition 2.4)
        // i.e., is saving the index where each LMS is
        N1 = 0;
        for (int i=0; i<N; i++) if (is_lms(i)) S1[N1++] = i;

        // in right, SA1[x] = z, x is the absolute order of the name z in S1, example: SA1[0] = 5 
        // and S1[z] = y, y is the the exact index where z starts in S, example: S1[5] = 13
        // so, in left, SA1[x] = y, is gonna tell us absolute order suffix x in S1 starts with index y in S
        for (int i=0; i<N1; i++) SA1[i] = S1[SA1[i]];

        fill(SA.begin(), SA.end(), -1);
        bucket();       
        for (int i=N1; i--;) {
            int j = SA1[i];
            SA[--B[S[j]]] = j;
        }
        induced_sort();
    }
    SAIS(const string &str) : str(str) {
        // N is the size of the string
        N = str.size()+1;
        // S is the representation ASCII of each letter of the input string,
        // which facilite comparations
        S = vector<int>(N);
        for (int i=0; i<N; i++) S[i] = str[i];
        // we proceed to apply recursion, where SA is going to store the suffix array at the end
        *this = SAIS(S, 256);
    }
};

int main() {
    string str = "DABRACADABRAC";
    SAIS sais(str);
    cout << str << endl;
    for(int i = 1; i < str.size(); i++)
        cout << "position " << i << ": " << sais.SA[i] << endl;
     
    return 0;
}
