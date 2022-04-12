CC = g++
#CFLAGS = -Wall -g #debug flag
CFLAGS = -Wall

receiver: receiver.o packet.o
	$(CC) $(CFLAGS) -o receiver receiver.o packet.o -lboost_system -lpthread

sender: sender.o packet.o
	$(CC) $(CFLAGS) -o sender sender.o packet.o -lboost_system -lpthread

network: main.o inputManager.o sender.o receiver.o packet.o
	$(CC) $(CFLAGS) -o network main.o inputManager.o packet.o

main.o: caseyln_sargeakb_teewz_packet.h caseyln_sargeakb_teewz_inputManager.h
	$(CC) $(CFLAGS) -c  caseyln_sargeakb_teewz_main.cpp

inputManager.o:  caseyln_sargeakb_teewz_inputManager.cpp  caseyln_sargeakb_teewz_inputManager.h  caseyln_sargeakb_teewz_packet.h
	$(CC) $(CFLAGS) -c  caseyln_sargeakb_teewz_inputManager.cpp

sender.o:  caseyln_sargeakb_teewz_sender.cpp  caseyln_sargeakb_teewz_packet.h
	$(CC) $(CFLAGS) -c  caseyln_sargeakb_teewz_sender.cpp

receiver.o:  caseyln_sargeakb_teewz_receiver.cpp  caseyln_sargeakb_teewz_packet.h
	$(CC) $(CFLAGS) -c  caseyln_sargeakb_teewz_receiver.cpp

packet.o:  caseyln_sargeakb_teewz_packet.cpp  caseyln_sargeakb_teewz_packet.h
	$(CC) $(CFLAGS) -c caseyln_sargeakb_teewz_packet.cpp

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.gch
	rm -f *~
	rm -f network
	rm -f sender
	rm -f receiver
