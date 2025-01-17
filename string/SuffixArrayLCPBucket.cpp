// By AlejandroSPosada
// Code about: Suffix Array and LCP. 
// SA construction in O(L log^2 L) time.
// PROBLEMS: the same ones as SAIS 

#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <stack>
#include<cassert>
using namespace std;
struct SuffixArray {
 	const int L;
 	string s;
 	vector<vector<int> > P;
 	vector<pair<pair<int,int>,int> > M;
 	SuffixArray(const string &s) : L(s.length()), s(s), P(1, vector<int>(L, 0)), M(L) {
		for (int i = 0; i < L; i++) P[0][i] = int(s[i]);
		for (int skip = 1, level = 1; skip < L; skip *= 2, level++) {
			P.push_back(vector<int>(L, 0));
			for (int i = 0; i < L; i++)
			M[i] = make_pair(make_pair(P[level-1][i], i + skip < L ? P[level-1][i + skip] : -1000), i);
			sort(M.begin(), M.end());
			for (int i = 0; i < L; i++)
			P[level][M[i].second] = (i > 0 && M[i].first == M[i-1].first) ? P[level][M[i-1].second] : i;
   		}    
 	}
 	vector<int> GetSuffixArray() {
		return P.back(); 
	}
 	// returns the length of the longest common prefix of s[i...L-1] and s[j...L-1]
 	int LongestCommonPrefix(int i, int j) {
		int len = 0;
		for (int k = P.size() - 1; k >= 0 && i < L && j < L; k--) {
			if (P[k][i] == P[k][j]) {
				i += 1 << k;
				j += 1 << k;
				len += 1 << k;
			}
		}
		return len;
 	}
};

int main(){
	string str;
	cin>>str;
	vector<int> hist;
	assert(str.length() <= 500000);
	SuffixArray suffix(str);
	vector<int> v = suffix.GetSuffixArray();
	int a[str.length()];
	for (int i = 0; i < v.size(); i++)
		a[v[i]] = i;
	for(int i=0 ; i< str.length() - 1 ; i++){
		hist.push_back(suffix.LongestCommonPrefix(a[i], a[i+1]));
	}
	cout << "CHARACTER INDEX SUFFIX" << endl;
	for(const int& a : v)
		cout << a << ", ";
	cout << endl;
	cout << "SUFFIX ARRAY (without $)" << endl;
	for(int i = 0; i < str.length(); i++)
		cout << a[i] << ", ";
	cout << endl;
	cout << "LONGEST COMMON PREFIX ARRAY (LCP)" << endl;
	for(const int& a : hist)
		cout << a << ", "; 
	return 0;
}


};
