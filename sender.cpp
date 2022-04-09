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
#include <queue>
#include <deque>
#include <list>


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
float numOfRetransmittedPackets=0;
vector<int> packetsToDrop; //empty if none
vector<int> packetsToLoseAck; //empty if none
vector<int> packetsToFailChecksum; //empty if none
string filePath;
int testingBitsTransferred = 0;
string allBits;
string contentToSend;
int numOfPackets = 0;
time_point<Clock> startTimer;
time_point<Clock> endTimeInSeconds;
string ipAddr;
int port;
queue<packet> q;
vector<string> acksRecv;
int seqNumCounter = 0;
bool printLog = true;
//string ack = "";
vector<packet> packets;
time_point<Clock> start;
milliseconds latency;
milliseconds waitTime = milliseconds(10);
vector<char> tempBytes;
int seqNumCtr = 0;
int packetCtr = 0;
vector<packet> packetsAr;
//vector<string> acksRecv;


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
    cout << "Receiver Window Size: " << currentSender.getSenderMaxWindowSize() << endl;
    contentToSend += std::to_string(currentSender.getSenderMaxWindowSize());
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
            contentToSend += "\n";
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
    boost::asio::read_until( socket, buf, "=|||=");
    string data =  boost::asio::buffer_cast<const char*>(buf.data());
    return data;
}

void sendData(tcp::socket& socket, const string& msg){
    boost::asio::socket_base::send_buffer_size option(sizeOfPacket + 40);
    socket.set_option(option);
    int size = option.value();
    const string& temp = msg + "=|||=";
    boost::asio::write(socket, buffer(temp));
}

void stats(){
    cout << "Number of original packets sent: _____\nNumber of retransmitted packets:______\nTotal elapsed time:_____\ntotal throughput (Mbps):_______\nEffective throughput:______" << endl;
}

void setNumberOfPackets(int fileSizeBytes, int sizeOfPackets) {
    if(fileSizeBytes%sizeOfPacket > 0) {
        numOfPackets = fileSizeBytes/sizeOfPacket + 1;
    } else {
        numOfPackets = fileSizeBytes/sizeOfPacket;
    }
    cout << "Num of packets: " << numOfPackets << endl;
}

bool notTimedOut(milliseconds currentTime) {
    if(currentTime.count() >= waitTime.count()) {
        return false;
    }
    return true;
}


void fillTemp(vector<char>& bytes){
    for (int i=0; i<bytes.size(); i++){
        tempBytes.push_back(bytes[i]);
    }
}


int fillQ(int packetCounter){
    packet newPacket;

    if(seqNumCounter == seqNumberUpperBound){
        seqNumCounter = 0;
    }


    //for how many packets can fit in queue
    //while queue is not empty
    while((q.size() < senderMaxWindowSize) && !tempBytes.empty()){
        string byteContent;

        if (tempBytes.size() >= sizeOfPacket) {
            string s(tempBytes.begin(), tempBytes.begin()+sizeOfPacket);
            byteContent = s;
            tempBytes.erase(tempBytes.begin(), tempBytes.begin()+sizeOfPacket);//remove stored bytes


        } else {
            string s(tempBytes.begin(), tempBytes.end());
            byteContent = s;
            tempBytes.erase(tempBytes.begin(), tempBytes.end());//remove stored bytes
        }

        //create the packet and add to queue
        newPacket = packet(packetCounter, seqNumCounter, byteContent, getChecksumVal(byteContent), 0);
        q.push(newPacket);
        seqNumCounter++;
        packetCounter++;

        //TODO: shouldn't seq num counter have an if statement here to restart it?
//        if (seqNumCounter == seqNumberUpperBound) {
//            seqNumCounter = 0;
//        }
    }

    return packetCounter;
}


void printAllQ(){
    queue<packet> printQ = q;

    int i = 0;
    while(i < printQ.size()){
        cout << "Packet " << to_string(printQ.front().getPacketNum()) << " sent" << endl;
        printQ.pop();
    }
}

