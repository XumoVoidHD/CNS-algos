#include <stdio.h>

#define MOD 0x1B // Only 0x1B needed after shift (AES reduction)

// ==========================================================
// Multiply by 2 in GF(2^8)
// ==========================================================
unsigned char xtime(unsigned char a)
{
    // Check if MSB is 1
    if (a & 0x80)
        return (a << 1) ^ MOD;   // Reduce using polynomial
    else
        return (a << 1);
}

// ==========================================================
// General GF multiplication
// ==========================================================
unsigned char gf_mul(unsigned char a, unsigned char b)
{
    unsigned char result = 0;

    while (b > 0)
    {
        if (b & 1)           // If LSB of b is 1
            result ^= a;     // Add current a

        a = xtime(a);        // Multiply a by 2
        b >>= 1;             // Divide b by 2
    }

    return result;
}

// ==========================================================
// MIX COLUMNS (Forward)
// ==========================================================
void mix_columns(unsigned char state[4][4])
{
    int column;

    // LOOP 1 → Process each column separately
    for (column = 0; column < 4; column++)
    {
        // Step 1: Copy current column
        unsigned char s0 = state[0][column];
        unsigned char s1 = state[1][column];
        unsigned char s2 = state[2][column];
        unsigned char s3 = state[3][column];

        // Step 2: Apply AES MixColumns matrix

        state[0][column] =
            xtime(s0) ^ (xtime(s1) ^ s1) ^ s2 ^ s3;

        state[1][column] =
            s0 ^ xtime(s1) ^ (xtime(s2) ^ s2) ^ s3;

        state[2][column] =
            s0 ^ s1 ^ xtime(s2) ^ (xtime(s3) ^ s3);

        state[3][column] =
            (xtime(s0) ^ s0) ^ s1 ^ s2 ^ xtime(s3);
    }
}

// ==========================================================
// INVERSE MIX COLUMNS
// ==========================================================
void inv_mix_columns(unsigned char state[4][4])
{
    int column;

    // LOOP 2 → Process each column
    for (column = 0; column < 4; column++)
    {
        // Step 1: Copy column
        unsigned char s0 = state[0][column];
        unsigned char s1 = state[1][column];
        unsigned char s2 = state[2][column];
        unsigned char s3 = state[3][column];

        // Step 2: Apply Inverse matrix

        state[0][column] =
            gf_mul(0x0E, s0) ^ gf_mul(0x0B, s1) ^ gf_mul(0x0D, s2) ^ gf_mul(0x09, s3);

        state[1][column] =
            gf_mul(0x09, s0) ^ gf_mul(0x0E, s1) ^ gf_mul(0x0B, s2) ^ gf_mul(0x0D, s3);

        state[2][column] =
            gf_mul(0x0D, s0) ^ gf_mul(0x09, s1) ^ gf_mul(0x0E, s2) ^ gf_mul(0x0B, s3);

        state[3][column] =
            gf_mul(0x0B, s0) ^ gf_mul(0x0D, s1) ^ gf_mul(0x09, s2) ^ gf_mul(0x0E, s3);
    }
}

// ==========================================================
// PRINT FUNCTION
// ==========================================================
void print_state(char *label, unsigned char state[4][4])
{
    printf("\n%s:\n", label);

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < 4; col++)
            printf("%02X ", state[row][col]);
        printf("\n");
    }
}

// ==========================================================
// MAIN FUNCTION
// ==========================================================
int main()
{
    unsigned char state[4][4] =
    {
        {0x63, 0x53, 0xE0, 0x8C},
        {0x7C, 0xC5, 0x82, 0x7D},
        {0x77, 0xD2, 0xF2, 0x6B},
        {0xF2, 0x30, 0x01, 0xA5}
    };

    unsigned char original[4][4];

    // Copy original state
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            original[i][j] = state[i][j];

    print_state("Original State", state);

    // Apply MixColumns
    mix_columns(state);
    print_state("After MixColumns", state);

    // Apply Inverse MixColumns
    inv_mix_columns(state);
    print_state("After Inverse MixColumns", state);

    return 0;
}