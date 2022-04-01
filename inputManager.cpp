//
// Created by Wei Zhong Tee on 3/6/22.
//

#include <iostream>
#include <iterator>

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
    cout << endl << "Choose your selected protocol, enter the corresponding number to make your choice: \n1: Go Back N \n2: Stop and Wait \n3: Selective Repeat" << endl;
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
    cout << endl << "Choose the size of packets in bytes: " << endl;
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
//    cout << endl << "Choose the lower bound of the sequence numbers: " << endl;
//    cout << "Input: ";
//    cin >> input;
//    cout << "You chose: " << input << endl;
    allInput.push_back(std::to_string(0)); // Set lower bound to user input
    allInput.push_back("\n");

    // Static or dynamic
    cout << endl << "Choose the timeout interval type: \n1. Static \n2. Dynamic" << endl;
    cout << "Input: ";
    cin >> input;
    cout << "You chose: ";
    allInput.push_back(std::to_string(input-1)); // 0 for static, 1 for dynamic
    allInput.push_back("\n");
    switch (input-1) {
        case 0:
            cout << "Static" << endl;
            cout << endl << "Choose the time in milliseconds to wait for a timeout:" << endl;
            cout << "Input: ";
            cin >> input;
            cout << "You chose: " << input << endl;
            allInput.push_back(std::to_string(input)); // User input for static seconds
            allInput.push_back("\n");
            allInput.push_back(std::to_string(0)); // 0 for dynamic round trip multiplier
            allInput.push_back("\n");
            break;
        case 1:
            cout << "Dynamic" << endl;
            cout << endl << "Choose the round trip time multiplier:" << endl;
            cout << "Input: ";
            cin >> input;
            cout << "You chose: " << input << endl;
            allInput.push_back(std::to_string(0)); // 0 for static seconds
            allInput.push_back("\n");
            allInput.push_back(std::to_string(input)); // User input for dynamic round trip multiplier
            allInput.push_back("\n");
            break;
    }

    // Select error type
    cout << endl << "Choose your selected error type, enter the corresponding number to make your choice: \n1: None \n2: Specific Packets to drop/lose ack/corrupt \n3: Random percentage of packets to drop, lose ack and corrupt" << endl;
    cout << "Input: ";
    cin >> input;
    cout << "You chose: ";
    input = input - 1;
    allInput.push_back(std::to_string(input));
    allInput.push_back("\n");
    switch (input) {
        case 0:
            cout << "No errors" << endl;
            allInput.push_back("\n"); // No percentage
            allInput.push_back("\n"); // No packets to drop
            allInput.push_back("\n"); // No packets to lose ack
            allInput.push_back("\n"); // No packets to fail checksum
            break;
        case 1:
            cout << "Specific Packets to drop/lose ack/corrupt" << endl;
            allInput.push_back("\n"); // No percentage
            // Packets to drop
            while(input > 0) {
                cout << endl << "Enter all packets to be dropped. Enter -1 to stop." << endl;
                cout << "Input: ";
                cin >> input;
                if (input > 0) {
                    allInput.push_back(std::to_string(input));
                    allInput.push_back(",");
                }
            }
            allInput.push_back("\n");
            input = 1;
            // Packets to lose ack
            while(input > 0) {
                cout << endl << "Enter all packets to lose the ack. Enter -1 to stop." << endl;
                cout << "Input: ";
                cin >> input;
                if (input > 0) {
                    allInput.push_back(std::to_string(input));
                    allInput.push_back(",");
                }
            }
            allInput.push_back("\n");
            input = 1;
            // Packets to fail checksum
            while(input > 0) {
                cout << endl << "Enter all packets to fail checksum test. Enter -1 to stop." << endl;
                cout << "Input: ";
                cin >> input;
                if (input > 0) {
                    allInput.push_back(std::to_string(input));
                    allInput.push_back(",");
                }
            }
            allInput.push_back("\n");

            break;
        case 2:
            cout << "Random percentage of packets to drop, lose ack and corrupt" << endl;
            cout << endl << "Choose the percentage:" << endl;
            cout << "Input: ";
            cin >> input;
            cout << "You chose: " << input << "%" << endl;
            allInput.push_back(std::to_string(input)); // User input for percentage
            allInput.push_back("\n");
            allInput.push_back("\n"); // No packets to drop
            allInput.push_back("\n"); // No packets to lose ack
            allInput.push_back("\n"); // No packets to fail checksum
            break;
    }
    cout << "\nCurrent input lines: " << endl;

    // Select error type
    string fileName;
    cout << endl << "Input the destination of the input file: " << endl;
    cout << "Input: ";
    cin >> fileName;
    cout << "Getting File: " << fileName << endl;
    allInput.push_back(fileName);
    for (int i = 0; i < allInput.size(); ++i) {
        cout << allInput[i];
    }

    std::ofstream output_file("./config.txt");
    std::ostream_iterator<std::string> output_iterator(output_file, "");
    std::copy(allInput.begin(), allInput.end(), output_iterator);
}
