#ifndef __Common_h__
#define __Common_h__
#include <time.h>
#define BIG_END 1
#define LITTLE_END 0

#define encType 2
#define DX_MAGIC_HEAD 0xddcc
//#define AES_ENCKEY "12345678"
#define CONFIG_FIE "/tmp/plug.conf"
#define false 0
#define true 1
typedef unsigned short  U16;
typedef unsigned char   U8;
typedef unsigned int    U32;
typedef unsigned long long    U64;
typedef enum
{
	
	checksum_err =11,
	token_invalid=12,//token 失效，重新注册
	device_sn_invalid=13,//IP，路由器设备不合法
	device_id_err=14,//未分配到设备直连分配的id
	token_get_err=15, //未分配到token
	plug_id_err=16,   //未分配到plug分配的id
	no_access_gateway=17,//无可用的接入网关，注册时返回

}ERROR_NO;

typedef enum
{
	
	errid_regist =101,
	errid_login=102,//token 失效，重新注册
	errid_heart=103,
	errid_forceout=104

}ERROR_ID;


typedef enum
{
	reserved=0,
	heart =1,
	heart_ack=2,
	force_logout=3,
	subscribe=4,
	dissub=5,
	dev_found=6,
	dev_apply=7,
	dev_auth=8,
	login_conform=9,
	connection_conform=10,
	ack=11,
	dev_divorce=12,
	notiry_dev_update=13,
	fireware_update=14,
	fireware_content=15,
	notify_dev_disconnet=16,
	dev_bissness_pack_up=17,
	dev_bissness_pack_down =18,
	dev_bissness_pack_ip_up=19,
	dev_bissness_pack_ip_down=20,
	user_logout=21,
	dev_bissness_pack_ack=25,
	dev_bissness_pack_ip_ack=26,
}PACKAGE_TYPE;
typedef struct
{
   U16 magic;   //插件和后台软件数据包交互标识符
   U8  headp;
  // U8 type;//:5;    //低5位标识数据包类型
  // U8 ack;// :1 ;    //第6位标识数据包是否需要回复
  // U8 reserved;//:2; //高两位保留
}__attribute__((packed)) PACKAGE_HEAD;

typedef struct
{
   U8   optLength; //可选包长度
   U8   checksum;//checksum后的所有字节总和
   U16  length;  //包数据的长度
}__attribute__((packed))  DATA_HEAD;


typedef struct
{
	char bussinessIp[16];
	int  bussinessPort;
	char tocken[16];
}__attribute__((packed)) LOGIN_SERVER_TOKEN;

//int GetEndType();


//U32 swapInt32(U32 value);

////U64 swapInt64(U64 value);

//U16 swapInt16(U16 value);

//void NewProgramProfile(const char *filePath);

//void DeleteProgramProfile();


//int getKeyValueStr(const char *app, const char *key, 
//			const char *default_str, char *ret_str, unsigned int ret_str_size);


#endif