//GBN
int sendQ(tcp::socket& socket, int lastPktNum, bool sendingWholeQ){

    bool badPacket = false;
    //drop packet
    if(!packetsToDrop.empty()) {
        if(packetsToDrop[0] == q.front().getPacketNum()) {
            packetsToDrop.erase(packetsToDrop.begin());


            if(printLog){ cout << "Packet " << to_string(q.front().getPacketNum()) << " sent" << endl;}

            sleep_for(waitTime + milliseconds(1)); // Let it time out
            if (printLog) {
                cout << "Packet " << to_string(q.front().getPacketNum()) << " ***** Timed Out *****" << endl;
                cout << "Packet " << to_string(q.front().getPacketNum()) << " Retransmitted." << endl;
            }
        }
    }

    //corrupt packet
    if(!packetsToFailChecksum.empty()) {
        if (packetsToFailChecksum[0] == q.front().getPacketNum()){
            if(printLog && !sendingWholeQ){ cout << "Packet " << to_string(q.front().getPacketNum()) << " sent" << endl;
            }packetsToFailChecksum.erase(packetsToFailChecksum.begin());
            sendData(socket, q.front().getCorruptedPacketMessage());// send corrupted message
            badPacket = true;
        }
    }
    if(!badPacket){
        if(printLog && !sendingWholeQ) {
            cout << "Packet " << to_string(q.front().getPacketNum()) << " sent" << endl;
        }
        string temp = q.front().getPacketMessage();
        sendData(socket, temp);
        return q.front().getPacketNum();
    }
}



void printCurrentWindow(){
    cout << "Current window = [";
    queue<packet> SW = q;
    int i = 0;
    while(i < SW.size()){
        cout << " " << SW.front().getSeqNum();
        SW.pop();
    }
    cout << " ]" << endl;
}

//SR
void sendQueue(tcp::socket& socket){
    queue<packet> tempQ = q;
    cout << "TempQ " << sizeof(q) << endl;

    cout << "Sending queue..." << endl;

    int i  = 0;
    while( i < tempQ.size()){
        bool badPacket = false;
        //drop packet
        if(!packetsToDrop.empty()) {
            if(packetsToDrop[0] == tempQ.front().getPacketNum()) {
                packetsToDrop.erase(packetsToDrop.begin());
                if(printLog){ cout << "Packet " << to_string(tempQ.front().getPacketNum()) << " sent" << endl;}

                sleep_for(waitTime + milliseconds(1)); // Let it time out
                if (printLog) {
                    cout << "Packet " << to_string(tempQ.front().getPacketNum()) << " ***** Timed Out *****" << endl;
                    cout << "Packet " << to_string(q.front().getPacketNum()) << " Retransmitted." << endl;
                }
            }
        }

        //corrupt packet
        if(!packetsToFailChecksum.empty()) {
            if (packetsToFailChecksum[0] == tempQ.front().getPacketNum()){
                if(printLog){ cout << "Packet " << to_string(tempQ.front().getPacketNum()) << " sent" << endl;
                }packetsToFailChecksum.erase(packetsToFailChecksum.begin());
                sendData(socket, tempQ.front().getCorruptedPacketMessage());// send corrupted essage
                badPacket = true;
                tempQ.pop();
            }
        }
        if(!badPacket){
            sendData(socket, tempQ.front().getPacketMessage());
            if(printLog){
                cout << "Packet " << to_string(tempQ.front().getPacketNum()) << " sent" << endl;
            }
            tempQ.pop();
        }
    }
}



//*************************************************************************************************************************
void GBN(tcp::socket& socket, vector<char>& bytes){
    fillTemp(bytes);
    int lastPkNumSent;
    int packetCounter = 0;
    packetCounter = fillQ( packetCounter);
    bool allDone = false;
    bool firstRun = true;
    int printerCounter = 0;
    bool printOnce = true;

    while((packetCounter <  (numOfPackets+senderMaxWindowSize)) && !allDone){
        packetCounter = fillQ( packetCounter);
        if(firstRun){
            if(printOnce){
                printAllQ();
                printOnce = false;
            }
            printerCounter++;
            lastPkNumSent = sendQ(socket, lastPkNumSent, true);
        }else{
            lastPkNumSent = sendQ(socket, lastPkNumSent, false);
        }
        string temp = getData(socket);
        if(printerCounter == senderMaxWindowSize){
            firstRun = false;
            printerCounter = 0;
        }

        if(temp == "ACK " + to_string(q.front().getPacketNum()) + "=|||="){
            if(printLog){cout << "ACK " << to_string(q.front().getPacketNum()) << " received" << endl;
            }
            q.pop();
            packetCounter = fillQ( packetCounter);
            if(printLog){
                printCurrentWindow();
            }
        }else{
            firstRun = true;
            printOnce = true;
            printerCounter = 0;
        }
        if(temp == ("ACK " + to_string((numOfPackets-1)) + "=|||=")){
            allDone = true;
        }
    }


    sendData(socket, "alldone");
    string recvPkt = getData(socket);
    if(recvPkt == "alldone=|||="){
        socket.close();
    }

}

