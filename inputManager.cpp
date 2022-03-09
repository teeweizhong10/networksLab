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

void inputManager::setAlgorithmType() {
    cout << "Choose which protocol you would like to use (Enter 1, 2 or 3): " << endl;
}

void inputManager::getInput() {
    welcomeMessage();
}