CC = g++
CFLAGS = -Wall -g #debug flag
#CFLAGS = -Wall

receiver: receiver.o packet.o
	$(CC) $(CFLAGS) -o receiver receiver.o packet.o

receiver.o: receiver.cpp packet.h
	$(CC) $(CFLAGS) -c receiver.cpp

packet.o: packet.cpp packet.h
	$(CC) $(CFLAGS) -c packet.cpp

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.gch
	rm -f *~
	rm -f test