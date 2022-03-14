//
// Created by Wei Zhong Tee on 3/6/22.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <stdio.h>

using namespace std;

class Receiver {
private:
    int selectedAlgorithm; // 0 for GBN, 1 for stop and Wait, 2 for SR
    int receiverMaxWindowSize;
    int sizeOfPacket;
    int seqNumberUpperBound;
    int seqNumberLowerBound;
    int staticOrDynamic; // 0 for static, 1 for dynamic
    int staticSeconds;
    int dynamicRoundTripTimeMultiplier;
    int selectedErrorType; // 0 for none, 1 for specific packets, 2 for percentage
    int errorPercentage; //0 if none
    vector<int> packetsToLoseAck; //empty if none
    vector<int> packetsToFailChecksum; //empty if none
    // packets to fail checksum, fail to send ack done in receiver

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

    void setStaticOrDynamic (int input) {staticOrDynamic = input;};
    int getStaticOrDyanamic() {return staticOrDynamic;};

    void setStaticSeconds(int input) {staticSeconds = input;};
    int getStaticSeconds() {return staticSeconds;};

    void setDynamicRoundTripMultiplier(int input) {dynamicRoundTripTimeMultiplier = input;};
    int getDynamicRoundTripMultiplier() {return dynamicRoundTripTimeMultiplier;};

    void setErrorType(int input) {selectedErrorType = (input - 1);};
    int getErrorType() {return selectedErrorType;};

    void setErrorPercentage(int input) {errorPercentage = input;};
    int getErrorPercentage() {return errorPercentage;};

    void setPacketsToDrop(vector<int> input) {packetsToDrop = input;};
    vector<int>& getPacketsToDrop() {return packetsToDrop;}

    void setFilePath(string input) {filePath = input;};
    string getFilePath() {return filePath;};
};

int selectedAlgorithm;
int receiverMaxWindowSize;
int sizeOfPacket;
int seqNumberUpperBound;
int seqNumberLowerBound;
int staticOrDynamic; // 0 for static, 1 for dynamic
int staticSeconds;
int dynamicRoundTripTimeMultiplier;
int selectedErrorType;
int errorPercentage; //0 if none
vector<int> packetsToLoseAck; //empty if none
vector<int> packetsToFailChecksum; //empty if none
string filePath;
void receiverWelcomeMessage() {
    cout << "Creating instance for: receiver." << endl;
}

int main() {
    receiverWelcomeMessage();
    return 0;
}
