#include <iostream>
using namespace std;

/* ------------------ TABLES ------------------ */

// Initial Permutation
int IP[64] = {
58,50,42,34,26,18,10,2,
60,52,44,36,28,20,12,4,
62,54,46,38,30,22,14,6,
64,56,48,40,32,24,16,8,
57,49,41,33,25,17,9,1,
59,51,43,35,27,19,11,3,
61,53,45,37,29,21,13,5,
63,55,47,39,31,23,15,7
};

// Final Permutation
int IP_INV[64] = {
40,8,48,16,56,24,64,32,
39,7,47,15,55,23,63,31,
38,6,46,14,54,22,62,30,
37,5,45,13,53,21,61,29,
36,4,44,12,52,20,60,28,
35,3,43,11,51,19,59,27,
34,2,42,10,50,18,58,26,
33,1,41,9,49,17,57,25
};

// Expansion
int E[48] = {
32,1,2,3,4,5,4,5,6,7,8,9,
8,9,10,11,12,13,12,13,14,15,16,17,
16,17,18,19,20,21,20,21,22,23,24,25,
24,25,26,27,28,29,28,29,30,31,32,1
};

// P-box
int P[32] = {
16,7,20,21,29,12,28,17,
1,15,23,26,5,18,31,10,
2,8,24,14,32,27,3,9,
19,13,30,6,22,11,4,25
};

// PC1
int PC1[56] = {
57,49,41,33,25,17,9,
1,58,50,42,34,26,18,
10,2,59,51,43,35,27,
19,11,3,60,52,44,36,
63,55,47,39,31,23,15,
7,62,54,46,38,30,22,
14,6,61,53,45,37,29,
21,13,5,28,20,12,4
};

// PC2
int PC2[48] = {
14,17,11,24,1,5,3,28,
15,6,21,10,23,19,12,4,
26,8,16,7,27,20,13,2,
41,52,31,37,47,55,30,40,
51,45,33,48,44,49,39,56,
34,53,46,42,50,36,29,32
};

// Shifts
int shiftBits[16] = {1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1};

/* ------------------ BASIC FUNCTIONS ------------------ */

string permute(string input, int table[], int size)
{
    string output = "";
    for(int i=0;i<size;i++)
        output += input[table[i]-1];
    return output;
}

string XOR(string a, string b)
{
    string result = "";
    for(int i=0;i<a.length();i++)
    {
        if(a[i]==b[i])
            result+="0";
        else
            result+="1";
    }
    return result;
}

string leftShift(string input, int shifts)
{
    while(shifts > 0)
    {
        input = input.substr(1) + input[0];
        shifts--;
    }
    return input;
}

/* ------------------ KEY GENERATION ------------------ */

void generateKeys(string key, string roundKeys[16])
{
    key = permute(key, PC1, 56);

    string left = key.substr(0,28);
    string right = key.substr(28,28);

    for(int i=0;i<16;i++)
    {
        left = leftShift(left, shiftBits[i]);
        right = leftShift(right, shiftBits[i]);

        roundKeys[i] = permute(left+right, PC2, 48);
    }
}

/* ------------------ SIMPLE S-BOX (ONLY 1 USED FOR DEMO) ------------------ */

int S[4][16] = {
{14,4,13,1,2,15,11,8,3,10,6,12,5,9,0,7},
{0,15,7,4,14,2,13,1,10,6,12,11,9,5,3,8},
{4,1,14,8,13,6,2,11,15,12,9,7,3,10,5,0},
{15,12,8,2,4,9,1,7,5,11,3,14,10,0,6,13}
};

string sbox(string input)
{
    string output = "";

    // There are 8 blocks of 6 bits in 48-bit input
    for(int i = 0; i < 8; i++)
    {
        // Take 6-bit block
        string block = input.substr(i * 6, 6);

        /* ----------- FIND ROW ----------- */
        int row;

        if(block[0] == '0' && block[5] == '0')
            row = 0;
        else if(block[0] == '0' && block[5] == '1')
            row = 1;
        else if(block[0] == '1' && block[5] == '0')
            row = 2;
        else
            row = 3;

        /* ----------- FIND COLUMN ----------- */
        int col = 0;

        if(block[1] == '1') col = col + 8;
        if(block[2] == '1') col = col + 4;
        if(block[3] == '1') col = col + 2;
        if(block[4] == '1') col = col + 1;

        /* ----------- GET VALUE FROM S-BOX ----------- */
        int value = S[row][col];

        /* ----------- CONVERT TO 4-BIT BINARY ----------- */
        if(value == 0) output += "0000";
        else if(value == 1) output += "0001";
        else if(value == 2) output += "0010";
        else if(value == 3) output += "0011";
        else if(value == 4) output += "0100";
        else if(value == 5) output += "0101";
        else if(value == 6) output += "0110";
        else if(value == 7) output += "0111";
        else if(value == 8) output += "1000";
        else if(value == 9) output += "1001";
        else if(value == 10) output += "1010";
        else if(value == 11) output += "1011";
        else if(value == 12) output += "1100";
        else if(value == 13) output += "1101";
        else if(value == 14) output += "1110";
        else output += "1111";
    }

    return output;
}

/* ------------------ DES FUNCTION ------------------ */

string DES(string text, string roundKeys[16])
{
    text = permute(text, IP, 64);

    string left = text.substr(0,32);
    string right = text.substr(32,32);

    for(int i=0;i<16;i++)
    {
        string expanded = permute(right, E, 48);
        string xored = XOR(expanded, roundKeys[i]);
        string sboxOut = sbox(xored);
        string pbox = permute(sboxOut, P, 32);

        string newRight = XOR(left, pbox);
        left = right;
        right = newRight;
    }

    string combined = right + left;

    return permute(combined, IP_INV, 64);
}

/* ------------------ MAIN ------------------ */

int main()
{
    string plaintext =
    "0001001000110100010101100111100010011010101111001101111011110001";

    string key =
    "0001001100110100010101110111100110011011101111001101111111110001";

    string roundKeys[16];

    generateKeys(key, roundKeys);

    // Encryption
    string cipher = DES(plaintext, roundKeys);
    cout << "Ciphertext: " << cipher << endl;

    // Reverse keys for decryption
    string reverseKeys[16];
    for(int i=0;i<16;i++)
        reverseKeys[i] = roundKeys[15-i];

    string decrypted = DES(cipher, reverseKeys);
    cout << "Decrypted: " << decrypted << endl;

    return 0;
}