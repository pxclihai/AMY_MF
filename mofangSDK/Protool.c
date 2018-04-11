
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
#include "MF_Common.h"
//#include "Plug.h"
#include "config.h"

#include "PlugSdk.h"

int            mRegist;
int            mLogin;
char           mLoginServer[16];
char           mLoginTocken[32];
char           mLoginPluginId[32];
int            mLoginPort;
int            mHeartInterval;
//int          mMacConfigNum;
long 		       mHeartTime;
int            mQuitProtool;
//ErrorCallback  mErrCB;


////////////////////////////////////////
#define VERSION_0   0x0A
#define VERSION_1   0x00
#define VERSION_2   0x00
#define VERSION_3   0x02


char g_pin_code[64] = "0200B00A33C0";
char g_sn_code[128] = "1234567890";
char gProtoolKey[32]= "DBC539BCCAED99FB";
char server_addr[32]= "171.208.222.113";
char server_port[16]= "6037";
 
 
 
/*static fun*/
static int RegistServer();
static int popup(PACKAGE_HEAD* phead,DATA_HEAD *dhead,char* optbuff,char *buff,short* seq);
static int push(char *buff,int length);

static int Heartbeep(void);
int CBHandle(PACKAGE_HEAD phead,DATA_HEAD dhead,char* optbuff,char *buff,short seq);
void ResponseHandle(void);
void RegistHandle(void );
u32 MF_Get_time(time_t* time);
/*********buf区域***********/
char mData[1024];
int  mOffset;
int NewTcpServer(const char *host,int port);
int SendIpUpData(unsigned char* buff,int len,int type) ;
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


/****************临时测试� 接受函数  中断�-->buf*************/	
u8 MF_recv_buf[1024];
static u8 MF_tx_flag = 0;	
static u16 Rx_count;
static u8 rx_flag;
void MF_net_rec(u8 data)
{

 
	if(Rx_count<1024 && rx_flag ==0)
	{
			MF_recv_buf[Rx_count] = data;
 			Rx_count++;
	}
	if((Rx_count)>=1024)
	{
	//printf("over flow\n");
	// mOffset = 0;
	}
	

		
}

/****************测试打印�************/
static int ret_pos; 
static int push_time;
void MF_RECV()
{
  	static int count;
	
	  
	  if(push_time >60000 &&MF_tx_flag == 1)
		{
			push_time =0;
			rx_flag = 1;
			push(MF_recv_buf,Rx_count);
			Rx_count = 0;
			rx_flag = 0;
	
		}
		push_time ++;
		if(count >100 && MF_tx_flag == 1)
		{
	
		//	ret_pos = strsearch_h("ZIPRECV",mData);
		//	if(ret_pos>0)
	  	{
			 // qwl_sendstring("*****************\n");
            ResponseHandle();
		//	qwl_sendstring();
			}
		
			//qwl_sendstring(mData);
			count = 0;
		}
		count++;
	
}
/****************在此增加加密*************/
static int aes_encrypt(char* enckey,char* encbuf,char* decbuf,int inlen,int* outlen)
{

		memcpy(decbuf,encbuf,inlen);
		*outlen = inlen;
	  return 0;
}
/****************在此增加解密*************/
int aes_decrypt(char* enckey,char* encbuf,char* decbuf,int inlen,int* outlen)
{

	{
		memcpy(decbuf,encbuf,inlen);
		*outlen = inlen;
	}
	return 0;

}
/***************得到校验�***************/
static unsigned char GetCheckSum(unsigned char* pack,int pack_len)
{
	unsigned char check_sum =0;
	while(--pack_len>=0)
	{
		check_sum += *pack++;
	}
	return check_sum;
}
/***************发送数�***************/
static int SendData(unsigned char* data,int size)
{
	 
	  if(net.reconnect_setp==LINK_OK)
		{
			
	  	MF_tx_flag = 1;
	  	mOffset   = 0;
	  	memset(mData,0,1024);
			push_time = 0;
			net_send(data,size);
		}	
}
/***************上传数据****************/
static int SendRouteUpData(unsigned char* optdata,unsigned char* data,PACKAGE_TYPE type,int isack,int optlen,int len,U8 checks)
{	
	char senddata[1024];
	int sendrt;
	PACKAGE_HEAD pk_head;
	DATA_HEAD dt_head;
	
	memset(&pk_head,0,sizeof(pk_head));
	memset(&dt_head,0,sizeof(dt_head));
	
	dt_head.optLength = optlen;
		
	if((len ==0)&&(optlen==0))
		return -1;
	isack =( isack|(encType<<1))&0x7;
	
	pk_head.magic = swapInt16(DX_MAGIC_HEAD);
	pk_head.headp = (type|(isack<<5));
	//printf("===%x\n",pk_head.headp);
	dt_head.checksum=checks;
	dt_head.length = swapInt16(len);	



	memcpy(senddata,&pk_head,3);
	memcpy(senddata+3,&dt_head,4);
	if((optlen)&&(optdata))
		memcpy(senddata+7,optdata,optlen);
	if((len)&&(data))
		memcpy(senddata+7+optlen,&data[0],len);

   sendrt = SendData((unsigned char*)senddata,7+optlen+len);
	if(sendrt==-1)
	{
		printf("mTcpConnectServer->sendData error\n");
		return -1;
	}
	return 0;
	
}	


