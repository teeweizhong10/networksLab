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
int userPrompts(){
        char protocol;
        int windowSize;
        char typeOfTO;
        int TOMultiplier;
        char sequNums;
        char typeError;
        char errors;

        //cout<<"Select a protocol to be used: 'GBN', 'SP', 'StopnWait':        ";
        //cin>>protocol;
        //cout<<"\nWhat is the window size?:    ";
        //cin>>windowSize;
        //cout<<"\nWhat type of Timeout? 'S' for static, 'D' for dynamic:       ";
        //cin>>typeOfTO;

        if(typeOfTO == 'D'){
                //cout<<"\nAt what multiplier?:  ";
                //cin>>TOMultiplier;
        }

        //cout<<"\nWhat is the range of sequence numbers:       ";
        //cin>>sequNums;
        //cout<<"\nWhat kind of errors: 'N' for none, 'R' for randomly generated, 'U' for user-specified:       ";
        //cin>>typeError;
        if(typeError == 'U'){
                //cout<<"\nSelect an option:\n1.Drop packets 2,4,5\n2.Lose ACK11\n3.Checksum Error\n4.10% of each\nSelected Choice:     ";
                //cin>>errors;
        }

        return 0;
}

int main(){
        userPrompts();

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