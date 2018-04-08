#ifndef _3GPRODA_H_
#define _3GPRODA_H_
#if CONF_WCDMA_EN
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PROTOCOL_HEADER_SIZE	15
#define MAX_PROTOCOL_TAIL_SIZE	3

typedef enum
{
   PROTOCOL_READ_POS_START,
   PROTOCOL_READ_POS_CENTER,
   PROTOCOL_READ_POS_END,
   PROTOCOL_READ_POS_UNKNOWN
}PROTOCOL_READ_POS;

typedef enum
{
	PROTOCOL_ANALYSIS_TYPE_DEVICE_BEAT,		//0x10 ==>心跳应答
	PROTOCOL_ANALYSIS_TYPE_CENTER_REPLY,	//0x20 ==>中心应答
	PROTOCOL_ANALYSIS_TYPE_BEATE_TEST,		//0x30 ==>心跳测试
	PROTOCOL_ANALYSIS_TYPE_SEND_TO_DEVICE,	//0x50 ==>下发数据到设备
	PROTOCOL_ANALYSIS_TYPE_WCDMA_PROGRAM,	//0x52 ==>下发数据到WCDMA
	PROTOCOL_ANALYSIS_TYPE_WCDMA_CTRL,	//0x54 ==>下发数据到WCDMA
	PROTOCOL_ANALYSIS_TYPE_SEND_TO_CENTER,	//0x60 ==>发送到中心
	PROTOCOL_ANALYSIS_TYPE_WCDMA_PROGRAM_REPLY,	//0x62 ==>结束的回应
	PROTOCOL_ANALYSIS_TYPE_WCDMA_CTRL_REPLY,	//0x64 ==>无线控制结束的回应
	PROTOCOL_ANALYSIS_TYPE_UNKNOWN,			//未知的协议
	PROTOCOL_ANALYSIS_TYPE_MAX
}PROTOCOL_ANALYSIS_TYPE;

#define MAX_RECEIVE_BUFFER	1024

typedef struct						  
{
	char header[MAX_PROTOCOL_HEADER_SIZE];
	int headerCount;
	char headerSender[MAX_PROTOCOL_HEADER_SIZE];
	char tail[MAX_PROTOCOL_TAIL_SIZE];
	char tailSender[MAX_PROTOCOL_TAIL_SIZE];
	int tailCount;
	PROTOCOL_ANALYSIS_TYPE paType;		//协议类型
	int16	dataSize;				//数据长度
	int16	dataSizeConst;			//数据长度，不变的值
	uint16_t checkSum;
	int16	dataSizeSend;			//待发送数据长度
	int16 	dataSizeSendConst;		//待发送数据长度，不变的值	  
	char*	recvBuffer/*[MAX_RECEIVE_BUFFER]*/;	//缓存,接收
	int16	recvSize;						//已接收数据的长度

}tagProtocolAnalysis;

//根据设备号打包
int Netdata_Protocol_Packet(WORD deviceID,PROTOCOL_ANALYSIS_TYPE paType ,char* data,uint16_t size);

int Netdata_ProtocolAnalysis_Packet(tagProtocolAnalysis* pa,PROTOCOL_ANALYSIS_TYPE paType ,char* data,uint16_t size);

uint16_t Netdata_Protocol_ReadSize(WORD deviceID,uint16_t size);

PROTOCOL_READ_POS  Netdata_Protocol_GetPostion(WORD deviceID);

uint16_t Netdata_Protocol_Read(WORD deviceID,char* data, uint16_t size);
/*接收一条全的主机数据	*/
int Netdata_Protocol_Anlysis(USART_FUNCTION uf,uchar ChrData);

uint16_t Netdata_Protocol_ReadAddressAndSize(WORD deviceID,PROTOCOL_READ_POS pos, char** buf,uint16_t* size,uint16_t sizeMax);

int Netdata_Program_Update(WORD deviceID);

int Netdata_System_Ctrl(WORD deviceID);

int Netdata_Init(void);

#include "SystemCtrl.h"
#ifdef __cplusplus
} // extern "C"
#endif
#endif
#endif


