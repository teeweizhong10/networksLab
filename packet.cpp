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
}

packet::packet(int packetNum, int seqNum, string bitContent, string checksumValue) {
    setPacketNum(packetNum);
    setSeqNum(seqNum);
    setBitContent(bitContent);
    setChecksumValue(checksumValue);
}

void packet::setPacketMessage() {
    string message =std::to_string(packetNum);
    message += "|";
    message += std::to_string(seqNum);
    message += "|";
    message += bitContent;
    message += "|";
    message += checksumValue;
    packetMessage = message;
}

