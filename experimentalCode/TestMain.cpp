
/*
#include <limits>
#include <bitset>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
//#include <bits/stdc++.h>

using namespace std;


int main()
{
    vector<char> bytes;
    char byte = 0;
    //just try read into char vector
    //then convert to binary

    fstream input_file;
    input_file.open ("/tmp/1G");

    if(!input_file.is_open()){
        cerr << "could not open file";
        return EXIT_FAILURE;
    }else{
        while(input_file.get(byte)){
            bytes.push_back(byte);
        }

        for(char i:bytes){
            cout << i << " ";
        }
        cout << "broke";
    }


//
//
//
}
 */

#include <iostream>
#include <fstream>
#include <vector>

using std::cout; using std::cerr;
using std::endl; using std::string;
using std::ifstream; using std::vector;

int main()
{
    string filename("input.txt");
    vector<char> bytes;

    FILE* input_file = fopen(filename.c_str(), "r");
    if (input_file == nullptr) {
        return EXIT_FAILURE;
    }

    unsigned char character = 0;
    while (!feof(input_file)) {
        character = getc(input_file);
        cout << character << "-";
    }
    cout << endl;
    fclose(input_file);

    return EXIT_SUCCESS;
}