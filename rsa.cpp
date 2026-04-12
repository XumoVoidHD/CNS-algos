#include <iostream>
using namespace std;

// GCD
int gcd(int a, int b) 
{
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// Modular Inverse (brute force)
int modInverse(int e, int phi) 
{
    for (int d = 1; d < phi; d++) {
        if ((e * d) % phi == 1)
            return d;
    }
    return -1;
}

// Modular Power
int modPower(int base, int exp, int mod) 
{
    int result = 1;
    base = base % mod;

    while (exp > 0) {
        if (exp % 2 == 1)
            result = (result * base) % mod;

        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}

int main() {
    int p = 3, q = 11;   // prime numbers

    int n = p * q;
    int phi = (p - 1) * (q - 1);

    // Choose e
    int e = 2;
    while (e < phi) {
        if (gcd(e, phi) == 1)
            break;
        e++;
    }

    // Find d
    int d = modInverse(e, phi);

    // Message
    int msg = 7;

    // Encryption
    int cipher = modPower(msg, e, n);

    // Decryption
    int decrypted = modPower(cipher, d, n);

    cout << "p: " << p << " q: " << q << endl;
    cout << "n: " << n << endl;
    cout << "phi: " << phi << endl;
    cout << "Public Key (e,n): (" << e << "," << n << ")" << endl;
    cout << "Private Key (d,n): (" << d << "," << n << ")" << endl;

    cout << "Message: " << msg << endl;
    cout << "Encrypted: " << cipher << endl;
    cout << "Decrypted: " << decrypted << endl;

    return 0;
}