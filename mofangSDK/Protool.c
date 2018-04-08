//#include <pthread.h>
//#include <stdio.h>
////#include <syslog.h>
//#include <string.h>
//#include <stdlib.h>
//#include <errno.h>
//#include <net/if.h>
//#include <arpa/inet.h>
//#include <netdb.h>
#include "Protool.h"
//#include <unistd.h>
//#include <netinet/tcp.h>
////#include "aes.h"
//#include "openssl/aes.h"
//#include "time.h"
//#include "Common.h"
//#include "Plug.h"
#include "config.h"


int            mRegist;
int            mLogin;
char           mLoginServer[16];
char           mLoginTocken[32];
char           mLoginPluginId[32];
int            mLoginPort;
int            mHeartInterval;
//int            mMacConfigNum;
long 		   mHeartTime;
int            mQuitProtool;
//ErrorCallback  mErrCB;



char g_pin_code[64] = "0200B00A33C0";
char g_sn_code[128] = "1234567890";
char gProtoolKey[32]= "DBC539BCCAED99FB";
char server_addr[32]= "171.208.222.113";
char server_port[16]= "6037";



void NewProtool()
{
	//pthread_mutex_init(&gProtoolMutex,NULL);
//	NewProgramProfile(CONFIG_FIE);
	//mTcpConnectServer =0;
	mRegist = 0;
	mLogin = 0;
	mQuitProtool = 0;
	mHeartInterval =10;
	//mMacConfigNum =0;
	mHeartTime=0;
	//RegistCB(0,RecCB);
	//RegistErrCB(RecErrCB);
}

void DeleteProtool()
{
  //	DeleteProgramProfile();
	//pthread_mutex_destroy(&gProtoolMutex);
	qwl_sendstring("pthread_mutex_destroy::Protool\n");
	
}


//int aes_encrypt(char* enckey,char* encbuf,char* decbuf,int inlen,int* outlen)
//{

//    AES_KEY aes;
////	pthread_mutex_lock(&gProtoolMutex);
//#if 1
//	if(encType ==0)
//	{
//	    char key[16]="";// = "12345678";
//		memcpy(key,enckey, 16);
//		char iv[16] = "";
//	    
//	    int nLen = inlen;//input_string.length();
//	    int nBei;

//		if((!encbuf)||(!decbuf))
//			return -1;
//		nBei = nLen / AES_BLOCK_SIZE + 1;
//		
//	    int nTotal = nBei * AES_BLOCK_SIZE;
//	    char *enc_s = (char*)malloc(nTotal);
//	    int nNumber;
//	    if (nLen % 16 > 0)
//	        nNumber = nTotal - nLen;
//	    else
//	        nNumber = 16;
//	    memset(enc_s, nNumber, nTotal);
//	    memcpy(enc_s, encbuf, nLen);

//	    if (AES_set_encrypt_key((unsigned char*)key, 128, &aes) < 0) {
//	        fprintf(stderr, "Unable to set encryption key in AES error !!!!!\n");
//			free(enc_s);
//	        return -1;
//	    }
//	    
//	    AES_cbc_encrypt((unsigned char *)enc_s, (unsigned char*)decbuf, nBei * 16, &aes, (unsigned char*)iv, AES_ENCRYPT);
//		//for(int i =0;i<(nBei * 16);i++)
//		//	printf("%02X ", decbuf[i]&0xff);
//		//printf("\n");

//		* outlen = nBei * 16;
//		free(enc_s);
//	}
//	else
//		#endif
//	{
//		memcpy(decbuf,encbuf,inlen);
//		*outlen = inlen;
//	}
////	pthread_mutex_unlock(&gProtoolMutex);
// 
//	return 0;
//}


//int aes_decrypt(char* enckey,char* encbuf,char* decbuf,int inlen,int* outlen)
//{


//	AES_KEY aes;
////	pthread_mutex_lock(&gProtoolMutex);
//	#if 1
//	if(encType ==0)
//	{
//	    char key[16]="";// = "12345678";
//		memcpy(key, enckey, 16);
//		char iv[16] = "";
//	    
//	    int nLen = inlen;//input_string.length();
//	    int nBei;

