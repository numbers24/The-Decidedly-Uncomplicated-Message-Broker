#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv){
	if (argc == 1 || argc == 2 || argc > 3){
		printf("Invalid number of arguments.\n");
		return 0;
	}
	const int port = atoi(argv[2]);
	char *ipaddress = argv[1];
	struct sockaddr_in server;
	char buffer[1024] = {0};
	char *hello = "hello";
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0){
		printf("CRITICAL ERROR\n");
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	struct hostent *host_name = gethostbyname(argv[1]);
	if (host_entry == NULL)
	{
		printf("Invalid hostname/IP address %s\n", argv[1]);
		return 0;
	}
	char *ipaddress = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));
	inet_pton(AF_INET, ipaddress, &server.sin_addr);
	int i;
	int test;
	for (i = 0; i < 3; i++){
		test = connect(sock, (struct sockaddr*) &server, sizeof(server));
		if (test < 0){
			printf("Error connecting, retrying after 3 seconds.\n");
			sleep(3);
			continue;
		}
	}
	if (test < 0){
		printf("Couldn't connect, shutting down.\n");
		return 0;
	}
	send(sock, hello, sizeof(hello), 0);
	read(sock, buffer, 1024);
	printf("%s\n", buffer);
	if (strcmp(buffer, hello) != 0){
		printf("Wrong message, terminating.\n");
		close(sock);
		return 0;
	}
}
