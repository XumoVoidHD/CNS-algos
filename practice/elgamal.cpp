#include <iostream>
#include <numeric>
using namespace std;

int modInverse(int s, int p)
{
    for(int i = 1; i < p; i++)
    {
        if((s*i) % p == 1)
        {
            return i;
        }
    }
    return -1;
}

int modPow(int base, int exp, int mod)
{
    if(exp >= 0)
    {
        int result = 1;
        for(int i = 0; i < exp; i++)
        {
            result = (result * base) % mod;
        }

        return result;
    }
    else
    {
        int x = modInverse(base, mod);

        if(x == -1)
        {
            cout << "Inverse doesn't exist" << endl;
        }

        return modPow(x, -exp, mod);
    }
}

int main()
{
    int p = 11;
    int d = 3;
    int e1 = 2;

    int e2 = modPow(e1, d, p);

    int r = 4;

    int c1 = modPow(e1,r,p);

    int pt = 7;

    int c2 = (pt * modPow(e2, r, p)) % p;
    cout << "Ciphertext: (" << c1 << ", " << c2 << ")" << endl;

    int s = modPow(c2, d, p);
    int s_inv = modPow(s, -1, p);

    cout << "Plaintext " << (c1 * s_inv) % p;
}