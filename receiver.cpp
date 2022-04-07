#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <sstream>
#include <algorithm>
#include <bitset>
#include <boost/asio.hpp>
#include "packet.h"

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
int senderMaxWindowSize;
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
int seqNumCounter = 0;
int currentSeqNum;
int currentPacketNum;
string bitDataComp;
//bool printLog = true;
string receivedBytes = "";
int tempSeq;

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
            }else if(itemCount == 1){
                senderMaxWindowSize = stoi(lineChars);
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
        //cout << line << endl;
        if (itemCount == 0) { // selected algorithm
            selectedAlgorithm = stoi(line);
            cout<<"Selected algorithm: "<<selectedAlgorithm<<endl;
        } else if (itemCount == 1) {
            receiverMaxWindowSize = stoi(line);
            cout<<"Receiver window size: "<<receiverMaxWindowSize<<endl;
        } else if (itemCount == 2) {
            sizeOfPacket = stoi(line);
            cout<<"Size of packet: "<<sizeOfPacket<<endl;
        } else if (itemCount == 3) { // seq num lower bound
            seqNumberLowerBound = stoi(line);
        } else if (itemCount == 4) { //seq num upper bound
            seqNumberUpperBound = stoi(line);
            cout<<"Sequence number upper bound: "<<seqNumberUpperBound<<endl;
        } else if (itemCount == 5) { // error type
            selectedErrorType = stoi(line);
            cout<<"Selected error type: "<<selectedErrorType<<endl;
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

    cout << "Packets to lose ack: ";
    for (int i = 0; i < packetsToLoseAck.size(); ++i) {
        cout << packetsToLoseAck[i] << "\t";
    }
    cout << endl;
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
    output << bitString;
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
            tempSeq = seqNum;
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
    //cout << "Byte data: " << bitData << endl;
    bitDataComp = checksumVal;
}
//*************************************************************************************************************************
string getData(tcp::socket & socket) {
    std::vector<uint8_t> data;
    boost::asio::read_until(
            socket,
            boost::asio::dynamic_buffer(data),
            "=|||=");
    string s(data.begin(), data.end());
    return s;
}


void sendData(tcp::socket & socket, const string& message) {
    const string& msg = message + "=|||=";
    boost::asio::write( socket, boost::asio::buffer(msg) );
}

void stats(){
    cout << "Last packet seq# received: ______\nNumber of original packets received: ____\nNumber of retransmitted packets received:_____ " << endl;
}

void GBN(tcp::socket& socket){
    int packetsReceived = 0;


    while(true){
        bool cksumFail = false;
        string recvPkt = getData(socket);

        if(recvPkt == "alldone=|||="){
            sendData(socket, "alldone");
            stats();
            break;
        }


        parseReceivingPacket(recvPkt);
        if(printLog){cout << "Packet " << packetNumber << " received" << endl;}
//	receivedBytes += bitData;
        string recvCk = checksum(bitData);

        //corrupted
        string s = addBinary(bitDataComp, recvCk);
        if(s.find('0') != std::string::npos){
            if(printLog){
                cout << "Checksum failed" << endl;
                cout << "Current window = [1]" << endl;
            }string ack = "NACK";
            cksumFail = true;
            sendData(socket, ack);
        }

        //lost
        for (int i = 0; i < packetsToLoseAck.size(); ++i) {
            if(packetNumber == packetsToLoseAck[i]) {
                packetsToLoseAck.erase(packetsToLoseAck.begin());
                string ack = "NACK";
                cksumFail = true;	sendData(socket, ack);
            }
        }

        if(printLog && !cksumFail){
            cout << "Checksum OK" << endl;
        }
        if(!cksumFail){
            string ack = "ACK " + to_string(packetNumber);
            sendData(socket, ack);
            packetsReceived++;
            receivedBytes += bitData;
        }
        if(printLog && !cksumFail){
            cout << "Ack " << to_string(packetNumber) << " sent"  << endl;
            cout << "Current window [1]" << endl;
        }

    }
}

void printCurrentWindow(){
    cout << "Current window: [ ";

    int k = tempSeq;
    int i = 0;
    int j = 0;

    while (i < senderMaxWindowSize){
        if((k+j)>=seqNumberUpperBound){
            k = 0;
            j= 0;
        }
        cout << k + j << " ";
        i++;
        j++;
    }
    cout << "]" << endl;
}


void SR(tcp::socket& socket){
    bool alldone = false;
    vector<packet> unorderedPackets;
    while(!alldone){
        bool badPacket = false;
        bool cksumFail = false;

        string recvPkt = getData(socket);

        if(recvPkt == "alldone=|||="){
            sendData(socket, "alldone");
            stats();
            break;
        }

        parseReceivingPacket(recvPkt);
        if(printLog){cout << "Packet " << to_string(packetNumber) << " received" << endl;}
        string receivedCk = checksum(bitData);
        std::string s = addBinary(bitDataComp, receivedCk);
        if (s.find('0') != std::string::npos) {
            if(printLog){ cout << "Checksum failed" << endl;
            }string ack = "NACK " + to_string(packetNumber);
            if(printLog){printCurrentWindow();
            }cksumFail = true;
            sendData(socket, ack);
            badPacket = true;
        }
        //else checksum succeeds
        // If checksum is bad or lose ack
        for (int i = 0; i < packetsToLoseAck.size(); ++i) {
            if(packetNumber == packetsToLoseAck[i]) {
                if(printLog){cout << "ACK " << packetNumber << " sent" << endl;
                }packetsToLoseAck.erase(packetsToLoseAck.begin());
                string ack = "NACK " + to_string(packetNumber);
                sendData(socket, ack);
                if(printLog){printCurrentWindow();
                }badPacket = true;
            }
        }

        if(printLog && !cksumFail){
            cout << "Checksum OK" << endl;
        }

        if(!badPacket){
            sendData(socket, "ACK " + to_string(packetNumber));

            if(printLog){cout << "ACK " << packetNumber << " sent" << endl;
                printCurrentWindow();
            }

            // Reordering packets
            if(tempSeq == seqNumCounter) {
                receivedBytes += bitData;
                cout << "Received bytes length: " << receivedBytes.length() << endl;
                seqNumCounter++;
            } else {
                packet newPacket = packet(packetNumber, tempSeq, bitData, "", 1);
                unorderedPackets.push_back(newPacket);

                for (int i = 0; i < unorderedPackets.size(); ++i) {
                    if(unorderedPackets[i].getSeqNum() == seqNumCounter) {
                        receivedBytes += unorderedPackets[i].getBitContent();
                        cout << "Received bytes length: " << receivedBytes.length() << endl;
                        seqNumCounter++;
                        unorderedPackets.erase(unorderedPackets.begin() + i);
                    }
                }
            }
            if(seqNumCounter == seqNumberUpperBound){
                seqNumCounter = 0;
            }
        }


    }
    cout << "Unordered packets: " << unorderedPackets.size() << endl;
}

void SNW(tcp::socket& socket){
//	bool cksumFail = false;
    while(true){
        string recvPkt = getData(socket);
        bool cksumFail = false;
        if(recvPkt == "alldone=|||="){
            string done = "alldone";
            sendData(socket, done);
            stats();
            break;
        }
        parseReceivingPacket(recvPkt);
        if(printLog){cout << "Packet " << to_string(packetNumber) << " received" << endl;}
//	receivedBytes += bitData;
        string receivedCk = checksum(bitData);
        std::string s = addBinary(bitDataComp, receivedCk);
        if (s.find('0') != std::string::npos) {
            if(printLog){ cout << "Checksum failed" << endl;
                cout << "Current window = [1]" << endl;
            }string ack = "NACK";
            cksumFail = true;
            sendData(socket, ack);

        }
        //else checksum succeeds
        // If checksum is bad or lose ack
        for (int i = 0; i < packetsToLoseAck.size(); ++i) {
            if(packetNumber == packetsToLoseAck[i]) {
                packetsToLoseAck.erase(packetsToLoseAck.begin());
                string ack = "NACK";
                cksumFail = true;
                sendData(socket, ack);
            }
        }

        if(printLog && !cksumFail){
            cout << "Checksum OK" << endl;
        }
        // If checksum is good, send back ack
        if(!cksumFail){
            string ack = "ACK " + to_string(packetNumber);
            sendData(socket, ack);
        }
        if(printLog && !cksumFail){
            cout << "Ack " << to_string(packetNumber) << " sent"  << endl;
            cout << "Current window = [1]" << endl;
            receivedBytes += bitData;
        }
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
        cout<<"selected algo "<<selectedAlgorithm<<endl;
        case 0:{
            GBN(socket);
            break;
        }
        case 1:{
            SNW(socket);
            break;
        }
        case 2:{
            SR(socket);
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
    receiverInstance = setReceiverInstance(selectedAlgorithm, receiverMaxWindowSize, seqNumberLowerBound, seqNumberUpperBound, sizeOfPacket, selectedErrorType, errorPercentage, packetsToLoseAck, port);

    receiverSimulation();
    setBitsToFile(finalBits);

    cout << "All received bytes length: " << receivedBytes.length() << endl;
    setBitsToFile(receivedBytes);

    return 0;
}
