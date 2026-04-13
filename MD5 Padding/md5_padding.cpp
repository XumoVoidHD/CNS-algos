#include <iostream>
#include <iomanip>
#include <vector>
using namespace std;

int main() {
    string message = "abc";
    vector<unsigned char> data(message.begin(), message.end());
    unsigned long long originalLength = data.size() * 8ULL;

    data.push_back(0x80);

    while (data.size() % 64 != 56) {
        data.push_back(0x00);
    }

    for (int i = 0; i < 8; i++) {
        data.push_back((originalLength >> (8 * i)) & 0xFF);
    }

    cout << "Padded message:\n";
    for (unsigned char byte : data) {
        cout << uppercase << hex << setw(2) << setfill('0') << (int)byte << " ";
    }
    cout << "\n";
    return 0;
}
