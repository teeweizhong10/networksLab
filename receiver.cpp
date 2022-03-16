//
// Created by Wei Zhong Tee on 3/6/22.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <string>
#include <algorithm>
#include <bitset>
#include <boost/asio.hpp>
#include <bits/stdc++.h>      //may be able to use <bitset> but this include is causing errors

using namespace std;
using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;
using std::cin;


class Receiver {
private:
    int selectedAlgorithm; // 0 for GBN, 1 for stop and Wait, 2 for SR
    int receiverMaxWindowSize;
    int sizeOfPacket;
    int seqNumberUpperBound;
    int seqNumberLowerBound;
    int selectedErrorType; // 0 for none, 1 for specific packets, 2 for percentage
    int errorPercentage; //0 if none
    vector<int> packetsToLoseAck; //empty if none
    vector<int> packetsToFailChecksum; //empty if none
    string filePath;

public:
    void setAlgorithmType(int input) {selectedAlgorithm = (input - 1);};
    int getSelectedAlgorithm() {return selectedAlgorithm;};

    void setReceiverMaxWindowSize(int input) {receiverMaxWindowSize = input;};
    int getReceiverMaxWindowSize() {return receiverMaxWindowSize;};

    void setErrorType(int input) {selectedErrorType = input;};
    int getErrorType() {return selectedErrorType;};

    void setErrorPercentage(int input) {errorPercentage = input;};
    int getErrorPercentage() {return errorPercentage;};

    void setPacketsToLoseAck(vector<int> input) {packetsToLoseAck = input;};
    vector<int>& getPacketsToLoseAck() {return packetsToLoseAck;}

    void setPacketsToFailChecksum(vector<int> input) {packetsToFailChecksum = input;};
    vector<int>& setPacketsToFailChecksum() {return packetsToFailChecksum;}

    void setFilePath(string input) {filePath = input;};
    string getFilePath() {return filePath;};
};

int selectedAlgorithm;
int receiverMaxWindowSize;
int sizeOfPacket;
int selectedErrorType;
int errorPercentage; //0 if none
vector<int> packetsToLoseAck; //empty if none
vector<int> packetsToFailChecksum; //empty if none
string filePath;

void receiverWelcomeMessage() {
    cout << "Creating instance for: receiver." << endl;
}

void getNetworkConfigFrom(string fileName) {
    ifstream inputFile;
    inputFile.open(fileName);
    if(inputFile.is_open()) {
        string line;
        int itemCount = 0;
        while (getline(inputFile, line)) {
            int len = line.length();
            char lineChars[len + 1];
            strcpy(lineChars, line.c_str());
            if (itemCount == 0) { // Set selected algorithm
                selectedAlgorithm = stoi(lineChars);
            } else if (itemCount == 2) { // Skip sender window size, set receiver window size
                if (selectedAlgorithm-1 != 2) { // GBN or Stop and Wait: Set window size to size 1
                    receiverMaxWindowSize = 1;
                } else { // Selective repeat: Set window size to size in config file
                    receiverMaxWindowSize =  stoi(lineChars);
                }
            } else if (itemCount == 9) { // Selected error type
                selectedErrorType = stoi(lineChars);
            } else if (itemCount == 10) { // Error percentage if percentage to be randomly dropped is chosen
                if(line == "") {
                    errorPercentage = 0;
                } else {
                    errorPercentage = stoi(line);
                }
            } else if (itemCount == 12) { // Frame IDs of packets to lose ack
                string currentNum = "";
                for (int i = 0; i < len; ++i) {
                    if(lineChars[i] != ',') {
                        currentNum += lineChars[i];
                    } else {
                        //cout << "Current number end: " << currentNum << endl;
                        int packet = stoi(currentNum);
                        packetsToLoseAck.push_back(packet);
                        currentNum = "";
                    }
                }
            } else if (itemCount == 13) { // Frame IDs of packets to fail checksum
                string currentNum = "";
                for (int i = 0; i < len; ++i) {
                    if(lineChars[i] != ',') {
                        currentNum += lineChars[i];
                    } else {
                        //cout << "Current number end: " << currentNum << endl;
                        int packet = stoi(currentNum);
                        packetsToFailChecksum.push_back(packet);
                        currentNum = "";
                    }
                }
            }
            itemCount++;
        }
    }
}

void showCurrentConfig(Receiver currentReceiver) {
    cout << endl << "Current sender configuration: " << endl;
    cout << "Selected algorithm: ";
    switch (currentReceiver.getSelectedAlgorithm()) {
        case 0:
            cout << "GBN" << endl;
            break;
        case 1:
            cout << "Stop and Wait" << endl;
            break;
        case 2:
            cout << "Selective Repeat" << endl;
            break;
    }
    cout << "Receiver Window Size: " << currentReceiver.getReceiverMaxWindowSize() << endl;
    cout << "Selected error type: " << currentReceiver.getErrorType() << endl;
    switch (currentReceiver.getErrorType()) {
        case 0:
            cout << "None" << endl;
            break;
        case 1:
            cout << "Specific Packets" << endl;
            cout << "Packets to lose ack: ";
            for (int i = 0; i < packetsToLoseAck.size(); ++i) {
                cout << packetsToLoseAck[i] << "\t";
            }
            cout << endl;
            cout << "Packets to fail checksum: ";
            for (int i = 0; i < packetsToFailChecksum.size(); ++i) {
                cout << packetsToFailChecksum[i] << "\t";
            }
            cout << endl;
            break;
        case 2:
            cout << "Percentage of randomly selected packets" << endl;
            cout << "Percentage: " << currentReceiver.getErrorPercentage() << "%" << endl;
            break;
    }
}

