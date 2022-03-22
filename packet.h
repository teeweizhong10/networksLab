//
// Created by Wei Zhong Tee on 3/6/22.
//
#include <string>
#include <cstring>
#ifndef NETWORKSLAB_PACKET_H
#define NETWORKSLAB_PACKET_H

using namespace std;

class packet {
private:
    int packetNum;
    int seqNum;
    string bitContent;
    string checksumValue;
    string packetMessage;

public:
    packet();
    packet(int packetNum, int seqNum, string bitContent, string checksumValue);

    void setPacketNum(int input) {packetNum = input;};
    int getPacketNum() {return packetNum;};

    void setSeqNum(int input) {seqNum = input;};
    int getSeqNum() {return seqNum;};

    void setBitContent (string bitInput) {bitContent = bitInput;};
    string getBitContent() {return bitContent;};

    void setChecksumValue(string input) {checksumValue = input;};
    string getChecksumValue() {return checksumValue;};

    void setPacketMessage();
    string getPacketMessage() {return packetMessage;};
};

/* Plan for packet structure that will be sent
 * Packet message: "packetNum|seqNum|bitContent|checksumVal"
 * "2|23|0101010101000|010100"
 * */

#endif //NETWORKSLAB_PACKET_H
