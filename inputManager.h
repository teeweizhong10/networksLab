//
// Created by Wei Zhong Tee on 3/6/22.
//

/*
 * config.txt integers:
 * algorithm: 0 for GBN, 1 for Stop and Wait, 2 for SR
 * senderMaxWindowSize
 * receiverMaxWindowSize
 * sizeOfPacket
 * seqNumberUpperBound
 * seqNumberLowerBound
 * staticOrDyanmic
 * staticSeconds
 * dynamicRoundTripTimeMultiplier
 * errorType: 0 for none, 2 for specific, 3 for percentage
 * errorPercentage: 0 if not used
 * packetsToDrop: position, position //empty if none
 * packetsToLoseAck: position, position //empty if none
 * packetsToFailChecksum: position, position //empty if none
 */

#ifndef NETWORKSLAB_INPUTMANAGER_H
#define NETWORKSLAB_INPUTMANAGER_H


#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <stdio.h>
using std::vector;
class inputManager {
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
    int roundTripTimeMultiplier;
    int selectedErrorType;
    int errorPercentage; //0 if none
    std::vector<int> packetsToDrop; //empty if none
    std::vector<int> packetsToLoseAck; //empty if none
    std::vector<int> packetsToFailChecksum; //empty if none

public:
    void welcomeMessage();

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

    void setErrorType(int input) {selectedErrorType = (input - 1);};
    int getErrorType() {return selectedErrorType;};

    void setErrorPercentage(int input) {errorPercentage = input;};
    int getErrorPercentage() {return errorPercentage;};

    void setPacketsToDrop(vector<int> input) {packetsToDrop = input;};
    vector<int>& getPacketsToDrop() {return packetsToDrop;}

    void setPacketsToLoseAck(vector<int> input) {packetsToLoseAck = input;};
    vector<int>& getPacketsToLoseAck() {return packetsToLoseAck;}

    void setPacketsToFailChecksum(vector<int> input) {packetsToFailChecksum = input;};
    vector<int>& getPacketsToFailChecksum() {return packetsToFailChecksum;}

    void getInput();
    void writeInputToFile(vector<std::string> configContent);
};


#endif //NETWORKSLAB_INPUTMANAGER_H
