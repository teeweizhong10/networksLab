#include <iostream>
#include <boost/asio.hpp>

//SERVER
//To compile : g++ server.cpp -o server -lboost_system -lpthread


using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;
using std::cin;

void GBN(){
        //ACK resets ACKtimer
        //timeout? send NACK
        //corrupted? discard.
        //early ACK? discard
        //error? All frames in window are resent. .
        }

void SNW(){
        //get packet? Send ACK.
        //timeout? Send NACK???
}


void SR(){
        //if NACK, resend
        //store early ACKs
        //timeout? resend.
}


void ckSum(){
        ///gets data, m, and checksum from client
        //breaks data into m segments and sums
        //sums those segments with sent checksum
        //if sum is 0, then accepts packet
        //if not, discard and prompt client for retransmit.
}

string read_(tcp::socket & socket) {
       boost::asio::streambuf buf;
       boost::asio::read_until( socket, buf, "\n" );
       string data = boost::asio::buffer_cast<const char*>(buf.data());
       return data;
}
void send_(tcp::socket & socket, const string& message) {
       const string msg = message + "\n";
       boost::asio::write( socket, boost::asio::buffer(message) );
}

int main() {
        ////Prompting
        char protocol[10];
        int windowSize;

        char acksLost[50];  //what types are these
        char sequNums[10];  //what types are these

        cout << "\n\nSelect the  protocol to be used: 'GBN', 'SP', 'StopnWait':  ";
        cin >> protocol;
        if(protocol == "GBN" | protocol == "StopnWait"){
                windowSize = 1;
        }else{
                cout << "What is your window size?:        ";
                cin >> windowSize;
        }
        cout << "List the ACKs to lose:    ";      //in what format?
        cin >> acksLost;
        cout << "What is the range of sequence numbers (in unit)?: ";      //as int or byte?
        cin >> sequNums;
        ////End Prompting

        cout << "\nStarting server...\n";

boost::asio::io_service io_service;
//listen for new connection
      tcp::acceptor acceptor(io_service, tcp::endpoint(tcp::v4(), 1234 ));
//socket creation
      tcp::socket socket(io_service);
//waiting for connection
      acceptor.accept(socket);
//read operation
      string message = read_(socket);
      cout << message << endl;
//write operation
      send_(socket, "Hello From Server!");
      cout << "Servent sent Hello message to Client!" << endl;
   return 0;
}