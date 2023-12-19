CC = gcc
CFLAGS = -O2 -g
LDFLAGS = -g -Wall

check : server client

server: string.o server_lib.o server.o
	$(CC) $(LDFLAGS) $^ -o $@ -lm
	mv server bin

client: client.o
	$(CC) $(LDFLAGS) $< -o $@ -lm
	mv client bin
	
server.o: ../server.c
	$(CC) $(CFLAGS) -c $< -o $@

client.o: ../client.c
	$(CC) $(CFLAGS)  -c $< -o $@

string.o: ../string.c
	$(CC) $(CFLAGS) -c $< -o $@

server_lib.o: ../server_lib.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f ./bin/server ./bin/client *.o
	rm -rf *.dSYM
.PHONY: clean
