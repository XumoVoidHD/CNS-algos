#include <iostream>
using namespace std;

long long gcd(long long a, long long b) {
    while (b != 0) {
        long long t = b;
        b = a % b;
        a = t;
    }
    return a;
}

long long modInverse(long long a, long long m) {
    a %= m;
    for (long long x = 1; x < m; x++) {
        if ((a * x) % m == 1) {
            return x;
        }
    }
    return -1;
}

long long modPower(long long base, long long exp, long long mod) {
    long long result = 1;
    base %= mod;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % mod;
        }
        base = (base * base) % mod;
        exp /= 2;
    }
    return result;
}

int main() {
    long long p = 3, q = 11;
    long long n = p * q;
    long long phi = (p - 1) * (q - 1);

    long long e = 2;
    while (e < phi && gcd(e, phi) != 1) {
        e++;
    }

    long long d = modInverse(e, phi);
    long long message = 7;

    long long signature = modPower(message, d, n);
    long long verifiedMessage = modPower(signature, e, n);

    cout << "Public Key (e, n): (" << e << ", " << n << ")\n";
    cout << "Private Key (d, n): (" << d << ", " << n << ")\n";
    cout << "Message: " << message << "\n";
    cout << "Signature: " << signature << "\n";
    cout << "Verified Message: " << verifiedMessage << "\n";
    cout << "Valid: " << (verifiedMessage == message ? "Yes" : "No") << "\n";
    return 0;
}
