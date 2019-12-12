#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>


typedef struct box
{
	char* name;
	char msg[1024][1024];
	int used;
	struct box* next;
}box;

box* list;
int sockets[256];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

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
		response="ER:WHAT?";
		send(newSock,response,strlen(response),0);
		return;
	}
	if(!isalpha(name[0]))
	{
		response="ER:WHAT?";
		send(newSock,response,strlen(response),0);
		return;
	}
	if(strlen(name)<5||strlen(name)>25)
	{
		response="ER:WHAT?";
		send(newSock,response,strlen(response),0);
		return;
	}
	else
	{
		struct box* new = (box*)malloc(sizeof(box));
		new->name=name;
		new->msg;
		new->next=NULL;
		new->used = 0;
		add(new);
		response="OK!";
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
			if (ptr->used == 1)
			{
				response="ER:OPEND";
				send(newSock,response,strlen(response),0);
				return;
			}
			prev->next=ptr->next;
			response="OK!";
			send(newSock,response,strlen(response),0);
			return;
		}
		
	}
	response = "ER:NEXST";
	send(newSock,response,strlen(response),0);
}

void *threadFunc(void *socket)
{
	int newSock = *((int *)socket);
	char command[1024];
	int rval=read(newSock,command,1024);
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
			{
				break;
			}
			else if(!strcmp(cmd,"CREAT "))
			{
				pthread_mutex_lock(&lock);
				create(content,newSock);
				pthread_mutex_unlock(&lock);
			}
			else if(!strcmp(cmd,"OPNBX "))
			{
				if(content==NULL)
				{
					response="ER:WHAT?";
					send(newSock,response,strlen(response),0);
					continue;
				}
				if(!isalpha(content[0]))
				{
					response="ER:WHAT?";
					send(newSock,response,strlen(response),0);
					continue;
				}
				if(strlen(content)<5||strlen(content)>25)
				{
					response="ER:WHAT?";
					send(newSock,response,strlen(response),0);
					continue;
				}
				for(curr=list;curr;curr=curr->next)
				{
					if(!strcmp(curr->name,content))
					{
					break;
					}
				}
				if(curr==NULL)
				{
					response = "ER:NEXST";
					send(newSock,response,strlen(response),0);
					continue;
				}
				if(curr->used == 1)
				{
					response = "ER:OPEND";
					send(newSock,response,strlen(response),0);
					continue;
				}
				curr->used = 1;
				response="OK!";
				send(newSock,response,strlen(response),0);
				q=i=0;
			}
			else if(!strcmp(cmd,"NXTMG "))
			{
				if(!curr)
				{
					response="ER:NOOPN";
					send(newSock,response,strlen(response),0);
					continue;
				}
				if(q>1024)
				{
					response="ER:EMPTY";
					send(newSock,response,strlen(response),0);
					continue;
				}
				printf("Message: %s, %d\n",curr->msg[q]);
				response=curr->msg[q];
				if(response==NULL)
				{
					response="ER:EMPTY";
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
					response="ER:NOOPN";
					send(newSock,response,strlen(response),0);
					continue;
				}
				if(q>1024)
				{
					response="ER:LIMIT";
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
				pthread_mutex_lock(&lock);
				delete(content,newSock);
				pthread_mutex_unlock(&lock);
			}
			else if(!strcmp(cmd,"CLSBX "))
			{
				if(!curr)
				{
					response="ER:NOOPN";
					send(newSock,response,strlen(response),0);
					continue;
				}				
				if(strcmp(content,curr->name))
				{
					response="ER:WHAT?";
					send(newSock,response,strlen(response),0);
					continue;
				}
				
				response="OK!",curr->name;
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
	pthread_mutex_unlock(&lock);
	close(newSock);
	printf("CLIENT DISCONNECTED\n");
	pthread_exit(NULL);

}

int main(int argc, char const **argv)
{
	int serverFD,newSock,opt,addrLen,status,PORT;
	opt=1;
        struct sockaddr_in addr; 
	addrLen=sizeof(addr);
	PORT=atoi(argv[1]);
     
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
    	if (listen(serverFD, 10)<0) 
    	{ 
        	perror("listen"); 
        	exit(EXIT_FAILURE); 
    	}
	printf("Waiting for connections\n");
	pthread_t threads[20];
	int i = 0;
	while (1){
		if ((newSock=accept(serverFD, (struct sockaddr *)&addr,(socklen_t*)&addrLen))<0)
		{ 
        		perror("accept"); 
        		exit(EXIT_FAILURE); 
    		}
		pthread_create(&threads[i], NULL, threadFunc, &newSock);
		i++;
		if (i > 9){
			for (i = 0; i < 10; i++){
				pthread_join(threads[i], NULL);
			}
			i = 0;
		}
	}	
	
    	return 0; 
} 
