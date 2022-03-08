//
// Created by Wei Zhong Tee on 3/6/22.
//

#include <iostream>
#include <cstdlib>

#include "inputManager.h"
#include "packet.h"

using namespace std;

void inputManager::welcomeMessage() {
    cout << "****** Welcome to CS462: Computer Networks Lab assignment. ******" << endl;
    cout << "Answer the following prompts to set up the configuration for the sender and receivers sessions. " << endl;
}

void inputManager::getMaxSeqNum() {
    cout << ""
}

void inputManager::getInput() {
    welcomeMessage();
}