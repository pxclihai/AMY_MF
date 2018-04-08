#ifndef PROTOCOL_H
#define  PROTOCOL_H
#include "stm32f10x.h"
//#define MAX_NET_IS_OPEN_NUM		200L
/*********************************************************************************************/
typedef enum {
	 RESOURCE_USE_STATUS_FREE, 			//空闲
	 RESOURCE_USE_STATUS_RECEIVE,		//接收
	 RESOURCE_USE_STATUS_SEND,			//发送
	 RESOURCE_USE_STATUS_MUTSEND,		//多次发送
	 RESOURCE_USE_STATUS_END,			//发送接收完毕
	 RESOURCE_USE_STATUS_MAX		    //个数
}RESOURCE_USE_STATUS;

/*********************************************************************************************/
typedef enum {
	PORT_TYPE_SERIAL,					//串口
	PORT_TYPE_WCDMA,					//网络模式
	PORT_TYPE_MSG,					//网络模式
	PORT_TYPE_MAX 		
}PORT_TYPE;
typedef enum {
	MODE_TYPE_NONE,					//无模块
	MODE_TYPE_ZTE,					//NB模块
	MODE_TYPE_BC95,					//中信模块，因为我的模块移动联通电信兼容，所以以模块为识别
	MODE_TYPE_MAX 		
}MODE_TYPE;

typedef enum {
	CONNECT_STATE_NONE,					//无连接
	CONNECT_STATE_OK,					//连接OK
	CONNECT_STATE_LINK,					//连接中
	CONNECT_STATE_MAX 		
}CONNECT_STATE;
/*********************************************************************************************/
typedef enum {
	PROTOCOL_RX_MODE_ANALYSIS,		   //协议解析
//	PROTOCOL_RX_MODE_DIRECT, 		   //透传		
}PROTOCOL_RX_MODE;
/*********************************************************************************************/
//配置对象是一类协议而不是一个协议
#define MAX_HEAD_BYTES 4  				//协议头字符串长度
#define MAX_TAIL_BYTES 4  				//协议尾字符串长度

#define SIGN_PROTOCAL_WITH_NO_HEAD	0xFF	  	//无头标志
#define SIGN_PROTOCAL_WITH_NO_TAIL	0xFF	 	//无尾标志
/*********************************************************************************************/
typedef __packed struct
{	u8 RHLenth;						//头长度
	u8 RHead[MAX_HEAD_BYTES];		//头字符串
	u8 RSLenth;						//协议最短长度--->主要为了防止在协议头中出现与头字符串或尾字符串相似的值而出错
	u8 RTLenth;						//尾长度
	u8 RTail[MAX_TAIL_BYTES];		//尾字符串
	PROTOCOL_TYPE ProtcolID;			//该协议对应的协议类型,如：ID==0公司协议,==1是邮电部协议，==其它
}PINFO;					    			//对PC机的协议结构
/*********************************************************************************************/
typedef __packed struct
{	u8 RHLenth;						//头长度
	u8 RHead[MAX_HEAD_BYTES+4];		//头字符串
	u8 RSLenth;						//协议最短长度--->主要为了防止在协议头中出现与头字符串或尾字符串相似的值而出错
	u8 RTLenth;						//尾长度
	u8 RTail[MAX_TAIL_BYTES+4];		//尾字符串
	PROTOCOL_TYPE ProtcolID;			//该协议对应的协议类型,如：ID==0公司协议,==1是邮电部协议，==其它
}PINFO_BC95;					    			//对PC机的协议结构
/*********************************************************************************************/
typedef int (*ProtocolFun)(void);		//串口发送和接收函数

/*********************************************************************************************/
typedef __packed struct 
{	uint16_t SendLen;
	uint8_t *pSendBuf;	
	uint8_t  SendNum;
	uint8_t  SendFlag;		
}NetSendNum;

extern NetSendNum gNetSend;				//网络分包发送标志

/*********************************************************************************************/
//通信端口协议资源结构
typedef __packed struct
{	
	uchar RxMode;						//接收模模式: 带协议解析 透传
	int size;							//收发缓存大小
	uchar* RxMessageBuffer;				//接收缓冲区
	uchar *PRxDaBuf;					//接收数据指针					
	uchar* TxMessageBuffer; 			//发送缓冲区
	uchar *PTxDaBuf;				   	//发送数据指针
	uint16 RxPro_Lenth;	        	   	//统计接收字节长度
	uint16 TxPro_Lenth;	        	   	//统计发送字节长度
	RESOURCE_USE_STATUS ResState;		//资源使用状态 =0.空闲 =1.正在结收数据 =2.正在发送数据 =3.准备多次发送数据 =4.一条协议接收完或主发准备完毕
	PROTOCOL_TYPE Pro_Type;        	   	//协议类型  =0.门禁协议 =1.邮电部协议 =2.公司协议 =3.DL/T645-2007协议
	PORT_TYPE Port_Type;		    	//通信端口 =0.为串口 =1.为网口                  
	MODE_TYPE Mode_Type;					//模块选择，MODE_TYPE_ZTE=0,MODE_TYPE_BC95=1选择
	CONNECT_STATE Connect_State;	//联网状态
}ProtocolResource;
/*********************************************************************************************/
#define Init_Protocol(Protocol)		{	(Protocol)->RxPro_Lenth = 0;	\
										(Protocol)->TxPro_Lenth = 0;	\
									   	(Protocol)->Pro_Type = PROTOCOL_TYPE_NONE;	\
										(Protocol)->ResState = RESOURCE_USE_STATUS_FREE;	\
									}

