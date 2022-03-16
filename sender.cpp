//
// Created by Wei Zhong Tee on 3/6/22.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <algorithm>
#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;
using std::cin;


class Sender {
private:
    int selectedAlgorithm; // 0 for GBN, 1 for stop and Wait, 2 for SR
    int senderMaxWindowSize;
    int sizeOfPacket;
    int seqNumberUpperBound;
    int seqNumberLowerBound;
    int staticOrDynamic; // 0 for static, 1 for dynamic
    int staticSeconds;
    int dynamicRoundTripTimeMultiplier;
    int selectedErrorType; // 0 for none, 1 for specific packets, 2 for percentage
    int errorPercentage; //0 if none
    vector<int> packetsToDrop; //empty if none
    string filePath;
    // packets to fail checksum, fail to send ack done in receiver

public:
    void setAlgorithmType(int input) {selectedAlgorithm = (input - 1);};
    int getSelectedAlgorithm() {return selectedAlgorithm;};

    void setSenderMaxWindowSize(int input) {senderMaxWindowSize = input;};
    int getSenderMaxWindowSize() {return senderMaxWindowSize;};

    void setSizeOfPacket(int input) {sizeOfPacket = input;};
    int getSizeOfPacket() {return sizeOfPacket;};

    void setSeqNumberUpperBound(int input) {seqNumberUpperBound = input;};
    int getSeqNumberUpperBound() {return seqNumberUpperBound;};

    void setSeqNumberLowerBound(int input) {seqNumberLowerBound = input;};
    int getSeqNumberLowerBound() {return seqNumberLowerBound;};

    void setStaticOrDynamic (int input) {staticOrDynamic = input;};
    int getStaticOrDyanamic() {return staticOrDynamic;};

    void setStaticSeconds(int input) {staticSeconds = input;};
    int getStaticSeconds() {return staticSeconds;};

    void setDynamicRoundTripMultiplier(int input) {dynamicRoundTripTimeMultiplier = input;};
    int getDynamicRoundTripMultiplier() {return dynamicRoundTripTimeMultiplier;};

    void setErrorType(int input) {selectedErrorType = input;};
    int getErrorType() {return selectedErrorType;};

    void setErrorPercentage(int input) {errorPercentage = input;};
    int getErrorPercentage() {return errorPercentage;};

    void setPacketsToDrop(vector<int> input) {packetsToDrop = input;};
    vector<int>& getPacketsToDrop() {return packetsToDrop;}

    void setFilePath(string input) {filePath = input;};
    string getFilePath() {return filePath;};
};

int selectedAlgorithm;
int senderMaxWindowSize;
int sizeOfPacket;
int seqNumberUpperBound;
int seqNumberLowerBound;
int staticOrDynamic; // 0 for static, 1 for dynamic
int staticSeconds;
int dynamicRoundTripTimeMultiplier;
int selectedErrorType;
int errorPercentage; //0 if none
vector<int> packetsToDrop; //empty if none
string filePath;
void senderWelcomeMessage() {
    cout << "Creating instance for: sender." << endl;
}

void getNetworkConfigFrom(string fileName) {
    ifstream inputFile;
    inputFile.open(fileName);
    if (inputFile.is_open()) {
        string line;
        int itemCount = 0;
        while (getline(inputFile, line)) {
            int len = line.length();
            char lineChars[len + 1];
            strcpy(lineChars, line.c_str());
            if(itemCount == 0) { // Set selected algorithm
                selectedAlgorithm = stoi(lineChars);
              } else if (itemCount == 1) { // Set sender window size
                if (selectedAlgorithm-1 != 1) { // GBN or SR: Set window size to size in config file
                    senderMaxWindowSize = stoi(lineChars);
                } else { // Stop and Wait: Set window size to 1
                    senderMaxWindowSize = 1;
                }
            } else if (itemCount == 3) { // Skip item count 2 as that is receiver window size. // Get size of packet
                sizeOfPacket = stoi(lineChars);
            } else if (itemCount == 4) { // Seq num upper bound
                seqNumberUpperBound = stoi(lineChars);
            } else if (itemCount == 5) { // Seq num lower bound
                seqNumberLowerBound = stoi(lineChars);
            }  else if (itemCount == 6) { // 0 for Static or 1 for Dynamic
                staticOrDynamic = stoi(lineChars);
            } else if (itemCount == 7) { // Get time interval in seconds for static option
                staticSeconds = stoi(lineChars);
            } else if (itemCount == 8) { // Get round trip time multiplier for dynamic option
                dynamicRoundTripTimeMultiplier = stoi(lineChars);
            } else if (itemCount == 9) { // Selected error type
                selectedErrorType = stoi(lineChars);
            } else if (itemCount == 10) { // Error percentage if percentage to be randomly dropped is chosen
                if(line == "") {
                    errorPercentage = 0;
                } else {
                    errorPercentage = stoi(line);
                }
            } else if (itemCount == 11) { // Frame IDs of packets to drop
                string currentNum = "";
                for (int i = 0; i < len; ++i) {
                    if(lineChars[i] != ',') {
                        currentNum += lineChars[i];
                    } else {
                        //cout << "Current number end: " << currentNum << endl;
                        int packet = stoi(currentNum);
                        packetsToDrop.push_back(packet);
                        currentNum = "";
                    }
                }
            } else if (itemCount == 14) { // Skip items 12 to 13, File path
                filePath = line;
            }
            itemCount++;
        }
    }
}

