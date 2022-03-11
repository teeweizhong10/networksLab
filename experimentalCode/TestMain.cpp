//
// Created by Kyle Sargeant on 3/9/22.
//


#include <iostream>
#include <Frame.h>
#incude <fstream>
using namespace std;

int main() {

    ifstream file("/tmp/500m");//locate 500m
    char b;
    while(file.get(b)){
        for(int i = 7; i>=0; i--){
            cout << ((c >> i) & 1);
        }
    }




};