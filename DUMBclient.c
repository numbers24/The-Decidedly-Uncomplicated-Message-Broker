#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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
		printf("Couldn't connect, terminating.\n");
		return 0;
	}
	send(sock, hello, sizeof(hello), 0);
	recv(sock, buffer, sizeof(buffer), 0);
	printf("%s\n", buffer);
	if (strcmp(buffer, "HELLO DUMBv0 ready!") != 0){
		printf("Wrong message, terminating.\n");
		close(sock);
		return 0;
	}else{
		printf("Connect successfully!\n");
	}
	printf("Available commands:\n");
	printf("  -  quit\n");
	printf("  -  create <name>\n");
	printf("  -  delete <name>\n");
	printf("  -  open <name>\n");
	printf("  -  close <name>\n");
	printf("  -  next\n");
	printf("  -  put <size> <msg>\n");
	printf("  -  help\n");
	while (1){
		memset(buffer, 0, 1024);
		fgets(buffer, 1024, stdin);
		buffer[strcspn(buffer, "\n")] = 0;
		printf("%s\n", buffer);
		if (strcmp(buffer, "quit") == 0){
			send(sock, goodbye, sizeof(goodbye), 0);
			close(sock);
			printf("Terminating client\n");
			break;
		}else if (strcmp(buffer, "create") == 0){
			memset(buffer, 0, 1024);
			printf("Okay, what should the name be?\n");
			fgets(buffer, 1024, stdin);
			buffer[strcspn(buffer, "\n")] = 0;
			char create[512];
			strcpy(create, "CREAT ");
			strcat(create, buffer);
			printf("%s\n", create);
			send(sock, create, sizeof(create), 0);
			memset(buffer, 0, 1024);
			recv(sock, buffer, sizeof(buffer), 0);
			printf("%s\n", buffer);
		}else if (strcmp(buffer, "open") == 0){
			memset(buffer, 0, 1024);
			printf("Okay, what box?\n");
			fgets(buffer, 1024, stdin);
			buffer[strcspn(buffer, "\n")] = 0;
			char open[512];
			strcpy(open, "OPNBX ");
			strcat(open, buffer);
			printf("Message sent: %s\n", open);
			send(sock, open, sizeof(open), 0);
			memset(buffer, 0, 1024);
			recv(sock, buffer, sizeof(buffer), 0);
			printf("%s\n", buffer);
		}else if (strcmp(buffer, "delete") == 0){
			memset(buffer, 0, 1024);
			printf("Okay, what box?\n");
			fgets(buffer, 1024, stdin);
			buffer[strcspn(buffer, "\n")] = 0;
			char delete[512];
			strcpy(delete, "DELBX ");
			strcat(delete, buffer);
			printf("%s\n", delete);
			send(sock, delete, sizeof(delete), 0);
			memset(buffer, 0, 1024);
			recv(sock, buffer, sizeof(buffer), 0);
			printf("%s\n", buffer);
		}else if (strcmp(buffer, "close") == 0){
			memset(buffer, 0, 1024);
			printf("Okay, what box?\n");
			fgets(buffer, 1024, stdin);
			buffer[strcspn(buffer, "\n")] = 0;
			char close[512];
			strcpy(close, "CLSBX ");
			strcat(close, buffer);
			printf("%s\n", close);
			send(sock, close, sizeof(close), 0);
			memset(buffer, 0, 1024);
			recv(sock, buffer, sizeof(buffer), 0);
			printf("%s\n", buffer);
		}else if (strcmp(buffer, "next") == 0){
			memset(buffer, 0, 1024);
			printf("Okay\n");
			char next[512];
			strcpy(next, "NXTMG ");
			printf("%s\n", next);
			send(sock, next, sizeof(next), 0);
			memset(buffer, 0, 1024);
			recv(sock, buffer, sizeof(buffer), 0);
			printf("%s\n", buffer);
		}else if (strcmp(buffer, "put") == 0){
			memset(buffer, 0, 1024);
			printf("Okay, what is the message\n");
			fgets(buffer, 1024, stdin);
			buffer[strcspn(buffer, "\n")] = 0;
			char put[512];
			char size[256];
			sprintf(size, "%d", strlen(buffer));
			strcpy(put, "PUTMG ");
			strcat(put, size);
			strcat(put, " ");
			strcat(put, buffer);
			printf("%s\n", put);
			send(sock, put, sizeof(put), 0);
			memset(buffer, 0, 1024);
			recv(sock, buffer, sizeof(buffer), 0);
			printf("%s\n", buffer);
		}else if (strcmp(buffer, "help") == 0){
			printf("Available commands:\n");
			printf("  -  quit\n");
			printf("  -  create <name>\n");
			printf("  -  delete <name>\n");
			printf("  -  open <name>\n");
			printf("  -  close <name>\n");
			printf("  -  next\n");
			printf("  -  put <size> <msg>\n");
			printf("  -  help\n");
		}else{
			printf("Command not valid\n");
		}
	}
	return 0;
}
