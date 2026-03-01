/*
 * S-DES (Simplified DES) - Exam-ready implementation
 * 8-bit block, 10-bit key, 2 rounds
 * Beginner-friendly, no external libraries.
 */

#include <stdio.h>
#include <string.h>

/* ========== S-DES TABLES (0-indexed positions; spec uses 1-based) ========== */

/* P10: 10-bit key permutation */
static const int P10[] = { 3, 5, 2, 7, 4, 10, 1, 9, 8, 6 };

/* P8: selects 8 bits from 10 for round key */
static const int P8[] = { 6, 3, 7, 4, 8, 5, 10, 9 };

/* Initial Permutation on 8-bit block */
static const int IP[] = { 2, 6, 3, 1, 4, 8, 5, 7 };

/* IP inverse */
static const int IPinv[] = { 4, 1, 3, 5, 7, 2, 8, 6 };

/* E/P Expansion: 4 bits -> 8 bits (output positions 1..8 get from input positions EP[0]..EP[7]) */
static const int EP[] = { 4, 1, 2, 3, 2, 3, 4, 1 };

/* P4: 4 bits after S-boxes */
static const int P4[] = { 2, 4, 3, 1 };

/* S0: 4 bits in (row=bit0,bit3; col=bit1,bit2) -> 2 bits out */
static const int S0[4][4] = {
    { 1, 0, 3, 2 },
    { 3, 2, 1, 0 },
    { 0, 2, 1, 3 },
    { 3, 1, 3, 2 }
};

/* S1 */
static const int S1[4][4] = {
    { 0, 1, 2, 3 },
    { 2, 0, 1, 3 },
    { 3, 0, 1, 0 },
    { 2, 1, 0, 3 }
};

/* Get bit at position pos (1..n) from byte array. For 10-bit key use 2 bytes (key[0]=bits 1-8, key[1]=bits 9-10 in high bits). */
static int get_bit10(const unsigned char *key, int pos) {
    if (pos <= 8) return (key[0] >> (8 - pos)) & 1;
    return (key[1] >> (10 - pos)) & 1;
}
static void set_bit10(unsigned char *key, int pos, int v) {
    if (pos <= 8) {
        if (v) key[0] |=  (1u << (8 - pos));
        else   key[0] &= ~(1u << (8 - pos));
    } else {
        if (v) key[1] |=  (1u << (10 - pos));
        else   key[1] &= ~(1u << (10 - pos));
    }
}

/* Get/set bit in 8-bit block (positions 1..8) */
static int get_bit8(const unsigned char *b, int pos) {
    return (b[0] >> (8 - pos)) & 1;
}
static void set_bit8(unsigned char *b, int pos, int v) {
    if (v) b[0] |=  (1u << (8 - pos));
    else   b[0] &= ~(1u << (8 - pos));
}

/* Permute 10 bits: src has 2 bytes (10 bits), dst 2 bytes, table T length 10 */
static void permute10(const unsigned char *src, unsigned char *dst, const int *T, int n) {
    int i;
    for (i = 0; i < n; i++)
        set_bit10(dst, i + 1, get_bit10(src, T[i]));
}

/* Permute 8 bits: one byte */
static void permute8(const unsigned char *src, unsigned char *dst, const int *T, int n) {
    int i;
    for (i = 0; i < n; i++)
        set_bit8(dst, i + 1, get_bit8(src, T[i]));
}

/* Left circular shift left half (bits 1-5) and right half (bits 6-10) by 1 */
static void ls1(unsigned char *key) {
    int b1 = get_bit10(key, 1), b6 = get_bit10(key, 6);
    int i;
    for (i = 1; i <= 4; i++) {
        set_bit10(key, i, get_bit10(key, i + 1));
        set_bit10(key, i + 5, get_bit10(key, i + 6));
    }
    set_bit10(key, 5, b1);
    set_bit10(key, 10, b6);
}

/* Shift by 2 (two single shifts) */
static void ls2(unsigned char *key) {
    ls1(key);
    ls1(key);
}

/* Generate K1 (first 8 bits after P8) and K2 (after one more LS then P8) */
static void key_schedule(const unsigned char *key_in, unsigned char *k1, unsigned char *k2) {
    unsigned char key[2];
    permute10(key_in, key, P10, 10);
    ls1(key);
    permute10(key, k1, P8, 8);  /* K1: first round key (8 bits in one byte) */
    ls2(key);                    /* total 3 shifts for K2 */
    permute10(key, k2, P8, 8);  /* K2 */
}

