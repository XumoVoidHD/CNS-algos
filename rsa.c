/*
 * RSA - Exam-ready implementation
 * No crypto or big-integer libraries. Uses unsigned long long for small examples.
 * GCD, Extended Euclidean Algorithm, Modular Inverse, Fast Modular Exponentiation,
 * Key generation, Encryption, Decryption.
 */

#include <stdio.h>

typedef unsigned long long ull;

/* GCD using Euclidean algorithm: gcd(a,b) */
ull gcd(ull a, ull b) {
    ull t;
    while (b) {
        t = b;
        b = a % b;
        a = t;
    }
    return a;
}

/*
 * Extended Euclidean Algorithm: find x, y such that a*x + b*y = gcd(a,b)
 * Returns gcd(a,b). x and y are set so that a*x + b*y = gcd.
 * Uses signed type for x,y so negative values are correct.
 */
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

/* Modular inverse of a mod m. Returns 0 if no inverse (gcd(a,m) != 1). */
ull mod_inverse(ull a, ull m) {
    long long x, y;
    if (extended_gcd(a, m, &x, &y) != 1)
        return 0;
    x = x % (long long)m;
    if (x < 0) x += (long long)m;
    return (ull)x;
}

/* (a * b) mod m without overflow for large a,b (using ull we still overflow for huge numbers) */
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

/* Fast modular exponentiation: (base^exp) % mod */
ull mod_pow(ull base, ull exp, ull mod) {
    ull result = 1;
    base = base % mod;
    while (exp) {
        if (exp & 1)
            result = mod_mul(result, base, mod);
        exp >>= 1;
        base = mod_mul(base, base, mod);
    }
    return result;
}

/* Simple primality check (trial division). For exam use small primes. */
int is_prime(ull n) {
    ull i;
    if (n < 2) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i * i <= n; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

/*
 * RSA key generation (small primes for hand calculation).
 * p, q primes; n = p*q; phi = (p-1)*(q-1); choose e with gcd(e,phi)=1; d = e^(-1) mod phi.
 */
void rsa_keygen(ull p, ull q, ull *n, ull *e, ull *d) {
    *n = p * q;
    ull phi = (p - 1) * (q - 1);

    /* Choose e: 3 or 17 or 65537 common; must be coprime to phi */
    *e = 17;
    if (gcd(*e, phi) != 1) {
        *e = 3;
        if (gcd(*e, phi) != 1) {
            for (*e = 2; *e < phi && gcd(*e, phi) != 1; (*e)++) ;
        }
    }

    *d = mod_inverse(*e, phi);
}

/* RSA encrypt: c = m^e mod n */
ull rsa_encrypt(ull m, ull e, ull n) {
    return mod_pow(m, e, n);
}

/* RSA decrypt: m = c^d mod n = c^d mod n */
ull rsa_decrypt(ull c, ull d, ull n) {
    return mod_pow(c, d, n);
}

int main(void) {
    ull p = 61, q = 53;   /* Small primes for example */
    ull n, e, d;
    ull plain = 123;
    ull cipher, decrypted;

    printf("=== RSA Example (small primes) ===\n");
    printf("p = %llu, q = %llu\n", p, q);

    rsa_keygen(p, q, &n, &e, &d);
    printf("n = p*q = %llu\n", n);
    printf("phi = (p-1)(q-1) = %llu\n", (p-1)*(q-1));
    printf("e (public) = %llu\n", e);
    printf("d (private) = %llu\n", d);

    printf("\nPlaintext m = %llu (must be < n)\n", plain);
    cipher = rsa_encrypt(plain, e, n);
    printf("Ciphertext c = m^e mod n = %llu\n", cipher);
    decrypted = rsa_decrypt(cipher, d, n);
    printf("Decrypted m' = c^d mod n = %llu\n", decrypted);

    return 0;
}
