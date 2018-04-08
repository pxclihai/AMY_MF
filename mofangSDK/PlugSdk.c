//#include <sys/types.h>
//#include <sys/socket.h>
//#include <sys/ioctl.h>
//#include <net/if.h>
//#include <string.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <linux/if_packet.h>
//#include <netinet/if_ether.h>
//#include <netinet/in.h>
#include "PlugSdk.h"
//#include <pthread.h>
//#include <unistd.h>

int InitPlugSdk()
{
	  int ret;
	  NewProtool();
  	ret = InitProtool();
	  return ret;
}
	
int DestroyPlugSdk()
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

//U16 get_dev_mac(U8 mac[6]){
//    //获取MAC的功能厂商自己实现，本函数仅供参考
//	char device[16]="eth0"; //teh0是网卡设备名  
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
//	return 0;
//}
//U16 get_dev_sn(char * sn){
//    //获取SN号的功能厂商自己实现，本函数仅供参考
//    sn[0] = '1';
//	sn[1] = '2';
//	sn[2] = '3';
//    if ((sn == NULL) || (strlen (sn) == 0)){
//        return -1;
//    }
//    return 0;
//};