int SendIpUpData(unsigned char* buff,int len,int type) 
{
	int datalen;	
	int dlen;
	static char miwen_hex[1024];
	int ret;
	short dlen2 ;
	static char checkkdata[1024];
	static	char  decdata[1024];
	u8 checks;
	if(!mLogin)
		return -1;
	if((len>1022)||(len<=0))
		return -1;
	
	memset(decdata,0,1024);
	
	memcpy(&decdata[2],buff,len);
	datalen = len;
	

				
	ret = aes_encrypt((char*)gProtoolKey,decdata,miwen_hex,datalen+2,&dlen);
	if(ret !=0)
		return ret;
	dlen2 = dlen;
	dlen2 = swapInt16(dlen2);
	
	memset(checkkdata,0,1024);
	memcpy(checkkdata,&dlen2,2);
	memcpy(checkkdata+2,decdata,datalen+2);
	
	checks = GetCheckSum((unsigned char*)checkkdata,2+datalen+2);
	
	return SendRouteUpData((unsigned char*)0,
			(unsigned char*)miwen_hex, dev_bissness_pack_ip_up, ACK_YES,0,
			dlen,checks);
	
}
/*****************MF_注册设备*************/
int RegistServer()
{
	
	static char  miwen_hex[1024];
	static char  decdata[1024];
	static u8    checkkdata[1024];
	//data_regist_opt optdata;
	char optdata[16];  
	data_regist data;

	u8    checks;
	short dlen2;
	int   ret;
	int   dlen;

	int   datalen ;
    u8 mac[6] = {0};
	memset(checkkdata,0,1024);
	//memset(&optdata,0,sizeof(data_regist_opt));

	if ((strlen (g_pin_code) == 0)){
	 	printf("!\n!\n!\n!\n########ERROR: PIN is empty, plese check your plug.conf!\n!\n!\n!\n");
	 	return -1;
	}
	if(encType ==0){
		if ((strlen (gProtoolKey) == 0)){
			 printf("!\n!\n!\n!\n########ERROR: KEY is empty, plese check your plug.conf!\n!\n!\n!\n");
			return -1;
		}
  }

	memcpy(&optdata[3],g_pin_code,strlen(g_pin_code));


	if (0 != MF_get_dev_sn(g_sn_code)){
		 printf("!\n!\n!\n!\n########ERROR: SN is empty!\n!\n!\n!\n");
		 return -1;
	};
	
	data.deviceSnLength=strlen(g_sn_code);////16;
	

	memset(decdata,0,1024);
	//int datalen = sizeof(data_regist)+data.deviceSnLength;//-1;
	datalen = 13+data.deviceSnLength+38;
	//前两个字节是seq

	memcpy(&decdata[2], g_pin_code, 12);

	
	decdata[2+12]=data.deviceSnLength;
	
	memcpy(&decdata[15], g_sn_code, data.deviceSnLength);

    //设备MAC应使用设备自身的MAC地址:
    MF_get_dev_mac(mac);
	/////test
  mac[0] = 0x11;
	mac[1] = 0x22;
	mac[2] = 0x33;
	mac[3] = 0x44;
	mac[4] = 0x55;
	mac[5] = 0x66;

	memcpy(&decdata[15+data.deviceSnLength],mac,6);

	
	ret = aes_encrypt((char*)gProtoolKey,decdata,miwen_hex,datalen+2,&dlen);
//	if(ret !=0)
//		return -2;
	//printf("aes_crypt_ecb encode: ----len %d---\n",dlen);
	dlen2 = dlen;
	dlen2=swapInt16(dlen2);
	memcpy(checkkdata,&dlen2,2);
	memcpy(checkkdata+2,optdata,15);
	memcpy(checkkdata+2+15,decdata,datalen+2);
	
    checks = GetCheckSum((unsigned char*)checkkdata,2+15+datalen+2);

	return SendRouteUpData((unsigned char*)optdata,
	                     	(unsigned char*)miwen_hex,login_conform,1,15,
	                     	dlen,checks);
	
}
int MF_Login_Connect()
{
	int ret =0;
	printf("quit regist,start login %s:%d\n",mLoginServer,mLoginPort);
	ret = NewTcpServer(mLoginServer,mLoginPort);
	if(ret==-2)
	{
		RecErrCB(errid_regist);
		return 0;
	}
}
void MF_Regist()
{
	int ret =0;
	int i =0,j=0;

	printf("mRegist %d,%d\n",mRegist,mQuitProtool);

	ret = RegistServer();
	if(ret  ==-2)
	{
		RecErrCB(errid_regist);
		return ;
	}
	
	
//	printf("quit regist,start login %s:%d\n",mLoginServer,mLoginPort);
//	ret = NewTcpServer(mLoginServer,mLoginPort);
//	if(ret==-2)
//	{
//		RecErrCB(errid_regist);
//		return ;
//	}
//	for(i =0;i<5;i++)
//	{
//		ret =InitTcpServer();
//		if(ret ==-2)
//		{
//			RecErrCB(errid_regist);
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
//		RecErrCB(errid_regist);
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
	
}