//		if((!encbuf)||(!decbuf))
//			return -1;
//		nBei = nLen / AES_BLOCK_SIZE + 1;
//		

//	    int nTotal = nBei * AES_BLOCK_SIZE;
//	    char *enc_s = (char*)malloc(nTotal);
//	    int nNumber;
//	    if (nLen % 16 > 0)
//	        nNumber = nTotal - nLen;
//	    else
//	        nNumber = 16;
//	    memset(enc_s, nNumber, nTotal);
//	    memcpy(enc_s, encbuf, nLen);

//	    if (AES_set_decrypt_key((unsigned char*)key, 128, &aes) < 0) {
//	        fprintf(stderr, "Unable to set decryption key in AES error !!!!!\n");
//			free(enc_s);
//	        return -1;
//	    }
//	    
//	    AES_cbc_encrypt((unsigned char *)enc_s, (unsigned char*)decbuf, nBei * 16, &aes, (unsigned char*)iv, AES_DECRYPT);
//		//for(int i =0;i<(nBei * 16);i++)
//		//	printf("%02X ", decbuf[i]&0xff);
//		//printf("\n");

//		* outlen = nBei * 16-16;
//		free(enc_s);
//	}
//	else
//		#endif
//	{
//		memcpy(decbuf,encbuf,inlen);
//		*outlen = inlen;
//	}
////	pthread_mutex_unlock(&gProtoolMutex);
//  
//	return 0;

//}
//int RegistErrCB(ErrorCallback cb){
//	mErrCB = cb;
//	
//	return 0;
//}