void showCurrentConfig(Sender currentSender) {
    cout << endl << "Current sender configuration: " << endl;
    cout << "Selected algorithm: ";
    switch (currentSender.getSelectedAlgorithm()) {
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
    cout << "Sender Window Size: " << currentSender.getSenderMaxWindowSize() << endl;
    cout << "Size of Packet: " << currentSender.getSizeOfPacket() << endl;
    cout << "Seq Num Lower Bound: " << currentSender.getSeqNumberLowerBound() << endl;
    cout << "Seq Num Upper Bound: " << currentSender.getSeqNumberUpperBound() << endl;
    cout << "Static or Dynamic time interval: ";
    if (currentSender.getStaticOrDyanamic() == 0) {
        cout << "Static" << endl;
        cout << "Time interval in seconds: " << currentSender.getStaticSeconds() << endl;
    } else {
        cout << "Dynamic" << endl;
        cout << "Round trip time multiplier: " << currentSender.getDynamicRoundTripMultiplier() << endl;
    }
    cout << "Selected error type: " << currentSender.getErrorType() << endl;
    switch (currentSender.getErrorType()) {
        case 0:
            cout << "None" << endl;
            break;
        case 1:
            cout << "Specific Packets" << endl;
            cout << "Packets to drop: ";
            for (int i = 0; i < packetsToDrop.size(); ++i) {
                cout << packetsToDrop[i] << "\t";
            }
            cout << endl;
            break;
        case 2:
            cout << "Percentage of randomly selected packets" << endl;
            cout << "Percentage: " << currentSender.getErrorPercentage() << "%" << endl;
            break;
    }
    cout << "Getting data from: " << currentSender.getFilePath() << endl;
}

Sender setSenderInstance(int selectedAlgorithm, int senderMaxWindowSize, int sizeOfPacket, int seqNumUpperBound, int seqNumLowerBound, int staticOrDynamic, int staticSeconds, int dynamicRoundTripTimeMultiplier, int selectedErrorType, int errorPercentage, vector<int> packetsToDrop, string filePath) {
    Sender senderInstance;
    senderInstance.setAlgorithmType(selectedAlgorithm);
    senderInstance.setSenderMaxWindowSize(senderMaxWindowSize);
    senderInstance.setSizeOfPacket(sizeOfPacket);
    senderInstance.setSeqNumberUpperBound(seqNumUpperBound);
    senderInstance.setSeqNumberLowerBound(seqNumLowerBound);
    senderInstance.setStaticOrDynamic(staticOrDynamic);
    senderInstance.setStaticSeconds(staticSeconds);
    senderInstance.setDynamicRoundTripMultiplier(dynamicRoundTripTimeMultiplier);
    senderInstance.setErrorType(selectedErrorType);
    senderInstance.setErrorPercentage(errorPercentage);
    senderInstance.setPacketsToDrop(packetsToDrop);
    senderInstance.setFilePath(filePath);
    return senderInstance;
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
bool passesChecksum(string message, string compli){
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
        if (addBinary(addition, compli) == "0"){
            return true;
        }else{
            return false;
        }
}

//FOR SOCKET SENDER
string read_(tcp::socket & socket) {
       boost::asio::streambuf buf;
       boost::asio::read_until( socket, buf, "\n" );
       string data = boost::asio::buffer_cast<const char*>(buf.data());
       return data;
}
void send_(tcp::socket & socket, const string& message) {
       const string msg = message + "\n";
       boost::asio::write( socket, boost::asio::buffer(message) );
}

int main() {
    /////////////////FOR CHECKING CHECKSUM -- returns boolean
    passesChecksum("ReplaceWithOriginalPacketData", "ReplaceWithPassedCksumCompliment");
    /////////////////
    
    Sender senderInstance;
    senderWelcomeMessage();
    getNetworkConfigFrom("config.txt");
    senderInstance = setSenderInstance(selectedAlgorithm, senderMaxWindowSize, sizeOfPacket, seqNumberUpperBound, seqNumberLowerBound, staticOrDynamic, staticSeconds, dynamicRoundTripTimeMultiplier, selectedErrorType, errorPercentage, packetsToDrop, filePath);
    showCurrentConfig(senderInstance);

//////////////KEEP FOR FORMATTING UNTIL WE HAVE PACKETS READY
    cout << "\nStarting server...\n";
    cout << "\nSender side setting up sockets...\n";

    boost::asio::io_service io_service;
//listen for new connection
      tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 1234 ));
//socket creation
      tcp::socket socket(io_service);
//waiting for connection
      acceptor.accept(socket);
//read operation
      string message = read_(socket);
      cout << message << endl;
//write operation
      send_(socket, "Hello From Sender!");
      cout << "Sender sent Hello message to Receiver!" << endl;

    return 0;
}
