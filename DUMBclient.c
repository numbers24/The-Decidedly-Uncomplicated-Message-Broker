#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char **argv){
	if (argc == 1 || argc == 2 || argc > 3){
		printf("Invalid number of arguments.\n");
		return 0;
	}
	const int port = atoi(argv[2]);
	struct sockaddr_in server;
	char buffer[1024] = {0};
	char *hello = "HELLO";
	char *goodbye = "GDBYE";
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0){
		printf("CRITICAL ERROR\n");
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	struct hostent *host = gethostbyname(argv[1]);
	if (host == NULL)
	{
		printf("Invalid hostname/IP address %s\n", argv[1]);
		return 0;
	}
	struct in_addr *address = (struct in_addr*) host->h_addr;
	char *ipaddress = inet_ntoa(*address);
	printf("Address: %s\n", ipaddress);
	inet_pton(AF_INET, ipaddress, &server.sin_addr);
	int i;
	int test;
	for (i = 0; i < 3; i++){
		test = connect(sock, (struct sockaddr*)&server, sizeof(server));
		if (test < 0){
			printf("Error connecting, retrying after 3 seconds.\n");
			sleep(3);
			continue;
		}else{
			break;
		}
	}
	if (test < 0){
		printf("Couldn't connect, shutting down.\n");
		return 0;
	}
	send(sock, hello, sizeof(hello), 0);
	recv(sock, buffer, sizeof(buffer), 0);
	printf("%s\n", buffer);
	if (strcmp(buffer, "HELLO DUMBv0 ready!") != 0){
		printf("Wrong message, terminating.\n");
		close(sock);
		return 0;
	}
	printf("WE MADE IT BOIS.\n");
	printf("Choose a command:\n");
	printf("quit\n");
	printf("create <name>\n");
	printf("delete <name>\n");
	printf("open <name>\n");
	printf("close <name>\n");
	printf("next\n");
	printf("put <size> <msg>\n");
	do {
		scanf("%s", buffer);
		if (strcmp(buffer, "quit") == 0){
			send(sock, goodbye, sizeof(goodbye), 0);
			close(sock);
			printf("Terminating client\n");
			return 0;
		}else if (strcmp(buffer, "create") == 0){
			printf("Okay, what box?\n");
			scanf("%s", buffer);
			char create[512];
			strcpy(create, "CREAT ");
			strcat(create, buffer);
			printf("%s\n", create);
			send(sock, create, sizeof(create), 0);
			//recv(sock, buffer, sizeof(buffer), 0);
			//printf("%s\n", buffer);
		}
	}while (1);
	return 0;
}
