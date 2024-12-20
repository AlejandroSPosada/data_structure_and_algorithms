#include <iostream>
#include <vector>
#include <iomanip> 
using namespace std;
typedef vector<vector<double>> Matrix;
Matrix multiply(const Matrix& a, const Matrix& b) {
    int n = a.size();
    Matrix result(n, vector<double>(n, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
    return result;
}
Matrix matrix_exponentiation(Matrix base, long long power) {
    int n = base.size();
    Matrix result(n, vector<double>(n, 0));
    for (int i = 0; i < n; ++i) {
        result[i][i] = 1.0;
    }
    while (power > 0) {
        if (power % 2 == 1) {
            result = multiply(result, base);
        }
        base = multiply(base, base);
        power /= 2;
    }
    return result;
}
int main() {
    Matrix M = {
        {1, 2},
        {2, 0}
    };
    Matrix Mn = matrix_exponentiation(M, 0);
    for(int i = 0; i < Mn.size(); i++){
        for(int j = 0; j < Mn[0].size(); j++){
            cout << Mn[i][j] << " "; 
        }
        cout << endl;
    }
    return 0;
}

/*
PROBLEMS:
CODEFORCES:
    1) Random Mood (also: Markov Chains)
*/
