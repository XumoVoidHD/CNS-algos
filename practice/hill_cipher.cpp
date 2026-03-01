#include <iostream>
using namespace std;

int key[2][2] = {{3,3},{2,5}};

/* -------- FIND MODULAR INVERSE -------- */
int modInverse(int num)
{
    num = (num % 26 + 26) % 26;

    for(int i = 1; i < 26; i++)
    {
        if((num * i) % 26 == 1)
            return i;
    }
    return -1;
}

/* -------- ENCRYPT -------- */
string encrypt(string text)
{
    string result = "";

    for(int i = 0; i < text.length(); i += 2)
    {
        int a = text[i] - 'A';
        int b = text[i+1] - 'A';

        int c1 = (key[0][0]*a + key[0][1]*b) % 26;
        int c2 = (key[1][0]*a + key[1][1]*b) % 26;

        result += char(c1 + 'A');
        result += char(c2 + 'A');
    }

    return result;
}

/* -------- DECRYPT -------- */
string decrypt(string text)
{
    string result = "";

    // Step 1: determinant
    int det = key[0][0]*key[1][1] - key[0][1]*key[1][0];
    det = (det % 26 + 26) % 26;

    int det_inv = modInverse(det);

    // Step 2: inverse matrix
    int inv[2][2];

    inv[0][0] =  key[1][1] * det_inv % 26;
    inv[0][1] = (-key[0][1] * det_inv) % 26;
    inv[1][0] = (-key[1][0] * det_inv) % 26;
    inv[1][1] =  key[0][0] * det_inv % 26;

    // Fix negatives
    for(int i=0;i<2;i++)
        for(int j=0;j<2;j++)
            inv[i][j] = (inv[i][j] % 26 + 26) % 26;

    // Step 3: decrypt
    for(int i = 0; i < text.length(); i += 2)
    {
        int a = text[i] - 'A';
        int b = text[i+1] - 'A';

        int p1 = (inv[0][0]*a + inv[0][1]*b) % 26;
        int p2 = (inv[1][0]*a + inv[1][1]*b) % 26;

        result += char(p1 + 'A');
        result += char(p2 + 'A');
    }

    return result;
}

/* -------- MAIN -------- */
int main()
{
    string text = "HI";

    string cipher = encrypt(text);
    cout << "Encrypted: " << cipher << endl;

    cout << "Decrypted: " << decrypt(cipher) << endl;

    return 0;
}