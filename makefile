CC = g++
CFLAGS = -Wall -g #debug flag
#CFLAGS = -Wall

receiver: receiver.o packet.o
	$(CC) $(CFLAGS) -o receiver receiver.o packet.o -lboost_system -lpthread

sender: sender.o packet.o
	$(CC) $(CFLAGS) -o sender sender.o packet.o -lboost_system -lpthread

network: main.o inputManager.o sender.o receiver.o packet.o
	$(CC) $(CFLAGS) -o network main.o inputManager.o packet.o

main.o: packet.h inputManager.h
	$(CC) $(CFLAGS) -c main.cpp

inputManager.o: inputManager.cpp inputManager.h packet.h
	$(CC) $(CFLAGS) -c inputManager.cpp

sender.o: sender.cpp packet.h
	$(CC) $(CFLAGS) -c sender.cpp

receiver.o: receiver.cpp packet.h
	$(CC) $(CFLAGS) -c receiver.cpp

packet.o: packet.cpp packet.h
	$(CC) $(CFLAGS) -c packet.cpp

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.gch
	rm -f *~
	rm -f network
	rm -f sender
	rm -f receiver
