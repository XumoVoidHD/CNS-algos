/*
 * AES MixColumns and Inverse MixColumns - Exam-ready implementation
 * Galois Field GF(2^8) with irreducible polynomial m(x) = x^8 + x^4 + x^3 + x + 1 (0x11B)
 * NO external libraries. Manual xtime and matrix multiplication.
 */

#include <stdio.h>

/* GF(2^8) modulus: 0x11B */
#define MOD 0x11B

/* Multiply by 2 in GF(2^8): left shift, then reduce if bit 8 set */
static unsigned char xtime(unsigned char a) {
    unsigned char high = (a >> 7) & 1;
    return (a << 1) ^ (high * MOD);
}

/* Multiply two bytes in GF(2^8) using xtime (peasant's method) */
static unsigned char gf_mul(unsigned char a, unsigned char b) {
    unsigned char p = 0;
    unsigned char hi;
    while (b) {
        if (b & 1) p ^= a;
        hi = (a >> 7) & 1;
        a = (a << 1) ^ (hi * MOD);
        b >>= 1;
    }
    return p;
}

/*
 * AES MixColumns matrix (forward):
 *   02 03 01 01
 *   01 02 03 01
 *   01 01 02 03
 *   03 01 01 02
 * Column c of state is multiplied by this matrix (column vector).
 */
static void mix_columns(unsigned char state[4][4]) {
    unsigned char col[4], out[4];
    int c, i;

    for (c = 0; c < 4; c++) {
        for (i = 0; i < 4; i++) col[i] = state[i][c];

        out[0] = xtime(col[0]) ^ xtime(col[1]) ^ col[1] ^ col[2] ^ col[3];  /* 02 03 01 01 */
        out[1] = col[0] ^ xtime(col[1]) ^ xtime(col[2]) ^ col[2] ^ col[3];  /* 01 02 03 01 */
        out[2] = col[0] ^ col[1] ^ xtime(col[2]) ^ xtime(col[3]) ^ col[3];  /* 01 01 02 03 */
        out[3] = xtime(col[0]) ^ col[0] ^ col[1] ^ col[2] ^ xtime(col[3]);  /* 03 01 01 02 */

        for (i = 0; i < 4; i++) state[i][c] = out[i];
    }
}

/*
 * AES Inverse MixColumns matrix:
 *   0E 0B 0D 09
 *   09 0E 0B 0D
 *   0D 09 0E 0B
 *   0B 0D 09 0E
 */
static void inv_mix_columns(unsigned char state[4][4]) {
    unsigned char col[4], out[4];
    int c, i;

    for (c = 0; c < 4; c++) {
        for (i = 0; i < 4; i++) col[i] = state[i][c];

        out[0] = gf_mul(0x0E, col[0]) ^ gf_mul(0x0B, col[1]) ^ gf_mul(0x0D, col[2]) ^ gf_mul(0x09, col[3]);
        out[1] = gf_mul(0x09, col[0]) ^ gf_mul(0x0E, col[1]) ^ gf_mul(0x0B, col[2]) ^ gf_mul(0x0D, col[3]);
        out[2] = gf_mul(0x0D, col[0]) ^ gf_mul(0x09, col[1]) ^ gf_mul(0x0E, col[2]) ^ gf_mul(0x0B, col[3]);
        out[3] = gf_mul(0x0B, col[0]) ^ gf_mul(0x0D, col[1]) ^ gf_mul(0x09, col[2]) ^ gf_mul(0x0E, col[3]);

        for (i = 0; i < 4; i++) state[i][c] = out[i];
    }
}

static void print_state(const char *label, unsigned char s[4][4]) {
    int i, j;
    printf("%s:\n", label);
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) printf("%02X ", s[i][j]);
        printf("\n");
    }
}

int main(void) {
    /* Example 4x4 state (column-major as in AES: column 0 = first 4 bytes) */
    unsigned char state[4][4] = {
        { 0x63, 0x53, 0xE0, 0x8C },
        { 0x7C, 0xC5, 0x82, 0x7D },
        { 0x77, 0xD2, 0xF2, 0x6B },
        { 0xF2, 0x30, 0x01, 0xA5 }
    };
    unsigned char state_copy[4][4];

    printf("=== AES MixColumns / Inverse MixColumns ===\n\n");
    print_state("Original state", state);

    /* MixColumns */
    mix_columns(state);
    print_state("After MixColumns", state);

    /* Copy and apply Inverse MixColumns to get back */
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            state_copy[i][j] = state[i][j];
    inv_mix_columns(state_copy);
    print_state("After InvMixColumns (should match original)", state_copy);

    /* Verify: apply Inv to original MixColumns output -> original */
    printf("\n--- Verify: InvMixColumns(MixColumns(original)) ---\n");
    unsigned char s2[4][4] = {
        { 0x63, 0x53, 0xE0, 0x8C },
        { 0x7C, 0xC5, 0x82, 0x7D },
        { 0x77, 0xD2, 0xF2, 0x6B },
        { 0xF2, 0x30, 0x01, 0xA5 }
    };
    mix_columns(s2);
    inv_mix_columns(s2);
    print_state("Result", s2);

    return 0;
}