int MF_LoginServer()
{
	data_login_opt optdata;
	data_login     data;
	static char  decdata[1024];
	static char  checkkdata[1024];
	static char miwen_hex[1024];
	int dlen;
	int ret;
	short dlen2;
	u8 checks;
	int datalen;
	memset(&data,0,sizeof(data));
	
	if((!mRegist)&&(!mLogin))
		return -2;

	printf(">>>>>LoginServer command  \n");
	memset(optdata.devicePin,0,sizeof(optdata.devicePin));
	memcpy(optdata.devicePin,g_pin_code,12);
	
	memset(optdata.token,0,sizeof(optdata.token));
	memcpy(optdata.token,mLoginTocken,32);
	//sprintf((char*)optdata.token,"%s",mLoginTocken);
	data.protocol=0;
	data.deviceVersion[0] = VERSION_0;
	data.deviceVersion[1] = VERSION_1;
	data.deviceVersion[2] = VERSION_2;
	data.deviceVersion[3] = VERSION_3;

	
	memset(decdata,0,1024);

	memset(checkkdata,0,1024);
	

	datalen = sizeof(data_login);

	memcpy(&decdata[2],&data,sizeof(data_login));
	
	ret = aes_encrypt((char*)gProtoolKey,decdata,miwen_hex,datalen+2,&dlen);
	if(ret !=0)
		return -2;
	dlen2 = dlen;
	dlen2 = swapInt16(dlen2);
	memcpy(checkkdata,&dlen2,2);
	memcpy(checkkdata+2,&optdata,sizeof(optdata));

	memcpy(checkkdata+2+sizeof(optdata),&decdata,datalen+2);

	checks = GetCheckSum((unsigned char*)checkkdata,2+datalen+sizeof(data_login_opt)+2);
	
	return SendRouteUpData((unsigned char*)&optdata,
			(unsigned char*)miwen_hex,connection_conform,1,sizeof(optdata),
			dlen,checks);

}
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
	
	//RegistHandle();
	//test
  //MF_RegistServer();
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
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※�// 函数名称  : DealMfPro
// 功能描述  : 魔方协议处理
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※�

