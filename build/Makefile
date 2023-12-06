check : server client

server :
	gcc -c ../server_lib/string.c -o string.o
	gcc -c ../server_lib/server_lib.c -o server_lib.o
	gcc -c ../server.c -o server.o
	gcc string.o server_lib.o server.o -o server
client :
	gcc ../client.c -o client

clean :
	rm -f server client *.o
