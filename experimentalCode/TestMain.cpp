

#include <limits>
#include <bitset>
#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <bits/stdc++.h>

using namespace std;


int main()
{
    vector<char> bits;
    char byte = 0;
    //just try read into char vector
    //then convert to binary

    fstream input_file;
    input_file.open ("/tmp/500M");

    if(!input_file.is_open()){
        cerr << "could not open file";
        return EXIT_FAILURE;
    }else{
        while(input_file.get(byte)){
            bits.push_back(bitset<8>(byte).to_string());
        }

        for(char i:bits){
            cout << i << " ";
        }

    }


//
//
//
}


