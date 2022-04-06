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
#include <algorithm>
#include <chrono>
#include <thread>
#include <math.h>
#include <unistd.h>
#include <sstream>
#include "packet.h"
//#include <bits/stdc++.h>

using namespace std;
using Clock = std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using std::this_thread::sleep_for;

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


bool printLog = true;

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
time_point<Clock> start;
milliseconds latency;
milliseconds waitTime = milliseconds(0);

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
    contentToSend += std::to_string(selectedAlgorithm);
    contentToSend += "\n";
    cout << "Sender Window Size: " << currentSender.getSenderMaxWindowSize() << endl;
    cout << "Receiver Window Size: " << currentSender.getReceiverMaxWindowSize() << endl;
    contentToSend += std::to_string(currentSender.getReceiverMaxWindowSize());
    contentToSend += "\n";
    cout << "Size of Packet (bytes): " << currentSender.getSizeOfPacket() << endl;
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
    int v1 = rand() % numOfPackets;
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
        v1 = rand() % numOfPackets;
    }
    sort(packetsToDrop.begin(), packetsToDrop.end());
}

void setRandomPacketsToLoseAck(int percentage, int numOfPackets) { // to send to receiver
    packetsToLoseAck.clear();
    int packetsLoseAckCount = numOfPackets * percentage/100;
    int v1 = rand() % numOfPackets;
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
        v1 = rand() % numOfPackets;
    }
    sort(packetsToLoseAck.begin(), packetsToLoseAck.end());
}

void setRandomPacketsToFailChecksum(int percentage, int numOfPackets) { //corrupt
    packetsToFailChecksum.clear();
    int packetsToFailChecksumCount = numOfPackets * percentage/100;
    int v1 = rand() % numOfPackets;
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
        v1 = rand() % numOfPackets;
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
    if (!packetsToLoseAck.empty()) {
        for (int i = 0; i < packetsToLoseAck.size(); ++i) {
            contentToSend += std::to_string(packetsToLoseAck[i]);
            contentToSend += ",";
        }
    }
}

//addBinary: takes in two strings of binary characters and adds them
string addBinary (string a, string b){
string result = "";
int temp = 0;
int size_a = a.size() - 1;
int size_b = b.size() - 1;

while(size_a >= 0 || size_b >= 0 || temp ==1){
temp += ((size_a >= 0)? a[size_a] - '0': 0);
      temp += ((size_b >= 0)? b[size_b] - '0': 0);
      result = char(temp % 2 + '0') + result;
      temp /= 2;
      size_a--; size_b--;
   }
   return result;

}

string checksum(string inPacket){
    string bytesToBits = "";
    for (std::size_t i = 0; i < inPacket.size(); ++i)
    {
        bytesToBits += bitset<8>(inPacket.c_str()[i]).to_string();
    }
    inPacket = bytesToBits;

    //Takes 16 bits of the data and adds
	string addition = "";
    for(signed int i = 1; i <= inPacket.length();i++){
        if ((i%16 == 0) && (i >= 16)) { //split data into this many bit segments
            
		addition = addBinary(addition, inPacket.substr((signed)(i-16),16));

        } else if (i >= (signed)((inPacket.length() - (signed)(inPacket.length()%16)))){
            addition = addBinary(addition, inPacket.substr(i-1, inPacket.length()-1));
	    i = inPacket.length();
        }
    }
    while ((signed)(addition.length() > 16)){
    signed int minus16 = addition.length() - 16;
    signed int minus1 = addition.length() - 1;
    addition = addBinary(addition.substr(0, minus16), addition.substr(minus16, minus1));
    }

    return addition;
}

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

string getChecksumVal(string input) {
    string com = checksum(input);
    com = compliment(com);
    return com;
}

void setBitsToFile(string bitString){
ofstream output;
output.open("OUTPUTFILE");

//cout << "\nBITSTRING: " << bitString; Test print bit string
for (int i = 0; i < bitString.length(); i++){
	string bitTemp = bitString.substr(i, 8);
	//cout << "\nSubstring: " << bitTemp;
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
    }
}

// Testing simulation functions
void simulateSendConfig(string configMessage) {
    //cout << "Config being sent to receiver: " << configMessage << endl;}
}
void simulateSendPacket(string packetMessage) {}

bool simulateSendAck(bool sendStatus) { //send a fake ack
    return sendStatus;
}

bool notTimedOut(milliseconds currentTime) {
    if(currentTime.count() >= waitTime.count()) {
        return false;
    }
    return true;
}