void fillQ(){
    int seqNumCounter = 0;

    packet newPacket;
    if(seqNumCounter == seqNumberUpperBound){
        seqNumCounter = 0;
    }

    //for how many packets can fit in queue
    //while queue is not empty
    while((q.size() < senderMaxWindowSize) && !tempBytes.empty()){
        string byteContent;

        if (tempBytes.size() >= sizeOfPacket) {
            string s(tempBytes.begin(), tempBytes.begin()+sizeOfPacket);
            byteContent = s;
            tempBytes.erase(tempBytes.begin(), tempBytes.begin()+sizeOfPacket);//remove stored bytes


        } else {
            string s(tempBytes.begin(), tempBytes.end());
            byteContent = s;
            tempBytes.erase(tempBytes.begin(), tempBytes.end());//remove stored bytes
        }

        //create the packet and add to queue
        newPacket = packet(packetCtr, seqNumCounter, byteContent, getChecksumVal(byteContent), 0);
        q.push(newPacket);
        seqNumCounter++;
        packetCtr++;
    }



}

//*************************************************************************************************************************
void printCurrentWindowSR(int SN){
    cout << "Current window: [ ";
    for(int i = 0; i < packetsAr.size(); i++){
        cout << to_string(packetsAr[i].getSeqNum()) << " ";
    }
    cout << "]" << endl;
}



void fillArray(){
    string byteContent = "";
    packet newPacket;
    if(seqNumCtr >= seqNumberUpperBound){
        seqNumCtr = 0;
    }

    while(packetsAr.size() < senderMaxWindowSize && !tempBytes.empty()){

        if (tempBytes.size() >= sizeOfPacket) {
            string s(tempBytes.begin(), tempBytes.begin()+sizeOfPacket);
            byteContent = s;
            tempBytes.erase(tempBytes.begin(), tempBytes.begin()+sizeOfPacket);//remove stored bytes
        } else {
            string s(tempBytes.begin(), tempBytes.end());
            byteContent = s;
            tempBytes.erase(tempBytes.begin(), tempBytes.end());//remove stored bytes
        }

        newPacket = packet(packetCtr, seqNumCtr, byteContent, getChecksumVal(byteContent), 0);
        packetsAr.push_back(newPacket);
        packetCtr+=1;
//	cout << "PACKET COUNTER: " << to_string(packetCtr) << endl;
        seqNumCtr+=1;
//	cout << "SEQ COUNTER: " << to_string(seqNumCtr) << endl;
    }

}

void sendPacket(tcp::socket& socket, packet item){
//drop packet
    bool badPacket = false;
    if(!packetsToDrop.empty()) {
        if(packetsToDrop[0] == item.getPacketNum()) {
            acksRecv.push_back("NACK " + to_string(packetsToDrop[0]) + "=|||=");
            packetsToDrop.erase(packetsToDrop.begin());
            badPacket = true;
        }
    }

    //corrupt packet
    if(!packetsToFailChecksum.empty()) {
        if (packetsToFailChecksum[0] == item.getPacketNum()){
            packetsToFailChecksum.erase(packetsToFailChecksum.begin());
            sendData(socket, item.getCorruptedPacketMessage());
            acksRecv.push_back(getData(socket));
            badPacket = true;
        }
    }


    if(!badPacket){
        sendData(socket, item.getPacketMessage());
        acksRecv.push_back(getData(socket));
    }
}


