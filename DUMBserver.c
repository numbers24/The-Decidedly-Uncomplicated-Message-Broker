#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#define PORT 6654

typedef struct box
{
	char* name;
	char* message;
	struct box* next;
}box;

box* list;
int sockets[256];

void add(box* new)
{
	struct box* ptr;
	for(ptr=list;ptr!=NULL;ptr=ptr->next)
	{
		if(ptr->next==NULL)
		{
			ptr->next = new;
			return;
		}
	}
}
box* getBox(char* name)
{
	box* curr;
	for(curr=list;curr!=NULL;curr=curr->next)
	{
		if(!strcmp(curr->name,name))
		return curr;
	}
	return NULL;
}

//Commands

void create(char* name)
{
	/*
		rules:
		1.) 5-25 characters
		2.) must start with an alphabetic character
	*/

	char* response;
	send(newSock,response,strlen(response),0);
	
	if(name==NULL)
	{
		response="You need a name, dude.";
		send(newSock,response,strlen(response),0);
		return;
	}
	if(!isalpha(name[0])
	{
		response="You need to start your name with an alphabetical character.";
		send(newSock,response,strlen(response),0);
		return;
	}
	if(strlen(name)<5||strlen(name)>25)
	{
		response="Your name must be between 5-25 characters long.";
		send(newSock,response,strlen(response),0);
		return;
	}
	else
	{
		box* new = (box*)malloc(sizeof(box));
		new->name=name;
		new->next=NULL;
		add(new);
	}
}

void open(char* name)
{
	char* response;
	send(newSock,response,strlen(response),0);
		
	if(name==NULL)
	{
		response="You need a name, dude.";
		send(newSock,response,strlen(response),0);
		return;
	}
	if(!isalpha(name[0])
	{
		response="You need to start your name with an alphabetical character.";
		send(newSock,response,strlen(response),0);
		return;
	}
	if(strlen(name)<5||strlen(name)>25)
	{
		response="Your name must be between 5-25 characters long.";
		send(newSock,response,strlen(response),0);
		return;
	}
	else
	{
		box* curr = getBox(name);
		if(curr==NULL)
		{
			response = "That name doesn't exist";
			send(newSock,response,strlen(response),0);
			return;
	}
}

int main(int argc, char const *argv[])
{ 
	char command[1024];
	int serverFD,newSock,rval,opt,addrLen,status; 
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
	/*
			Commands:
			HELLO_
			GDBYE_
			CREAT_
			OPNBX_
			NXTMG_
			PUTMG_
			DELBX_
			CLSBX_
	*/
	rval=read(newSock,command,1024);	
	char cmd[6];
	char content[1018];
	char* response;
	printf("%s\n",command);
	if(!strcmp(command,"HELLO"))
	{
		response="HELLO DUMBv0 ready!";
		send(newSock,response,strlen(response),0);
		while(1)
		{	
			rval=recv(newSock,command,1024,0);
			strncpy(cmd,command,6);
			strncpy(content,command+6,1024);
			printf("%s\n",cmd);
			if(!strcmp(command,"GDBYE"))
			break;
			else if(!strcmp(cmd,"CREAT "))
			{
				
			}
			else if(!strcmp(cmd,"OPNBX "))
			{
				
			}
			else if(!strcmp(cmd,"NXTMG "))
			{
				
			}
			else if(!strcmp(cmd,"PUTMG "))
			{
				
			}
			else if(!strcmp(cmd,"DELBX "))
			{
				
			}
			else if(!strcmp(cmd,"CLSBX "))
			{
				
			}

		}
	}
	else
	{
		response="Error: Incorrect Greetings";
		send(newSock,response,strlen(response),0);
	}
    	return 0; 
} 
