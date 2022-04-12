CC = g++
CFLAGS = -Wall -g #debug flag
#CFLAGS = -Wall

receiver: receiver.o packet.o
	$(CC) $(CFLAGS) -o receiver receiver.o packet.o

receiver.o:  caseyln_sargeakb_teewz_receiver.cpp  caseyln_sargeakb_teewz_packet.h
	$(CC) $(CFLAGS) -c  caseyln_sargeakb_teewz_receiver.cpp

packet.o:  caseyln_sargeakb_teewz_packet.cpp  caseyln_sargeakb_teewz_packet.h
	$(CC) $(CFLAGS) -c  caseyln_sargeakb_teewz_packet.cpp

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.gch
	rm -f *~
	rm -f test