//
// Created by Wei Zhong Tee on 3/6/22.
//

/*
 * config.txt integers:
 * maxSeqNum
 * algorithm: 0 for GBN, 1 for Stop and Wait, 2 for SR
 * senderMaxWindowSize
 * receiverMaxWindowSize
 * sizeOfPacket
 * seqNumberRangeBits
 * staticOrDyanmic
 * roundTripTimeMultiplier
 * errorType: 0 for none, 1 for random, 2 for specific, 3 for percentage
 * errorPercentage: 0 if not used
 * packetsToDrop: position, position
 * packetsToLoseAck: position, position
 * packetsThatFailChecksum: position, position

 */

#ifndef NETWORKSLAB_INPUTMANAGER_H
#define NETWORKSLAB_INPUTMANAGER_H


class inputManager {
private:
    int maxSeqNum;
    enum algorithm {
        GBN,
        stopAndWait,
        SR
    };
    algorithm selectedAlgoritm;
    int senderMaxWindowSize;
    int receiverMaxWindowSize;
    int sizeOfPacket;
    int seqNumRangeBits;
    int staticOrDynamic; // 0 for static, 1 for dynamic
    int roundTripTimeMultiplier;
    enum errorType {
        none,
        random,
        specificPackets,
        percentage
    };
    errorType selectedErrorType;

public:
    void setMaxSeqNum();
    int getMaxSeqNum() {return maxSeqNum;};

    void setAlgorithmType();
    algorithm returnAlgorithm() {return selectedAlgoritm;};

    void setSenderMaxWindowSize();
    int getSenderMaxWindowSize() {return senderMaxWindowSize;};

    void setReceiverMaxWindowSize();
    int getReceiverMaxWindowSize() {return receiverMaxWindowSize;};

    void setSizeOfPacket();
    int getSizeOfPacket() {return sizeOfPacket;};

    void setStaticOrDynamic();
    int getStaticOrDyanamic() {return staticOrDynamic;};

    void setRoundTripMultiplier();
    int getRoundTripMultiplier() {return roundTripTimeMultiplier;};

    void setErrorType();
    errorType getErrorType() {return selectedErrorType;};
};


#endif //NETWORKSLAB_INPUTMANAGER_H
