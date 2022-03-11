CC = g++
CFLAGS = -Wall -g #debug flag
#CFLAGS = -Wall


network: main.o errorHandler.o inputManager.o sender.o receiver.o packet.o
	$(CC) $(CFLAGS) -o network main.o errorHandler.o inputManager.o packet.o

main.o: packet.h inputManager.h errorHandler.h
	$(CC) $(CFLAGS) -c main.cpp

errorHandler.o: errorHandler.cpp errorHandler.h
	$(CC) $(CFLAGS) -c errorHandler.cpp

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
	rm -f test