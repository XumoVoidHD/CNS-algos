/*
 * DES (Data Encryption Standard) - Exam-ready implementation
 * 64-bit block, 56-bit key, 16 rounds
 * NO external crypto libraries. All tables and logic manual.
 */

#include <stdio.h>
#include <string.h>

/* ========== PERMUTATION TABLES (1-based in spec, we use 0-based index) ========== */

/* Initial Permutation: 64 bits reordered */
static const int IP[] = {
    58, 50, 42, 34, 26, 18, 10,  2,
    60, 52, 44, 36, 28, 20, 12,  4,
    62, 54, 46, 38, 30, 22, 14,  6,
    64, 56, 48, 40, 32, 24, 16,  8,
    57, 49, 41, 33, 25, 17,  9,  1,
    59, 51, 43, 35, 27, 19, 11,  3,
    61, 53, 45, 37, 29, 21, 13,  5,
    63, 55, 47, 39, 31, 23, 15,  7
};

/* Final Permutation = IP inverse */
static const int FP[] = {
    40,  8, 48, 16, 56, 24, 64, 32,
    39,  7, 47, 15, 55, 23, 63, 31,
    38,  6, 46, 14, 54, 22, 62, 30,
    37,  5, 45, 13, 53, 21, 61, 29,
    36,  4, 44, 12, 52, 20, 60, 28,
    35,  3, 43, 11, 51, 19, 59, 27,
    34,  2, 42, 10, 50, 18, 58, 26,
    33,  1, 41,  9, 49, 17, 57, 25
};

/* Expansion E: 32 bits -> 48 bits (for R half before XOR with round key) */
static const int E[] = {
    32,  1,  2,  3,  4,  5,
     4,  5,  6,  7,  8,  9,
     8,  9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32,  1
};

/* P permutation: 32 bits after S-boxes */
static const int P[] = {
    16,  7, 20, 21, 29, 12, 28, 17,
     1, 15, 23, 26,  5, 18, 31, 10,
     2,  8, 24, 14, 32, 27,  3,  9,
    19, 13, 30,  6, 22, 11,  4, 25
};

/* Key schedule: PC1 (64 -> 56 bits, drop parity) */
static const int PC1[] = {
    57, 49, 41, 33, 25, 17,  9,  1, 58, 50, 42, 34, 26, 18,
    10,  2, 59, 51, 43, 35, 27, 19, 11,  3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,  7, 62, 54, 46, 38, 30, 22,
    14,  6, 61, 53, 45, 37, 29, 21, 13,  5, 28, 20, 12,  4
};

/* PC2: 56 -> 48 bits per round key */
static const int PC2[] = {
    14, 17, 11, 24,  1,  5,  3, 28, 15,  6, 21, 10,
    23, 19, 12,  4, 26,  8, 16,  7, 27, 20, 13,  2,
    41, 52, 31, 37, 47, 55, 30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32
};

/* Left shift schedule per round (1 or 2 bits) */
static const int SHIFTS[] = { 1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1 };

/* 8 S-Boxes: each 6 bits in -> 4 bits out. Stored as [box][row][col] row = outer 2 bits, col = inner 4 */
static const int S[8][4][16] = {
    /* S1 */
    { { 14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7 },
      {  0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8 },
      {  4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0 },
      { 15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13 } },
    /* S2 */
    { { 15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10 },
      {  3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5 },
      {  0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15 },
      { 13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9 } },
    /* S3 */
    { { 10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8 },
      { 13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1 },
      { 13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7 },
      {  1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12 } },
    /* S4 */
    { {  7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15 },
      { 13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9 },
      { 10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4 },
      {  3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14 } },
    /* S5 */
    { {  2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9 },
      { 14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6 },
      {  4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14 },
      { 11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3 } },
    /* S6 */
    { { 12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11 },
      { 10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8 },
      {  9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6 },
      {  4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13 } },
    /* S7 */
    { {  4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1 },
      { 13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6 },
      {  1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2 },
      {  6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12 } },
    /* S8 */
    { { 13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7 },
      {  1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2 },
      {  7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8 },
      {  2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11 } }
};

/* Helper: get bit at position pos (1-based) from block of 8 bytes (64 bits) */
static int get_bit(const unsigned char *block, int pos) {
    int byte_idx = (pos - 1) / 8;
    int bit_idx  = 7 - (pos - 1) % 8;
    return (block[byte_idx] >> bit_idx) & 1;
}

/* Helper: set bit at position pos (1-based) in block */
static void set_bit(unsigned char *block, int pos, int val) {
    int byte_idx = (pos - 1) / 8;
    int bit_idx  = 7 - (pos - 1) % 8;
    if (val)
        block[byte_idx] |=  (1u << bit_idx);
    else
        block[byte_idx] &= ~(1u << bit_idx);
}

/* Permute: apply table T of length len (positions 1-based) from src to dst */
static void permute(const unsigned char *src, unsigned char *dst, const int *T, int len) {
    int i;
    for (i = 0; i < len; i++)
        set_bit(dst, i + 1, get_bit(src, T[i]));
}

/* Left rotate 28-bit half (stored in 4 bytes, high bits unused) by n */
static void rotate_left_28(unsigned char *half, int n) {
    unsigned int val = (half[0] << 20) | (half[1] << 12) | (half[2] << 4) | (half[3] >> 4);
    val = ((val << n) | (val >> (28 - n))) & 0x0FFFFFFF;
    half[0] = (val >> 20) & 0xFF;
    half[1] = (val >> 12) & 0xFF;
    half[2] = (val >>  4) & 0xFF;
    half[3] = (val <<  4) & 0xF0;
}

