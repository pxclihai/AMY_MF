#ifndef __Protool_h__
#define __Protool_h__
#include "TcpServer.h"
//#include "Common.h"
#include "config.h"
#define ACK_YES 1
#define ACK_NO 1
//171.208.222.109:6039
//#define LOGIN_SERVER "171.208.222.109"
//#define LOGIN_PORT  6040

#define REGIST_SERVER "171.208.222.113"
#define REGIST_PORT  6037
//----------------------------------------注册

#define CONFIG_MAC_FILE "/tmp/macconfig"

typedef struct{ 
    U8 loginType;             //保留
    U16 reserved;             //保留
    //U8  len;/
    U8 devicePin[];            //设备ID( SN)
}__attribute__((packed)) data_regist_opt;


typedef struct{
    U8 devicePin[12];             //设备的PIN码
    U8 deviceSnLength;	      //表示设备 SN 长度
	//U8 deviceSN[];	          //不定长数据，表示设备ID( SN) 
	U8* deviceSN;			  //不定长数据，表示设备ID( SN) 

	U8 deviceMac[6];          //设备的MAC地址
	U8 deviceId[32];          //32位设备ID
}__attribute__((packed)) data_regist;


typedef struct {
     U16 random;	   //16 位定长随机数
} __attribute__((packed)) data_regist_response_opt;

typedef struct{
       U16 code;	//错误代码，具体参考附录
}__attribute__((packed)) data_regist_response_err_opt;



typedef struct{
   // U16     random;            //16 位定长随机数
    U8      token[32];           //32 字节定长登录 session
                                
    U8      ip[4];               //4 字节定长 ip 地址
    U16     port;                //端口号
    U8      deviceId [32];       //32位字符插件ID 
} __attribute__((packed)) data_regist_response;	


//----------------------------------------登陆


typedef struct{
	U8 devicePin[12];			  //设备的PIN码

    U8 token[32];	//32 个字节的定长 token 信息
}__attribute__((packed)) data_login_opt;

typedef struct{ 
    U8 protocol;  //插件支持的数据协议
	U8 deviceVersion[4];			//4 字节定长设备版本号
}__attribute__((packed)) data_login;


typedef struct{
    U16 code;	//错误代码，具体参考附录
}__attribute__((packed)) data_login_response_opt_err;

typedef struct{
    U16 random;	  //16 位定长随机数
}__attribute__((packed)) data_login_response_opt;


typedef struct{ 
   // U16 random;       
    U16 heartbeat;    //服务器要求的心跳间隔时间
    U8  timezone;    //当前所在的时区
    U32 timestamp;    //基于 0 时区的时间秒数值
}__attribute__((packed)) data_longin_response;


//----------------------------------------心跳

typedef struct{
    U32 timestamp;	//基于 0 时区的时间秒数值
}__attribute__((packed)) data_heartbeat_opt;


typedef struct{
    U32 timestamp;	//基于 0 时区的时间秒数值
}__attribute__((packed)) data_heartbeat_respon_opt;

//---------------------------------------插件业务包(上下行)
//仅用于IP设备和平台的数据交互19,20
typedef struct{
    U16 code;	//错误代码，具体参考附录
}__attribute__((packed)) data_plug_respont_opt;

#if 0
typedef  struct{
	U16		dataTyp;			//业务包数据类型
	//U16		snLength;			//sn的长度
	//U8		*sn;				//设备或者路由的sn或者mac
	//U16		functionId;			//操作
	U16		length;             //业务数据长度
	U8		*dataByte;			//业务数据
}__attribute__((packed)) data_plug;

typedef  struct{
	U8		result;				//0 OK   非0 失败
	U16     length;             // 返回的数据长度
    U8*     dataByte;          // 返回的数据
}__attribute__((packed)) data_plug_response;  //26
#endif


void DealMfPro(USART_FUNCTION uf,uchar *DaPtr,uint16 len);


extern int MF_RegistServer(void);
extern u8 MF_recv_buf[1024];
void MF_net_rec(u8 data); // 放入中断接受

extern void MF_Regist(void);
extern int MF_Login_Connect(void);
extern void MF_RECV(void);
extern int MF_LoginServer(void);
extern void MF_Heartbeep(void);
extern void MF_Time_count(void);
extern void MF_Time_CheckHandle(void);
//	typedef int (*ErrorCallback)(int errorid);
//	int RegistErrCB(ErrorCallback cb);
//	int RecErrCB(int errorid);
//	void NewProtool();
//	void DeleteProtool();
//	int  GetLoginStatus();
//	int  InitProtool();
//	int  DestroyProtool();
//	int  RegistServer();
//	int  Heartbeep();
//	int  LoginServer();
//	
//	int  RecCB(void* pOwner,PACKAGE_HEAD phead,DATA_HEAD dhead,char* optbuff,char *buff,short seq);
//	int  CBHandle(PACKAGE_HEAD phead,DATA_HEAD dhead,char* optbuff,char *buff,short seq);

	int SendIpUpData(unsigned char* data,int len,int type);
//	int SendRouteUpData(unsigned char* optdata,unsigned char* data,PACKAGE_TYPE type,int isack,int optlen,int len,U8 checksum);
//	
//	int SendIpAckData(short errcode,unsigned char* buff,int len);
//	//TcpServer*	   mTcpConnectServer;
//	
//	int 		   aes_decrypt(char* key,char* in,char* out,int inlen,int* outlen);
//	int 		   aes_encrypt(char* enckey,char* encbuf,char* decbuf,int inlen,int* outlen);
//	unsigned char  GetCheckSum(unsigned char* pack,int pack_len);
////	unsigned char  getCheckSum(unsigned char* pack,int pack_len);
//	void*    	   RegistThread( void * arg );
//	void           RegistHandle();
//	void*   	   CheckThread( void * arg );
//	void           CheckHandle();

//};
#endif
