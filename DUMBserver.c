#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pthread.h>


typedef struct box
{
	char name[1024];
	char msg[1024][1024];
	int used;
	struct box* next;
}box;

box* list;
int sockets[256];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void add(struct box* new,int newSock)
{
	if(list==NULL)
	{
		list=new;
		return;
	}	
	struct box* ptr;
	for(ptr=list;ptr!=NULL;ptr=ptr->next)
	{
		if(!strcmp(ptr->name,new->name))
		{
			char* response= "ER:EXIST";
			send(newSock,response,strlen(response),0);
			return;
		}		
		if(ptr->next==NULL)
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
		struct box* new = (box*)malloc(sizeof(box));
		strncpy(new->name,name,1024);
		new->msg;
		new->next=NULL;
		new->used = 0;
		add(new,newSock);
		response="OK!";
		send(newSock,response,strlen(response),0);
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
			if(ptr->msg[0][0]!='\0')
			{
				response="ER:NOT EMPTY";
				send(newSock,response,strlen(response),0);
				return;
			}
			prev->next=ptr->next;
			free(ptr);
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
	printf("%d\n",newSock);
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
		int i,open;
		open=0;
		while(1)
		{
			printf("%s\n",cmd);
			rval=recv(newSock,command,1024,0);
			if(!strcmp(command,"GDBYE"))
			{
				printf("Client Left");
				break;
			}
			strncpy(cmd,command,6);
			strncpy(content,command+6,1024);
			
			
			if(!strcmp(cmd,"CREAT "))
			{
				pthread_mutex_lock(&lock);
				create(content,newSock);
				pthread_mutex_unlock(&lock);
			}
			else if(!strcmp(cmd,"OPNBX "))
			{
				if(open==1)
				{
					response="ER:OPEND";
					send(newSock,response,strlen(response),0);
					continue;
				}
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
				for(curr=list;curr!=NULL;curr=curr->next)
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
				i=0;
				open=1;
			}
			else if(!strcmp(cmd,"NXTMG "))
			{
				if(!curr)
				{
					response="ER:NOOPN";
					send(newSock,response,strlen(response),0);
					continue;
				}
				if(i>1024)
				{
					response="ER:EMPTY";
					send(newSock,response,strlen(response),0);
					continue;
				}
				printf("Message: '%s'\n",curr->msg[0]);
				response=curr->msg[0];
				if(response==NULL)
				{
					response="ER:EMPTY";
					send(newSock,response,strlen(response),0);
					continue;
				}
				int q,size;
				char sz[1024];
				size=strlen(response);
				sprintf(sz, "OK!%d!",size);
				printf("%d\n",size);
				strcat(sz,response);
				for(q=0;curr->msg[q][0]!='\0';q++){					
					memset(curr->msg[q], 0, 1024);
					strncpy(curr->msg[q], curr->msg[q+1], 1024);
				} 
				send(newSock,sz,strlen(sz),0);
			}
			else if(!strcmp(cmd,"PUTMG!"))
			{
				if(!curr)
				{
					response="ER:NOOPN";
					send(newSock,response,strlen(response),0);
					continue;
				}
				if(i>1024)
				{
					response="ER:LIMIT";
					send(newSock,response,strlen(response),0);
					break;
				}	
				int size=0;
				char sz[1024];
				for(i=0;content[i]!='!';i++)
				{
					size*=10;
					printf("%c\n",content[i]);
					size+=content[i]-'0';
				}
				printf("%d, %d\n",size, i);
				sprintf(sz,"OK!%d",size);
				strncpy(content,content+(i+1),1024);
				printf("%s\n",content);
				for(i=0;curr->msg[i][0]!='\0';i++)
					printf("%s, %d\n",curr->msg[i],i);
				
				strncpy(curr->msg[i],content,1024);
				send(newSock,sz,strlen(sz),0);
					
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
				curr->used=0;
				curr=NULL;
				open=0;
				send(newSock,response,strlen(response),0);
			}
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
