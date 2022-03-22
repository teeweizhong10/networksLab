//
// Created by Wei Zhong Tee on 3/6/22.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <cstdlib>
#include <time.h>
#include <stdlib.h>
#include <bitset>
#include "packet.h"

using namespace std;

class Sender {
private:
    int selectedAlgorithm; // 0 for GBN, 1 for stop and Wait, 2 for SR
    int senderMaxWindowSize;
    int receiverMaxWindowSize;
    int sizeOfPacket;
    int seqNumberUpperBound;
    int seqNumberLowerBound;
    int staticOrDynamic; // 0 for static, 1 for dynamic
    int staticSeconds;
    int dynamicRoundTripTimeMultiplier;
    int selectedErrorType; // 0 for none, 1 for specific packets, 2 for percentage
    int errorPercentage; //0 if none
    vector<int> packetsToDrop; //empty if none
    vector<int> packetsToLoseAck; //empty if none
    vector<int> packetsToFailChecksum; //empty if none
    string filePath;
    // packets to fail checksum, fail to send ack done in receiver

public:
    void setAlgorithmType(int input) {selectedAlgorithm = (input - 1);};
    int getSelectedAlgorithm() {return selectedAlgorithm;};

    void setSenderMaxWindowSize(int input) {senderMaxWindowSize = input;};
    int getSenderMaxWindowSize() {return senderMaxWindowSize;};

    void setReceiverMaxWindowSize(int input) {receiverMaxWindowSize = input;};
    int getReceiverMaxWindowSize() {return receiverMaxWindowSize;};

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

    void setPacketsToLoseAck(vector<int> input) {packetsToLoseAck = input;};
    vector<int>& getPacketsToLoseAck() {return packetsToLoseAck;}

    void setPacketsToFailChecksum(vector<int> input) {packetsToFailChecksum = input;};
    vector<int>& setPacketsToFailChecksum() {return packetsToFailChecksum;}

    void setFilePath(string input) {filePath = input;};
    string getFilePath() {return filePath;};
};

int selectedAlgorithm;
int senderMaxWindowSize;
int receiverMaxWindowSize;
int sizeOfPacket;
int seqNumberUpperBound;
int seqNumberLowerBound;
int staticOrDynamic; // 0 for static, 1 for dynamic
int staticSeconds;
int dynamicRoundTripTimeMultiplier;
int selectedErrorType;
int errorPercentage; //0 if none
vector<int> packetsToDrop; //empty if none
vector<int> packetsToLoseAck; //empty if none
vector<int> packetsToFailChecksum; //empty if none
string filePath;

string allBits;
string contentToSend;
int numOfPackets;

vector<packet> packets;

int getNumOfPackets(string bits) {
    numOfPackets = 0;
    return numOfPackets;
}