void senderGetTimeout() { //determining how long set wait time (dynamic wait time)
    bool ackReceived;
    //Send config over socket
    start = Clock::now();
    simulateSendConfig(contentToSend);

    // Receive ack
    sleep_for(milliseconds(1)); // Simulate time taken for packets to send
    ackReceived = simulateSendAck(true); // socket code: if error try again
    time_point<Clock> end = Clock::now();
    milliseconds diff = duration_cast<milliseconds>(end - start);
    latency = diff;
    cout << "Latency for one round trip: " << latency.count() << "ms" << std::endl;
    if (staticOrDynamic == 0) {
        waitTime = milliseconds(staticSeconds);
        cout << "Wait time mode: static, wait time is " << waitTime.count() << "ms" << endl;
    } else {
        waitTime = (latency.count()*milliseconds(dynamicRoundTripTimeMultiplier));
        cout << "Wait time mode: dynamic, wait time is " << waitTime.count() << "ms" << endl;
    }

}

bool simulateReceiverStopAndWait(string packetMessage) { //simulating receiver sending back an ack for stop and wait
    // Parsing the packet message (in receiver.cpp already)
    //cout << "\nReceived packet: " << packetMessage << endl;

    int len = packetMessage.length();
    char lineChars[len + 1];
    strcpy(lineChars, packetMessage.c_str());
    int itemCount = 0;

    string item = "";
    int packetNum = 0;
    int seqNum = 0;
    string bitContent = "";
    string checksumVal = "";
    int ackReceived = 0;

    std::string delimiter = "=||=";
    size_t pos = 0;
    std::string token;
    while ((pos = packetMessage.find(delimiter)) != std::string::npos) {
        token = packetMessage.substr(0, pos);
        if(itemCount == 0) { // packet number
            itemCount = 1;
            packetNum = stoi(token);
        } else if(itemCount == 1) { // seq number
            itemCount = 2;
            seqNum = stoi(token);
        } else if(itemCount == 2) { // byte content
            itemCount = 3;
            bitContent = token;
        } else if(itemCount == 3) { // byte content
            itemCount = 4;
            checksumVal = token;
        } else if(itemCount == 5) { // byte content
            ackReceived = stoi(token);
        }
        packetMessage.erase(0, pos + delimiter.length());
    }

//    cout << "Packet number: " << packetNum << endl;
//    cout << "Sequence number: " << seqNum << endl;
//    cout << "Bit Content: " << bitContent << endl;
//    cout << "Checksum value: " << checksumVal << endl;
//    cout << "Ack received value: " << ackReceived << endl;

    //sleep_for(milliseconds(1)); //Simulate time taken to process and send ack

    // TODO: Do checksum
    string receivedCk = checksum(bitContent);
    std::string s = addBinary(checksumVal, receivedCk);
    if (s.find('0') != std::string::npos) {
        if(printLog) {
            cout << "Packet did not pass checksum." << endl;
        }
        sleep_for(waitTime + milliseconds(1)); // let it time out
        return false; // checksum failed, doesn't add up to all 1s
    }
        //else checksum succeeds

    // If checksum is bad or lose ack
    for (int i = 0; i < packetsToLoseAck.size(); ++i) {
        if(packetNum == packetsToLoseAck[i]) {
            packetsToLoseAck.erase(packetsToLoseAck.begin());
            sleep_for(waitTime + milliseconds(1)); // let it time out
            return false;
        }
    }

    // If checksum is good, send back ack
    return true;
}


