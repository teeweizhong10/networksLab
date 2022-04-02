//
// Created by Wei Zhong Tee on 3/6/22.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <sstream>
#include <algorithm>
#include <bitset>
#include <boost/asio.hpp>

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
    string filePath;
    int port;

public:
    void setAlgorithmType(int input) {selectedAlgorithm = (input - 1);};
    int getSelectedAlgorithm() {return selectedAlgorithm;};

    void setReceiverMaxWindowSize(int input) {receiverMaxWindowSize = input;};
    int getReceiverMaxWindowSize() {return receiverMaxWindowSize;};

    void setSizeOfPacket(int input) {sizeOfPacket = input;};
    int getSizeOfPacket() {return sizeOfPacket;};

    void setSeqNumberUpperBound(int input) {seqNumberUpperBound = input;};
    int getSeqNumberUpperBound() {return seqNumberUpperBound;};

    void setSeqNumberLowerBound(int input) {seqNumberLowerBound = input;};
    int getSeqNumberLowerBound() {return seqNumberLowerBound;};

    void setErrorType(int input) {selectedErrorType = input;};
    int getErrorType() {return selectedErrorType;};

    void setErrorPercentage(int input) {errorPercentage = input;};
    int getErrorPercentage() {return errorPercentage;};

    void setPacketsToLoseAck(vector<int> input) {packetsToLoseAck = input;};
    vector<int>& getPacketsToLoseAck() {return packetsToLoseAck;}

    void setFilePath(string input) {filePath = input;};
    string getFilePath() {return filePath;};

    void setPortNumber(int input) { port = input;};
    int getPortNumber(){return port;};
};
//*************************************************************************************************************************
int selectedAlgorithm;
int receiverMaxWindowSize;
int sizeOfPacket;
int seqNumberUpperBound;
int seqNumberLowerBound;
int selectedErrorType;
int errorPercentage; //0 if none
vector<int> packetsToLoseAck; //empty if none
vector<int> packetsToFailChecksum; //empty if none
string filePath;
int port;
string finalBits;
int packetNumber;
string bitData;
int currentSeqNum;
int currentPacketNum;
string bitDataComp;

string receivedBytes = "";

bool printLog = true;
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
            }else if(itemCount == 3){
                sizeOfPacket = stoi(lineChars);
            }else if (itemCount == 9) { // Selected error type
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
                    }else {
                        int packet = stoi(currentNum);
                        packetsToLoseAck.push_back(packet);
                        currentNum = "";
                    }
                }
            }else if(itemCount == 16){
                port = stoi(line);
            }
            itemCount++;
        }
    }
}

void parseConfigFromString(string input) {
    cout << "Current input: " << endl;
    istringstream f(input);
    string line;
    int itemCount = 0;

    while (getline(f, line)) {
        int len = line.length();
        cout << line << endl;
        if (itemCount == 0) { // selected algorithm
            selectedAlgorithm = stoi(line);
        } else if (itemCount == 1) {
            receiverMaxWindowSize = stoi(line);
        } else if (itemCount == 2) {
            sizeOfPacket = stoi(line);
        } else if (itemCount == 3) { // seq num lower bound
            seqNumberLowerBound = stoi(line);
        } else if (itemCount == 4) { //seq num upper bound
            seqNumberUpperBound = stoi(line);
        } else if (itemCount == 5) { // error type
            selectedErrorType = stoi(line);
        } else if (itemCount == 6) { // Frame IDs of packets to lose ack
            string currentNum = "";
            for (int i = 0; i < len; ++i) {
                if(line[i] != ',') {
                    currentNum += line[i];
                } else {
                    int packet = stoi(currentNum);
                    packetsToLoseAck.push_back(packet);
                    currentNum = "";
                }
            }
        }
        itemCount++;
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
    cout << "Packet Size: " << sizeOfPacket << endl;
    //currentReceiver.getSizeOfPacket() << endl;
    cout << "Seq Num Lower Bound: " << currentReceiver.getSeqNumberLowerBound() << endl;
    cout << "Seq Num Upper Bound: " << currentReceiver.getSeqNumberUpperBound() << endl;
    cout << "Selected error type: " << currentReceiver.getErrorType() << endl;
    cout << "Selected port number: " << currentReceiver.getPortNumber() << endl;

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
            break;
        case 2:
            cout << "Percentage of randomly selected packets" << endl;
            cout << "Packets to lose ack: ";
            for (int i = 0; i < packetsToLoseAck.size(); ++i) {
                cout << packetsToLoseAck[i] << "\t";
            }

            cout << endl;
            break;
    }
}

