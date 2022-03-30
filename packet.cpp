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
    message += "|";
    message += std::to_string(seqNum);
    message += "|";
    message += bitContent;
    message += "|";
    message += checksumValue;
    message += "|";
    message += std::to_string(ackReceived);
    packetMessage = message;
}

string packet::getCorruptedPacketMessage() {
    char char_array[bitContent.length() + 1];
    strcpy(char_array, bitContent.c_str());
    if (char_array[0] == '1') {
        bitContent.replace(0,1,"0");
    } else {
        bitContent.replace(0,1,"1");
    }

    string message =std::to_string(packetNum);
    message += "|";
    message += std::to_string(seqNum);
    message += "|";
    message += bitContent;
    message += "|";
    message += checksumValue;
    message += "|";
    message += std::to_string(ackReceived);
    packetMessage = message;
    return packetMessage;
}

