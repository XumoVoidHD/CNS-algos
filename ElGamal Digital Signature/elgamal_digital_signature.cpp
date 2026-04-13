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
    if (a < 0) {
        a += m;
    }
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
    long long p = 23;
    long long g = 5;
    long long x = 6;
    long long y = modPower(g, x, p);

    long long message = 13;
    long long k = 7;

    while (gcd(k, p - 1) != 1) {
        k++;
    }

    long long r = modPower(g, k, p);
    long long kInverse = modInverse(k, p - 1);
    long long s = (kInverse * (message - x * r)) % (p - 1);
    if (s < 0) {
        s += (p - 1);
    }

    long long left = modPower(g, message, p);
    long long right = (modPower(y, r, p) * modPower(r, s, p)) % p;

    cout << "Public values: p = " << p << ", g = " << g << "\n";
    cout << "Private key x = " << x << "\n";
    cout << "Public key y = " << y << "\n";
    cout << "Message = " << message << "\n";
    cout << "Signature (r, s) = (" << r << ", " << s << ")\n";
    cout << "Left side = " << left << "\n";
    cout << "Right side = " << right << "\n";
    cout << "Valid: " << (left == right ? "Yes" : "No") << "\n";
    return 0;
}
