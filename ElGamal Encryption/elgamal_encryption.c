/*
 * ElGamal Encryption - Exam-ready implementation
 * Key generation, encryption, decryption. Uses modular inverse and modular exponentiation.
 * NO crypto libraries.
 */

#include <stdio.h>

typedef unsigned long long ull;

ull mod_mul(ull a, ull b, ull m) {
    ull r = 0;
    a = a % m;
    while (b) {
        if (b & 1) r = (r + a) % m;
        a = (a * 2) % m;
        b >>= 1;
    }
    return r;
}

ull mod_pow(ull base, ull exp, ull m) {
    ull result = 1;
    base = base % m;
    while (exp) {
        if (exp & 1) result = mod_mul(result, base, m);
        exp >>= 1;
        base = mod_mul(base, base, m);
    }
    return result;
}

/* Extended GCD for modular inverse */
ull extended_gcd(ull a, ull b, long long *x, long long *y) {
    if (b == 0) {
        *x = 1;
        *y = 0;
        return a;
    }
    long long x1, y1;
    ull g = extended_gcd(b, a % b, &x1, &y1);
    *x = y1;
    *y = x1 - (long long)(a / b) * y1;
    return g;
}

ull mod_inverse(ull a, ull m) {
    long long x, y;
    if (extended_gcd(a, m, &x, &y) != 1) return 0;
    x = x % (long long)m;
    if (x < 0) x += (long long)m;
    return (ull)x;
}

/*
 * ElGamal key generation
 * p = prime, g = generator (primitive root of p)
 * Private key: x (random, 1 < x < p-1)
 * Public key: (p, g, y) where y = g^x mod p
 */
void elgamal_keygen(ull p, ull g, ull x, ull *y) {
    *y = mod_pow(g, x, p);
}

/*
 * ElGamal encrypt: message m, public key (p, g, y), random k
 * c1 = g^k mod p, c2 = m * y^k mod p
 * Ciphertext = (c1, c2)
 */
void elgamal_encrypt(ull m, ull p, ull g, ull y, ull k, ull *c1, ull *c2) {
    *c1 = mod_pow(g, k, p);
    *c2 = mod_mul(m, mod_pow(y, k, p), p);
}

/*
 * ElGamal decrypt: (c1, c2), private key x, prime p
 * m = c2 * (c1^x)^(-1) mod p = c2 * c1^(-x) mod p
 */
ull elgamal_decrypt(ull c1, ull c2, ull x, ull p) {
    ull s = mod_pow(c1, x, p);
    ull s_inv = mod_inverse(s, p);
    return mod_mul(c2, s_inv, p);
}

int main(void) {
    ull p = 23, g = 5;   /* Public: prime and generator */
    ull x = 6;           /* Alice's private key */
    ull y;               /* Alice's public key */
    ull m = 10;          /* Plaintext (must be < p) */
    ull k = 3;           /* Random ephemeral key (1 < k < p-1) */
    ull c1, c2, dec;

    printf("=== ElGamal Encryption ===\n");
    printf("Public: p = %llu, g = %llu\n", p, g);
    printf("Private key x = %llu\n", x);

    elgamal_keygen(p, g, x, &y);
    printf("Public key y = g^x mod p = %llu\n\n", y);

    printf("Plaintext m = %llu\n", m);
    printf("Random k = %llu\n", k);

    elgamal_encrypt(m, p, g, y, k, &c1, &c2);
    printf("Ciphertext (c1, c2) = (%llu, %llu)\n", c1, c2);

    dec = elgamal_decrypt(c1, c2, x, p);
    printf("Decrypted m' = %llu\n", dec);

    return 0;
}