void senderStopAndWait(vector<packet> packets) { //simulating sender stop and wait
    bool receivedAck;
    bool packetSent;
    int packetsSent = 0;
    while (packetsSent!=numOfPackets) {
        time_point<Clock> timeoutStart = Clock::now();
        milliseconds currentTimeCount = duration_cast<milliseconds>(milliseconds (0)- milliseconds(0));
        while (notTimedOut(currentTimeCount)) { // Check for timeout
            packetSent = false;
            // TODO: Simulate receive ack, corrupt and lose ack
            if(!packetsToDrop.empty()) { // Time out and don't send dropped packet if there are packets to be dropped
                if(packetsToDrop[0] == packetsSent) { // If packet is meant to be dropped
                    packetsToDrop.erase(packetsToDrop.begin());
                    sleep_for(waitTime + milliseconds(1)); // Let it time out
                    cout << "Packet " << packetsSent << " timed out" << endl;
                    // packet not sent
                }
            }

            if(!packetsToFailChecksum.empty()) {
                if (packetsToFailChecksum[0] == packetsSent){
                    packetsToFailChecksum.erase(packetsToFailChecksum.begin());
                    cout << "Packet " << packetsSent << " was corrupted" << endl;
                    simulateSendPacket(packets[packetsSent].getCorruptedPacketMessage()); // send corrupted message
                    cout << "Corrupted Packet " << packetsSent << " was sent" << endl;
                    packetSent = true;
                    receivedAck = simulateReceiverStopAndWait(packets[packetsSent].getCorruptedPacketMessage()); // Receiver gets corrupted packet
                }
            }
            if (!packetSent) {
                simulateSendPacket(packets[packetsSent].getPacketMessage()); // send packet
                cout << "Packet " << packetsSent << " was sent" << endl;
                packetSent = true;
                receivedAck = simulateReceiverStopAndWait(packets[packetsSent].getPacketMessage()); // Receiver gets good packet
            }
            if (receivedAck) {
                cout << "ACK for packet " << packetsSent << " was received." << endl;
                packets[packetsSent].setAckReceived(1); // Set ack received for given packet
                packetsSent++;
            } else {
                cout << "Failed to receive ACK for packet " << packetsSent << " and timed out. Trying again." << endl;
            }
            time_point<Clock> timeoutend = Clock::now();
            currentTimeCount = duration_cast<milliseconds>(timeoutend - start);
        }
    }
}


// REFACTORING: sending packets as file is broken into bits

void setNumberOfPackets(int fileSizeBytes, int sizeOfPackets) {
//    ifstream in_file(file, ios::binary);
//    in_file.seekg(0, ios::end);
//    int file_size = in_file.tellg()*8;
    if(fileSizeBytes%sizeOfPacket > 0) {
        numOfPackets = fileSizeBytes/sizeOfPacket + 1;
    } else {
        numOfPackets = fileSizeBytes/sizeOfPacket;
    }
    cout << "Num of packets: " << numOfPackets << endl;
}

void refactorGenerateErrors(string file, int sizeOfPackets, int percentage) {
    ifstream in_file(file, ios::binary);
    in_file.seekg(0, ios::end);
    int file_size = in_file.tellg()*8;
    cout<<"Size of the file is"<<" "<< file_size<<" "<<"bits" << endl;
    int numOfPackets;
    if(file_size%sizeOfPacket > 0) {
        numOfPackets = file_size/sizeOfPacket + 1;
    } else {
        numOfPackets = file_size/sizeOfPacket;
    }
    //cout << "Num of packets: " << numOfPackets << endl;
    setPacketErrors(percentage, numOfPackets);
}

