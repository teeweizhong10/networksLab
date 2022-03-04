CC = g++
CFLAGS = -Wall -g
#CFLAGS = -Wall


makeClient: testClient.o testServer.o
	$(CC) $(CFLAGS) -o test testClient.o

client.o: testClient.h
	$(CC) $(CFLAGS) -c testClient.cpp

server.o: testServer.h
	$(CC) $(CFLAGS) -c testServer.cpp

clean:
	rm -f *.o
	rm -f *.out
	rm -f *.gch
	rm -f *~
	rm -f test