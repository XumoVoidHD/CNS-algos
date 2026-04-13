#include <iostream>
using namespace std;

string prepareKey(string key) {
    string result = "";
    bool used[26] = {false};

    for (int i = 0; i < key.length(); i++) {
        char c = key[i];
        if (c == 'J') c = 'I';

        if (!used[c - 'A']) {
            result += c;
            used[c - 'A'] = true;
        }
    }

    for (char c = 'A'; c <= 'Z'; c++) {
        if (c == 'J') continue;
        if (!used[c - 'A']) {
            result += c;
            used[c - 'A'] = true;
        }
    }

    return result;
}

void createMatrix(string key, char mat[5][5]) {
    key = prepareKey(key);
    int k = 0;

    for (int i = 0; i < 5; i++)
        for (int j = 0; j < 5; j++)
            mat[i][j] = key[k++];
}

void findPos(char mat[5][5], char ch, int &row, int &col) {
    if (ch == 'J') ch = 'I';

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (mat[i][j] == ch) {
                row = i;
                col = j;
                return;
            }
        }
    }
}

string prepareText(string text) {
    string res = "";

    for (int i = 0; i < text.length(); i++) {
        char a = text[i];
        char b;

        if (i + 1 < text.length()) {
            b = text[i + 1];
        } else {
            b = 'X';
        }

        if (a == b) {
            res += a;
            res += 'X';
        } else {
            res += a;
            res += b;
            i++;
        }
    }

    if (res.length() % 2 != 0)
        res += 'X';

    return res;
}

string encrypt(string text, char mat[5][5]) {
    string result = "";

    for (int i = 0; i < text.length(); i += 2) {
        char a = text[i];
        char b = text[i + 1];

        int r1, c1, r2, c2;
        findPos(mat, a, r1, c1);
        findPos(mat, b, r2, c2);

        if (r1 == r2) {
            result += mat[r1][(c1 + 1) % 5];
            result += mat[r2][(c2 + 1) % 5];
        }
        else if (c1 == c2) {
            result += mat[(r1 + 1) % 5][c1];
            result += mat[(r2 + 1) % 5][c2];
        }
        else {
            result += mat[r1][c2];
            result += mat[r2][c1];
        }
    }

    return result;
}

string decrypt(string text, char mat[5][5]) {
    string result = "";

    for (int i = 0; i < text.length(); i += 2) {
        char a = text[i];
        char b = text[i + 1];

        int r1, c1, r2, c2;
        findPos(mat, a, r1, c1);
        findPos(mat, b, r2, c2);

        if (r1 == r2) {
            result += mat[r1][(c1 + 4) % 5]; // LEFT
            result += mat[r2][(c2 + 4) % 5];
        }
        else if (c1 == c2) {
            result += mat[(r1 + 4) % 5][c1]; // UP
            result += mat[(r2 + 4) % 5][c2];
        }
        else {
            result += mat[r1][c2];
            result += mat[r2][c1];
        }
    }

    return result;
}

int main() {
    string key = "MONARCHY";
    string text = "BALLOON";

    char mat[5][5];
    createMatrix(key, mat);

    string prepared = prepareText(text);
    string encrypted = encrypt(prepared, mat);
    string decrypted = decrypt(encrypted, mat);

    cout << "Matrix:\n";
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++)
            cout << mat[i][j] << " ";
        cout << endl;
    }

    cout << "\nPrepared Text: " << prepared << endl;
    cout << "Encrypted: " << encrypted << endl;
    cout << "Decrypted: " << decrypted << endl;

    return 0;
}