Receiver setReceiverInstance(int selectedAlgorithm, int receiverMaxWindowSize, int selectedErrorType, int errorPercentage, vector<int> packetsToLoseAck, vector<int> packetsToFailCheckSum) {
    Receiver receiverInstance;
    receiverInstance.setAlgorithmType(selectedAlgorithm);
    receiverInstance.setReceiverMaxWindowSize(receiverMaxWindowSize);
    receiverInstance.setErrorType(selectedErrorType);
    receiverInstance.setErrorPercentage(errorPercentage);
    receiverInstance.setPacketsToLoseAck(packetsToLoseAck);
    receiverInstance.setPacketsToFailChecksum(packetsToFailCheckSum);
    return receiverInstance;
}

//addBinary: takes in two strings of binary characters and adds them
string addBinary (string a, string b){

        if(a.length() > b.length()){
                return addBinary(b, a);
        }

        int diff = b.length() - a.length();
        string padding;

        for (int i = 0; i < diff; i++){
                padding.push_back('0');
        }

        a = padding + a;
        string res;
        char carry = '0';

        for(int i = a.length() - 1; i >= 0; i--){
                if(a[i] == '1' && b[i]){
                        if(carry == '1'){
                                res.push_back('1');
                                carry = '1';
                        }else{
                                res.push_back('0');
                                carry = '1';
                        }
                }else if (a[i] == '0' && b[i] == '0'){
                        if(carry == '1'){
                                res.push_back('1');
                                carry = '0';
                        }else{
                                res.push_back('0');
                                carry = '0';
                        }
                }else if (a[i] != b[i]){
                        if(carry == '1'){
                                res.push_back('0');
				carry = '1';
                        }else{
                                res.push_back('1');
                                carry = '0';
                        }
                }
        }

                if (carry == '1'){
                        res.push_back(carry);
                }
                reverse(res.begin(), res.end());

                return res;
        }


//checksum : performs the internet checksum on given data as 16 bits
string checksum(string message){
        //Takes 16 bits of the data and adds
        string addition = "";
        for(int i = 0; i < message.length();i++){
                if (i % 16 == 0){       //split data into this many bit segments
                        cout << "\nAdding string: " << message.substr(i, 16);
                        addition = addBinary(addition, message.substr(i, 16));
                        i = i + 15;
                }else if ( (i > message.length() - 16)){
                        cout << "\nAdding string: " << message.substr(i, message.length() - i);
                        addition = addBinary(addition, message.substr(i, message.length()-i));
                        i = message.length();
                }
        }
        addition = addBinary(addition.substr(0, addition.length()-16), addition.substr(addition.length()-16, addition.length()-1));
        return addition;
}
//gets compliment of checksum binary string
string compliment(string cksum){
        string compli = "";

        for (int i = 0; i < cksum.length();i++){
                if(cksum[i] =='0'){
                        compli = compli + "1";
                }else if (cksum[i] == '1'){
                        compli = compli + "0";
                }
        }
        return compli;
}

int main() {

    //FOR WHEN WE NEED TO RUN CHECKSUM
    string cksum = checksum("ReplaceThisStringWithPacketData");
    string complimentofChkSum = compliment(cksum);
    // //////////////////////////////////////////////

    Receiver receiverInstance;
    receiverWelcomeMessage();
    getNetworkConfigFrom("config.txt");
    receiverInstance = setReceiverInstance(selectedAlgorithm, receiverMaxWindowSize, selectedErrorType, errorPercentage, packetsToLoseAck, packetsToFailChecksum);
    showCurrentConfig(receiverInstance);



    /////////////////////////////
    //estabilishes service
        boost::asio::io_service io_service;

        //creates socket
        tcp::socket socket(io_service);

        //connects socket to server
        socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));

        //message from client -- WILL BE REPLACED
        const string msg = "Hello from Receiver!\n";
        boost::system::error_code error;
        boost::asio::write(socket, boost::asio::buffer(msg), error);
        if(!error){
                cout<<"Receiver sent message"<<endl;
        }else{
                cout<<"Send failed"<<endl;
        }
        //server response
        boost::asio::streambuf recv_buf;
        boost::asio::read(socket, recv_buf, boost::asio::transfer_all(), error);
        if(error && error != boost::asio::error::eof){
                cout<<"recv failed"<<endl;
        }else{
                const char* data = boost::asio::buffer_cast<const char*>
                        (recv_buf.data());
                cout<< data<<endl;
        }
        /////////////////////////////

    return 0;
}

