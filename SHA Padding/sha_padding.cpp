#include <iostream>
#include <vector>
using namespace std;

int main() {
    string msg = "abc";
    vector<unsigned char> data(msg.begin(), msg.end());

    long long original_len = data.size() * 8;

    // Step 1: append 0x80
    data.push_back(0x80);

    // Step 2: pad with zeros
    while ((data.size() % 64) != 56) {
        data.push_back(0x00);
    }

    // Step 3: append length (BIG-ENDIAN)
    for (int i = 7; i >= 0; i--) {
        data.push_back((original_len >> (8 * i)) & 0xFF);
    }

    // Print hex
    cout << "Padded message:\n";
    for (auto x : data)
        printf("%02X ", x);

    return 0;
}