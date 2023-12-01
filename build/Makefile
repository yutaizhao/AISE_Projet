check : server client

server :
	gcc -c ../server_lib.c -o server_lib.o
	gcc -c ../server.c -o server.o
	gcc server_lib.o server.o -o server
client :
	gcc ../client.c -o client

clean :
	rm -f server client *.o
