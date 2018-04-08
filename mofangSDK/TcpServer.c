//#include <pthread.h>
//#include <stdio.h>
////#include <syslog.h>
//#include <string.h>
//#include <stdlib.h>
//#include <errno.h>
//#include <net/if.h>
//#include <arpa/inet.h>
//#include <netdb.h>
//#include "TcpServer.h"
//#include <unistd.h>
//#include <netinet/tcp.h>

//#include  <sys/types.h>
//#include <sys/socket.h>
//#include <stdio.h>
//#include <sys/un.h>
//#include <fcntl.h>
//#include <sys/ioctl.h>
//#include <netinet/in.h>

////extern NetDump* gNetDump;
//char mBindIp[16];
//int  mPort;
//int  mSocket;
//int  mQuit;
//RecCallback mCB;
//void*		mPCB;
//int 		mConnected;

//pthread_mutex_t mMutexTcp;

//int GetServerIp(const char * hostname, char *server_ip){
//    struct hostent *hptr;
//    if((hptr = gethostbyname(hostname)) == NULL)
//    {
//        printf("######## gethostbyname error for host:%s\n", hostname);
//        return -1; 
//    }
//    int i = 0;
//    
//    while(hptr->h_addr_list[i] != NULL)
//    {
//        printf("######## hostname: %s\n",hptr->h_name);
//        sprintf(server_ip, "%s", inet_ntoa(*(struct in_addr*)hptr->h_addr_list[i]));
//        printf("######## ip: %s\n", server_ip);
//        break;
//    }
//    if (0 == strlen(server_ip)){
//        printf("######## server_ip is empty.\n");
//        return -2; 
//    }

//    return 0;
//};

//int NewTcpServer(const char *host,int port)
//{
//    char bindIp[64] = "";
//	int ret = 0;
//	mPort = port;
//	mSocket =0;
//	mConnected =0;
//	mQuit =0;
//    
//    ret = GetServerIp(host, bindIp);
//    if (0 != ret){
//        printf("######## ERROR: GetServerIp failed.\n\n");
//        return ret;
//    }
//    
//	snprintf(mBindIp, sizeof( mBindIp ), "%s", bindIp);
//	
//    ret = pthread_mutex_init(&mMutexTcp,NULL);
//    if (0 != ret){
//        printf("######## ERROR: pthread_mutex_init failed: errno: %d \n\n", ret);
//        return ret;
//    }
//	return 0;
//}

//void DeleteTcpServer()
//{
//	mQuit = 1;
//	pthread_mutex_destroy(&mMutexTcp);
//}

//int DestroyTcpServer()
//{
//	mQuit = 1;
//	sleep(1);
//	pthread_mutex_lock(&mMutexTcp);
//	if(mSocket)
//		close(mSocket);
//	mSocket =0;
//	pthread_mutex_unlock(&mMutexTcp);
//	return 0;
//}
//int RegistCB(void* pOwner,RecCallback cb){
//	mCB = cb;
//	mPCB = pOwner;
//	return 0;
//}

//int InitTcpServer()
//{
//	//int ret = 0;
//	int yes = 1, bufSize = 12800;//50*1024;
//	 
//	struct sockaddr_in server_addr;  
//    socklen_t socklen = sizeof(server_addr);  
//	pthread_mutex_lock(&mMutexTcp);
//	
//	mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//	if (mSocket == -1) {
//		mSocket =0;
//		pthread_mutex_unlock(&mMutexTcp);
//		printf("Tcp socket() fail.");
//		return -2;
//	}
//	
//	
//    printf("init_tcp_sender() - connecting to %s\n", mBindIp);
//	
//    if (setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR,
//		(char *)&yes, sizeof(int)) == -1)
//	{
//		printf("init_tcp_sender(), setsockopt() - error\n");
//		
//		close(mSocket);
//		mSocket =0;
//		pthread_mutex_unlock(&mMutexTcp); 
//		return -2;
//	}
//	
//	if (setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char *)&bufSize, sizeof(int)) == -1)
//	{
//		printf("init_tcp_sender(), setsockopt() - error\n");
//		close(mSocket);
//		mSocket =0;
//		pthread_mutex_unlock(&mMutexTcp); 
//		return -2;
//	}
//	
//	
//    bzero(&server_addr, sizeof(server_addr));  
//    server_addr.sin_family = AF_INET;  
//    inet_aton(mBindIp, &server_addr.sin_addr);  
//    server_addr.sin_port = htons(mPort);  
//  
//    if(connect(mSocket, (struct sockaddr*)&server_addr, socklen) < 0)  
//    {  
//        printf("can not connect to %s:%d \n", mBindIp,mPort);  
//        //printf("%s\n", strerror(errno));  
//		close(mSocket);
//		mSocket =0;
//		pthread_mutex_unlock(&mMutexTcp); 
//       return -1; 
//    }  
//	printf("init_tcp_sender() - connected to %s\n", mBindIp);
//	U32 mode = 1;
//    ioctl(mSocket, FIONBIO, &mode);
//	
//	mConnected = 1;
//	pthread_mutex_unlock(&mMutexTcp); 
//	
//	return 0;


//}



//int SendData(unsigned char* data,int size)
//{
//	int counter = 0;
//	int counter1 =0;
//    int num_bytes, network_bytes;
//    int total_bytes = 0;
//    int file_bytes = size; 
//    unsigned char *pBuf = data;

