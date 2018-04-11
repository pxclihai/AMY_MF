
#include "PlugSdk.h"

#include "Protool.h"

#include "MF_Common.h"
#include "config.h"

#include "PlugSdk.h"



int mofanSDK()
{
	  #define CONNECT_INIT           0 
	  #define REGISTER               1
	  #define REGISTER_WAIT_ACK      2
	  #define LOGIN_CONNECT          3
	  #define LOGIN_CONNECT_WAIT_ACK 4
	  #define LOGIN_SERVER            5
	  #define LOGIN_SERVER_WAIT_ACK  6
	  #define HEARTBEEP           7
	  #define WAIT_NONE              20
	  static u8 connect_step; 
  	int ret;
    switch (connect_step)
	  {
		  case CONNECT_INIT:
			   if(0!=InitPlugSdk())
			   {
					qwl_sendstring("gNetDump init error\n");
					DestroyPlugSdk();
					return -1;
			   }
		       connect_step = REGISTER ;
			  break;
		  case REGISTER :
				   MF_Regist();
			       connect_step = REGISTER_WAIT_ACK ;
			   break;
		  case REGISTER_WAIT_ACK:
			  if((!mRegist)&&(!mQuitProtool))
			  {
				  
			  }
			  else
			  {
				  connect_step = LOGIN_CONNECT ;
			  }
			  break;
		  case LOGIN_CONNECT:
			 
						MF_Login_Connect(); 
						net.reconnect_setp = LINK_IP;//粗糙的改变状态。

						connect_step =LOGIN_CONNECT_WAIT_ACK;
			  break;
		  case LOGIN_CONNECT_WAIT_ACK:
			  if(net.reconnect_setp == LINK_OK)
			  {
				 connect_step = LOGIN_SERVER ;
			  }
			  break;
			  
		  case LOGIN_SERVER :
			     MF_LoginServer();
		       connect_step = LOGIN_SERVER_WAIT_ACK;
			 break;
		 case LOGIN_SERVER_WAIT_ACK :
			    if(mLogin == 1)
					{
						connect_step = HEARTBEEP;
					}
			 break;
			case HEARTBEEP:
			  	API_SetTimer(1000,-1,TIMER_TYPE_MF_HEARTBEEP,MF_Heartbeep,NULL);
		    	connect_step = WAIT_NONE;
				break;
			
	  }

	
	return 0;
	
}
int InitPlugSdk(void)
{
	  int ret;
	  NewProtool();
  	  ret = InitProtool();
	
	  API_SetTimer(1000,-1,TIMER_TYPE_MF_HEARTBEEP,MF_Time_count,NULL);
  	API_SetTimer(1000,-1,TIMER_TYPE_MF_HEARTBEEP,MF_Time_CheckHandle,NULL);
	
	  return ret;
}
	
int DestroyPlugSdk(void)
{
	//if(mProtool)
	{
	//	DestroyProtool();
	//	DeleteProtool();
	//	delete mProtool;
	}
	//mProtool =0;

	return 0;
}
u16 MF_get_dev_mac(U8 mac[6]){
    //获取MAC的功能厂商自己实现，本函数仅供参考
	char device[16]="eth0"; //teh0是网卡设备名  
//	unsigned char macaddr[ETH_ALEN]; //ETH_ALEN（6）是MAC地址长度  
//	//AF_INET = 1;  
//	int i,s;  
//	s = socket(AF_INET,SOCK_DGRAM,0); //建立套接口  
//	struct ifreq req;  
//	int err;
//    char * ret;  
//	
//	ret = strcpy(req.ifr_name,device); //将设备名作为输入参数传入  
//	err=ioctl(s,SIOCGIFHWADDR,&req); //执行取MAC地址操作  
//	close(s);  
//	if( err != -1 )  
//	{  
//		memcpy(macaddr,req.ifr_hwaddr.sa_data,ETH_ALEN); //取输出的MAC地址  
//		printf("########MAC= \n");
//		for(i=0;i<ETH_ALEN;i++)  
//		printf("%02x:",macaddr[i]);
//		memcpy(mac, macaddr, ETH_ALEN);
//	}  
//	printf("\r\n"); 
	return 0;
}
u16 MF_get_dev_sn(char * sn){
    //获取SN号的功能厂商自己实现，本函数仅供参考
		sn[0] = '1';
		sn[1] = '2';
		sn[2] = '3';
		if ((sn == NULL) || (strlen (sn) == 0)){
				return -1;
		}
		return 0;
};





