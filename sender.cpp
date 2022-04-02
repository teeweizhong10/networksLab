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
#include <boost/asio.hpp>
#include <chrono>
#include <thread>


using namespace std;
using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;
using std::cin;

using Clock = std::chrono::steady_clock;
using std::chrono::time_point;
using std::chrono::duration_cast;
using std::chrono::milliseconds;
using namespace std::literals::chrono_literals;
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
//*************************************************************************************************************************
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
time_point<Clock> startTimer;
time_point<Clock> endTimeInSeconds;
string ipAddr;
int port;

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
            }else if(itemCount == 15){
                ipAddr = line;
            }else if(itemCount == 16){
                port = stoi(line);
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
//*************************************************************************************************************************
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
//*************************************************************************************************************************
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


//TODO: ask Lauren about checksum errors
string checksum(string inPacket) {
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

string getChecksumVal(string input) {
    string com = checksum(input);
    com = compliment(com);
    return com;
}


//*************************************************************************************************************************
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
    }
}
//*************************************************************************************************************************
void startTotalTimer(){
    startTimer = Clock::now();
}

void dropTimer(milliseconds secs){
    sleep_for(secs*100);
}

void timeoutTimerStart(){
}

string endTotalTimer(){
    endTimeInSeconds = Clock::now();
    milliseconds difftime = duration_cast<milliseconds>(endTimeInSeconds - startTimer);
    return to_string(difftime.count());
}
//*************************************************************************************************************************
string getData(tcp::socket& socket){
    boost::asio::streambuf buf;
    boost::asio::read_until(socket, buf, "\n");
    string data =  boost::asio::buffer_cast<const char*>(buf.data());
    return data;
}

void sendData(tcp::socket& socket, const string& msg){
    boost::asio::write(socket, buffer(msg + "\n"));
}

void stats(){
    cout << "Number of original packets sent: _____\nNumber of retransmitted packets:______\nTotal elapsed time:_____\ntotal throughput (Mbps):_______\nEffective throughput:______" << endl;
}

void whatsInThisVector(vector<int> vec){
    cout << "\n\n\n";
    for (int i = 0; i < vec.size(); i++){
        cout << to_string(vec[i]) <<endl;
    }
}

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

void GBN(){}
void SR(){}
void SNW(tcp::socket& socket, vector<char>& bytes){
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
                    sendData(socket, newPacket.getCorruptedPacketMessage()) // send corrupted message
                    packetSent = true;
                    string ack = getData(socket); // Receiver gets good packet
                    if(ack == "ACK " + to_string(packetNumber)) {
                        receivedAck = true;
                    } // Receiver gets corrupted packet (won't happen)
                }
            }

            if (!packetSent) {
                simulateSendPacket(newPacket.getPacketMessage()); // send packet
                if (printLog) {
                    cout << "Packet " << packetCounter << " was sent" << endl;
                }
                packetSent = true;
                string ack = getData(socket); // Receiver gets good packet
                if(ack == "ACK " + to_string(packetNumber)) {
                    receivedAck = true;
                }
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
}

void beginTransaction(vector<char>& bytes){
    cout << "IP: " << ipAddr << endl;
    cout << "PORT: " << port << endl;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket(io_service);
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(ipAddr), port));
    startTotalTimer();


    sendData(socket, "Begin transaction...");

    string response = getData(socket);
    if(response == "Begin transaction...\n"){
        switch(selectedAlgorithm){
            case 1:{
                GBN();
                break;
            }
            case 2:{
                SNW(socket, bytes);
                break;
            }
            case 3:{
                SR();
                break;
            }
        }
    }
}
//*************************************************************************************************************************
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
    beginTransaction(bytes);


    //refactorSenderStopAndWait(filePath);
    cout << endl;

    time_point<Clock> end = Clock::now();
    milliseconds totalElapsedTime = duration_cast<milliseconds>(end - start);
    cout << "Total elapsed time: " << totalElapsedTime.count() << "ms" << std::endl;
    cout << "Done sending all " << numOfPackets << " packets" << endl;
}
