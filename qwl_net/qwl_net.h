#ifndef _qwl_net_H
#define _qwl_net_H
#include "stm32f10x.h"
#include "config.h"

typedef enum {
	PLATFORM_CHOICE_PUBLIC,//公用
	PLATFORM_CHOICE_UPDATE	//升级 		
}PLATFORM_CHOICE;

typedef __packed struct
{
	u8 UPDATE_overtime_readnb;//超时读NB时间标志，每三十秒读一次
	u16 UPDATE_link_overtime;//超时时间 //没有收到下载指令
	u16 MAX_UPDATE_link_overtime;//最大超时时间 //没有收到下载指令
	u8 UPDATE_link_times;//连接次数
	u8 MAX_UPDATE_link_times;//最大连接次数，超过后切换回正常平台
	PLATFORM_CHOICE Platform_Choice;// 平台选择，升级或者正常使用平台	
}EXCHANGE_PLATFORM;

extern EXCHANGE_PLATFORM exchange_platform;

typedef enum {

	LINK_OK,			//连接OK
	LINK_IP,			//连接IP
	OPEN_GPRS,		//打开网络
	MODE_INIT,	//模块初始化
	MODE_CHOICE,//模块选择
	SETP_TYPE_MAX 		
}SETP_TYPE;

typedef __packed struct
{
			
			u8 delay_times; //超时次数
			u8 delay;			//步骤中的超时机制
			u8 setp_setp;	//每个重连步骤中的步骤
			SETP_TYPE reconnect_setp;//重连步骤
			MODE_TYPE mode_type;
}CONNECT_TYPE;

extern CONNECT_TYPE net;

u8 net_state(void);
void net_rec(u8 k);
u8 net_send(u8 *p,u16 len);
void qwl_net_init(void);
void cdma_or_nblot(void);
void qwl_net_sendstring1(u8 *p);
void qwl_net_sendstring(u8 *p);
void qwl_net_sendchar(u8 ch);



extern u8 cdma_or_nblot_flag;
extern u8 connect_flag;

extern u16 Rx;
#define  RxIn 1024
extern u8 SystemBuf[];  //储存出口接收数据

const extern	u8 ASCII_hex[24];
extern u8 net_error_flag;//网络错误标志
extern u8 deal_process;
extern u8 update_ip[22];

extern u8  getwayheart_sendok_flag;//心跳有否发送成功，确认标志
extern u8  getwaylist_sendok_flag;//列表有否发送成功，确认标志
extern u8  getwayoff_sendok_flag;//列表有否发送成功，确认标志
extern u8 suipian_delay;
#endif 
