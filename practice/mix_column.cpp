#include <iostream>
using namespace std;

#define MOD 0x1B

unsigned char xtime(unsigned char s)
{
    if(s & 0x80)
    {
        return (s << 1) ^ MOD;
    }
    return s << 1;
}

unsigned char gf_mul(unsigned char a, unsigned char b)
{
    unsigned char result = 0;

    while(b > 0)
    {
        if(b & 1)
        {
            result ^= a;
        }
        a = xtime(a);
        b >>= 1;
    }

    return result;
}

void mix_column(unsigned char state[4][4])
{
    for(int column = 0; column < 4; column++)
    {
        unsigned char s0 = state[0][column];
        unsigned char s1 = state[1][column];
        unsigned char s2 = state[2][column];
        unsigned char s3 = state[3][column];

        state[0][column] = xtime(s0) ^ (xtime(s1) ^ s1) ^ s2  ^ s3;
        state[1][column] = s0 ^ xtime(s1) ^ (xtime(s2) ^ s2) ^ s3;
        state[2][column] = s0 ^ s1 ^ xtime(s2) ^ (xtime(s3) ^ s3);
        state[3][column] = (xtime(s0) ^ s0) ^ s1 ^ s2 ^ xtime(s3);
    }
}

void inv_mix_column(unsigned char state[4][4])
{
    for(int column = 0; column < 4; column++)
    {
        unsigned char s0 = state[0][column];
        unsigned char s1 = state[1][column];
        unsigned char s2 = state[2][column];
        unsigned char s3 = state[3][column];

        state[0][column] = gf_mul(0x0E, s0) ^ gf_mul(0x0B, s1) ^ gf_mul(0x0D, s2) ^ gf_mul(0x09, s3);
        state[1][column] = gf_mul(0x09, s0) ^ gf_mul(0x0E, s1) ^ gf_mul(0x0B, s2) ^ gf_mul(0x0D, s3);
        state[2][column] = gf_mul(0x0D, s0) ^ gf_mul(0x09, s1) ^ gf_mul(0x0E, s2) ^ gf_mul(0x0B, s3);
        state[3][column] = gf_mul(0x0B, s0) ^ gf_mul(0x0D, s1) ^ gf_mul(0x09, s2) ^ gf_mul(0x0E, s3);
    }
}

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
    mix_column(state);
    print_state("After MixColumns", state);
    inv_mix_column(state);
    print_state("After Inverse MixColumns", state);

    return 0;
}