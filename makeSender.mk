CC = g++
CFLAGS = -Wall -g #debug flag
#CFLAGS = -Wall

sender:
	 g++ sender.cpp -o sender -lboost_system -lpthread


sender.o: sender.cpp packet.h
	$(CC) $(CFLAGS) -c sender.cpp

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.gch
	rm -f *~
	rm -f test