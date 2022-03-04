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

int userPrompt(){
        char protocol;
        int windowSize;

        char acksLost;  //what types are these
        char sequNums;  //what types are these

        //cout<<"Select the  protocol to be used: 'GBN', 'SP', 'StopnWait':  ";
        //cin>>protocol;
        //cout<<"\nWhat is your window size?:        ";
        //cin>>windowSize;
        //cout<<"\nList the ACKs to lose:    ";      //in what format?
        //cin>>acksLost;
        //cout<<"\nWhat is the range of sequence numbers (in unit)?: ";      //as int or byte?
        //cin>>sequNums;
return 0;
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
        //cout<<"Server starting..."<<endl;
      userPrompt();
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