#include <iostream>
using namespace std;

int mul2(int x) {
    return (2 * x) % 256;
}

int mul4(int x) {
    return (mul2(mul2(x))) % 256;
}

int mul8(int x) {
    return (mul2(mul4(x))) % 256;
}

int mul9(int x) {
    return (mul8(x) + x) % 256;
}

int mul11(int x) {
    return (mul8(x) + mul2(x) + x) % 256;
}

int mul13(int x) {
    return (mul8(x) + mul4(x) + x) % 256;
}

int mul14(int x) {
    return (mul8(x) + mul4(x) + mul2(x)) % 256;
}

void inverseMixColumn(int a[4][4]) {
    int temp[4][4];

    for (int j = 0; j < 4; j++) {
        temp[0][j] = (mul14(a[0][j]) + mul11(a[1][j]) + mul13(a[2][j]) + mul9(a[3][j])) % 256;
        temp[1][j] = (mul9(a[0][j]) + mul14(a[1][j]) + mul11(a[2][j]) + mul13(a[3][j])) % 256;
        temp[2][j] = (mul13(a[0][j]) + mul9(a[1][j]) + mul14(a[2][j]) + mul11(a[3][j])) % 256;
        temp[3][j] = (mul11(a[0][j]) + mul13(a[1][j]) + mul9(a[2][j]) + mul14(a[3][j])) % 256;
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

    inverseMixColumn(a);

    cout << "\nAfter Inverse MixColumns:\n";
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++)
            cout << a[i][j] << " ";
        cout << endl;
    }
}