int InitProtool()
{
	int regist_port,ret;
	mQuitProtool =0;mRegist =0;mLogin =0;mHeartTime=0;
	
    
	printf("g_pin_code-----%s----%d\n",g_pin_code,(int)strlen(g_pin_code));
	printf("g_sn_code-----%s----%d\n",g_sn_code,(int)strlen(g_sn_code));
	printf("gProtoolKey-----%s----%d\n",gProtoolKey,(int)strlen(gProtoolKey));
	printf("server_addr-----%s----%d\n",server_addr,(int)strlen(server_addr));
	printf("server_port-----%s----%d\n",server_port,(int)strlen(server_port));
    
	regist_port = atoi(server_port);

//	ret = Running();
//	if(0!=ret)
//	{
//		printf("mTcpConnectServer->Running() error error !!!!!\n");
//	
//		return ret;
//		//exit (-1);
//	}	
//	
//	pthread_attr_t attr;
//	pthread_attr_init(&attr);
//	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
//	ret = pthread_create( &regisThreadId, &attr, RegistThread, 0 );
//	if( 0 != ret )
//	{
//		printf(  "Unable to create a thread for regist thread %s error !!!!!",
//			strerror( errno ) ) ;
//		
//		return -2;
//		//exit(-1);
//	}
	
	return 0;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DealMfPro
// 功能描述  : 魔方协议处理
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※ 

void DealMfPro(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
{	
	    printf("DealMfPro");
}
//int CBHandle(PACKAGE_HEAD phead,DATA_HEAD dhead,char* optbuff,char *buff,short seq)
//{
//	char out[1024];
//	int outlen;
//	//printf(">>>>>>CBHandle seq %d\n",seq);
//	
//	if(seq ==-1)//force logout
//	{
//		printf("net error reconnect\n");
//		mQuitProtool = 1;
//		return 0;
//	}
//	
//	switch ((phead.headp&0x1f))
//	{
//		case force_logout:
//			 printf("force disconnect exit !!!!!\n");
//			// exit( -1);
//			 break;
//		case heart_ack:
//			 printf("heart beep ack\n");
//			 time_t curtime;
//			 time(&curtime);
//			 mHeartTime = curtime;
//			 break;
//		case dev_bissness_pack_ip_ack:
//		{
//			printf("dev_bissness_pack_ip_ack %d\n",dhead.length);
//			if((dhead.length>0))
//			{
//				int ret = aes_decrypt((char*)gProtoolKey,buff, out,dhead.length,&outlen);
//				if(ret !=0)
//				{
//					printf("descrypt error\n");
//					break;
//				}
//				
//			/*	for(int i =2;i<outlen;i++)
//				{
//					printf("%c ",out[i]);
//				}
//				printf("\n");*/
//				
//			}
//			  
//			 break;
//		}
//		case dev_bissness_pack_ip_down:
//		{
//			printf("dev_bissness_pack_ip_down\n");
//			
//			if(dhead.length>0)
//			{
//				int ret = aes_decrypt((char*)gProtoolKey,buff, out,dhead.length,&outlen);
//				if(ret !=0)
//				{
//					printf("descrypt error\n");
//					break;
//				}
//				printf("len %d,outlen = %d,%d\n",dhead.length,outlen,out[outlen-1]);
//				if(encType==0)
//					outlen -=out[outlen-1];
//				
//				/*for(int i =2;i<outlen;i++)
//				{
//					printf("%c ",out[i]);
//				}
//				printf("\n");*/
//				char  checkkdata[1024];
//				memset(checkkdata,0,1024);
//				memcpy(checkkdata,&dhead.length,2);
//				
//				if(dhead.optLength)
//				memcpy(checkkdata+2,optbuff,dhead.optLength);
//				if(outlen>0)
//				memcpy(checkkdata+2+dhead.optLength,out,outlen);

//				U8 checks = GetCheckSum((unsigned char*)checkkdata,2+dhead.optLength+outlen);
//				printf("checks = %x, %x\n",checks,dhead.checksum);
//				
//				if(checks != dhead.checksum)
//				{
//					SendIpAckData(checksum_err,0,0);
//				}
//				else
//					SendIpAckData(0,(unsigned char*)"ack",3);
//			}
//			
//			break;
//		}
//		
//		
//		case ack:
//		{
//			if(!mRegist)
//			{
//				if(dhead.optLength>0)
//				{
//					short errerno = *(short*)optbuff;
//					
//					printf("optlen %d,code or radom %d\n",dhead.optLength,swapInt16(errerno));
//				}
//				printf("regist ack ===len %d\n",dhead.length);
//				if(dhead.length >0)
//				{

//					int ret = aes_decrypt((char*)gProtoolKey,buff, out,dhead.length,&outlen);
//					if(ret !=0)
//					{
//						printf("descrypt error\n");
//						break;
//					}
//					data_regist_response* res = (data_regist_response*)(out+2);
//					memcpy(mLoginPluginId,res->deviceId,32);
//					printf("regist ack======ip:%d,%d,%d,%d,port:%d,pluginId %s\n",
//						res->ip[0],res->ip[1],res->ip[2],res->ip[3],res->port,mLoginPluginId);
//					sprintf(mLoginServer,"%d.%d.%d.%d",res->ip[0],res->ip[1],res->ip[2],res->ip[3]);
//					mLoginPort = swapInt16(res->port);
//					mRegist = 1;

//					memcpy(mLoginTocken,res->token,32);
//					printf("regist ok tocken ,port %d\n",mLoginPort);
//				}
//				else
//				{
//					printf("regist ack error\n");
//					
//				}
//			}
//			else
//			{
//				printf("login ack\n");
//				if(dhead.optLength>0)
//				{
//					printf("optlen %d,code or radom %d\n",dhead.optLength,*(short*)optbuff);
//				
//				}
//				if(dhead.length >0)
//				{
//					int outlen;
//					
//					int ret = aes_decrypt((char*)gProtoolKey,buff, out,dhead.length,&outlen);
//					if(ret !=0)
//					{
//						printf("descrypt error\n");
//						break;
//					}
//						
//					data_longin_response* res = (data_longin_response*)(out+2);
//					mHeartInterval = swapInt16(res->heartbeat);
//					if(mHeartInterval<5)
//						mHeartInterval = 5;
//					printf("login ack heartbeep interval %d\n",swapInt16(res->heartbeat));
//					mLogin = 1;
//					
//				}
//				else
//				{
//					printf("log ack error\n");

//					//判断错误码，重新启动注册
//					mQuitProtool = 1;
//					
//					
//					
//				}
//			}
//			break;
//		}
//		
//		default:
//			break;
//	}
//	
//	
//	return 0;
//	
//}
//int RecCB(void* pOwner,PACKAGE_HEAD phead,DATA_HEAD dhead,char* optbuff,char *buff,short seq)
//{
//	//Protool* pProtool = (Protool*)pOwner;
//	return CBHandle( phead, dhead, optbuff,buff, seq);
//}
//int RecErrCB(int errorid)
//{
//	//Protool* pProtool = (Protool*)pOwner;
//	printf("--RecErrCB %d\n",errorid);
//	switch (errorid)
//		{
//		case errid_regist:
//		case errid_login:
//		case errid_heart:
//		case errid_forceout:
//			{
//				DestroyPlugSdk();
//				exit(0);
//				
//			}
//			break;
//		default:
//			break;
//		}
//	return 0;
//}

//void RegistHandle( )
//{
//	int ret =0;
//	int i =0,j=0;
//	pthread_t checkThreadId;


//	printf("mRegist %d,%d\n",mRegist,mQuitProtool);
//	while((!mRegist)&&(!mQuitProtool))
//	{
//		ret = RegistServer();
//		if(ret  ==-2)
//		{
//			mErrCB(errid_regist);
//			return ;
//		}
//		for(i =0;i<60;i++)
//		{
//			if((mQuitProtool)||(mRegist))
//				break;
//			sleep(1);
//		}
//	}

//	
//	//mTcpConnectServer->Destroy();
//	DestroyTcpServer();
//	DeleteTcpServer();
//	//delete mTcpConnectServer;

//	printf("quit regist,start login %s:%d\n",mLoginServer,mLoginPort);
//	ret = NewTcpServer(mLoginServer,mLoginPort);
//	if(ret==-2)
//	{
//		mErrCB(errid_regist);
//		return ;
//	}
//	for(i =0;i<5;i++)
//	{
//		ret =InitTcpServer();
//		if(ret ==-2)
//		{
//			mErrCB(errid_regist);
//			return ;
//		}
//		if(0!=ret)
//		{
//			printf("mTcpConnectServer->Init() error\n");
//			sleep(5);
//			continue;
//		}
//		else
//			break;
//	}

//	if((i ==5)||(mQuitProtool))
//	{
//		printf("login failed !\n");
//		DestroyTcpServer();
//		DeleteTcpServer();
//		//delete mTcpConnectServer;
//		if(mQuitProtool !=2)
//		{
//			ret = InitProtool();
//			if(ret==-2)
//			{
//				mErrCB(errid_regist);
//				return ;
//			}
//		}
//		return;
//	}
//	
//	ret =Running();
//	if(0!=ret)
//	{
//		printf("mTcpConnectServer->Running() error exit !!!!!\n");
//		mErrCB(errid_regist);
//		return ;
//		//exit(-1) ;
//	}
////	RegistCB(0,RecCB);	
//	
//	
//	for(i =0;i<3;i++)
//	{
//		ret = LoginServer();
//		if(ret==-2)
//		{
//			mErrCB(errid_login);
//			return ;
//		}
//		for( j =0;j<60;j++)
//		{
//			if((mQuitProtool)||(mLogin))
//				break;
//			sleep(1);
//		}
//		if((mQuitProtool)||(mLogin))
//				break;
//	}
//	//printf("login break i = %d\n",i);
//	if((i ==3)||(mQuitProtool))
//	{
//		printf("login failed !\n");
//		DestroyTcpServer();
//		DeleteTcpServer();
//		//delete mTcpConnectServer;
//		if(mQuitProtool !=2)
//		{
//			ret = InitProtool();
//			if(ret ==-2)
//			{
//				mErrCB(errid_login);
//				return ;
//			}
//		}
//		return;
//	}
//	pthread_attr_t attr;
//	pthread_attr_init(&attr);
//	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
//	ret = pthread_create( &checkThreadId, &attr, CheckThread, 0 );
//	if( 0!= ret ) {
//		 
//		printf(  "Unable to create a thread for checkThreadId thread %s exit !!!!!",
//			strerror( errno ) ) ;
//		mErrCB(errid_forceout);
//		//exit(-1);
//	}
//	while(!mQuitProtool)
//	{
//		//sleep(mHeartInterval);
//		for( i =0;i<mHeartInterval;i++)
//		{
//			if(mQuitProtool)
//				break;
//			sleep(1);
//		}
//		
//		Heartbeep();
//	}
//	//if(mTcpConnectServer)
//	{
//		DestroyTcpServer();
//		DeleteTcpServer();
//		//delete mTcpConnectServer;
//	}
//	if(mQuitProtool==1)
//	{
//		sleep(10);
//		ret = InitProtool();
//		if(ret ==-2)
//		{
//			mErrCB(errid_heart);
//		}
//	}
//	
//}
//void CheckHandle()
//{
//	while(!mQuitProtool)
//	{
//		time_t curtime;
//		time(&curtime);
//		long updateTime = curtime;
//		
//		//此处对心跳ack监测
//	
//		if((mHeartTime!=0)&&(updateTime-mHeartTime)>60)
//		{
//			mQuitProtool = 1;
//			printf("heart beep error\n");
//		}
//		
//		sleep(1);
//	}
//}
//void * CheckThread( void * arg )
//{
//	//Protool *pServer = (Protool *)arg;
//	pthread_detach(pthread_self());
//	CheckHandle();
//	return 0;
//}

//void * RegistThread( void * arg )
//{
//	//Protool *pServer = (Protool *)arg;
//	pthread_detach(pthread_self());
//	RegistHandle();
//	return 0;
//}


//int DestroyProtool()
//{
//	mQuitProtool =2;
//	//if(mTcpConnectServer)
//	sleep(1);
//	DestroyTcpServer();
//	DeleteTcpServer();
//	//delete mTcpConnectServer;
//	
//	return 0;

//}
//int GetLoginStatus()
//{
//	return mLogin;
//}
//unsigned char GetCheckSum(unsigned char* pack,int pack_len)
//{
//	unsigned char check_sum =0;
//	while(--pack_len>=0)
//	{
//		check_sum += *pack++;
//	}
//	return check_sum;
//}


//int RegistServer()
//{
//	//data_regist_opt optdata;
//	char optdata[16];
//	data_regist     data;
//	U8  checkkdata[1024];
//	memset(checkkdata,0,1024);
//	//memset(&optdata,0,sizeof(data_regist_opt));

//    if ((g_pin_code == NULL) || (strlen (g_pin_code) == 0)){
//        printf("!\n!\n!\n!\n########ERROR: PIN is empty, plese check your plug.conf!\n!\n!\n!\n");
//        exit(-1);
//    }
//	if(encType ==0){
//        if ((gProtoolKey == NULL) || (strlen (gProtoolKey) == 0)){
//            printf("!\n!\n!\n!\n########ERROR: KEY is empty, plese check your plug.conf!\n!\n!\n!\n");
//            exit(-1);
//        }
//    }

//	memcpy(&optdata[3],g_pin_code,strlen(g_pin_code));

//    
//    if (0 != get_dev_sn(g_sn_code)){
//        printf("!\n!\n!\n!\n########ERROR: SN is empty!\n!\n!\n!\n");
//        exit(-1);
//    };
//	
//	data.deviceSnLength=strlen(g_sn_code);////16;
//	char  decdata[1024];
//	memset(decdata,0,1024);
//	//int datalen = sizeof(data_regist)+data.deviceSnLength;//-1;
//	int datalen = 13+data.deviceSnLength+38;
//	//前两个字节是seq

//	memcpy(&decdata[2], g_pin_code, 12);

//	
//	decdata[2+12]=data.deviceSnLength;
//	//int ramdom = rand()%255;
//	//printf("ramdom %x\n",ramdom);
//	//if(ramdom==0)
//	//	ramdom = rand()%255;
//	//if(ramdom==0)
//	//	ramdom = 0x32;
//	//memset(&decdata[15],/*ramdom*/0x32,data.deviceSnLength);
//	
//	memcpy(&decdata[15], g_sn_code, data.deviceSnLength);

//    //设备MAC应使用设备自身的MAC地址:
//	U8 mac[6] = {0};
//    get_dev_mac(mac);

//    
//	memcpy(&decdata[15+data.deviceSnLength],mac,6);
////	memset(&decdata[15+data.deviceSnLength],0x32,6); //这里应设置为终端MAC
//	int dlen;
//	char miwen_hex[1024];
// 	int ret = aes_encrypt((char*)gProtoolKey,decdata,miwen_hex,datalen+2,&dlen);
//	if(ret !=0)
//		return -2;
//	//printf("aes_crypt_ecb encode: ----len %d---\n",dlen);
//	short dlen2 = dlen;
//	dlen2=swapInt16(dlen2);
//	memcpy(checkkdata,&dlen2,2);
//	memcpy(checkkdata+2,optdata,15);
//	memcpy(checkkdata+2+15,decdata,datalen+2);
//	
//	U8 checks = GetCheckSum((unsigned char*)checkkdata,2+15+datalen+2);
//	
//	return SendRouteUpData((unsigned char*)optdata,
//		(unsigned char*)miwen_hex,login_conform,1,15,
//		dlen,checks);
//	
//}

//int Heartbeep()
//{
//	data_heartbeat_opt optdata = {0};
//	time_t curtime;
//	time(&curtime);
//	//memset(&optdata,0,sizeof(data_heartbeat_opt));
//	optdata.timestamp = swapInt32(curtime);
//	U8 checks = GetCheckSum((unsigned char*)&optdata,sizeof(optdata));
//	int ret = SendRouteUpData((unsigned char*)&optdata,0,heart,1,sizeof(optdata),
//		0,checks);

//	
//	return ret;
//	
//}

//int LoginServer()
//{
//	data_login_opt optdata;
//	data_login     data;
//	memset(&data,0,sizeof(data));
//	
//	if((!mRegist)&&(!mLogin))
//		return -2;

//	printf(">>>>>LoginServer command  \n");
//	memset(optdata.devicePin,0,sizeof(optdata.devicePin));
//	memcpy(optdata.devicePin,g_pin_code,12);
//	
//	memset(optdata.token,0,sizeof(optdata.token));
//	memcpy(optdata.token,mLoginTocken,32);
//	//sprintf((char*)optdata.token,"%s",mLoginTocken);
//	data.protocol=0;
//	data.deviceVersion[0] = VERSION_0;
//	data.deviceVersion[1] = VERSION_1;
//	data.deviceVersion[2] = VERSION_2;
//	data.deviceVersion[3] = VERSION_3;

//	char  decdata[1024];
//	memset(decdata,0,1024);
//	char  checkkdata[1024];
//	memset(checkkdata,0,1024);
//	

//	int datalen = sizeof(data_login);

//	memcpy(&decdata[2],&data,sizeof(data_login));
//	
//	
//	int dlen;
//	char miwen_hex[1024];
//	
//	int ret = aes_encrypt((char*)gProtoolKey,decdata,miwen_hex,datalen+2,&dlen);
//	if(ret !=0)
//		return -2;
//	short dlen2 = dlen;
//	dlen2 = swapInt16(dlen2);
//	memcpy(checkkdata,&dlen2,2);
//	memcpy(checkkdata+2,&optdata,sizeof(optdata));

//	memcpy(checkkdata+2+sizeof(optdata),&decdata,datalen+2);

//	U8 checks = GetCheckSum((unsigned char*)checkkdata,2+datalen+sizeof(data_login_opt)+2);
//	
//	return SendRouteUpData((unsigned char*)&optdata,
//			(unsigned char*)miwen_hex,connection_conform,1,sizeof(optdata),
//			dlen,checks);

//}
///*unsigned char getCheckSum(unsigned char* pack,int pack_len)
//{
//	unsigned char check_sum =0;
//	while(--pack_len>=0)
//	{
//		check_sum += *pack++;
//	}
//	return check_sum;
//}
//*/

//int SendRouteUpData(unsigned char* optdata,unsigned char* data,PACKAGE_TYPE type,int isack,int optlen,int len,U8 checks)
//{
//	PACKAGE_HEAD pk_head;
//    DATA_HEAD dt_head;
//	//short dt_serial =0;
//	
//	memset(&pk_head,0,sizeof(pk_head));
//	memset(&dt_head,0,sizeof(dt_head));
//	
//	dt_head.optLength = optlen;
//		
//	if((len ==0)&&(optlen==0))
//		return -1;
//	isack =( isack|(encType<<1))&0x7;
//	
//	pk_head.magic = swapInt16(DX_MAGIC_HEAD);
//	pk_head.headp = (type|(isack<<5));
//	//printf("===%x\n",pk_head.headp);
//	dt_head.checksum=checks;
//	dt_head.length = swapInt16(len);	


//	char senddata[1024];
//	memcpy(senddata,&pk_head,3);
//	memcpy(senddata+3,&dt_head,4);
//	if((optlen)&&(optdata))
//	memcpy(senddata+7,optdata,optlen);
//	if((len)&&(data))
//	memcpy(senddata+7+optlen,&data[0],len);

//	int sendrt = SendData((unsigned char*)senddata,7+optlen+len);
//	if(sendrt==-1)
//	{
//		printf("mTcpConnectServer->sendData error\n");
//		return -1;
//	}
//	return 0;
//	
//}



//int SendIpUpData(unsigned char* buff,int len,int type) 
//{
//	int datalen;	
//	if(!mLogin)
//		return -1;
//	if((len>1022)||(len<=0))
//		return -1;
//	
//	char  decdata[1024];
//	memset(decdata,0,1024);
//	
//	memcpy(&decdata[2],buff,len);
//	datalen = len;
//	
//	int dlen;
//	char miwen_hex[1024];
//				
//	int ret = aes_encrypt((char*)gProtoolKey,decdata,miwen_hex,datalen+2,&dlen);
//	if(ret !=0)
//		return ret;
//	short dlen2 = dlen;
//	dlen2 = swapInt16(dlen2);
//	char  checkkdata[1024];
//	memset(checkkdata,0,1024);
//	memcpy(checkkdata,&dlen2,2);
//	memcpy(checkkdata+2,decdata,datalen+2);
//	
//	U8 checks = GetCheckSum((unsigned char*)checkkdata,2+datalen+2);
//	
//	return SendRouteUpData((unsigned char*)0,
//			(unsigned char*)miwen_hex, dev_bissness_pack_ip_up, ACK_YES,0,
//			dlen,checks);
//	
//}

//int SendIpAckData(short errcode,unsigned char* buff,int len) //type =0--route,1--ip dev
//{
//		
//	if(!mLogin)
//		return -1;
//	if(len>1022)
//		return -1;
//	char optdata[2];
//	errcode = swapInt16(errcode);
//	memcpy(optdata,&errcode,2);
//	
//	if(len>0)
//	{
//		char  decdata[1024];
//		memset(decdata,0,1024);
//		
//		memcpy(&decdata[2],buff,len);
//		
//		int datalen = len;
//		int dlen;
//		char miwen_hex[1024];
//						
//		int ret = aes_encrypt((char*)gProtoolKey,decdata,miwen_hex,datalen+2,&dlen);
//		if(ret !=0)
//			return ret;
//		
//		short dlen2 = dlen;
//		dlen2 = swapInt16(dlen2);
//		char  checkkdata[1024];
//		memset(checkkdata,0,1024);
//		memcpy(checkkdata,&dlen2,2);
//		memcpy(checkkdata+2,optdata,2);
//		memcpy(checkkdata+2+2,decdata,datalen+2);
//		
//		U8 checks = GetCheckSum((unsigned char*)checkkdata,2+2+datalen+2);
//		
//		return SendRouteUpData((unsigned char*)optdata,
//				(unsigned char*)miwen_hex, dev_bissness_pack_ip_ack, ACK_NO,2,
//				dlen,checks);
//	}
//	else
//	{
//		int dlen =0;
//		char  checkkdata[1024];
//		short dlen2 = dlen;
//		dlen2 = swapInt16(dlen2);
//		memset(checkkdata,0,1024);
//		memcpy(checkkdata,&dlen2,2);
//		memcpy(checkkdata+2,optdata,2);
//		
//		
//		U8 checks = GetCheckSum((unsigned char*)checkkdata,2+2);
//		return SendRouteUpData((unsigned char*)optdata,
//				(unsigned char*)0, dev_bissness_pack_ip_ack, ACK_NO,2,
//				0,checks);
//	}
//}





