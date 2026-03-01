#include <iostream>
#include <numeric>
using namespace std;

int find_d(int e, int phi)
{
    for(int i = 1; i < phi; i++)
    {
        if((e*i) % phi == 1)
        {
            return i;
        }
    }

    return -1;
}

long long modPow(int base, int exp, int mod)
{
    long long result = 1;

    for(int i = 0; i < exp; i++)
    {
        result = (result * base) % mod;
    }

    return result;
}

long long modInverse(long long a, long long mod)
{
    for(long long x = 1; x < mod; x++)
    {
        if((a * x) % mod == 1)
            return x;
    }
    return -1; // inverse not found
}

long long modPowSigned(long long base, long long exp, long long mod)
{
    // If exponent is positive → normal power
    if(exp >= 0)
    {
        return modPow(base, exp, mod);
    }
    else
    {
        // If exponent is negative
        long long inverse = modInverse(base, mod);

        if(inverse == -1)
        {
            cout << "Modular inverse does not exist." << endl;
            return -1;
        }

        // Make exponent positive
        long long positiveExp = -exp;

        return modPow(inverse, positiveExp, mod);
    }
}

int main()
{
    int p = 3;
    int q = 11;

    int n = p*q;

    int phi = (p-1)*(q-1);

    int e;
    for(e = 2; e < phi; e++)
    {
        if(gcd(e, phi) == 1)
        {
            break;
        }
    }

    int d = find_d(e, phi);

    cout << "Public Key (e, n): (" << e << ", " << n << ")" << endl;
    cout << "Private Key (d, n): (" << d << ", " << n << ")" << endl;

    // Message
    int message = 7;

    long long cipher = modPow(message, e, n);
    cout << "Encrypted: " << cipher << endl;

    long long decrypted = modPow(cipher, d, n);
    cout << "Decrypted: " << decrypted << endl;


    cout << modPowSigned(125, -3, 11);
    return 0;
}