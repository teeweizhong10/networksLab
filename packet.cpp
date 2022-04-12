//
// Created by Wei Zhong Tee on 3/6/22.
//

#include <iostream>
#include <string>
#include "packet.h"

packet::packet() {
    packetNum = -1;
    seqNum = -1;
    bitContent = "";
    checksumValue = -1;
    packetMessage = "";
    ackReceived = -1;
}

packet::packet(int packetNum, int seqNum, string bitContent, string checksumValue, int ackReceived) {
    setPacketNum(packetNum);
    setSeqNum(seqNum);
    setBitContent(bitContent);
    setChecksumValue(checksumValue);
    setAckReceived(ackReceived);
    setPacketMessage();
//    this->packetNum = packetNum;
//    this->seqNum = seqNum;
//    this->bitContent = bitContent;
//    this->checksumValue = checksumValue;
}

void packet::setPacketMessage() {
    string message =std::to_string(packetNum);
    message += "=||=";
    message += std::to_string(seqNum);
    message += "=||=";
    message += bitContent;
    message += "=||=";
    message += checksumValue;
    message += "=||=";
    message += std::to_string(ackReceived);
    message += "=|||=";
    packetMessage = message;
}

string packet::getCorruptedPacketMessage() {
    string corruptBitContent = bitContent;
    char char_array[corruptBitContent.length() + 1];
    strcpy(char_array, corruptBitContent.c_str());
    if (char_array[0] == '1') {
        corruptBitContent[0] = '0';
    } else {
        corruptBitContent[0] = '1';
    }

    string message =std::to_string(packetNum);
    message += "=||=";
    message += std::to_string(seqNum);
    message += "=||=";
    message += corruptBitContent;
    message += "=||=";
    message += checksumValue;
    message += "=||=";
    message += std::to_string(ackReceived);
    message += "=|||=";
    string corruptPacketMessage = message;
    return corruptPacketMessage;
}