void DealMfPro(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
{	
	    printf("DealMfPro");
}
/*************recv handle*******************/
static int push(char *buff,int length)
{
	
	if((mOffset+length)>1024)
	{
		printf("over flow\n");
		return -1;
	}
	memcpy(mData+mOffset,buff,length);
	mOffset+= length;
	return 0;
}
static int popup(PACKAGE_HEAD* phead,DATA_HEAD *dhead,char* optbuff,char *buff,short* seq)
{
	int i =0;
	int j =0;
	int tmp =0;
	int tmplen ;
	static char tmp_buf [1024];
	if((int)(mOffset)>(int)(sizeof(PACKAGE_HEAD)))//最小要大于包头
	{
		for( i =0;i<mOffset-2;i++)
		{
			*phead= *(PACKAGE_HEAD*)(mData+i);
			
			if((*phead).magic == swapInt16(DX_MAGIC_HEAD))
			{
				
			//	printf("found head:%d\n",(*phead).magic);
				if((int)(mOffset-i-3)>=(int)(sizeof(DATA_HEAD)))
				{
					
					*dhead = *(DATA_HEAD*)(mData+i+3);
					(*dhead).length = swapInt16((*dhead).length);
					
					//printf("DATA_HEAD====%d,%d\n",(mOffset-i-3-sizeof(DATA_HEAD)),(*dhead).length+(*dhead).optLength+2);
				
					if((int)(mOffset-i-3-sizeof(DATA_HEAD))>=(int)((*dhead).length+(*dhead).optLength+tmp))	
					{
					
						if((*dhead).optLength>0)
						{
							
							 memcpy(optbuff,mData+i+3+sizeof(DATA_HEAD),(*dhead).optLength);
						}
						if((*dhead).length>0)
						{
							 *seq =*(short*)(mData+i+3+sizeof(DATA_HEAD)+(*dhead).optLength);
							 *seq = swapInt16(*seq);
							 memcpy(buff , mData+i+3+sizeof(DATA_HEAD)+(*dhead).optLength,(*dhead).length);
							
							// for(int j =i;j<(i+3+sizeof(DATA_HEAD)+(*dhead).optLength+(*dhead).length);j++)
							 //	printf("%02x ",mData[j]&0xff);
							// printf("\n");
						}
						tmplen = ((*dhead).length+(*dhead).optLength+tmp+i+3+sizeof(DATA_HEAD));
						mOffset -=tmplen;
						
						memcpy(tmp_buf,mData+tmplen,mOffset);
						memcpy(mData,tmp_buf,mOffset);
//						for( j =0;j<mOffset;j++)
//							 	printf("%02x ",mData[j]&0xff);
//							  printf("\n");
						//	printf("phead%x    %x    dhead %x   %x  %x ",(phead)->magic,phead->headp,dhead->optLength,dhead->checksum,dhead->length);
            return 0;
					}
					else
					{
					
						return -1;
					}
				}
				else
				{
			
					return -1;
				}
					
			}
			
		}
	}
	return -1;
}


void RecvHandle()
{
	int rlen;
	int size = 1022;



		if(rlen>0)
		{
			
		//	push(MF_recv_buf+ret_pos+5,rlen);
		
			
		}
		
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

}

void ResponseHandle()
{

		
		static int ret;
		static char buffer[1024];
		static PACKAGE_HEAD phead;
		static DATA_HEAD dhead;
		static char  optbuff[1024];
		static short seq;

		ret = popup(&phead,&dhead,optbuff,buffer,&seq);
		if(ret==0)
		{
			//printf("=====pop data\n");
			if((phead.headp&0x1f)==11)
		      printf("regist or connnect ack len = %d,%d\n",dhead.length,dhead.optLength);
			  CBHandle(phead,dhead,optbuff,buffer,seq);//?????
		}
		else
		{
			//sleep(1);
			
		}
}

int SendIpAckData(short errcode,unsigned char* buff,int len) //type =0--route,1--ip dev
{
	int datalen = len;
	int dlen;
	static char miwen_hex[1024];
	int ret;
	static	char  decdata[1024];
	static  char  checkkdata[1024];
	u8 checks;
	char optdata[2];
  short dlen2 ;

	
	if(!mLogin)
		return -1;
	if(len>1022)
		return -1;
	
	errcode = swapInt16(errcode);
	memcpy(optdata,&errcode,2);
	
	if(len>0)
	{
		memset(decdata,0,1024);
		
		memcpy(&decdata[2],buff,len);
		

						
		ret = aes_encrypt((char*)gProtoolKey,decdata,miwen_hex,datalen+2,&dlen);
		if(ret !=0)
			return ret;
		
		dlen2 = dlen;
		dlen2 = swapInt16(dlen2);
	
		
		memset(checkkdata,0,1024);
		memcpy(checkkdata,&dlen2,2);
		memcpy(checkkdata+2,optdata,2);
		memcpy(checkkdata+2+2,decdata,datalen+2);
		
		checks = GetCheckSum((unsigned char*)checkkdata,2+2+datalen+2);
		
		return SendRouteUpData((unsigned char*)optdata,
				(unsigned char*)miwen_hex, dev_bissness_pack_ip_ack, ACK_NO,2,
				dlen,checks);
	}
	else
	{
		dlen =0;
		dlen2 = dlen;
		dlen2 = swapInt16(dlen2);
		memset(checkkdata,0,1024);
		memcpy(checkkdata,&dlen2,2);
		memcpy(checkkdata+2,optdata,2);
		
		
		checks = GetCheckSum((unsigned char*)checkkdata,2+2);
		return SendRouteUpData((unsigned char*)optdata,
				(unsigned char*)0, dev_bissness_pack_ip_ack, ACK_NO,2,
				0,checks);
	}
}

int CBHandle(PACKAGE_HEAD phead,DATA_HEAD dhead,char* optbuff,char *buff,short seq)
{
	static char out[1024];
	int outlen;
  time_t curtime;
	static char  checkkdata[1024];
	U8 checks;
	int ret;
	data_longin_response* longin_res;
	data_regist_response* regist_res;

	short errerno ;
	printf(">>>>>>CBHandle seq %d\n",seq);
	if(seq ==-1)//force logout
	{
		printf("net error reconnect\n");
		mQuitProtool = 1;
		return 0;
	}
	
	switch ((phead.headp&0x1f))
	{
		case force_logout:
			 printf("force disconnect exit !!!!!\n");
			// exit( -1);
			 break;
		case heart_ack:
			 printf("heart beep ack\n");
		
			 MF_Get_time(&curtime);
			 mHeartTime = curtime;
			 break;
		case dev_bissness_pack_ip_ack:
		{
			printf("dev_bissness_pack_ip_ack %d\n",dhead.length);
			if((dhead.length>0))
			{
				 ret = aes_decrypt((char*)gProtoolKey,buff, out,dhead.length,&outlen);
				if(ret !=0)
				{
					printf("descrypt error\n");
					break;
				}
				
			/*	for(int i =2;i<outlen;i++)
				{
					printf("%c ",out[i]);
				}
				printf("\n");*/
				
			}
			  
			 break;
		}
		case dev_bissness_pack_ip_down:
		{
			printf("dev_bissness_pack_ip_down\n");
			
			if(dhead.length>0)
			{
				ret = aes_decrypt((char*)gProtoolKey,buff, out,dhead.length,&outlen);
				if(ret !=0)
				{
					printf("descrypt error\n");
					break;
				}
				printf("len %d,outlen = %d,%d\n",dhead.length,outlen,out[outlen-1]);
				if(encType==0)
					outlen -=out[outlen-1];
				
				/*for(int i =2;i<outlen;i++)
				{
					printf("%c ",out[i]);
				}
				printf("\n");*/
	
				memset(checkkdata,0,1024);
			//	memcpy(checkkdata,&dhead.length,2); ���������⣿
				checkkdata[0] = dhead.length<<8;
				checkkdata[1] = dhead.length;
				if(dhead.optLength)
					memcpy(checkkdata+2,optbuff,dhead.optLength);
				
				if(outlen>0)
					memcpy(checkkdata+2+dhead.optLength,out,outlen);
				
				checks = GetCheckSum((unsigned char*)checkkdata,2+dhead.optLength+outlen);
				printf("checks = %x, %x\n",checks,dhead.checksum);
				
				if(checks != dhead.checksum)
				{
					SendIpAckData(checksum_err,0,0);
				}
				else
					;
					SendIpAckData(0,(unsigned char*)"ack",3);;
			}
			
			break;
		}
		
		
		case ack:
		{
			if(!mRegist)
			{
				if(dhead.optLength>0)
				{
					errerno = *(short*)optbuff;
					
					printf("optlen %d,code or radom %d\n",dhead.optLength,swapInt16(errerno));
				}
				printf("regist ack ===len %d\n",dhead.length);
				if(dhead.length >0)
				{

					 ret = aes_decrypt((char*)gProtoolKey,buff, out,dhead.length,&outlen);
					if(ret !=0)
					{
						printf("descrypt error\n");
						break;
					}
					regist_res = (data_regist_response*)(out+2);
					memcpy(mLoginPluginId,regist_res->deviceId,32);
					printf("regist ack======ip:%d,%d,%d,%d,port:%d,pluginId %s\n",
						regist_res->ip[0],regist_res->ip[1],regist_res->ip[2],regist_res->ip[3],regist_res->port,mLoginPluginId);
					
					sprintf(mLoginServer,"%d.%d.%d.%d",regist_res->ip[0],regist_res->ip[1],regist_res->ip[2],regist_res->ip[3]);
					
					mLoginPort = swapInt16(regist_res->port);
					mRegist = 1;

					memcpy(mLoginTocken,regist_res->token,32);
					printf("regist ok tocken ,port %d\n",mLoginPort);
				}
				else
				{
					printf("regist ack error\n");
					
				}
			}
			else
			{
				printf("login ack\n");
				if(dhead.optLength>0)
				{
					printf("optlen %d,code or radom %d\n",dhead.optLength,*(short*)optbuff);
				
				}
				if(dhead.length >0)
				{
					int outlen;
					
					int ret = aes_decrypt((char*)gProtoolKey,buff, out,dhead.length,&outlen);
					if(ret !=0)
					{
						printf("descrypt error\n");
						break;
					}
						
					longin_res = (data_longin_response*)(out+2);
					mHeartInterval = swapInt16(longin_res->heartbeat);
					if(mHeartInterval<5)
						mHeartInterval = 5;
					printf("login ack heartbeep interval %d\n",swapInt16(longin_res->heartbeat));
					mLogin = 1;
					
				}
				else
				{
					printf("log ack error\n");

					//判断错误码，重新启动注册
					mQuitProtool = 1;
					
					
					
				}
			}
			break;
		}
		
		default:
			break;
	}
	
	
	return 0;
	
}

int RecErrCB(int errorid)
{
	//Protool* pProtool = (Protool*)pOwner;
	printf("--RecErrCB %d\n",errorid);
	switch (errorid)
		{
		case errid_regist:
		case errid_login:
		case errid_heart:
		case errid_forceout:
			{
				DestroyPlugSdk();
				printf("error-----\n");
				return -1;
				
			}
			break;
		default:
			break;
		}
	return 0;
}
/**************��Ҫ����**************/
int NewTcpServer(const char *host,int port)
{
	u8 ret;
	char string_port[10];
	ret = strsearch_h(",",light_TCP_test);
	sprintf(string_port,"%d",port);
	memcpy(light_TCP_test+16,string_port,4);
//	printf(light_TCP_test);
	// light_TCP_test[22]={"171.208.222.113,6037"};
   //   cdma_reconnect();
}
void MF_Heartbeep(void)
{
	static u16 HeartIntervalCount;
	static u8 testCount;
	static char senddata[256];
	time_t curtime = 0;

	if(!mQuitProtool)
	{
		//sleep(mHeartInterval);
		if( HeartIntervalCount > mHeartInterval )
		{
			printf("hearting \"{\"switch\":\"0\"}\"---->%d\n",mHeartInterval);
			HeartIntervalCount = 0;
			Heartbeep();
	
		}
		/////������----
		else if(testCount >20)
		{
			testCount = 0;
	    MF_Get_time(&curtime);
	    printf(senddata);
			sprintf(senddata,"{\"KEY\":0}",(int)curtime);
			
			SendIpUpData((unsigned char*)senddata, strlen(senddata),0);
		//	sleep(10);
			
		//	sprintf(senddata,"{'devoffline':'60-01-94-87-12-CE','time':'%d'}",(int)curtime);
		//	SendIpUpData((unsigned char*)senddata, strlen(senddata),1);
		}
	
	}
	else
	{
		
	}
	testCount ++;
	HeartIntervalCount++;
}
static int Heartbeep()
{
	data_heartbeat_opt optdata = {0};
	u8 checks;
	int ret;
	time_t curtime = 0;
	MF_Get_time(&curtime);
	//memset(&optdata,0,sizeof(data_heartbeat_opt));
	optdata.timestamp = swapInt32(curtime);
	checks = GetCheckSum((unsigned char*)&optdata,sizeof(optdata));
	ret = SendRouteUpData((unsigned char*)&optdata,0,heart,1,sizeof(optdata),
		0,checks);

	return ret;
	
}
static u32 TimeCount;
void MF_Time_count()
{
	  TimeCount++;
}
u32 MF_Get_time(time_t* time)
{
	 *time = TimeCount;
}
void MF_Time_CheckHandle()
{
	  long updateTime ;
		time_t curtime;
		MF_Get_time(&curtime);
    updateTime = curtime;
		
		if((mHeartTime!=0)&&(updateTime-mHeartTime)>60)
		{
			mQuitProtool = 1;
			printf("heart beep error\n");
		}
}