Receiver setReceiverInstance(int selectedAlgorithm, int receiverMaxWindowSize, int seqNumLowerBound, int seqNumUpperBound, int sizeOfPacket, int selectedErrorType, int errorPercentage, vector<int> packetsToLoseAck, int port){
    Receiver receiverInstance;
    receiverInstance.setAlgorithmType(selectedAlgorithm);
    receiverInstance.setReceiverMaxWindowSize(receiverMaxWindowSize);
    receiverInstance.setSizeOfPacket(sizeOfPacket);
    receiverInstance.setSeqNumberLowerBound(seqNumLowerBound);
    receiverInstance.setSeqNumberUpperBound(seqNumUpperBound);
    receiverInstance.setErrorType(selectedErrorType);
    receiverInstance.setErrorPercentage(errorPercentage);
    receiverInstance.setPacketsToLoseAck(packetsToLoseAck);
    receiverInstance.setPortNumber(port);
    return receiverInstance;
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

bool passesChecksum(string originalCksum, string passedCksum){
    if(addBinary(originalCksum, passedCksum)== "1111111111111111"){
        return true;
    }else{
        return false;
    }
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
//*************************************************************************************************************************
void setBitsToFile(string bitString){
    ofstream output;
    output.open("OUTPUTFILE");

    for (int i = 0; i < bitString.length(); i++){
        string bitTemp = bitString.substr(i, 8);
        bitset<8> temp(bitTemp);
        output << char(temp.to_ulong());
        i= i+7;
    }
    output.close();
}

void parseReceivingPacket(string input) {
    string packetMessage = input;
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
    packetNumber = packetNum;
    bitData = bitContent;
    cout << "Byte data: " << bitData << endl;
    bitDataComp = checksumVal;
}
//*************************************************************************************************************************


string getData(tcp::socket & socket) {
    boost::asio::streambuf buf;
    boost::system::error_code err_code;
    std::size_t n = sizeOfPacket;
    //boost::asio::read_until( socket, buf, "=|||=" );
    boost::asio::read(socket, buf,
                      boost::asio::transfer_exactly(n), err_code);
    string data = boost::asio::buffer_cast<const char*>(buf.data());
    return data;
}
void sendData(tcp::socket & socket, const string& message) {
    const string& msg = message + "=|||=";
    boost::asio::write( socket, boost::asio::buffer(msg) );
}

void stats(){
    cout << "Last packet seq# received: ______\nNumber of original packets received: ____\nNumber of retransmitted packets received:_____" << endl;
}

void GBN(){}

void SR(){}

void SNW(tcp::socket& socket){

    while(true){
        string recvPkt = getData(socket);
        cout << "Received packet: " << recvPkt << endl;

        if(recvPkt == "alldone=|||="){
            cout <<"alldone received" << endl;
            string done = "alldone";
            sendData(socket, done);
            stats();
            break;
        }
        parseReceivingPacket(recvPkt);
        receivedBytes += bitData;
        string receivedCk = checksum(bitData);
        std::string s = addBinary(bitDataComp, receivedCk);
        if (s.find('0') != std::string::npos) {
            cout << "Checksum failed" << endl;
            cout << "Current window [1]" << endl;
            string ack = "NACK";
            sendData(socket, ack);

        }
        //else checksum succeeds
        // If checksum is bad or lose ack
        for (int i = 0; i < packetsToLoseAck.size(); ++i) {
            if(packetNumber == packetsToLoseAck[i]) {
                packetsToLoseAck.erase(packetsToLoseAck.begin());
                cout << "Losing ACK for packet " << packetNumber << "\nCurrent window [1]" << endl;
                string ack = "NACK";
                sendData(socket, ack);
            }
        }

        // If checksum is good, send back ack
        string ack = "ACK " + to_string(packetNumber);
        sendData(socket, ack);
        cout << "Ack " << to_string(packetNumber) << " sent"  << endl;
        cout << "Current window [1]" << endl;

    }
}





void receiverSimulation(){
    boost::asio::io_service io_service;

    boost::asio::ip::tcp::acceptor acceptor(io_service, tcp::endpoint(boost::asio::ip::tcp::v4(), port));
    boost::asio::ip::tcp::socket socket(io_service);
//    socket.open(boost::asio::ip::tcp::v4());
    acceptor.accept(socket);
    boost::asio::socket_base::receive_buffer_size option(sizeOfPacket + 40);    //can recv up to 1G
    socket.set_option(option);

    //first packet sent is config
    string config = getData(socket);
    parseConfigFromString(config);
    sendData(socket, "configReceived");


    //getting Begin Transaction
    string recv = getData(socket);
    cout << recv;

    //break into "begin transaction..." and port number and set port
    if(recv == "Begin transaction...=|||="){
        sendData(socket, "Begin transaction...");
        cout << "Begin transaction..." << endl;
    }

    switch(selectedAlgorithm){
        case 0:{
            GBN();
            break;
        }
        case 1:{
            SNW(socket);
            break;
        }
        case 2:{
            SR();
            break;
        }
    }

}

//*************************************************************************************************************************
int main() {
    Receiver receiverInstance;
    receiverWelcomeMessage();
    //receive config by sockets
    getNetworkConfigFrom("config.txt");
    receiverInstance = setReceiverInstance(selectedAlgorithm, receiverMaxWindowSize, seqNumberLowerBound, seqNumberUpperBound, sizeOfPacket, selectedErrorType, errorPercentage, packetsToLoseAck, port);    showCurrentConfig(receiverInstance);
    //parseConfigFromString("config.txt");

    receiverSimulation();
    setBitsToFile(finalBits);

    cout << "All received bytes length: " << receivedBytes.length() << endl;

    return 0;
}

