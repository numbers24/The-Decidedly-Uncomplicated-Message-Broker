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
	char msg[1024][1024];
	struct box* next;
}box;

box* list;
int sockets[256];

void add(struct box* new)
{
	if(!list)
	{
		list=new;
		return;
	}	
	struct box* ptr;
	for(ptr=list;ptr;ptr=ptr->next)
	{
		if(ptr->next==NULL || list==NULL)
		{	
			ptr->next = new;
			return;
		}
	}
}
box* getBox(char* name)
{
	struct box* curr;
	for(curr=list;curr;curr=curr->next)
	{
		if(!strcmp(curr->name,name))
		return curr;
	}
	return NULL;
}

//Commands

void create(char* name,int newSock)
{
	char* response;
	send(newSock,response,strlen(response),0);
	
	if(name==NULL)
	{
		response="You need a name, dude.";
		send(newSock,response,strlen(response),0);
		return;
	}
	if(!isalpha(name[0]))
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
		struct box* new = (box*)malloc(sizeof(box));
		new->name=name;
		new->msg;
		new->next=NULL;
		add(new);
		response="Account created!";
		send(newSock,response,strlen(response),0);
	}
}
void delete(char* name, int newSock)
{
	char* response;	
	struct box* ptr;
	struct box* prev;
	for(ptr=prev=list;ptr;prev=ptr,ptr=ptr->next)
	{
		if(!strcmp(ptr->name,name))
		{
			prev->next=ptr->next;
			response="OK!";
			send(newSock,response,strlen(response),0);
			return;
		}
		
	}
	response = "ER: DNE.";
	send(newSock,response,strlen(response),0);
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
	rval=read(newSock,command,1024);	
	char cmd[6];
	char content[1018];
	char* response;
	printf("%s\n",command);
	if(!strcmp(command,"HELLO"))
	{
		response="HELLO DUMBv0 ready!";
		struct box* curr;
		send(newSock,response,strlen(response),0);
		int q,i;
		while(1)
		{	
			strncpy(cmd,command,6);
			strncpy(content,command+6,1024);
			
			if(!strcmp(command,"GDBYE"))
			break;
			else if(!strcmp(cmd,"CREAT "))
			{
				create(content,newSock);
			}
			else if(!strcmp(cmd,"OPNBX "))
			{
				if(content==NULL)
				{
					response="ER: NULL NAME";
					send(newSock,response,strlen(response),0);
					continue;
				}
				if(!isalpha(content[0]))
				{
					response="ER: You need to start your name with an alphabetical character.";
					send(newSock,response,strlen(response),0);
					continue;
				}
				if(strlen(content)<5||strlen(content)>25)
				{
					response="ER: Your name must be between 5-25 characters long.";
					send(newSock,response,strlen(response),0);
					continue;
				}
				for(curr=list;curr;curr=curr->next)
				{
					if(!strcmp(curr->name,content))
					break;
				}
				if(curr==NULL)
				{
					response = "ER: DNE";
					send(newSock,response,strlen(response),0);
					continue;
				}
				response="OK!";
				send(newSock,response,strlen(response),0);
				q=i=0;
			}
			else if(!strcmp(cmd,"NXTMG "))
			{
				if(!curr)
				{
					response="ER: NO BOX OPEN";
					send(newSock,response,strlen(response),0);
					continue;
				}
				if(q>1024)
				{
					response="ER: EXCEDED MSGBOX SIZE";
					send(newSock,response,strlen(response),0);
					continue;
				}
				printf("Message: %s, %d\n",curr->msg[q]);
				response=curr->msg[q];
				if(response==NULL)
				{
					response="ER: NO MSG";
					send(newSock,response,strlen(response),0);
					continue;
				}
				q++;
				send(newSock,response,strlen(response),0);
			}
			else if(!strcmp(cmd,"PUTMG "))
			{
				if(!curr)
				{
					response="ER: NO BOX OPEN";
					send(newSock,response,strlen(response),0);
					continue;
				}
				if(q>1024)
				{
					response="ER: EXCEDED MSGBOX SIZE";
					send(newSock,response,strlen(response),0);
					break;
				}	
				response="OK!";
				strncpy(curr->msg[i],content,1024);
				i++;
				send(newSock,response,strlen(response),0);
					
			}
			else if(!strcmp(cmd,"DELBX "))
			{
				delete(content,newSock);
			}
			else if(!strcmp(cmd,"CLSBX "))
			{
				if(!curr)
				{
					response="ER: NO BOX OPEN";
					send(newSock,response,strlen(response),0);
					continue;
				}				
				if(strcmp(content,curr->name))
				{
					response="ER: DOES NOT MATCH";
					send(newSock,response,strlen(response),0);
					continue;
				}
				
				response="Box %s closed!\n",curr->name;
				send(newSock,response,strlen(response),0);
				curr=NULL;
			}
			printf("%s\n",cmd);
			rval=recv(newSock,command,1024,0);
		}
	}
	else
	{
		response="ER: CANNOT ENTER";
		send(newSock,response,strlen(response),0);
	}
    	return 0; 
} 