void senderWelcomeMessage() {
    cout << "Creating instance for: sender." << endl;
    srand(time(NULL));
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
                if (selectedAlgorithm-1 != 1) { // GBN or SR: Set sender window size to size in config file
                    senderMaxWindowSize = stoi(lineChars);
                } else { // Stop and Wait: Set window size to 1
                    senderMaxWindowSize = 1;
                }
            } else if (itemCount == 2) { // Set receiver window size
                if (selectedAlgorithm-1 == 2) { // SR: Set receiver window size to size in config file
                    receiverMaxWindowSize = stoi(lineChars);
                } else { // Stop and Wait: Set window size to 1
                    receiverMaxWindowSize = 1;
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
    contentToSend += std::to_string(currentSender.getSelectedAlgorithm());
    contentToSend += "\n";
    cout << "Sender Window Size: " << currentSender.getSenderMaxWindowSize() << endl;
    cout << "Receiver Window Size: " << currentSender.getReceiverMaxWindowSize() << endl;
    contentToSend += std::to_string(currentSender.getReceiverMaxWindowSize());
    contentToSend += "\n";
    cout << "Size of Packet: " << currentSender.getSizeOfPacket() << endl;
    contentToSend += std::to_string(currentSender.getSizeOfPacket());
    contentToSend += "\n";
    cout << "Seq Num Lower Bound: " << currentSender.getSeqNumberLowerBound() << endl;
    contentToSend += std::to_string(currentSender.getSeqNumberLowerBound());
    contentToSend += "\n";
    cout << "Seq Num Upper Bound: " << currentSender.getSeqNumberUpperBound() << endl;
    contentToSend += std::to_string(currentSender.getSeqNumberUpperBound());
    contentToSend += "\n";
    cout << "Static or Dynamic time interval: ";
    if (currentSender.getStaticOrDyanamic() == 0) {
        cout << "Static" << endl;
        cout << "Time interval in seconds: " << currentSender.getStaticSeconds() << endl;
    } else {
        cout << "Dynamic" << endl;
        cout << "Round trip time multiplier: " << currentSender.getDynamicRoundTripMultiplier() << endl;
    }
    cout << "Selected error type: " << currentSender.getErrorType() << endl;
    contentToSend += std::to_string(currentSender.getErrorType());
    contentToSend += "\n";
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
            cout << "Percentage: " << currentSender.getErrorPercentage() << "%" << endl;
            cout << "Packets to drop: ";
            for (int i = 0; i < packetsToDrop.size(); ++i) {
                cout << packetsToDrop[i] << "\t";
            }
            cout << endl;
            cout << "Packets to lose ack: ";
            for (int i = 0; i < packetsToLoseAck.size(); ++i) {
                cout << packetsToLoseAck[i] << "\t";
                contentToSend += std::to_string(packetsToLoseAck[i]);
                contentToSend += ",";
            }
            cout << endl;
            cout << "Packets to fail checksum: ";
            for (int i = 0; i < packetsToFailChecksum.size(); ++i) {
                cout << packetsToFailChecksum[i] << "\t";
            }
            cout << endl;
            contentToSend += "\n";
            break;
    }
    cout << "Getting data from: " << currentSender.getFilePath() << endl;
    //cout << "****************************\n" << "Content to send: \n" << contentToSend << endl;
}

Sender setSenderInstance(int selectedAlgorithm, int senderMaxWindowSize, int receiverMaxWindowSize, int sizeOfPacket, int seqNumUpperBound, int seqNumLowerBound, int staticOrDynamic, int staticSeconds, int dynamicRoundTripTimeMultiplier, int selectedErrorType, int errorPercentage, vector<int> packetsToDrop,  vector<int> packetsToLoseAck, vector<int> packetsToFailCheckSum, string filePath) {
    Sender senderInstance;
    senderInstance.setAlgorithmType(selectedAlgorithm);
    senderInstance.setSenderMaxWindowSize(senderMaxWindowSize);
    senderInstance.setReceiverMaxWindowSize(receiverMaxWindowSize);
    senderInstance.setSizeOfPacket(sizeOfPacket);
    senderInstance.setSeqNumberUpperBound(seqNumUpperBound);
    senderInstance.setSeqNumberLowerBound(seqNumLowerBound);
    senderInstance.setStaticOrDynamic(staticOrDynamic);
    senderInstance.setStaticSeconds(staticSeconds);
    senderInstance.setDynamicRoundTripMultiplier(dynamicRoundTripTimeMultiplier);
    senderInstance.setErrorType(selectedErrorType);
    senderInstance.setErrorPercentage(errorPercentage);
    senderInstance.setPacketsToDrop(packetsToDrop);
    senderInstance.setPacketsToLoseAck(packetsToLoseAck);
    senderInstance.setPacketsToFailChecksum(packetsToFailCheckSum);
    senderInstance.setFilePath(filePath);
    return senderInstance;
}