//	if(mSocket<=0)
//		return -1;

//    while (file_bytes > 0) 
//    {
//        if (file_bytes > 12800) 
//            network_bytes = 12800;
//        else 
//            network_bytes = file_bytes;
//		pthread_mutex_lock(&mMutexTcp); 
//		num_bytes=send(mSocket, pBuf, network_bytes, MSG_DONTWAIT);
//		pthread_mutex_unlock(&mMutexTcp); 
//	    if (num_bytes== -1) 
//        {
//			if(errno==104||errno ==88||errno ==32)
//			{
//				printf("send error no =%d",errno);
//				PACKAGE_HEAD phead;
//				DATA_HEAD dhead;
//							
//				mCB(mPCB,phead,dhead,0,0,0xffff);//?????
//				return -1;
//			}

//			if(errno!=EWOULDBLOCK)
//			{
//				counter++;
//			}
//			else
//			{
//				counter1++;
//				usleep(10);
//			}
//            num_bytes = 0;
//            if (counter > 1000) {
//               
//                return -1;
//            }
//			 if (counter1 > 500) {
//            
//                return 0;
//            }
//        }
//		else
//		{
//			counter1 = 0;
//			counter=0;
//		}

//        if ( num_bytes != 0 )
//        {
//            file_bytes -= num_bytes;
//            total_bytes += num_bytes;
//            pBuf += num_bytes;
//        }
//    }

//    return 0;
//}




//int Running()
//{
//	int ret = -1;
//	printf("TcpServer::running\n");


//	
//	ret = pthread_create( &acceptThreadId, &attr, ResponseThread, 0 );
//	
//	if( 0 == ret ) {
//		printf("Thread #%ld has been created to accept on port [%d]", acceptThreadId, mPort );
//	} else {
//		printf("Unable to create a thread for TCP server accept on port [%d], %s",
//			mPort, strerror( errno ) ) ;
//		return -2;
//	}

//	ret = pthread_create( &recvThreadId, &attr, RecvThread, 0 );
//	if( 0 == ret ) {
//		printf("Thread #%ld has been created to recv on port [%d]", recvThreadId, mPort );
//	} else {
//		printf("Unable to create a thread for TCP server recv on port [%d], %s",
//			mPort, strerror( errno ) ) ;
//		return -2;
//	}
//	return ret;
//}

//void ResponseHandle()
//{
//	int ret;
//	char buffer[1024];
//	PACKAGE_HEAD phead;
//	DATA_HEAD dhead;
//	char  optbuff[1024];
//	short seq;
//	
//	pthread_detach(pthread_self());
//	while (!mQuit)
//	{
//		
//		ret = popup(&phead,&dhead,optbuff,buffer,&seq);
//		if(ret==0)
//		{
//			//printf("=====pop data\n");
//			
//			if((phead.headp&0x1f)==11)
//				printf("regist or connnect ack len = %d,%d\n",dhead.length,dhead.optLength);
//			mCB(mPCB,phead,dhead,optbuff,buffer,seq);//?????
//			continue;	
//		}
//		else
//		{
//			sleep(1);
//			
//		}
//	}
//	pthread_exit(NULL);
//}

//void RecvHandle()
//{
//	int rlen;
//	char buffer[1024];
//	int size = 1022;
//	printf(">>>>>>RecvHandle\n");
//	pthread_detach(pthread_self());
//				
//	while (!mQuit) 
//	{
//		pthread_mutex_lock(&mMutexTcp); 
//		rlen = recv(mSocket, buffer, size,0);	
//		pthread_mutex_unlock(&mMutexTcp); 
//		if(rlen>0)
//		{
//		//	printf("====%d\n",rlen);
//		//	for(int i =0;i<rlen;i++)
//		//		printf("%02x ",buffer[i]&0xff);
//		//	printf("\n");
//			push(buffer,rlen);
//			continue;
//			
//		}
//		
//		else if(rlen==0)
//		{
//			
//		
//			 usleep(1000);
//			 continue;
//		}
//		
//		else if(rlen ==-1)
//		{

//			if(errno!=EWOULDBLOCK)
//			{
//				PACKAGE_HEAD phead;
//				DATA_HEAD dhead;
//				printf("net error errno %d\n",errno);				
//				mCB(mPCB,phead,dhead,0,0,0xffff);//?????
//				 break;	
//			}

//			 usleep(100000);
//			 continue;
//		}
//		// usleep(1);
//				
//	}
//	pthread_mutex_lock(&mMutexTcp); 
//	if(mSocket)
//	{
//		close(mSocket);
//		mSocket =0;
//	}
//	pthread_mutex_unlock(&mMutexTcp); 
//	pthread_exit(NULL);
//			
//}

//void Response()
//{
//}
//void * SendThread( void * arg )
//{
//	//TcpServer *pServer = (TcpServer *)arg;
//	unsigned char test[]="123";
//	while(1)
//	{
//		SendData(test,3);
//		sleep(10);
//	}
//	//pServer->acceptHandle();
//	return 0;
//}


//void * ResponseThread( void * arg )
//{
//	//TcpServer *pServer = (TcpServer *)arg;
//	pthread_detach(pthread_self());
//	ResponseHandle();
//	return 0;
//}

//void * RecvThread( void * arg )
//{
//	//TcpServer *pServer = (TcpServer *)arg;
//	pthread_detach(pthread_self());
//	RecvHandle();
//	return 0;
//}


