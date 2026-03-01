#include <iostream>
#include <numeric>
using namespace std;

int modPow(int base, int exp, int mod)
{
    int result = 1;
    for(int i = 0; i < exp; i++)
    {
        result = (result * base) % mod;
    }

    return result;
}

int main()
{
    int x = 3;
    int y = 5;
    int n = 11;
    int g = 7;

    int A = modPow(g, x, 11);
    int B = modPow(g, y, 11);

    int K1 = modPow(B, x, 11);
    int K2 = modPow(A, y, 11);

    cout << K1 << " " << K2;
}