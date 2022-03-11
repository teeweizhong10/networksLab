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
    std::vector<string> allInput; //empty if none
    welcomeMessage();

    int input;
    // Select protocol
    cout << endl << "Choose your selected algorithm, enter the corresponding number to make your choice: \n1: Go Back N \n2: Stop and Wait \n3: Selective Repeat" << endl;
    cout << "Input: ";
    cin >> input;
    cout << "You chose: ";
    switch (input-1) {
        case 0:
            cout << "Go Back N" << endl;
            break;
        case 1:
            cout << "Stop and Wait" << endl;
            break;
        case 2:
            cout << "Selective Repeat" << endl;
            break;
    }
    allInput.push_back(std::to_string(input));
    allInput.push_back("\n");

    // Sender and receiver window sizes
    switch (input-1) {
        case 0:
            cout << endl << "Choose the sender window size: " << endl;
            cout << "Input: ";
            cin >> input;
            cout << "You chose: " << input << endl;
            allInput.push_back(std::to_string(input)); // Set sender window size to user input
            allInput.push_back("\n");
            allInput.push_back(std::to_string(1)); // Set receiver window size to 1
            allInput.push_back("\n");
            break;
        case 1:
            allInput.push_back(std::to_string(1)); // Set sender window size to 1
            allInput.push_back("\n");
            allInput.push_back(std::to_string(1)); // Set receiver window size to 1
            allInput.push_back("\n");
            break;
        case 2:
            cout << endl << "Choose the sender window size: " << endl;
            cout << "Input: ";
            cin >> input;
            cout << "You chose: " << input << endl;
            allInput.push_back(std::to_string(input)); // Set sender window size to user input
            allInput.push_back("\n");
            cout << endl << "Choose the receiver window size: " << endl;
            cout << "Input: ";
            cin >> input;
            cout << "You chose: " << input << endl;
            allInput.push_back(std::to_string(input)); // Set receiver window size to user input
            allInput.push_back("\n");
            break;
    }

    // Size of packet
    cout << endl << "Choose the size of packets in bits: " << endl;
    cout << "Input: ";
    cin >> input;
    cout << "You chose: " << input << endl;
    allInput.push_back(std::to_string(input)); // Set size of packets to user input
    allInput.push_back("\n");

    // seqNum Upper Bound
    cout << endl << "Choose the upper bound of the sequence numbers: " << endl;
    cout << "Input: ";
    cin >> input;
    cout << "You chose: " << input << endl;
    allInput.push_back(std::to_string(input)); // Set upper bound to user input
    allInput.push_back("\n");

    // seqNum Lower Bound
    cout << endl << "Choose the lower bound of the sequence numbers: " << endl;
    cout << "Input: ";
    cin >> input;
    cout << "You chose: " << input << endl;
    allInput.push_back(std::to_string(input)); // Set lower bouond to user input
    allInput.push_back("\n");

    cout << "\nCurrent input lines: " << endl;
    for (int i = 0; i < allInput.size(); ++i) {
        cout << allInput[i];
    }
}