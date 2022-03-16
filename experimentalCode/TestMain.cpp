
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
    string filename("/tmp/1G");
    vector<char> bytes;
    char byte = 0;

    ifstream input_file(filename);
    if (!input_file.is_open()) {
        cerr << "Could not open the file - '"
             << filename << "'" << endl;
        return EXIT_FAILURE;
    }

    while (input_file.get(byte)) {
        bytes.push_back(byte);
    }
    for (const auto &i : bytes) {
        cout << i << "-";
    }
    cout << endl;
    input_file.close();

    return EXIT_SUCCESS;
}