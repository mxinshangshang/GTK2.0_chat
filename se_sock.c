/************************************************
 * se_sock.c                                  		     *
 * function:                                  		     *
 *    create a socket                        		     *
 *    create a new thread,for receive function  *
 *    send message                             	      *
 ************************************************/
#include "server.h"
socklen_t len;
bool from_host;//表示客户端是否已经向本服务器发送来信息
int sockfd;
int cl_sockfd;
struct sockaddr_in saddr,caddr;//saddr表示本地服务器地址信息，caddr用于保存向本服务器发送信息的主机地址信息
void *build_socket(void *arg)
{
	int res;
	pthread_t recv_thread;
	pthread_attr_t thread_attr;
	len=sizeof(caddr);
	/* Set status of thread */
	res=pthread_attr_init(&thread_attr);
	if(res!=0)
	{
		show_err("Setting detached attribute failed");
		exit(EXIT_FAILURE);
	}
	/* Create a socket */
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		show_err("Socket Error");
		exit(1);
	}
	
	bzero(&saddr,sizeof(struct sockaddr));
	saddr.sin_family=AF_INET;
	saddr.sin_addr.s_addr=htonl(INADDR_ANY);
	saddr.sin_port=htons(PORT);
	if(bind(sockfd,(struct sockaddr *)&saddr,sizeof(struct sockaddr_in))==-1)
	{
		show_err("Bind Error");
		exit(1);
	}
	/* Set the status of thread,don't wait for return of the subthread */
	res=pthread_attr_setdetachstate(&thread_attr,PTHREAD_CREATE_DETACHED);
	if(res!=0)
	{
		show_err("Setting detached attribute failed");
		exit(EXIT_FAILURE);
	}
	if(listen(sockfd,5)<-1)
	{
		show_err("Listen Error");
		exit(1);
	}    
        while(1)
	{
                cl_sockfd=accept(sockfd,(struct sockaddr *)&caddr,&len);
        	if (cl_sockfd < 0)
        	{
            	printf("Server Accept Failed!\n");
            	exit(EXIT_FAILURE);
        	}
               /* Create a sub thread,call recv_func() */
	       res=pthread_create(&recv_thread,&thread_attr,recv_func,NULL); 
		//g_thread_new(NULL,recv_func,NULL);
	       if(res!=0)
	       {
		     show_err("Thread create error");
		     exit(EXIT_FAILURE);
	       }
               from_host=true;
	       /* Callback the attribute of thread */
	       (void)pthread_attr_destroy(&thread_attr);
	}
}
/* Receive function */
void *recv_func(void *arg)
{
	char recv_text[MAXSIZE];
	while(1)
	{	/* To Receive message from client and get the address infomation */
		if(recv(cl_sockfd,recv_text,MAXSIZE,0)<0)  
		{
			show_err("server recv error");
			exit(1);
		}
		if( from_host==false ) from_host=true;
		show_remote_text(recv_text);
	}
}
/* Send function,send message to client */
void send_func(const char * send_text)
{
	/* If there is no text,continue */
	if(strlen(send_text)==1)
		return;
	if( !from_host ) show_err("Unknown the client's address infomation/nPlease wait the client to connect you first !/n");
	/* Send message */
	if(from_host && send(cl_sockfd,send_text,MAXSIZE,0)<0)
	{
		show_err("S send error");
		exit(1);
	}
}