/* f_k(R, K): R is 4 bits (right half), K is 8 bits. Output 4 bits. */
static void f_k(const unsigned char *R, unsigned char K, unsigned char *out) {
    unsigned char ep[1];   /* 8 bits */
    unsigned char xored;
    int r0, c0, r1, c1, i;
    unsigned char s_out;
    unsigned char p4[1];

    /* Expand R (4 bits in high nibble) to 8 bits using E/P */
    for (i = 0; i < 8; i++)
        set_bit8(ep, i + 1, get_bit8(R, EP[i]));

    xored = ep[0] ^ K;

    /* S0: bits 1-4 of xored. Row = bit1,bit4; Col = bit2,bit3 */
    r0 = (get_bit8(&xored, 1) << 1) | get_bit8(&xored, 4);
    c0 = (get_bit8(&xored, 2) << 1) | get_bit8(&xored, 3);
    /* S1: bits 5-8 */
    r1 = (get_bit8(&xored, 5) << 1) | get_bit8(&xored, 8);
    c1 = (get_bit8(&xored, 6) << 1) | get_bit8(&xored, 7);

    s_out = (S0[r0][c0] << 2) | S1[r1][c1];  /* 4 bits */

    set_bit8(p4, 1, (s_out >> 3) & 1);
    set_bit8(p4, 2, (s_out >> 2) & 1);
    set_bit8(p4, 3, (s_out >> 1) & 1);
    set_bit8(p4, 4, s_out & 1);
    permute8(p4, out, P4, 4);
}

/* S-DES encrypt: block is 1 byte (8 bits), key is 2 bytes (10 bits in high bits) */
void sdes_encrypt(const unsigned char *key, unsigned char *block) {
    unsigned char k1, k2;
    unsigned char ip_out[1], L[1], R[1], f_out[1], new_L[1];

    key_schedule(key, &k1, &k2);

    permute8(block, ip_out, IP, 8);
    /* Keep 4-bit halves in high nibble for get_bit8 */
    L[0] = ip_out[0] & 0xF0;           /* left 4 bits */
    R[0] = (ip_out[0] & 0x0F) << 4;

    /* Round 1: L1 = R0, R1 = L0 XOR f(R0, K1) */
    f_k(R, k1, f_out);
    new_L[0] = R[0];
    R[0] = (L[0] ^ f_out[0]) & 0xF0;
    L[0] = new_L[0];

    /* Round 2: L2 = R1, R2 = L1 XOR f(R1, K2) */
    f_k(R, k2, f_out);
    new_L[0] = R[0];
    R[0] = (L[0] ^ f_out[0]) & 0xF0;
    L[0] = new_L[0];

    /* Combine: output = R2 || L2 (swap), then IP^-1. R2 in high nibble, L2 in low */
    block[0] = (R[0] & 0xF0) | (L[0] >> 4);
    permute8(block, block, IPinv, 8);
}

/* S-DES decrypt: same but K2 first, then K1 */
void sdes_decrypt(const unsigned char *key, unsigned char *block) {
    unsigned char k1, k2;
    unsigned char ip_out[1], L[1], R[1], f_out[1], new_L[1];

    key_schedule(key, &k1, &k2);

    permute8(block, ip_out, IP, 8);
    L[0] = ip_out[0] & 0xF0;
    R[0] = (ip_out[0] & 0x0F) << 4;

    /* Round 1 with K2 */
    f_k(R, k2, f_out);
    new_L[0] = R[0];
    R[0] = (L[0] ^ f_out[0]) & 0xF0;
    L[0] = new_L[0];

    /* Round 2 with K1 */
    f_k(R, k1, f_out);
    new_L[0] = R[0];
    R[0] = (L[0] ^ f_out[0]) & 0xF0;
    L[0] = new_L[0];

    block[0] = (R[0] & 0xF0) | (L[0] >> 4);
    permute8(block, block, IPinv, 8);
}

int main(void) {
    /* Key 10 bits: 0x2E8 = 1011101000 -> key[0]=0xBA (bits 1-8), key[1]=0x80 (bits 9-10) */
    unsigned char key[2] = { 0xBA, 0x80 };
    unsigned char block[1] = { 0x65 };  /* 8-bit plaintext 0x65 */

    printf("=== S-DES Example ===\n");
    printf("Key (10 bits): 0x2E8\n");
    printf("Plaintext: 0x%02X\n", block[0]);

    sdes_encrypt(key, block);
    printf("Ciphertext: 0x%02X\n", block[0]);

    sdes_decrypt(key, block);
    printf("Decrypted: 0x%02X\n", block[0]);

    return 0;
}
