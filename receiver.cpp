//
// Created by Wei Zhong Tee on 3/6/22.
//

#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <stdio.h>
#include <sstream>

using namespace std;

class Receiver {
private:
    int selectedAlgorithm; // 0 for GBN, 1 for stop and Wait, 2 for SR
    int receiverMaxWindowSize;
    int sizeOfPacket;
    int seqNumberUpperBound;
    int seqNumberLowerBound;
    int selectedErrorType; // 0 for none, 1 for specific packets, 2 for percentage
    int errorPercentage; //0 if none
    vector<int> packetsToLoseAck; //empty if none
    string filePath;

public:
    void setAlgorithmType(int input) {selectedAlgorithm = (input - 1);};
    int getSelectedAlgorithm() {return selectedAlgorithm;};

    void setReceiverMaxWindowSize(int input) {receiverMaxWindowSize = input;};
    int getReceiverMaxWindowSize() {return receiverMaxWindowSize;};

    void setSizeOfPacket(int input) {sizeOfPacket = input;};
    int getSizeOfPacket() {return sizeOfPacket;};

    void setErrorType(int input) {selectedErrorType = input;};
    int getErrorType() {return selectedErrorType;};

    void setErrorPercentage(int input) {errorPercentage = input;};
    int getErrorPercentage() {return errorPercentage;};

    void setPacketsToLoseAck(vector<int> input) {packetsToLoseAck = input;};
    vector<int>& getPacketsToLoseAck() {return packetsToLoseAck;}

    void setFilePath(string input) {filePath = input;};
    string getFilePath() {return filePath;};
};

int selectedAlgorithm;
int receiverMaxWindowSize;
int sizeOfPacket;
int selectedErrorType;
int errorPercentage; //0 if none
vector<int> packetsToLoseAck; //empty if none
vector<int> packetsToFailChecksum; //empty if none
string filePath;

void receiverWelcomeMessage() {
    cout << "Creating instance for: receiver." << endl;
}

void getNetworkConfigFrom(string fileName) {
    ifstream inputFile;
    inputFile.open(fileName);
    if(inputFile.is_open()) {
        string line;
        int itemCount = 0;
        while (getline(inputFile, line)) {
            int len = line.length();
            char lineChars[len + 1];
            strcpy(lineChars, line.c_str());
            if (itemCount == 0) { // Set selected algorithm
                selectedAlgorithm = stoi(lineChars);
            } else if (itemCount == 2) { // Skip sender window size, set receiver window size
                if (selectedAlgorithm-1 != 2) { // GBN or Stop and Wait: Set window size to size 1
                    receiverMaxWindowSize = 1;
                } else { // Selective repeat: Set window size to size in config file
                    receiverMaxWindowSize =  stoi(lineChars);
                }
            } else if (itemCount == 9) { // Selected error type
                selectedErrorType = stoi(lineChars);
            } else if (itemCount == 10) { // Error percentage if percentage to be randomly dropped is chosen
                if(line == "") {
                    errorPercentage = 0;
                } else {
                    errorPercentage = stoi(line);
                }
            } else if (itemCount == 12) { // Frame IDs of packets to lose ack
                string currentNum = "";
                for (int i = 0; i < len; ++i) {
                    if(lineChars[i] != ',') {
                        currentNum += lineChars[i];
                    } else {
                        //cout << "Current number end: " << currentNum << endl;
                        int packet = stoi(currentNum);
                        packetsToLoseAck.push_back(packet);
                        currentNum = "";
                    }
                }
            }
            itemCount++;
        }
    }
}

void parseFromString(string input) {
    cout << "Current input: " << endl;
    istringstream f(input);
    string line;
    int itemCount = 0;
    while (getline(f, line)) {
        int len = line.length();
        cout << line << endl;
        if (itemCount == 0) { // selected algorithm
            selectedAlgorithm = stoi(line);
        } else if (itemCount == 1) {
            receiverMaxWindowSize = stoi(line);
        } else if (itemCount == 2) {
            sizeOfPacket = stoi(line);
        } else if (itemCount == 5) { // skip sequence numbers
            selectedErrorType = stoi(line);
        } else if (itemCount == 6) { // Frame IDs of packets to lose ack
            string currentNum = "";
            for (int i = 0; i < len; ++i) {
                if(line[i] != ',') {
                    currentNum += line[i];
                } else {
                    //cout << "Current number end: " << currentNum << endl;
                    int packet = stoi(currentNum);
                    packetsToLoseAck.push_back(packet);
                    currentNum = "";
                }
            }
        }
        itemCount++;
    }
}

void showCurrentConfig(Receiver currentReceiver) {
    cout << endl << "Current sender configuration: " << endl;
    cout << "Selected algorithm: ";
    switch (currentReceiver.getSelectedAlgorithm()) {
        case 0:
            cout << "GBN" << endl;
            break;
        case 1:
            cout << "Stop and Wait" << endl;
            break;
        case 2:
            cout << "Selective Repeat" << endl;
            break;
    }
    cout << "Receiver Window Size: " << currentReceiver.getReceiverMaxWindowSize() << endl;
    cout << "Packet Size: " << currentReceiver.getSizeOfPacket() << endl;
    cout << "Selected error type: " << currentReceiver.getErrorType() << endl;
    switch (currentReceiver.getErrorType()) {
        case 0:
            cout << "None" << endl;
            break;
        case 1:
            cout << "Specific Packets" << endl;
            cout << "Packets to lose ack: ";
            for (int i = 0; i < packetsToLoseAck.size(); ++i) {
                cout << packetsToLoseAck[i] << "\t";
            }
            cout << endl;
            break;
        case 2:
            cout << "Percentage of randomly selected packets" << endl;
            cout << "Packets to lose ack: ";
            for (int i = 0; i < packetsToLoseAck.size(); ++i) {
                cout << packetsToLoseAck[i] << "\t";
            }
            cout << endl;
            break;
    }
}

Receiver setReceiverInstance(int selectedAlgorithm, int receiverMaxWindowSize, int sizeOfPacket, int selectedErrorType, int errorPercentage, vector<int> packetsToLoseAck) {
    Receiver receiverInstance;
    receiverInstance.setAlgorithmType(selectedAlgorithm);
    receiverInstance.setReceiverMaxWindowSize(receiverMaxWindowSize);
    receiverInstance.setSizeOfPacket(sizeOfPacket);
    receiverInstance.setErrorType(selectedErrorType);
    receiverInstance.setErrorPercentage(errorPercentage);
    receiverInstance.setPacketsToLoseAck(packetsToLoseAck);
    return receiverInstance;
}



int main() {
    Receiver receiverInstance;
    receiverWelcomeMessage();
    string test = "3\n1\n64000\n1\n100\n2\n4,5,6,";
    parseFromString(test);
    //getNetworkConfigFrom("config.txt");
    cout << selectedAlgorithm;
    receiverInstance = setReceiverInstance(selectedAlgorithm, receiverMaxWindowSize, sizeOfPacket, selectedErrorType, errorPercentage, packetsToLoseAck);
    showCurrentConfig(receiverInstance);
    return 0;
}
