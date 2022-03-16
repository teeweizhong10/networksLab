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
    ifstream input_file("/tmp/1G");
    if(!input_file.is_open()){
        cerr << "could not open file";
        return EXIT_FAILURE;
    }

    while(input_file.get(byte)){
        bytes.push_back(byte);
    }

    for(char i:bytes){
        cout << i << " ";
    }
    cout << "broke";
//
//
//
}