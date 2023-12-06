all : server client 
server: string.o server_lib.o server.o
	gcc -g -Wall $^ -o $@

client: ../client.c
	gcc -g $< -o $@

string.o: ../server_lib/string.c
	gcc -g -c $< -o $@

server_lib.o: ../server_lib/server_lib.c
	gcc -g -c $< -o $@

server.o: ../server.c
	gcc -g -c $< -o $@

clean:
	rm -f server client *.o

