#include <iostream>
using namespace std;

// ===== CONSTANTS =====
int P10[10]   = {3,5,2,7,4,10,1,9,8,6};
int P8[8]     = {6,3,7,4,8,5,10,9};
int IP[8]     = {2,6,3,1,4,8,5,7};
int IP_inv[8] = {4,1,3,5,7,2,8,6};
int EP[8]     = {4,1,2,3,2,3,4,1};
int P4[4]     = {2,4,3,1};

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

// ===== FUNCTIONS =====

string permute(string k, int arr[], int n) {
    string res = "";
    for (int i = 0; i < n; i++)
        res += k[arr[i] - 1];
    return res;
}

string shift(string k, int shifts) {
    return k.substr(shifts) + k.substr(0, shifts);
}

string XOR(string a, string b) {
    string res = "";
    for (int i = 0; i < a.size(); i++)
        res += (a[i] == b[i]) ? '0' : '1';
    return res;
}

string fk(string right, string key) {
    string temp = permute(right, EP, 8);
    temp = XOR(temp, key);

    string left = temp.substr(0,4);
    string right2 = temp.substr(4,4);

    int row = (left[0]-'0')*2 + (left[3]-'0');
    int col = (left[1]-'0')*2 + (left[2]-'0');
    int val1 = S0[row][col];

    row = (right2[0]-'0')*2 + (right2[3]-'0');
    col = (right2[1]-'0')*2 + (right2[2]-'0');
    int val2 = S1[row][col];

    string res = "";
    res += char(val1/2 + '0');
    res += char(val1%2 + '0');
    res += char(val2/2 + '0');
    res += char(val2%2 + '0');

    return permute(res, P4, 4);
}

string SDES(string text, string K1, string K2, bool encrypt) {
    string temp = permute(text, IP, 8);

    string left = temp.substr(0,4);
    string right = temp.substr(4,4);

    if (encrypt)
        left = XOR(left, fk(right, K1));
    else
        left = XOR(left, fk(right, K2));

    // Swap
    string swapped = right + left;
    left = swapped.substr(0,4);
    right = swapped.substr(4,4);

    if (encrypt)
        left = XOR(left, fk(right, K2));
    else
        left = XOR(left, fk(right, K1));

    return permute(left + right, IP_inv, 8);
}

// ===== MAIN =====

int main() {
    string key = "1010000010";
    string plaintext = "10101010";

    // Key Generation
    string temp = permute(key, P10, 10);

    string left = temp.substr(0,5);
    string right = temp.substr(5,5);

    left = shift(left, 1);
    right = shift(right, 1);

    string K1 = permute(left + right, P8, 8);

    left = shift(left, 2);
    right = shift(right, 2);

    string K2 = permute(left + right, P8, 8);

    // Encryption
    string cipher = SDES(plaintext, K1, K2, true);

    // Decryption
    string decrypted = SDES(cipher, K1, K2, false);

    cout << "K1: " << K1 << endl;
    cout << "K2: " << K2 << endl;
    cout << "Plaintext: " << plaintext << endl;
    cout << "Encrypted: " << cipher << endl;
    cout << "Decrypted: " << decrypted << endl;

    return 0;
}