void setRandomPacketsToDrop(int percentage, int numOfPackets) {
    packetsToDrop.clear();
    int packetsDropCount = numOfPackets * percentage/100;
    int v1 = rand() % 100;
    bool add;
    while (packetsDropCount > 0) {
        if(packetsToDrop.size() ==  numOfPackets * percentage/100) {
            packetsDropCount = 0;
        }
        for (int i = 0; i < packetsToDrop.size(); ++i) {
            if(packetsToDrop[i] == v1) {
                add = false;
            }
        }

        if (add == true) {
            packetsToDrop.push_back(v1);
            packetsDropCount--;
        } else {
            add = true;
        }
        v1 = rand() % 100;
    }
    sort(packetsToDrop.begin(), packetsToDrop.end());
}

void setRandomPacketsToLoseAck(int percentage, int numOfPackets) { // to send to receiver
    packetsToLoseAck.clear();
    int packetsLoseAckCount = numOfPackets * percentage/100;
    int v1 = rand() % 100;
    bool add;
    while (packetsLoseAckCount > 0) {
        if(packetsToLoseAck.size() ==  numOfPackets * percentage/100) {
            packetsLoseAckCount = 0;
        }
        for (int i = 0; i < packetsToLoseAck.size(); ++i) {
            if(packetsToLoseAck[i] == v1) {
                add = false;
            }
        }

        for (int i = 0; i < packetsToDrop.size(); ++i) {
            if(packetsToDrop[i] == v1) {
                add = false;
                packetsLoseAckCount--;
            }
        }

        if (add == true) {
            packetsToLoseAck.push_back(v1);
            packetsLoseAckCount--;
        } else {
            add = true;
        }
        v1 = rand() % 100;
    }
    sort(packetsToLoseAck.begin(), packetsToLoseAck.end());
}

void setRandomPacketsToFailChecksum(int percentage, int numOfPackets) { //corrupt
    packetsToFailChecksum.clear();
    int packetsToFailChecksumCount = numOfPackets * percentage/100;
    int v1 = rand() % 100;
    bool add;
    while (packetsToFailChecksumCount > 0) {
        if(packetsToFailChecksum.size() ==  numOfPackets * percentage/100) {
            packetsToFailChecksumCount = 0;
        }
        for (int i = 0; i < packetsToFailChecksum.size(); ++i) {
            if(packetsToFailChecksum[i] == v1) {
                add = false;
            }
        }

        for (int i = 0; i < packetsToDrop.size(); ++i) {
            if(packetsToDrop[i] == v1) {
                add = false;
                packetsToFailChecksumCount--;
            }
        }

        for (int i = 0; i < packetsToLoseAck.size(); ++i) { // If both lose ack and fail checksum, drop instead
            if(packetsToLoseAck[i] == v1) {
                packetsToLoseAck.erase(packetsToLoseAck.begin() + i); // Remove from lose ack
                packetsToDrop.push_back(v1);
                add = false;
                packetsToFailChecksumCount--;
            }
        }

        if (add == true) {
            packetsToFailChecksum.push_back(v1);
            packetsToFailChecksumCount--;
        } else {
            add = true;
        }
        v1 = rand() % 100;
    }
    sort(packetsToFailChecksum.begin(), packetsToFailChecksum.end());
}