void SR(tcp::socket& socket, vector<char> bytes){
    fillTemp(bytes);
    int pktCtr = 0;

    //printContents();

    fillArray();
    for(int p = 0; p < packetsAr.size();p++){
        string ack = "";
        sendPacket(socket, packetsAr[p]);
        cout << "Packet " << to_string(packetsAr[p].getPacketNum()) << " sent" << endl;
    }


    //printContents();
    while(pktCtr != numOfPackets){
        int i = 0;
        string nack = "NACK " + to_string(packetsAr[i].getPacketNum()) + "=|||=";
        string ack = "ACK " + to_string(packetsAr[i].getPacketNum()) + "=|||=";

        //cout << "I: " << i << endl;
        //cout << "PACKET: " << to_string(packetsAr[i].getPacketNum()) << endl;
        //cout << "size of ACKS: " << to_string(acksRecv.size()) << endl;
        //cout << "size of PACKETS: " << to_string(packetsAr.size()) << endl;


        //if packet has not been sent yet
        if((count(acksRecv.begin(), acksRecv.end(), nack) <= 0) && (count(acksRecv.begin(), acksRecv.end(), ack) <= 0)){
            sendPacket(socket, packetsAr[i]);
            cout << "Packet " << to_string(packetsAr[i].getPacketNum()) << " sent" << endl;
        }

        //if packet sent but was error
        if(count(acksRecv.begin(), acksRecv.end(), nack)>0){
            sleep_for(waitTime + milliseconds(1));

            cout << "Packet " << to_string(packetsAr[i].getPacketNum()) << " ***** Timed Out *****" << endl;
            cout << "Packet " << to_string(packetsAr[i].getPacketNum()) << " Re-transmitted" << endl;

            sendPacket(socket, packetsAr[i]);
        }

        //if packet received
        if(count(acksRecv.begin(), acksRecv.end(), ack)>0){
            cout << "ACK " << to_string(packetsAr[i].getPacketNum()) << " received" << endl;
            // cout << "Before erase: " << to_string(acksRecv.size()) << endl;
            //    acksRecv.erase(acksRecv.begin());
            //   cout << "After erase: " << to_string(acksRecv.size()) << endl;
            packetsAr.erase(packetsAr.begin());
            pktCtr++;
            //		    cout << "FILL AR" << endl;
            fillArray();
//		printContents();
            printCurrentWindowSR(packetsAr[i].getSeqNum());
        }

        // }
    }

    sendData(socket, "alldone");
    string tmp = getData(socket);
    if(tmp ==  "alldone=|||="){
        socket.close();
    }
}

