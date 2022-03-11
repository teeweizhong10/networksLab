CC = g++
CFLAGS = -Wall -g #debug flag
#CFLAGS = -Wall

sender: sender.o
	$(CC) $(CFLAGS) -o sender sender.o

sender.o: sender.cpp packet.h
	$(CC) $(CFLAGS) -c sender.cpp

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.gch
	rm -f *~
	rm -f test