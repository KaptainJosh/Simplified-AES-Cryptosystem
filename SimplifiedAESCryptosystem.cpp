/*
    Author: KaptainJosh
    Description: This program implements a modified, simplified version of AES. It reads in plain text from an input file and a key from another file.
*/
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

using namespace std;

int rgfMul(int x, int y); // rgf multiplication function

int main()
{
    ifstream plaintext; // file pointer for plaintext file
    ifstream keyfile; // file pointer for key file
    ofstream ciphertext; // file pointer for ciphertext output file

    string pfilename; // plaintext file name
    string kfilename; // key file name
    string cfilename; // ciphertext file nam

    cout << "Enter the name of the input plaintext file: ";
    cin >> pfilename;

    cout << "Enter the name of the input key file: ";
    cin >> kfilename;

    cout << "Enter the name of the output ciphertext file: ";
    cin >> cfilename;

    // opening plaintext file
    plaintext.open(pfilename.c_str());
    if (!(plaintext.is_open()))
    {
        cout << "Input plaintext file could not be opened\n";
        exit(1);
    }

    // opening key file
    keyfile.open(kfilename.c_str());
    if (!(keyfile.is_open()))
    {
        cout << "Input key file could not be opened\n";
        exit(1);
    }

    // opening ciphertext file
    ciphertext.open(cfilename.c_str());
    if (!(ciphertext.is_open()))
    {
        cout << "Ciphertext output file could not be opened\n";
        exit(1);
    }

    string word; // an individual word read from the file
    string message; // the entire message from the input file
    
    // This while loop reads the whole text file into a string and removes whitespace/newlines
    while (plaintext >> word)
    {
        message.append(word);
    }

    // This for loop iterates over the string and removes punctuation for preprocessing
    for (int i = 0; i < message.size(); i++)
    {
        if (ispunct(message[i]))
        {
            message.erase(i--, 1);
        }
    }

    cout << "Preprocessing:\n" << message << endl << endl;
    ciphertext << "Preprocessing:\n" << message << endl << endl;
    
    string key; // holds the key for the Viginere cipher
    keyfile >> key;
    int counter = 0; // index for the key
    // This for loop encrypts the message using the Viginere cipher and given key
    for (int j = 0; j < message.size(); j++)
    {
        int tempKey = key[counter];// holds a character from the key
        tempKey -= 65;
        int tempMsg = message[j]; // holds a character from the message
        tempMsg -= 65;

        int cipherLetter = (tempMsg + tempKey) % 26; // holds the encrypted letter (0-25)
        cipherLetter += 65; // convert back to regular letter
        
        message[j] = cipherLetter;
        
        counter++;
	
	//if counter goes above the size of the key, start over
        if (counter >= key.size())
        {
            counter = 0;
        }
    }

    cout << "Subsitution:\n" << message << endl << endl;
    ciphertext << "Subsitution:\n" << message << endl << endl;

    // This section deals with adding any needed padding

    int remainder = message.size() % 16; // determines how much off a multiple of 16 the size of the string is
    
    if (remainder != 0)
    {
        int padding = 16 - remainder; // how many 'A's to add
        // Adding padding to message
        for (int l = 0; l < padding; l++)
        {
            message.append("A");
        }
    }

    cout << "Padding:\n";
    ciphertext << "Padding:\n";

    // Printing out the message in 4x4 blocks
    for (int k = 0; k < message.size(); k++)
    {
        if (k % 4 == 3)
        {
            cout << message[k] << "\n";
            ciphertext << message[k] << "\n";
        }

        else
        {
            cout << message[k];
            ciphertext << message[k];
        }

        if (k % 16 == 15)
        {
            cout << "\n";
            ciphertext << "\n";
        }
    }

    cout << "ShiftRows:\n";
    ciphertext << "ShiftRows:\n";

    // The below while loop swaps the characters in the message for shift rows
    int m = 0; // counter for while loop
    while (m < message.size())
    {
        // first row do nothing

        // second row shift left one position
        swap(message[m+4], message[m+5]);
        swap(message[m+5], message[m+6]);
        swap(message[m+6], message[m+7]);

        // third row shift left two positions
        swap(message[m+8], message[m+10]);
        swap(message[m+9], message[m+11]);

        // fourth row shift left three position
        swap(message[m+14], message[m+15]);
        swap(message[m+13], message[m+14]);
        swap(message[m+12], message[m+13]);

        m += 16; // move to the next block
    }

    // Printing out the message in 4x4 blocks
    for (int k = 0; k < message.size(); k++)
    {
        if (k % 4 == 3)
        {
            cout << message[k] << "\n";
            ciphertext << message[k] << "\n";
        }

        else
        {
            cout << message[k];
            ciphertext << message[k];
        }

        if (k % 16 == 15)
        {
            cout << "\n";
            ciphertext << "\n";
        }
    }

    cout << "Parity Bit:\n";
    ciphertext << "Parity Bit:\n";

    // This implements the parity bit encryption
    for (int n = 0; n < message.size(); n++)
    {
        int tempi = message[n]; // ascii value of the character
        int parity; // will eventually hold the parity of character in tempi
	// calculating the parity of tempi
        parity = tempi ^ (tempi >> 1);
        parity = parity ^ (parity >> 2);
        parity = parity ^ (parity >> 4);
        parity = parity ^ (parity >> 8);
        parity = parity ^ (parity >> 16);

        parity = parity & 1;
        tempi = tempi & 0x000000FF;
	// if the parity is odd, then we add 1 to the MSB
        if (parity == 1)
        {
            tempi = tempi | 0x80;
        }

        message[n] = tempi;
    }

    // printing out message
    for (int k = 0; k < message.size(); k++)
    {
        int tempc = message[k]; // holds ascii value
        tempc = tempc & 0xFF;
        if (k % 4 == 3)
        {
            cout << hex << tempc << "\n";
            ciphertext << hex << tempc << "\n";
        }

        else
        {
            cout << hex << tempc << " ";
            ciphertext << hex << tempc << " ";
        }
    }

    cout << "\nMixColumns:\n";
    ciphertext << "\nMixColumns:\n";

    // This implements the MixColumns encryption
    int o = 0;
    while (o < message.size())
    {
        // 1st column
        int c1 = message[o]; // 1st row
        c1 = c1 & 0xFF;
        int c2 = message[o+4]; // 2nd row
        c2 = c2 & 0xFF;
        int c3 = message[o+8]; // 3rd row
        c3 = c3 & 0xFF;
        int c4 = message[o+12]; // 4th row
        c4 = c4 & 0xFF;

        int a0 = rgfMul(c1, 2) ^ rgfMul(c2, 3) ^ c3 ^ c4;
        int a1 = c1 ^ rgfMul(c2, 2) ^ rgfMul(c3, 3) ^ c4;
        int a2 = c1 ^ c2 ^ rgfMul(c3, 2) ^ rgfMul(c4, 3);
        int a3 = rgfMul(c1, 3) ^ c2 ^ c3 ^ rgfMul(c4, 2);

        message[o] = a0;
        message[o+4] = a1;
        message[o+8] = a2;
        message[o+12] = a3;
        
        // 2nd column
        c1 = message[o+1]; // 1st row
        c1 = c1 & 0xFF;
        c2 = message[o+5]; // 2nd row
        c2 = c2 & 0xFF;
        c3 = message[o+9]; // 3rd row
        c3 = c3 & 0xFF;
        c4 = message[o+13]; // 4th row
        c4 = c4 & 0xFF;

        a0 = rgfMul(c1, 2) ^ rgfMul(c2, 3) ^ c3 ^ c4;
        a1 = c1 ^ rgfMul(c2, 2) ^ rgfMul(c3, 3) ^ c4;
        a2 = c1 ^ c2 ^ rgfMul(c3, 2) ^ rgfMul(c4, 3);
        a3 = rgfMul(c1, 3) ^ c2 ^ c3 ^ rgfMul(c4, 2);

        message[o+1] = a0;
        message[o+5] = a1;
        message[o+9] = a2;
        message[o+13] = a3;

        // 3rd column
        c1 = message[o+2]; // 1st row
        c1 = c1 & 0xFF;
        c2 = message[o+6]; // 2nd row
        c2 = c2 & 0xFF;
        c3 = message[o+10]; // 3rd row
        c3 = c3 & 0xFF;
        c4 = message[o+14]; // 4th row
        c4 = c4 & 0xFF;

        a0 = rgfMul(c1, 2) ^ rgfMul(c2, 3) ^ c3 ^ c4;
        a1 = c1 ^ rgfMul(c2, 2) ^ rgfMul(c3, 3) ^ c4;
        a2 = c1 ^ c2 ^ rgfMul(c3, 2) ^ rgfMul(c4, 3);
        a3 = rgfMul(c1, 3) ^ c2 ^ c3 ^ rgfMul(c4, 2);

        message[o+2] = a0;
        message[o+6] = a1;
        message[o+10] = a2;
        message[o+14] = a3;

        // 4th column
        c1 = message[o+3]; // 1st row
        c1 = c1 & 0xFF;
        c2 = message[o+7]; // 2nd row
        c2 = c2 & 0xFF;
        c3 = message[o+11]; // 3rd row
        c3 = c3 & 0xFF;
        c4 = message[o+15]; // 4th row
        c4 = c4 & 0xFF;

        a0 = rgfMul(c1, 2) ^ rgfMul(c2, 3) ^ c3 ^ c4;
        a1 = c1 ^ rgfMul(c2, 2) ^ rgfMul(c3, 3) ^ c4;
        a2 = c1 ^ c2 ^ rgfMul(c3, 2) ^ rgfMul(c4, 3);
        a3 = rgfMul(c1, 3) ^ c2 ^ c3 ^ rgfMul(c4, 2);

        message[o+3] = a0;
        message[o+7] = a1;
        message[o+11] = a2;
        message[o+15] = a3;

        o += 16; // move to next block
    }

    // print out message
    for (int k = 0; k < message.size(); k++)
    {
        int tempc = message[k]; // holds ascii value
        tempc = tempc & 0xFF;
        if (k % 4 == 3)
        {
            cout << hex << tempc << "\n";
            ciphertext << hex << tempc << "\n";
        }

        else
        {
            cout << hex << tempc << " ";
            ciphertext << hex << tempc << " ";
        }
    }

    // closing files
    plaintext.close();
    keyfile.close();
    ciphertext.close();

    return 0;
}

// This function helps to do the rgf multiplication in MixColumns
int rgfMul(int x, int y)
{
    int check = x & 0xF0; // 2ND BYTE IN NUMBER

    if (y == 2)
    {
        x = x << 1; // multiply by 2
	// If the MSB before shifting was 1 then XOR x with 0x1B
        if (check == 0x80 | check == 0x90 | check == 0xA0 | check == 0xB0 | check == 0xC0 | check == 0xD0 | check == 0xE0 | check == 0xF0)
        {
            x = x ^ 0x1B;
        }
        return x;
    }

    else if (y == 3)
    {
        int temp = x;
        x = x << 1; // multiply by 2
        x = x ^ temp; // XOR with original value

	//If the MSB before shifting was 1 then XOR x with 0x1B
        if (check == 0x80 | check == 0x90 | check == 0xA0 | check == 0xB0 | check == 0xC0 | check == 0xD0 | check == 0xE0 | check == 0xF0)
        {
            x = x ^ 0x1B;
        }
        return x;
    }
    return 1;
}
