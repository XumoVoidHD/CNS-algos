/*
 * Diffie-Hellman Key Exchange - Exam-ready implementation
 * Primitive root (generator) g, prime p. Shared secret = g^(a*b) mod p.
 * NO crypto libraries. Manual modular exponentiation.
 */

#include <stdio.h>

typedef unsigned long long ull;

/* (a * b) % m avoiding overflow where possible */
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

/* base^exp mod m */
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

/*
 * Check if g is a primitive root of p (order of g mod p is p-1).
 * Simple check: g^(phi(p)/f) != 1 for every prime factor f of phi(p)=p-1.
 * For exam we use known p and g (e.g. p=23, g=5).
 */
int is_primitive_root(ull g, ull p) {
    ull phi = p - 1;
    ull n = phi;
    ull e;
    if (mod_pow(g, phi, p) != 1) return 0;
    for (e = 2; e * e <= n; e++) {
        if (n % e) continue;
        if (mod_pow(g, phi / e, p) == 1) return 0;
        while (n % e == 0) n /= e;
    }
    if (n > 1 && mod_pow(g, phi / n, p) == 1) return 0;
    return 1;
}

int main(void) {
    ull p = 23;   /* Prime modulus */
    ull g = 5;    /* Generator (primitive root mod 23) */
    ull a = 6;    /* Alice's private key */
    ull b = 15;   /* Bob's private key */
    ull A, B;     /* Public keys: A = g^a mod p, B = g^b mod p */
    ull s_a, s_b; /* Shared secrets: s_a = B^a, s_b = A^b */

    printf("=== Diffie-Hellman Key Exchange ===\n");
    printf("Public: p = %llu, g = %llu\n", p, g);
    printf("Alice private a = %llu\n", a);
    printf("Bob   private b = %llu\n", b);

    A = mod_pow(g, a, p);
    B = mod_pow(g, b, p);
    printf("\nAlice sends A = g^a mod p = %llu\n", A);
    printf("Bob   sends B = g^b mod p = %llu\n", B);

    s_a = mod_pow(B, a, p);
    s_b = mod_pow(A, b, p);
    printf("\nAlice shared secret = B^a mod p = %llu\n", s_a);
    printf("Bob   shared secret = A^b mod p = %llu\n", s_b);
    printf("Match: %s\n", s_a == s_b ? "Yes" : "No");

    return 0;
}