void SNW(tcp::socket& socket, vector<char>& bytes){
    cout << endl;
    string byteContent;
    string allBytesBefore(bytes.begin(), bytes.end());
    string allBytesAfter = "";

    string receivedBytes = "";
    int packetCounter = 0;
    int seqNumCounter = 0;
    bool packetSent = false;
    packet newPacket;

    while(packetCounter != numOfPackets) {
        bool receivedAck = false;
        if (bytes.size() >= sizeOfPacket) {
            string s(bytes.begin(), bytes.begin()+sizeOfPacket);
            byteContent = s;
        } else {
            string s(bytes.begin(), bytes.end());
            byteContent = s;
        }

        newPacket = packet(packetCounter, seqNumCounter, byteContent, getChecksumVal(byteContent), 0);

        packetSent = false;
        time_point<Clock> timeoutStart = Clock::now();
        milliseconds currentTimeCount = duration_cast<milliseconds>(milliseconds (0)- milliseconds(0));


        while (notTimedOut(currentTimeCount)) {
            bool retransmitted = false;

            currentTimeCount += waitTime;
            //drop packet
            if(!packetsToDrop.empty()) {
                if(packetsToDrop[0] == packetCounter) {
                    packetsToDrop.erase(packetsToDrop.begin());
                    sleep_for(waitTime + milliseconds(1)); // Let it time out
                    if (printLog) {
                        cout << "Packet " << packetCounter << " sent" << endl;
                        cout << "Packet " << packetCounter << " ***** Timed Out *****" << endl;
                        retransmitted = true;
                    }
                }
            }

            //corrupt packet
            if(!packetsToFailChecksum.empty()) {
                if (packetsToFailChecksum[0] == packetCounter){
                    packetsToFailChecksum.erase(packetsToFailChecksum.begin());
                    sendData(socket, newPacket.getCorruptedPacketMessage());// send corrupted message
                    packetSent = true;
                    string ack = getData(socket); // Receiver gets good packet
                    if(ack == "ACK " + to_string(newPacket.getPacketNum()) + "=|||=") {
                        receivedAck = true;
                    } // Receiver gets corrupted packet (won't happen)
                }
            }

            if (!packetSent) {
                sendData(socket, newPacket.getPacketMessage()); // send packet
                if (printLog && !retransmitted) {
                    cout << "Packet " << packetCounter << " sent" << endl;
                }else if(printLog && retransmitted){
                    cout << "Packet " << packetCounter << " Re-transmitted." << endl;
                }
                packetSent = true;

                string ack = getData(socket); // Receiver gets good packet
                if(ack == "ACK " + to_string(newPacket.getPacketNum()) + "=|||=") {
                    receivedAck = true;
                }
            }

            if(receivedAck) {
                if (printLog) {
                    cout << "ACK " << packetCounter << " received." << endl;
                    cout << "Current window = [1]" << endl;
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
                    cout << "Packet " << packetCounter << " *****Timed Out *****" << endl;
                    cout << "Packet " << packetCounter << " Re-transmitted" << endl;
                    retransmitted = true;
                    newPacket = packet();
                }

            }

            time_point<Clock> timeoutend = Clock::now();
            currentTimeCount = duration_cast<milliseconds>(timeoutend - start);
            if(receivedAck) {
                currentTimeCount = waitTime;
            }
        }
    }

    sendData(socket, "alldone");
    string recvPkt = getData(socket);
    if(recvPkt == "alldone=|||="){
        cout <<"alldone received" << endl;
        socket.close();
    }
}
//*************************************************************************************************************************
void beginTransaction(vector<char>& bytes){
    string response;
    cout << "IP: " << ipAddr << endl;
    cout << "PORT: " << port << endl;
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket socket(io_service);
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string(ipAddr), port));
    startTotalTimer();


    //first packet sent is config
    contentToSend += to_string(numOfPackets);
    contentToSend += "\n";
    //cout << "contentToSend:  " << contentToSend << endl;
    sendData(socket, contentToSend);
    response = getData(socket);

    //Begin Transaction is sent after config
    sendData(socket, "Begin transaction...");
    response = getData(socket);
    cout<< "got response from receiver" << endl;

    if(response == "Begin transaction...=|||="){
        cout << "Begin transaction..." << endl;
        switch(selectedAlgorithm){
            case 1:{
                GBN(socket, bytes);
                break;
            }
            case 2:{
                SNW(socket, bytes);
                break;
            }
            case 3:{
                SR(socket, bytes);
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
    // read bytes from file
    std::ifstream input(filePath, std::ios::binary);

    cout << "\n\nReading the file " << filePath << "..." << endl;
    std::vector<char> bytes(
            (std::istreambuf_iterator<char>(input)),
            (std::istreambuf_iterator<char>()));
    input.close();

    cout << "File size in bytes: " << bytes.size() << endl;
    int file_size = bytes.size();
    setNumberOfPackets(bytes.size(), sizeOfPacket);
    //cout<<"set number of packets"<<endl;
    if (selectedErrorType == 2) { // set errors for random percentage
        setPacketErrors(errorPercentage, numOfPackets);
    }
    showCurrentConfig(senderInstance);
    //update numOfRetansmittedPackets
    numOfRetransmittedPackets+=packetsToDrop.size();
    numOfRetransmittedPackets+=packetsToLoseAck.size();
    numOfRetransmittedPackets+=packetsToFailChecksum.size();

    start = Clock::now(); // for total elapsed time
    cout << "\n************ Protocol work ************" << endl;
    beginTransaction(bytes);


    //refactorSenderStopAndWait(filePath);
    cout << endl;

    time_point<Clock> end = Clock::now();
    milliseconds totalElapsedTime = duration_cast<milliseconds>(end - start);
    cout << "number of original packets sent: "<<numOfPackets<<endl;
    cout << "number of retransmitted packets: "<<numOfRetransmittedPackets<<endl;
    cout << "Total elapsed time: " << totalElapsedTime.count() << "ms" << std::endl;
    //mbps = 8(filesize/(totalElapsedTime.count()/1000))
    //int MbpsWithErrors = 8*((file_size)/(totalElapsedTime.count()/1000));
    //TODO: Is size of packet in bits or bytes?
    int MbpsWithErrors = file_size + (numOfRetransmittedPackets*sizeOfPacket);
    MbpsWithErrors /= 100000;
    MbpsWithErrors /= (totalElapsedTime.count()/1000);
    MbpsWithErrors *= 8;
    MbpsWithErrors /= 2;
    cout << "Throughput: " << MbpsWithErrors << " Mbps" << endl;

    int MbpsWithoutErrors = file_size - (numOfRetransmittedPackets*sizeOfPacket);
    MbpsWithoutErrors /= 100000;
    MbpsWithoutErrors /= (totalElapsedTime.count()/1000);
    MbpsWithoutErrors *= 8;
    MbpsWithoutErrors /= 2;
    cout << "Effective Throughput: " << MbpsWithoutErrors <<" Mbps" << endl;

    cout << "All received bytes length: " << file_size << endl;
}
