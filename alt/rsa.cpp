#include <iostream>
#include <cmath>
#include <numeric>
using namespace std;

/* ----------- GCD FUNCTION ----------- */
// int gcd(int a, int b)
// {
//     while(b != 0)
//     {
//         int temp = b;
//         b = a % b;
//         a = temp;
//     }
//     return a;
// }

/* ----------- FIND MODULAR INVERSE ----------- */
int find_d(int e, int phi)
{
    for(int d = 1; d < phi; d++)
    {
        if((e * d) % phi == 1)
            return d;
    }
    return -1;
}

/* ----------- POWER MOD FUNCTION ----------- */
long long modPow(long long base, long long exp, long long mod)
{
    long long result = 1;

    for(int i = 0; i < exp; i++)
    {
        result = (result * base) % mod;
    }

    return result;
}

/* ----------- MAIN ----------- */
int main()
{
    int p = 3;
    int q = 11;

    // Step 1: Compute n
    int n = p * q;

    // Step 2: Compute phi
    int phi = (p - 1) * (q - 1);

    // Step 3: Choose e
    int e;
    for(e = 2; e < phi; e++)
    {
        if(gcd(e, phi) == 1)
            break;
    }

    // Step 4: Find d
    int d = find_d(e, phi);

    cout << "Public Key (e, n): (" << e << ", " << n << ")" << endl;
    cout << "Private Key (d, n): (" << d << ", " << n << ")" << endl;

    // Message
    int message = 7;

    // Encryption
    long long cipher = modPow(message, e, n);
    cout << "Encrypted: " << cipher << endl;

    // Decryption
    long long decrypted = modPow(cipher, d, n);
    cout << "Decrypted: " << decrypted << endl;

    return 0;
}