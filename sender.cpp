//
// Created by Wei Zhong Tee on 3/6/22.
//

#include <iostream>
#include <vector>
#include <fstream>
using namespace std;

class Sender {
private:
    int selectedAlgorithm;
    int senderMaxWindowSize;
    int sizeOfPacket;
    int seqNumberUpperBound;
    int seqNumberLowerBound;
    int staticOrDynamic; // 0 for static, 1 for dynamic
    int roundTripTimeMultiplier;
    int selectedErrorType;
    int errorPercentage; //0 if none
    std::vector<int> packetsToDrop; //empty if none

public:
    void setAlgorithmType(int input) {selectedAlgorithm = (input - 1);};
    int getSelectedAlgorithm() {return selectedAlgorithm;};

    void setSenderMaxWindowSize(int input) {senderMaxWindowSize = input;};
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
    int getErrorType() {return selectedErrorType;};
};

int selectedAlgorithm;
int senderMaxWindowSize;
int sizeOfPacket;
int seqNumberUpperBound;
int seqNumberLowerBound;
int staticOrDynamic; // 0 for static, 1 for dynamic
int roundTripTimeMultiplier;
int selectedErrorType;
int errorPercentage; //0 if none
std::vector<int> packetsToDrop; //empty if none

void senderWelcomeMessage() {
    cout << "Creating instance for: sender." << endl;
}

void getNetworkConfigFrom(string fileName) {
    ifstream inputFile;
    inputFile.open(fileName);
    if (inputFile.is_open() ) {
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
            }
            itemCount++;
        }
    }
}

void showCurrentConfig(Sender currentSender) {
    cout << "Current sender configuration: " << endl;
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
}

Sender setSenderInstance(int selectedAlgorithm, int senderMaxWindowSize) {
    Sender senderInstance;
    senderInstance.setAlgorithmType(selectedAlgorithm);
    senderInstance.setSenderMaxWindowSize(senderMaxWindowSize);
    return senderInstance;
}

int main() {
    Sender senderInstance;
    senderWelcomeMessage();
    getNetworkConfigFrom("config.txt");
    senderInstance = setSenderInstance(selectedAlgorithm, senderMaxWindowSize);
    showCurrentConfig(senderInstance);
    return 0;
}