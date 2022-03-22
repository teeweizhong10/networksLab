#include <iostream>
#include <boost/asio.hpp>


//CLIENT
//To compile : g++ client.cpp -o client -lboost_system -lpthread


using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;
using std::cin;

void GBN(){
        //client frame by user, server frame is 1
        //when error: all frames in window retransmit
        //ACK for frame means all previous frames were recieved
        //Each ACK resets ACKtimer. If timeout, server sends NACK
        //Corrupted packets? discard them.
        //Early ACKs?: discard them.
}

void SNW(){
        //both client and server have window size of 1
        //waits for ACK1 before sending ACK2 (hence window size is 1).
        //Uses timeouts
        //Lost packet???
}

void SR(){
        //only resends corrupted or lost packets
        //server and client window is SAME
        //n bits for sequence numbers then window = 2^(n-1)
        //corrupted? do not discard. send NACK.
        //Accepts early ACKS and stores them.
        //Lost? Timeout and resend
}

void ckSum(){
        //Given m bits (sequence range??). Data is divided into segments of m bits.
        //Segments are added.
        //if sum bits - m isn't 0, add extra again
        //send to reciever

}

int main(){

//Promtping
        char protocol[10];
        int windowSize;
        char typeOfTO[5];
        int TOMultiplier;
        char sequNums[50];
        char typeError[5];
        char errors[50];

        cout << "\n\nSelect a protocol to be used: 'GBN', 'SR', 'StopnWait':        ";
        cin >> protocol;
        if(protocol == "GBN" | protocol == "SR"){
                cout << "What is the window size?:    ";
                cin >> windowSize;
        }else{
                windowSize = 1;
                cout << "What type of Timeout? 'S' for static, 'D' for dynamic:       ";
                cin >> typeOfTO;
        }
        if(typeOfTO == "D"){
                cout << "At what multiplier?:  ";
                cin >> TOMultiplier;
        }

        cout << "What is the range of sequence numbers:       ";
        cin >> sequNums;
        cout << "What kind of errors: 'N' for none, 'R' for randomly generated, 'U' for user-specified:       ";
        cin >> typeError;
        if(typeError == "U"){
                cout << "Select an option:\n1.Drop packets 2,4,5\n2.Lose ACK11\n3.Checksum Error\n4.10% of each\nSelected Choice:     ";
                cin >> errors;
        }

//End Prompting

//estabilishes service
        boost::asio::io_service io_service;

        //creates socket
        tcp::socket socket(io_service);

        //connects socket to server
        socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));


        ///////////////////////////////////////
        //message from client
        const string msg = "Hello from Client!\n";
        boost::system::error_code error;
        boost::asio::write(socket, boost::asio::buffer(msg), error);
        if(!error){
                cout<<"Client sent message"<<endl;
        }else{
                cout<<"Send failed"<<endl;
        }
/////////////////////////////
        //server response
        boost::asio::streambuf recv_buf;
        boost::asio::read(socket, recv_buf, boost::asio::transfer_all(), error);
        if(error && error != boost::asio::error::eof){
                cout<<"recv failed"<<endl;
        }else{
                const char* data = boost::asio::buffer_cast<const char*>
                        (recv_buf.data());
                cout<< data<<endl;
        }
        return 0;
}