#include <iostream>
using namespace std;

int P10[10] = {3,5,2,7,4,10,1,9,8,6};
int P8[8]   = {6,3,7,4,8,5,10,9};
int IP[8]   = {2,6,3,1,4,8,5,7};
int IP_INV[8] = {4,1,3,5,7,2,8,6};
int EP[8]   = {4,1,2,3,2,3,4,1};
int P4[4]   = {2,4,3,1};

int S0[4][4] = {
{1,0,3,2},
{3,2,1,0},
{0,2,1,3},
{3,1,3,2}
};

int S1[4][4] = {
{0,1,2,3},
{2,0,1,3},
{3,0,1,0},
{2,1,0,3}
};

string permute(string text, int table[], int size)
{
    string output = "";
    for(int i = 0; i < size; i++)
    {
        output += text[table[i]-1];
    }

    return output;
}

string leftShift(string input)
{
    return input.substr(1) + input[0];
}

void generateKeys(string key, string &K1, string &K2)
{
    key = permute(key, P10, 10);
    string left = key.substr(0, 5);
    string right = key.substr(5, 5);

    left = leftShift(left);
    right = leftShift(right);

    K1 = permute(left + right, P8, 8);

    left = leftShift(left);
    right = leftShift(right);

    K2 = permute(left + right, P8, 8);
}

string XOR(string input1, string input2)
{
    string result = "";
    for(int i=0;i<input1.length();i++)
    {
        if(input1[i] == input2[i])
            result += "0";
        else
            result += "1";
    }
    return result;
}

string sbox(string input, int table[4][4])
{
    int row, column;
    if(input[0] == '0' && input[3] == '0') row = 0;
    if(input[0] == '0' && input[3] == '1') row = 1;
    if(input[0] == '1' && input[3] == '0') row = 2;
    if(input[0] == '1' && input[3] == '1') row = 3;

    if(input[1] == '0' && input[2] == '0') column = 0;
    if(input[1] == '0' && input[2] == '1') column = 1;
    if(input[1] == '1' && input[2] == '0') column = 2;
    if(input[1] == '1' && input[2] == '1') column = 3;

    int temp = table[row][column];

    string ans = "";
    if(temp == 0)
    {
        ans = "00";
    }
    if(temp == 1)
    {
        ans = "01";
    }
    if(temp == 2)
    {
        ans = "10";
    }
    if(temp == 3)
    {
        ans = "11";
    }

    return ans;
}

string F(string input, string key)
{
    string expanded = permute(input, EP, 8);
    string xored = XOR(expanded, key);

    string leftPart = xored.substr(0, 4);
    string rightPart = xored.substr(4, 4);

    string s0_box = sbox(leftPart, S0);
    string s1_box = sbox(rightPart, S1);

    return permute(s0_box+s1_box, P4, 4);
}

string encrypt(string plaintext, string K1, string K2)
{
    plaintext = permute(plaintext, IP, 8);

    string left = plaintext.substr(0, 4);
    string right = plaintext.substr(4, 4);

    // Round 1
    string temp = F(right, K1);
    left = XOR(temp, left);

    string swap = right + left;
    left = swap.substr(0, 4);
    right = swap.substr(4, 4);

    // Round 2
    temp = F(right, K2);
    left = XOR(temp, left);

    string combined = left + right;

    return permute(combined, IP_INV, 8);
}

string decrypt(string ciphertext, string K1, string K2)
{
    ciphertext = permute(ciphertext, IP, 8);

    string left = ciphertext.substr(0, 4);
    string right = ciphertext.substr(4, 4);

    // Round 1 (use K2 first)
    string temp = F(right, K2);
    left = XOR(temp, left);

    string swap = right + left;
    left = swap.substr(0, 4);
    right = swap.substr(4, 4);

    // Round 2 (use K1)
    temp = F(right, K1);
    left = XOR(temp, left);

    string combined = left + right;

    return permute(combined, IP_INV, 8);
}

int main()
{
    string key = "1010000010";
    string plaintext = "11010111";

    string K1, K2;

    generateKeys(key, K1, K2);

    string cipher = encrypt(plaintext, K1, K2);

    cout << "Ciphertext: " << cipher;

    string text = decrypt(cipher, K1, K2);

    cout << "Text: " << text;

    return 0;
}