#define Reset_Protocol(Protocol)    {	Init_Protocol(Protocol);			\
										(Protocol)->PRxDaBuf = (uchar*)((Protocol)->RxMessageBuffer);	\
										(Protocol)->PTxDaBuf = (uchar*)((Protocol)->TxMessageBuffer);	\
									}



#define Reset_Protocol_Rx(Protocol) {	(Protocol)->RxPro_Lenth = 0;	\
										(Protocol)->PRxDaBuf = (uchar*)((Protocol)->RxMessageBuffer);	\
									}

#define Reset_And_Init_Protocol_Rx(Protocol)  { Reset_Protocol_Rx(Protocol);	\
												(Protocol)->Pro_Type = PROTOCOL_TYPE_NONE;	\
												(Protocol)->ResState = RESOURCE_USE_STATUS_FREE;	\
											}	

#define Reset_Protocol_Tx(Protocol) {	(Protocol)->TxPro_Lenth = 0;	\
										(Protocol)->PTxDaBuf = (uchar*)((Protocol)->TxMessageBuffer);	\
									}

#define Init_ProSendPrt(Protocol)   {	(Protocol)->PTxDaBuf = (uchar*)((Protocol)->TxMessageBuffer);  \
									}
/*********************************************************************************************/
#include "Protocol_debug.h"
#if CONF_QWL_NET_EN									
#include "Protocol_wcdma.h"
#endif
#if CONF_ARMCHAIR_EN									
#include "Protocol_MSG.h"
#endif
/*********************************************************************************************/
const PINFO* getPINFO(PROTOCOL_TYPE pt);	  //获取协议的信息
//注册一类协议的相关参数
int RegisterProtocolFun(USART_FUNCTION uf,ProtocolFun send,ProtocolFun recv,ProtocolFun doProtocol,ProtocolResource* pRes);
int ProtocolSendByte(USART_FUNCTION uf);  //向一个协议发送一个字符
int ProtocolReceiveByte(USART_FUNCTION uf);	//接收一个字符
int DoProtocolReceiveAnalysis(ProtocolResource *Protocol,uchar ChrData);	//分析接收过来的数据的协议
int DoProtocol_DMA_ReceiveAnalysis(ProtocolResource *Protocol,int len);
int DoProtocol_DMA_ReceiveAnalysis(ProtocolResource *Protocol,int len); 	
int ReceiveCallBack(USART_FUNCTION uf,PROTOCOL_TYPE Pro_Type,uchar* buffer, size_t size);

ProtocolResource * GetProtocolResource(USART_FUNCTION uf);	

/*外部API调用开始*/
/*协议初始化*/
void Protocols_Init(void);			
/*主循环体调用的执行所有协议，用于分析接收过来的数据*/	
int DoProtocols(void);
/*发送数据*/	
int SendProtocolBuffer(USART_FUNCTION uf,uchar* buffer, size_t size);
/*复位收发缓存*/
int ResetProtocolBuffer(USART_FUNCTION uf,RESETRTBUFFTYPE ResetType);
/*向指定的协议类型缓存添加发送数据*/
int AddProtocolBuffer(USART_FUNCTION uf,uchar* buffer, size_t size);
/*获取指定的协议缓存结构*/
int GetProtocolBufferOrLen(USART_FUNCTION uf,uchar **ptr,uint16 **len);
/*指定的协议类型启动发送数据*/
int StartSendBuffer(USART_FUNCTION uf);
/*网络数据接收*/	
void NetDeal(void);
/*设置接收回调函数*/
int SetProtocolReceiveCallBack(USART_FUNCTION uf,ReceiveFun fun);


void DealMfProtocol(USART_FUNCTION uf,uchar *DaPtr,uint16 len);
/*串口参数设置*/
int SetUartParameter(
	USART_FUNCTION uf,					//串口协议
	uint32_t baudRate,	 				//波特率
	uint16_t parity,					//校验位
	uint16_t wordLength,				//数据长度
	uint16_t stopBits					//停止位
	);
/*外部API调用结束*/
/*********************************************************************************************/
#endif
