CC = g++
CFLAGS = -Wall -g #debug flag
#CFLAGS = -Wall

receiver: receiver.o
	$(CC) $(CFLAGS) -o receiver receiver.o -lboost_system -lpthread

receiver.o: receiver.cpp packet.h
	$(CC) $(CFLAGS) -c receiver.cpp

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.gch
	rm -f *~
	rm -f test