void refactorSenderStopAndWait(string file) {
    vector<char> bytes;
    char byte = 0;
    string bits="";
    fstream input_file;
    input_file.open (file);

    int packetCounter = 0;
    int seqNumCounter = 0;
    bool packetSent = false;
    bool receivedAck = false;

    string receivedBits = "";
    packet newPacket;
    string bitsForPacket = "";
    if(!input_file.is_open()){
        cerr << "could not open file";
        return ;
    } else {
        time_point<Clock> timeoutStart = Clock::now();
        milliseconds currentTimeCount = duration_cast<milliseconds>(milliseconds (0)- milliseconds(0));
        while (notTimedOut(currentTimeCount)) { // Check for timeout

            // Loop 1; still reading bits and sending
            while (input_file.get(byte)) {
                packetSent = false;
                bits += bitset<8>(byte).to_string();
                if(bits.length() >= sizeOfPacket) {
                    bitsForPacket = bits.substr(0,sizeOfPacket);
                    bits.erase(0,sizeOfPacket);
                    //cout << "New packet " << packetCounter << " Bit contents: " << bitsForPacket << endl;
                    newPacket = packet(packetCounter, seqNumCounter, bitsForPacket, getChecksumVal(bitsForPacket), 0);
                }

                //simulate sending packet

                if(bits.length() >= sizeOfPacket) {
                    //drop packet
                    if(!packetsToDrop.empty()) {
                        if(packetsToDrop[0] == packetCounter) {
                            packetsToDrop.erase(packetsToDrop.begin());
                            sleep_for(waitTime + milliseconds(1)); // Let it time out
                            if (printLog) {
                                cout << "Packet " << packetCounter << " timed out, trying again." << endl;
                            }
                        }
                    }

                    //corrupt packet
                    if(!packetsToFailChecksum.empty()) {
                        if (packetsToFailChecksum[0] == packetCounter){
                            packetsToFailChecksum.erase(packetsToFailChecksum.begin());
                            if (printLog) {
                                cout << "Packet " << packetCounter << " was corrupted" << endl;
                                cout << "Corrupted Packet " << packetCounter << " was sent" << endl;
                            }
                            simulateSendPacket(newPacket.getCorruptedPacketMessage()); // send corrupted message
                            packetSent = true;
                            receivedAck = simulateReceiverStopAndWait(newPacket.getCorruptedPacketMessage()); // Receiver gets corrupted packet
                        }
                    }

                    if (!packetSent) {
                        simulateSendPacket(newPacket.getPacketMessage()); // send packet
                        if (printLog) {
                            cout << "Packet " << packetCounter << " was sent" << endl;
                        }
                        packetSent = true;
                        receivedAck = simulateReceiverStopAndWait(newPacket.getPacketMessage()); // Receiver gets good packet
                    }
                    if (receivedAck) {
                        if (printLog) {
                            cout << "ACK for packet " << packetCounter << " was received." << endl;
                            receivedBits += newPacket.getBitContent();
                        }
                        // Prepare for next packet
                        packetCounter++;
                        seqNumCounter++;
                        if (seqNumCounter == seqNumberUpperBound) {
                            seqNumCounter = 0;
                        }
                    } else {
                        if (printLog) {
                            cout << "Failed to receive ACK for packet " << packetCounter << " and timed out. Trying again." << endl;
                        }
                        bits.insert(0,bitsForPacket); // reinsert previously removed bits because it was not sent
                    }
                } else {
                    bits.insert(0,bitsForPacket); // reinsert previously removed bits because it was not sent
                }
            }

            // Loop 2: done reading , not done sending remaining bits
            while (bits.length() != 0) {
                packetSent = false;
                // If remaining bits == size of packet
                if(bits.length() >= sizeOfPacket) {
                    bitsForPacket = bits.substr(0,sizeOfPacket);
                    bits.erase(0,sizeOfPacket);
                    //cout << "New packet " << packetCounter << " Bit contents: " << bitsForPacket << endl;
                    newPacket = packet(packetCounter, seqNumCounter, bitsForPacket, getChecksumVal(bitsForPacket), 0);
                } else { // remaining bits < size of packet
                    bitsForPacket = bits.substr(0,sizeOfPacket);
                    //cout << "Remaining bits length: " << bitsForPacket.length() << endl;
                    bits = "";
                    //cout << "New packet " << packetCounter << " Bit contents: " << bitsForPacket << endl;
                    newPacket = packet(packetCounter, seqNumCounter, bitsForPacket, getChecksumVal(bitsForPacket), 0);
                }

                //simulate sending packet

                //drop packet
                if(!packetsToDrop.empty()) {
                    if(packetsToDrop[0] == packetCounter) {
                        packetsToDrop.erase(packetsToDrop.begin());
                        sleep_for(waitTime + milliseconds(1)); // Let it time out
                        if (printLog) {
                            cout << "Packet " << packetCounter << " timed out, trying again." << endl;
                        }
                    }
                }

                //corrupt packet
                if(!packetsToFailChecksum.empty()) {
                    if (packetsToFailChecksum[0] == packetCounter){
                        packetsToFailChecksum.erase(packetsToFailChecksum.begin());
                        if (printLog) {
                            cout << "Packet " << packetCounter << " was corrupted" << endl;
                            cout << "Corrupted Packet " << packetCounter << " was sent" << endl;
                        }
                        simulateSendPacket(newPacket.getCorruptedPacketMessage()); // send corrupted message
                        packetSent = true;
                        receivedAck = simulateReceiverStopAndWait(newPacket.getCorruptedPacketMessage()); // Receiver gets corrupted packet
                    }
                }

                if (!packetSent) {
                    simulateSendPacket(newPacket.getPacketMessage()); // send packet
                    if (printLog) {
                        cout << "Packet " << packetCounter << " was sent" << endl;
                    }
                    packetSent = true;
                    receivedAck = simulateReceiverStopAndWait(newPacket.getPacketMessage()); // Receiver gets good packet
                }
                if (receivedAck) {
                    if (printLog) {
                        cout << "ACK for packet " << packetCounter << " was received." << endl;
                        receivedBits += newPacket.getBitContent();
                    }
                    // Prepare for next packet
                    packetCounter++;
                    seqNumCounter++;
                    if (seqNumCounter == seqNumberUpperBound) {
                        seqNumCounter = 0;
                    }
                } else {
                    if (printLog) {
                        cout << "Failed to receive ACK for packet " << packetCounter << " and timed out. Trying again." << endl;
                    }
                    bits.insert(0,bitsForPacket); // reinsert previously removed bits because it was not sent
                }
            }

            time_point<Clock> timeoutend = Clock::now();
            currentTimeCount = duration_cast<milliseconds>(timeoutend - start);
        }
    }
//    if (printLog) {
//        cout << "Received bits: " << receivedBits << endl;
//    }
}

