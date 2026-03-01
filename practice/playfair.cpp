#include <iostream>
using namespace std;

char matrix[5][5];

void createMatrix(string key)
{
    string alphabet = "ABCDEFGHIKLMNOPQRSTUVWXYZ";
    string used = "";

    int k = 0;

    for(int i = 0; i < key.length(); i++)
    {
        char ch = toupper(key[i]);

        if(ch == 'J') ch = 'I';

        if(used.find(ch) == string::npos)
        {
            matrix[k/5][k%5] = ch;
            used += ch;
            k++;
        }
    }

    for(int i = 0; i < alphabet.length(); i++)
    {
        if(used.find(alphabet[i]) == string::npos)
        {
            matrix[k/5][k%5] = alphabet[i];
            k++;
        }
    }
}

void findPosition(char ch, int &row, int &col)
{
    for(int i=0;i<5;i++)
        for(int j=0;j<5;j++)
            if(matrix[i][j]==ch)
            {
                row=i;
                col=j;
            }
}

string encrypt(string text)
{
    string result = "";

    for(int i = 0; i < text.size(); i+=2)
    {
        char a = text[i];
        char b = text[i+1];

        int r1, r2, c1, c2;

        findPosition(a, r1, c1);
        findPosition(b, r2, c2);

        if(r1 == r2)
        {
            result += matrix[r1][(c1 + 1) % 5];
            result += matrix[r2][(c2 + 1) % 5];
        }
        else if (c1 == c2)
        {
            result += matrix[(r1 + 1) % 5][c1];
            result += matrix[(r2 + 1) % 5][c2];
        }
        else
        {
            result += matrix[r1][c2];
            result += matrix[r2][c1];
        }
    }

    return result;
}

string decrypt(string text)
{
    string result="";

    for(int i=0;i<text.length();i+=2)
    {
        char a = text[i];
        char b = text[i+1];

        int r1,c1,r2,c2;
        findPosition(a,r1,c1);
        findPosition(b,r2,c2);

        // Same row → shift LEFT
        if(r1==r2)
        {
            result+=matrix[r1][(c1+4)%5];
            result+=matrix[r2][(c2+4)%5];
        }
        // Same column → shift UP
        else if(c1==c2)
        {
            result+=matrix[(r1+4)%5][c1];
            result+=matrix[(r2+4)%5][c2];
        }
        // Rectangle → swap columns
        else
        {
            result+=matrix[r1][c2];
            result+=matrix[r2][c1];
        }
    }

    return result;
}

int main()
{
    string key="MONARCHY";
    string text="INSTRUMENTS";

    createMatrix(key);

    cout<<"Encrypted: "<<encrypt(text)<<endl;
    cout <<"Decrypted: " << decrypt(encrypt(text)) << endl;

    return 0;
}