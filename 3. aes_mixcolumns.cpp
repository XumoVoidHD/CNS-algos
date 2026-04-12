#include <iostream>
using namespace std;

int mul2(int x) {
    int res = x * 2;
    if (res >= 256)
        res = res % 256;
    return res;
}

int mul3(int x) {
    return (mul2(x) + x) % 256;
}

void mixColumn(int a[4][4]) {
    int temp[4][4];

    for (int j = 0; j < 4; j++) {
        temp[0][j] = (mul2(a[0][j]) + mul3(a[1][j]) + a[2][j] + a[3][j]) % 256;
        temp[1][j] = (a[0][j] + mul2(a[1][j]) + mul3(a[2][j]) + a[3][j]) % 256;
        temp[2][j] = (a[0][j] + a[1][j] + mul2(a[2][j]) + mul3(a[3][j])) % 256;
        temp[3][j] = (mul3(a[0][j]) + a[1][j] + a[2][j] + mul2(a[3][j])) % 256;
    }

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            a[i][j] = temp[i][j];
}

int main() {
    int a[4][4];

    cout << "Enter 4x4 matrix:\n";
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            cin >> a[i][j];

    mixColumn(a);

    cout << "\nAfter MixColumns:\n";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++)
            cout << a[i][j] << " ";
        cout << endl;
    }
}