//
// Created by Wei Zhong Tee on 3/6/22.
//

#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

class Sender {
private:
    enum algorithm {
        GBN,
        stopAndWait,
        SR
    };
    algorithm selectedAlgorithm;
    int senderMaxWindowSize;
    int sizeOfPacket;
    int seqNumberUpperBound;
    int seqNumberLowerBound;
    int staticOrDynamic; // 0 for static, 1 for dynamic
    int roundTripTimeMultiplier;
    enum errorType {
        none,
        random,
        specificPackets,
        percentage
    };
    errorType selectedErrorType;
    int errorPercentage; //0 if none
    std::vector<int> packetsToDrop; //empty if none

public:
    void setAlgorithmType(int input) {
        input = input - 1;
        if (input == 0) {
            selectedAlgorithm = GBN;
        } else if (input == 1) {
            selectedAlgorithm = stopAndWait;
        } else {
            selectedAlgorithm = SR;
        }
    };
    algorithm returnSelectedAlgorithm() {return selectedAlgorithm;};

    void setSenderMaxWindowSize();
    int getSenderMaxWindowSize() {return senderMaxWindowSize;};

    void setSizeOfPacket();
    int getSizeOfPacket() {return sizeOfPacket;};

    void setSeqNumberUpperBound();
    int getSeqNumberUpperBound() {return seqNumberUpperBound;};

    void setSeqNumberLowerBound();
    int getSeqNumberLowerBound() {return seqNumberLowerBound;};

    void setStaticOrDynamic();
    int getStaticOrDyanamic() {return staticOrDynamic;};

    void setRoundTripMultiplier();
    int getRoundTripMultiplier() {return roundTripTimeMultiplier;};

    void setErrorType();
    errorType getErrorType() {return selectedErrorType;};
};

void senderWelcomeMessage() {
    cout << "Creating instance for: sender." << endl;
}

void getNetworkConfigFrom(string fileName) {
    ifstream inputFile;
    inputFile.open(fileName);
    if (inputFile.is_open() ) {
        string line;
        while (getline(inputFile, line)) {
            int len = line.length();
            char lineChars[len + 1];
            int itemCount = 0;
            strcpy(lineChars, line.c_str());
            cout << lineChars << endl;
            cout << "New loop" << endl;
        }
    }
}

void showCurrentConfig() {

}

int main() {
    senderWelcomeMessage();
    getNetworkConfigFrom("config.txt");
    showCurrentConfig();
    return 0;
}