/* Generate 16 round keys (48 bits each) from 64-bit key. Round key r in keys[r] */
static void key_schedule(const unsigned char *key, unsigned char keys[16][6]) {
    unsigned char pc1_out[7];   /* 56 bits */
    unsigned char c[4], d[4];  /* 28 bits each, c = left, d = right */
    int r;

    permute(key, pc1_out, PC1, 56);

    /* Split into C0 and D0 */
    c[0] = pc1_out[0]; c[1] = pc1_out[1]; c[2] = pc1_out[2]; c[3] = pc1_out[3] & 0xF0;
    d[0] = (pc1_out[3] << 4) | (pc1_out[4] >> 4);
    d[1] = (pc1_out[4] << 4) | (pc1_out[5] >> 4);
    d[2] = (pc1_out[5] << 4) | (pc1_out[6] >> 4);
    d[3] = pc1_out[6] << 4;

    for (r = 0; r < 16; r++) {
        rotate_left_28(c, SHIFTS[r]);
        rotate_left_28(d, SHIFTS[r]);
        /* Concatenate C and D into 56 bits in temp buffer then PC2 */
        unsigned char cd[7];
        cd[0] = c[0]; cd[1] = c[1]; cd[2] = c[2];
        cd[3] = (c[3] & 0xF0) | (d[0] >> 4);
        cd[4] = (d[0] << 4) | (d[1] >> 4);
        cd[5] = (d[1] << 4) | (d[2] >> 4);
        cd[6] = (d[2] << 4) | (d[3] >> 4);
        permute(cd, keys[r], PC2, 48);
    }
}

/* XOR two blocks (byte-wise), len in bytes */
static void xor_block(unsigned char *a, const unsigned char *b, int len) {
    int i;
    for (i = 0; i < len; i++) a[i] ^= b[i];
}

/* f function: R is 32 bits (4 bytes), round_key is 48 bits (6 bytes), output 32 bits */
static void f(const unsigned char *R, const unsigned char *round_key, unsigned char *out) {
    unsigned char expanded[6];
    unsigned char after_s[4];  /* 32 bits from 8 S-boxes */
    int b, i;

    permute(R, expanded, E, 48);
    xor_block(expanded, round_key, 6);

    /* Each S-box: 6 bits in -> 4 bits out. Bits 0-5, 6-11, ... 42-47 */
    for (b = 0; b < 8; b++) {
        int six = 0;
        for (i = 0; i < 6; i++)
            six = (six << 1) | get_bit(expanded, b * 6 + i + 1);
        int row = ((six >> 5) << 1) | (six & 1);
        int col = (six >> 1) & 0xF;
        int four = S[b][row][col];
        for (i = 0; i < 4; i++)
            set_bit(after_s, b * 4 + i + 1, (four >> (3 - i)) & 1);
    }
    permute(after_s, out, P, 32);
}

/* One round: L and R are 32 bits each (4 bytes). Round key 48 bits. */
static void round_fn(unsigned char *L, unsigned char *R, const unsigned char *round_key) {
    unsigned char f_out[4];
    unsigned char new_L[4];
    memcpy(new_L, R, 4);
    f(R, round_key, f_out);
    xor_block(f_out, L, 4);
    memcpy(L, new_L, 4);
    memcpy(R, f_out, 4);
}

/* DES encrypt one 64-bit block. key is 8 bytes, block in/out 8 bytes */
void des_encrypt(const unsigned char *key, unsigned char *block) {
    unsigned char keys[16][6];
    unsigned char ip_out[8];
    unsigned char L[4], R[4];
    int r;

    key_schedule(key, keys);
    permute(block, ip_out, IP, 64);

    memcpy(L, ip_out, 4);
    memcpy(R, ip_out + 4, 4);

    for (r = 0; r < 16; r++)
        round_fn(L, R, keys[r]);

    /* After 16 rounds, swap L and R then apply FP. (Round outputs L16,R16; we have L=R16, R=L16 in our vars) */
    unsigned char pre_fp[8];
    memcpy(pre_fp, R, 4);
    memcpy(pre_fp + 4, L, 4);
    permute(pre_fp, block, FP, 64);
}

/* DES decrypt: same as encrypt but round keys used in reverse order */
void des_decrypt(const unsigned char *key, unsigned char *block) {
    unsigned char keys[16][6];
    unsigned char ip_out[8];
    unsigned char L[4], R[4];
    int r;

    key_schedule(key, keys);
    permute(block, ip_out, IP, 64);

    memcpy(L, ip_out, 4);
    memcpy(R, ip_out + 4, 4);

    for (r = 15; r >= 0; r--)
        round_fn(L, R, keys[r]);

    unsigned char pre_fp[8];
    memcpy(pre_fp, R, 4);
    memcpy(pre_fp + 4, L, 4);
    permute(pre_fp, block, FP, 64);
}

/* Print hex block */
static void print_block(const char *label, const unsigned char *b, int len) {
    int i;
    printf("%s: ", label);
    for (i = 0; i < len; i++) printf("%02X ", b[i]);
    printf("\n");
}

int main(void) {
    /* Example: 64-bit block and 64-bit key */
    unsigned char key[8]   = { 0x13, 0x34, 0x57, 0x79, 0x9B, 0xBC, 0xDF, 0xF1 };
    unsigned char block[8] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };

    printf("=== DES Example ===\n");
    print_block("Key   ", key, 8);
    print_block("Plain ", block, 8);

    des_encrypt(key, block);
    print_block("Cipher", block, 8);

    des_decrypt(key, block);
    print_block("Decrypt", block, 8);

    return 0;
}
