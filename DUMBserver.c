#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 6654
int main(int argc, char const *argv[]) 
{ 
	char command[1024];
	int serverFD,newSock,rval,opt,addrLen; 
	opt=1;
        struct sockaddr_in addr; 
	addrLen=sizeof(addr);
       
     
	if ((serverFD = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
        	exit(EXIT_FAILURE); 
    	} 
	if (setsockopt(serverFD, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))) 
    	{ 
        	perror("setsockopt"); 
        	exit(EXIT_FAILURE); 
    	} 
    	addr.sin_family = AF_INET; 
    	addr.sin_addr.s_addr = INADDR_ANY; 
    	addr.sin_port = htons( PORT );
    	if (bind(serverFD, (struct sockaddr *)&addr,sizeof(addr))<0) 
    	{ 
        	perror("bind failed"); 
        	exit(EXIT_FAILURE); 
    	} 
    	if (listen(serverFD, 3)<0) 
    	{ 
        	perror("listen"); 
        	exit(EXIT_FAILURE); 
    	} 
	if ((newSock=accept(serverFD, (struct sockaddr *)&addr,(socklen_t*)&addrLen))<0) 
    	{ 
        	perror("accept"); 
        	exit(EXIT_FAILURE); 
    	} 
    	rval=read(newSock,command,1024); 
    	printf("%s\n",command);
    	char*hello="HELLO FROM SERVER";
    	send(newSock,hello,strlen(hello),0); 
    	printf("Hello message sent\n"); 
    	return 0; 
} 
