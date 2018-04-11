#ifndef _API_H
#define _API_H

//挂载的客户端数量
#define MOUNT_DEVICE_COUNT	2

/******************************************************************************/
/*串口功能选择*/
typedef enum
{
	USART_FUNCTION_WCDMA,
	//USART_FUNCTION_MSG,
	USART_FUNCTION_MOUNT,
	USART_FUNCTION_MSG = USART_FUNCTION_MOUNT + MOUNT_DEVICE_COUNT,
	USART_FUNCTION_DEBUG,
	USART_FUNCTION_MAX		
}USART_FUNCTION;

/******************************************************************************/
typedef enum {
	PROTOCOL_TYPE_POSTS,				//邮电部协议
	PROTOCOL_TYPE_COMPANY,				//公司协议
	PROTOCOL_TYPE_WIRELESS,				//无线传输协议
	PROTOCOL_TYPE_DOWNLOAD,				//程序升级协议
	PROTOCOL_TYPE_MAX,
	PROTOCOL_TYPE_CHECKING,				//有符合需要的协议，但是还是需要继续判断
	PROTOCOL_TYPE_MF,
	PROTOCOL_TYPE_NONE = 0XFF			//没有协议
}PROTOCOL_TYPE;

/*******************************************************************************/
/*复位收发缓存选择*/
typedef enum {
	RESETRTBUFF_TYPE_ALL,
	RESETRTBUFF_TYPE_RX,
	RESETRTBUFF_TYPE_TX,
}RESETRTBUFFTYPE;

/*******************************************************************************/
/*唯一定时器类型*/
typedef enum {
	TIMER_TYPE_Beep = USART_FUNCTION_MAX,    /*蜂鸣器定时器*/
	TIMER_TYPE_UART_DMA,					 /*串口DMA数据处理*/
	TIMER_TYPE_SETCOMMBPS,					 /*设置串口参数延时*/
	TIMER_TYPE_HORSE_LED,								 /*跑马灯回调函数*/
	TIMER_TYPE_SEND_DELAY,								 /*数据发送等待OK延时回调函数*/
	TIMER_TYPE_IAP_UPDATE_END,				 /*IAP程序下载完成后延时重启*/
	TIMER_TYPE_IAP_REV_TIMEOUT,				 /*IAP程序升级超时*/
	TIMER_TYPE_IAP_SELECT_PROGRAM,			 /*选择进入用户或引导程序*/
	TIMER_TYPE_QWL_NET_DEAL, /*联网专用延时函数*/
	TIMER_TYPE_QWL_TOUBIJI_DEAL, /*投币机延时函数延时函数*/
	TIMER_TYPE_ARMCHAIR_TIME, /*按摩椅专用延时函数*/
	TIMER_TYPE_MF_RESONSE,    /*魔方*/
	TIMER_TYPE_MF_HEARTBEEP,/*魔方*/
	TIMER_TYPE_MAX 
}TIMER_TYPE;

/*******************************************************************************/

typedef __packed struct     //客户端模式信息体
{	vu16 net_type[4];		//模式
	vu32 rip[4];   		    //远端主机ip地址
	vu16 rport[4]; 		    //远端主机端口
	vu32 reserve[4];        //预留
}client_struct; 
/*******************************************************************************/
typedef __packed struct 
{	vu16   Baud;            // 9600
	vu16   Parity;          //串口奇偶校验位
	vu16   WordLength;      //串口数据位
	vu16   StopBits;        //串口停止位
}ComConfigStruct; 
/*******************************************************************************/


/******************************************************************************/
typedef void (*TimerEvent)(void);
typedef int (*ReceiveFun)(USART_FUNCTION uf,PROTOCOL_TYPE Pro_Type,uchar* buffer, size_t size);
/*测试发送字符串*/
int API_qwl_sendstring(u8 *p);	
/*测试发送字节*/
int API_qwl_sendchar(u8 ch);	


/*蜂鸣器初始化*/
int API_Led_Init(void);
/*FLASH初始化*/
int API_SPI_Flash_Init(void);
/*LED初始化*/
void API_horse_race_lamp(void);
/*API函数预定义开始*/								
/******************************************************************************/
/*看门狗初始化*/
int API_WatchDog_Init(void);
/*喂狗*/											
int API_WatchDog_Feed(void);
/*看门狗延时*/								
int API_WatchDog_ClockPro(void);							
/******************************************************************************/
/*协议初始化*/
int API_Protocols_Init(void);
/*主循环体调用的执行所有协议，用于分析接收过来的数据*/
int API_DoProtocols(void);
/*直接发送数据*/
int API_SendProtocolBuffer(USART_FUNCTION uf,uchar* buffer, size_t size);
/*复位收发缓存*/
int API_ResetProtocolBuffer(USART_FUNCTION uf,RESETRTBUFFTYPE ResetType);
/*向指定的协议类型缓存添加发送数据*/
int API_AddProtocolBuffer(USART_FUNCTION uf,uchar* buffer, size_t size);
/*获取指定的协议发送缓存指针和长度*/
int API_GetProtocolBufferOrLen(USART_FUNCTION uf,uchar **ptr,uint16 **len);
/*指定的协议类型启动发送数据*/
int API_StartSendBuffer(USART_FUNCTION uf);
/*设置接收回调函数*/
int API_SetProtocolReceiveCallBack(USART_FUNCTION uf,ReceiveFun fun);

/*设置串口参数*/
int API_SetComParameter(
	USART_FUNCTION uf,					//串口协议
	uint32_t baudRate,	 				//波特率
	uint16_t parity,					//校验位
	uint16_t wordLength,				//数据长度
	uint16_t stopBits					//停止位
	); 
/******************************************************************************/
/*定时器初始化*/
int API_Timers_Init(void);
/*创建定时器*/
int API_SetTimer(int time,int circleCount,TIMER_TYPE timerType,TimerEvent tEvent,TimerEvent cEvent);
/*删除定时器*/
int API_KillTimer(TIMER_TYPE timerType);
/*启动定时器*/
int API_StartRunTimer(void);
/******************************************************************************/
/*地址跳转及参数保存*/
void API_goto_iap(void);

/*获取保存的升级参数*/
int API_Get_Iap_Config(void);
/*IAP程序更新写*/
int API_IAP_Update_Write(char* buf,size_t size);
/*IAP程序更新开始*/
DWORD API_IAP_Update_Start(BOOL force,char* buf,size_t size);
/*IAP程序更新结束*/
int API_IAP_Update_End(void);
/*IAP程序拷贝到用户区*/
int API_IAP_copyflash(u32 src_add,u32 dest_add,u32 copy_long);
/*IAP程序拷贝到用户区从外部flash*/

#if CONF_SPI_FLASH_EN
int API_IAP_copyflash1(u32 src_add,u32 dest_add,u32 copy_long);
#endif

/*跳转到用户程序*/
int API_Jump_To_UserProgram(void);
/*选择进入用户或引导程序*/
void API_Select_Program_run(void);
/*检查进入用户或引导程序标志*/
void API_Check_Run_User_Program_Falg(void);
/******************************************************************************/

//************************************************************
/*查询联网状态*/
int API_qwl_net_init(void);
int API_net_state(void);
int API_net_rec(u8 ch);	
int API_net_data_send(void); 
int API_net_data_send_init(void);
/*联网模块初始化*/
void API_nblot_init_0(void);

//**********************************
/*按摩椅相关*/
void API_Armchair_IO_Init(void);
void API_Armchair_Init(void);
void API_ARMCHAIR_TIME(void);
void API_ARMCHAIR_DEAL(void);
#endif

