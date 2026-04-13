/*
 * Man-in-the-Middle (MITM) Attack on Diffie-Hellman - Exam-ready simulation
 * Attacker sits between Alice and Bob, replaces A and B with attacker's own values,
 * establishes two shared secrets: one with Alice, one with Bob.
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

int main(void) {
    ull p = 23, g = 5;
    ull a = 6;   /* Alice's private */
    ull b = 15;  /* Bob's private */
    ull x = 3;   /* Attacker's private (for forging keys) */

    ull A_real, B_real;       /* Real A from Alice, real B from Bob */
    ull A_fake, B_fake;       /* Attacker sends these instead */
    ull s_alice_bob;          /* What Alice thinks is shared with Bob */
    ull s_bob_alice;          /* What Bob thinks is shared with Alice */
    ull s_attacker_alice;     /* Attacker's secret with Alice */
    ull s_attacker_bob;       /* Attacker's secret with Bob */

    printf("=== MITM Attack on Diffie-Hellman ===\n\n");
    printf("Public: p = %llu, g = %llu\n", p, g);
    printf("Alice private a = %llu\n", a);
    printf("Bob   private b = %llu\n", b);
    printf("Attacker private x = %llu\n\n", x);

    /* Normal DH: Alice computes A = g^a, Bob computes B = g^b */
    A_real = mod_pow(g, a, p);
    B_real = mod_pow(g, b, p);
    printf("(Without attacker) Alice would send A = %llu, Bob would send B = %llu\n", A_real, B_real);

    /* Attacker intercepts. Attacker sends g^x to both (or different forges; common is same g^x) */
    A_fake = mod_pow(g, x, p);  /* Attacker sends this to Bob as "Alice's A" */
    B_fake = mod_pow(g, x, p);  /* Attacker sends this to Alice as "Bob's B" */

    printf("\n--- Attacker intercepts and replaces ---\n");
    printf("Attacker sends to Bob   (pretending to be Alice): A' = g^x = %llu\n", A_fake);
    printf("Attacker sends to Alice (pretending to be Bob):   B' = g^x = %llu\n", B_fake);

    /* Alice thinks she's doing DH with Bob: she has a, receives B' = g^x. Her shared secret = (g^x)^a */
    s_attacker_alice = mod_pow(B_fake, a, p);
    s_alice_bob = s_attacker_alice;  /* What Alice computes */

    /* Bob thinks he's doing DH with Alice: he has b, receives A' = g^x. His shared secret = (g^x)^b */
    s_attacker_bob = mod_pow(A_fake, b, p);
    s_bob_alice = s_attacker_bob;    /* What Bob computes */

    printf("\n--- Shared secrets ---\n");
    printf("Alice's key (with fake Bob) = (B')^a mod p = %llu\n", s_alice_bob);
    printf("Bob's   key (with fake Alice) = (A')^b mod p = %llu\n", s_bob_alice);
    printf("Attacker's key with Alice = (B')^a = %llu (same as Alice's)\n", s_attacker_alice);
    printf("Attacker's key with Bob   = (A')^b = %llu (same as Bob's)\n", s_attacker_bob);

    printf("\n--- Result ---\n");
    printf("Alice and Bob do NOT share the same secret (Alice has %llu, Bob has %llu).\n", s_alice_bob, s_bob_alice);
    printf("Attacker shares secret with Alice and with Bob, and can decrypt/relay messages.\n");

    return 0;
}
