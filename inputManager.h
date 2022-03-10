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
 * roundTripTimeMultiplier
 * errorType: 0 for none, 1 for random, 2 for specific, 3 for percentage
 * errorPercentage: 0 if not used
 * packetsToDrop: position, position //empty if none
 * packetsToLoseAck: position, position //empty if none
 * packetsToFailChecksum: position, position //empty if none

 */

#ifndef NETWORKSLAB_INPUTMANAGER_H
#define NETWORKSLAB_INPUTMANAGER_H

#include <vector>
using std::vector;
class inputManager {
private:
    enum algorithm {
        GBN,
        stopAndWait,
        SR
    };
    algorithm selectedAlgoritm;
    int senderMaxWindowSize;
    int receiverMaxWindowSize;
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
    std::vector<int> packetsToLoseAck; //empty if none
    std::vector<int> packetsToFailChecksum; //empty if none



public:
    void welcomeMessage();

    void setAlgorithmType();
    algorithm getSelectedAlgorithm() {return selectedAlgoritm;};

    void setSenderMaxWindowSize();
    int getSenderMaxWindowSize() {return senderMaxWindowSize;};

    void setReceiverMaxWindowSize();
    int getReceiverMaxWindowSize() {return receiverMaxWindowSize;};

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

    bool goodInputForNumbers(int input);
    bool goodInputForAlgorithmSelection(int input);
    bool goodInputForErrorSelection(int input);

    void getInput();
    void writeInputToFile(algorithm selectedAlgorithm, int senderMaxWindowSize, int receiverMaxWindowSize, int sizeOfPacket, int seqNumberUpperBound, int seqNumberLowerBound, int staticOrDynamic, int roundTripTimeMultiplier,  errorType selectedErrorType, int errorPercentage, std::vector<int> packetsToDrop, std::vector<int> packetsToLoseAck, std::vector<int> packetsToFailChecksum);
};


#endif //NETWORKSLAB_INPUTMANAGER_H
