CC = g++
#CFLAGS = -Wall -g #debug flag
CFLAGS = -Wall

receiver:  caseyln_sargeakb_teewz_receiver.o  caseyln_sargeakb_teewz_packet.o
	$(CC) $(CFLAGS) -o receiver  caseyln_sargeakb_teewz_receiver.o  caseyln_sargeakb_teewz_packet.o -lboost_system -lpthread

sender:  caseyln_sargeakb_teewz_sender.o  caseyln_sargeakb_teewz_packet.o
	$(CC) $(CFLAGS) -o sender  caseyln_sargeakb_teewz_sender.o  caseyln_sargeakb_teewz_packet.o -lboost_system -lpthread

network:  caseyln_sargeakb_teewz_main.o  caseyln_sargeakb_teewz_inputManager.o  caseyln_sargeakb_teewz_sender.o  caseyln_sargeakb_teewz_receiver.o  caseyln_sargeakb_teewz_packet.o
	$(CC) $(CFLAGS) -o network main.o inputManager.o packet.o

 caseyln_sargeakb_teewz_main.o: caseyln_sargeakb_teewz_packet.h caseyln_sargeakb_teewz_inputManager.h
	$(CC) $(CFLAGS) -c  caseyln_sargeakb_teewz_main.cpp

 caseyln_sargeakb_teewz_inputManager.o:  caseyln_sargeakb_teewz_inputManager.cpp  caseyln_sargeakb_teewz_inputManager.h  caseyln_sargeakb_teewz_packet.h
	$(CC) $(CFLAGS) -c  caseyln_sargeakb_teewz_inputManager.cpp

 caseyln_sargeakb_teewz_sender.o:  caseyln_sargeakb_teewz_sender.cpp  caseyln_sargeakb_teewz_packet.h
	$(CC) $(CFLAGS) -c  caseyln_sargeakb_teewz_sender.cpp

 caseyln_sargeakb_teewz_receiver.o:  caseyln_sargeakb_teewz_receiver.cpp  caseyln_sargeakb_teewz_packet.h
	$(CC) $(CFLAGS) -c  caseyln_sargeakb_teewz_receiver.cpp

 caseyln_sargeakb_teewz_packet.o:  caseyln_sargeakb_teewz_packet.cpp  caseyln_sargeakb_teewz_packet.h
	$(CC) $(CFLAGS) -c caseyln_sargeakb_teewz_packet.cpp

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.gch
	rm -f *~
	rm -f network
	rm -f sender
	rm -f receiver
