CC = g++
#CFLAGS = -Wall -g #debug flag
CFLAGS = -Wall

 sender:  caseyln_sargeakb_teewz_sender.o  caseyln_sargeakb_teewz_packet.o
	$(CC) $(CFLAGS) -o  sender caseyln_sargeakb_teewz_sender.o  caseyln_sargeakb_teewz_packet.o

 caseyln_sargeakb_teewz_sender.o:  caseyln_sargeakb_teewz_sender.cpp  caseyln_sargeakb_teewz_packet.h
	$(CC) $(CFLAGS) -c  caseyln_sargeakb_teewz_sender.cpp

 caseyln_sargeakb_teewz_packet.o: caseyln_sargeakb_teewz_packet.cpp caseyln_sargeakb_teewz_packet.h
	$(CC) $(CFLAGS) -c  caseyln_sargeakb_teewz_packet.cpp

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.gch
	rm -f *~
	rm -f test