void setPacketErrors(int percentage, int numOfPackets) {
    setRandomPacketsToDrop(percentage, numOfPackets);
    setRandomPacketsToLoseAck(percentage, numOfPackets);
    setRandomPacketsToFailChecksum(percentage,numOfPackets);
    sort(packetsToDrop.begin(), packetsToDrop.end());
    sort(packetsToLoseAck.begin(), packetsToLoseAck.end());
    sort(packetsToFailChecksum.begin(), packetsToFailChecksum.end());
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


//TODO: ask Lauren about checksum errors
string checksum(string inPacket){
    //Takes 16 bits of the data and adds
	string addition = "";
    for(int i = 0; i < inPacket.length();i++){
        if (i % 16 == 0) { //split data into this many bit segments
            addition = addBinary(addition, inPacket.substr(i, 16));
            i = i + 15;
        } else if ((i > inPacket.length() - 16)){
            addition = addBinary(addition, inPacket.substr(i, inPacket.length()-i));
            i = inPacket.length();
        }
    }
    addition = addBinary(addition.substr(0, addition.length()-16), addition.substr(addition.length()-16, addition.length()-1));
    return addition;
}

//TODO: ask Lauren if we need to keep this
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



void setBitsToFile(string bitString){
ofstream output;
output.open("OUTPUTFILE");

cout << "\nBITSTRING: " << bitString;
for (int i = 0; i < bitString.length(); i++){
	string bitTemp = bitString.substr(i, 8);
	cout << "\nSubstring: " << bitTemp;
	bitset<8> temp(bitTemp);
	output << char(temp.to_ulong());
	i= i+7;
}
output.close();
}

void setBitsFromFile(string file) {
    vector<char> bytes;
    char byte = 0;
    string bits="";
    //just try read into char vector
    //then convert to binary

    fstream input_file;
    input_file.open (file);

    if(!input_file.is_open()){
        cerr << "could not open file";
        return ;
    } else {
        while(input_file.get(byte)){
            bits += bitset<8>(byte).to_string();
        }
        allBits = bits;
	setBitsToFile(allBits);
    }
}

int main() {
    Sender senderInstance;
    senderWelcomeMessage();
    getNetworkConfigFrom("config.txt");
    if (selectedErrorType == 2) { // set errors for random percentage
        setPacketErrors(errorPercentage, numOfPackets);
    }

    setBitsFromFile(filePath);
    cout << "Length of file in bits: " << allBits.length();

    senderInstance = setSenderInstance(selectedAlgorithm, senderMaxWindowSize, receiverMaxWindowSize, sizeOfPacket, seqNumberUpperBound, seqNumberLowerBound, staticOrDynamic, staticSeconds, dynamicRoundTripTimeMultiplier, selectedErrorType, errorPercentage, packetsToDrop, packetsToLoseAck, packetsToFailChecksum, filePath);
    showCurrentConfig(senderInstance);
    cout << endl;

    //allBits += "010101111010101010101110101101000011100101101010111001010000101010110101110101010101011100"; // Test adding bits

    // Begin coding here
    // Get the amount of packets based on the string length
    if(allBits.length()%sizeOfPacket > 0) {
        numOfPackets = allBits.length()/sizeOfPacket + 1;
    } else {
        numOfPackets = allBits.length()/sizeOfPacket;
    }

    cout << "\nNumber of packets: " << numOfPackets << endl;
    cout << "All bits: " << allBits << endl;


    // TODO: Test checksum code
    string testChecksum = checksum("0");
    cout << testChecksum;

    // Putting bit strings into packets based on user input size of packets
    remove(allBits.begin(), allBits.end(), ' ');
    vector<char> bitArray(allBits.begin(), allBits.end());
    string currentSet = "";
    int j = 0;
    bool runOnce = true;
    int packetCounter = 0;
    int seqNumCounter = 0;
    for (int i = 0; i < numOfPackets; ++i) {
        while (j < allBits.length()) {
            currentSet += bitArray[j];
            j++;
            if (j%sizeOfPacket == 0) {
                //cout << "Current set: " << currentSet << endl;
                seqNumCounter=(packetCounter)%(seqNumberUpperBound+1);
                packet newPacket = packet(packetCounter,seqNumCounter,currentSet,"00");
                packets.push_back(newPacket);
                currentSet = "";
                packetCounter++;
            }
        }
        if (runOnce) {
            if (j%numOfPackets > 0){
                //cout << "Current set: " << currentSet << endl;
                if(seqNumCounter%seqNumberUpperBound != 0) {
                    seqNumCounter++;
                } else {
                    seqNumCounter = 0;
                }
                packet newPacket = packet(packetCounter,seqNumCounter,currentSet,"00");
                packets.push_back(newPacket);
            }
        }
        runOnce = false;
    }

    // Test print all packet objects getMessage()
    for (int i = 0; i < packets.size(); ++i) {
        cout << packets[i].getPacketMessage() << endl;
    }
    return 0;
}