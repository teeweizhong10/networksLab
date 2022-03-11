//
// Created by Wei Zhong Tee on 3/6/22.
//

#include <iostream>

#include "inputManager.h"

using namespace std;

void inputManager::welcomeMessage() {
    cout << "****** Welcome to CS462: Computer Networks Lab assignment. ******" << endl;
    cout << "Answer the following prompts to set up the configuration for the sender and receivers sessions. " << endl;
}

void inputManager::getInput() {
    int selectedAlgorithm; // 0 for GBN, 1 for stop and Wait, 2 for SR
    int senderMaxWindowSize;
    int receiverMaxWindowSize;
    int sizeOfPacket;
    int seqNumberUpperBound;
    int seqNumberLowerBound;
    int staticOrDynamic; // 0 for static, 1 for dynamic
    int staticSeconds;
    int dynamicRoundTripTimeMultiplier;
    int roundTripTimeMultiplier;
    int selectedErrorType;
    int errorPercentage; //0 if none
    std::vector<int> packetsToDrop; //empty if none
    std::vector<int> packetsToLoseAck; //empty if none
    std::vector<int> packetsToFailChecksum; //empty if none

    welcomeMessage();
}