// try again

void trySenderStopAndWait(vector<char>& bytes) {
    cout << endl;
    string byteContent;
    string allBytesBefore(bytes.begin(), bytes.end());
    string allBytesAfter = "";

    string receivedBytes = "";
    int packetCounter = 0;
    int seqNumCounter = 0;
    bool packetSent = false;
    bool receivedAck = false;
    packet newPacket;
    while(packetCounter != numOfPackets) {
        if (bytes.size() >= sizeOfPacket) {
            string s(bytes.begin(), bytes.begin()+sizeOfPacket);
            byteContent = s;
        } else {
            string s(bytes.begin(), bytes.end());
            byteContent = s;
        }


        //TODO: SEND BYTES HERE
        // need checksum working with byte content
        cout << "Sending packet " << packetCounter << endl;
        newPacket = packet(packetCounter, seqNumCounter, byteContent, getChecksumVal(byteContent), 0);
        packetSent = false;
        time_point<Clock> timeoutStart = Clock::now();
        milliseconds currentTimeCount = duration_cast<milliseconds>(milliseconds (0)- milliseconds(0));
        while (notTimedOut(currentTimeCount)) {
            currentTimeCount += waitTime;
            //drop packet
            if(!packetsToDrop.empty()) {
                if(packetsToDrop[0] == packetCounter) {
                    packetsToDrop.erase(packetsToDrop.begin());
                    sleep_for(waitTime + milliseconds(1)); // Let it time out
                    if (printLog) {
                        cout << "Packet " << packetCounter << " timed out, trying again." << endl;
                    }
                }
            }

            //corrupt packet
            if(!packetsToFailChecksum.empty()) {
                if (packetsToFailChecksum[0] == packetCounter){
                    packetsToFailChecksum.erase(packetsToFailChecksum.begin());
                    if (printLog) {
                        cout << "Packet " << packetCounter << " was corrupted" << endl;
                        cout << "Corrupted Packet " << packetCounter << " was sent" << endl;
                    }
                    simulateSendPacket(newPacket.getCorruptedPacketMessage()); // send corrupted message
                    packetSent = true;
                    receivedAck = simulateReceiverStopAndWait(newPacket.getCorruptedPacketMessage()); // Receiver gets corrupted packet
                }
            }

            if (!packetSent) {
                simulateSendPacket(newPacket.getPacketMessage()); // send packet
                if (printLog) {
                    cout << "Packet " << packetCounter << " was sent" << endl;
                }
                packetSent = true;
                receivedAck = simulateReceiverStopAndWait(newPacket.getPacketMessage()); // Receiver gets good packet
            }

            if(receivedAck) {
                if (printLog) {
                    cout << "ACK for packet " << packetCounter << " was received." << endl;
                    receivedBytes += newPacket.getBitContent();
                }

                //Move on to next packet to send
                packetCounter++;
                seqNumCounter++;
                allBytesAfter += byteContent;
                if (bytes.size() >= sizeOfPacket) {
                    bytes.erase(bytes.begin(), bytes.begin()+sizeOfPacket);
                }
                if (seqNumCounter == seqNumberUpperBound) {
                    seqNumCounter = 0;
                }
            } else {
                if (printLog) {
                    cout << "Failed to receive ACK for packet " << packetCounter << " and timed out. Trying again." << endl;
                }
            }

            time_point<Clock> timeoutend = Clock::now();
            currentTimeCount = duration_cast<milliseconds>(timeoutend - start);
            if(receivedAck) {
                currentTimeCount = waitTime;
            }
        }
    }
    if (allBytesBefore == receivedBytes) {
        cout << "Byte strings match up" << endl;
    } else {
        cerr << "Byte strings do not match up" << endl;
    }
}



