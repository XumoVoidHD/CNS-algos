#include <iostream>
using namespace std;

string encrypt(string text, int key)
{
    string result = "";
    for(int i = 0; i < text.length(); i++)
    {
        char ch = text[i];
        if(ch >= 'A' && ch <= 'Z')
        {
            result += char( (ch - 'A' + key) % 26 + 'A');
        }
        else if(ch >= 'a' && ch <= 'z')
        {
            result += char( (ch - 'a' + key) % 26 + 'a');
        }
        else
        {
            result += ch;
        }
    }

    return result;
}

string decrypt(string text, int key)
{
    return encrypt(text, 26-key);
}

int main()
{
    string text = "HELLO";
    int key = 3;

    string cipher = encrypt(text, key);
    cout << "Encrypted: " << cipher << endl;

    cout << "Decrypted: " << decrypt(cipher, key) << endl;

    return 0;
}