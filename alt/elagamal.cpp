#include <iostream>
#include <numeric>
using namespace std;

/* ----------- MOD POWER ----------- */
long long modPow(long long base, long long exp, long long mod)
{
    long long result = 1;

    for(int i = 0; i < exp; i++)
    {
        result = (result * base) % mod;
    }

    return result;
}

/* ----------- MOD INVERSE ----------- */
long long modInverse(long long a, long long mod)
{
    for(long long i = 1; i < mod; i++)
    {
        if((a * i) % mod == 1)
            return i;
    }
    return -1;
}

/* ----------- MAIN ----------- */
int main()
{
    // Public values
    int p = 11;      // Prime
    int g = 2;       // Generator

    // Private key
    int x = 3;

    // Public key
    int y = modPow(g, x, p);

    cout << "Public Key (p, g, y): "
         << p << ", " << g << ", " << y << endl;

    // Message
    int m = 7;

    // Random k
    int k = 4;

    // Encryption
    int c1 = modPow(g, k, p);
    int c2 = (m * modPow(y, k, p)) % p;

    cout << "Ciphertext: (" << c1 << ", " << c2 << ")" << endl;

    // Decryption
    int s = modPow(c1, x, p);
    int s_inv = modInverse(s, p);

    int decrypted = (c2 * s_inv) % p;

    cout << "Decrypted Message: " << decrypted << endl;

    return 0;
}