void trySR(vector<char>& bytes) {
    char byte = 0;
    string bits="";

    int packetCounter = 0;
    int seqNumCounter = 0;
    bool packetSent = false;
    bool receivedAck = false;

    string receivedBytes = "";
    packet newPacket;
    string byteContent = "";

    time_point<Clock> timeoutStart = Clock::now();
    milliseconds currentTimeCount = duration_cast<milliseconds>(milliseconds (0)- milliseconds(0));

    cout << endl;
    string allBytesBefore(bytes.begin(), bytes.end());
    string allBytesAfter = "";

    packet packetsInWindow[senderMaxWindowSize];

    for (int i = 0; i < senderMaxWindowSize; ++i) {

    }

    while(packetCounter != numOfPackets) {
        bool receivedAck = false;
        for (int i = 0; i < senderMaxWindowSize; ++i) {
            //creating packets by window size
            if (bytes.size() >= sizeOfPacket) {
                string s(bytes.begin(), bytes.begin()+sizeOfPacket);
                byteContent = s;
                newPacket = packet(packetCounter, seqNumCounter, byteContent, getChecksumVal(byteContent), 0);
                packetsInWindow[i] = newPacket;

            } else {
                string s(bytes.begin(), bytes.end());
                byteContent = s;
                newPacket = packet(packetCounter, seqNumCounter, byteContent, getChecksumVal(byteContent), 0);
                packetsInWindow[i] = newPacket;
            }

            if (bytes.size() >= sizeOfPacket) {
                bytes.erase(bytes.begin(), bytes.begin()+sizeOfPacket);
            }
        }

        cout << "Packets in window: " << endl;
        for (int i = 0; i < receiverMaxWindowSize; ++i) {
            cout << packetsInWindow[i].getPacketNum() << endl;
        }
        cout << "********" << endl;


        while (notTimedOut(currentTimeCount)) { // Check for timeout
            //send packet

            packetCounter++;
            seqNumCounter++;
            if (seqNumCounter == seqNumberUpperBound) {
                seqNumCounter = 0;
            }
            allBytesAfter += byteContent;

            time_point<Clock> timeoutend = Clock::now();
            currentTimeCount = duration_cast<milliseconds>(timeoutend - start);
        }
    }
}
int main() {
    Sender senderInstance;
    senderWelcomeMessage();
    getNetworkConfigFrom("config.txt");
    senderInstance = setSenderInstance(selectedAlgorithm, senderMaxWindowSize, receiverMaxWindowSize, sizeOfPacket, seqNumberUpperBound, seqNumberLowerBound, staticOrDynamic, staticSeconds, dynamicRoundTripTimeMultiplier, selectedErrorType, errorPercentage, packetsToDrop, packetsToLoseAck, packetsToFailChecksum, filePath);
    showCurrentConfig(senderInstance);

    // read bytes from file
    std::ifstream input(filePath, std::ios::binary);

    cout << "\n\nReading the file " << filePath << "..." << endl;
    std::vector<char> bytes(
            (std::istreambuf_iterator<char>(input)),
            (std::istreambuf_iterator<char>()));

    input.close();

    cout << "File size in bytes: " << bytes.size() << endl;

    setNumberOfPackets(bytes.size(), sizeOfPacket);

    if (selectedErrorType == 2) { // set errors for random percentage
        setPacketErrors(errorPercentage, numOfPackets);
    }

    if (selectedErrorType != 0) {
        cout << "ERRORS: " << endl;
        cout << "Packets to drop: ";
        for (int i = 0; i < packetsToDrop.size(); ++i) {
            cout << packetsToDrop[i] << "\t"; //Test print packets to drop
        }
        cout << endl;

        cout << "Packets to corrupt: ";
        for (int i = 0; i < packetsToFailChecksum.size(); ++i) {
            cout << packetsToFailChecksum[i] << "\t"; //Test print packets to drop
        }
        cout << endl;

        cout << "Packets to lose ack: ";
        for (int i = 0; i < packetsToLoseAck.size(); ++i) {
            cout << packetsToLoseAck[i] << "\t"; //Test print packets to drop
        }
        cout << endl;
    }

    start = Clock::now(); // for total elapsed time

    cout << "\n************ Protocol work ************" << endl;

    //sends config to receiver and gets time out value
    senderGetTimeout();

    //trySenderStopAndWait(bytes);
    trySR(bytes);


    //refactorSenderStopAndWait(filePath);
    cout << endl;

    time_point<Clock> end = Clock::now();
    milliseconds totalElapsedTime = duration_cast<milliseconds>(end - start);
    cout << "Total elapsed time: " << totalElapsedTime.count() << "ms" << std::endl;
    cout << "Done sending all " << numOfPackets << " packets" << endl;
    return 0;
}


