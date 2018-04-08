#include  "config.h"
#include "AtCmdAnalysis.h"

typedef enum
{
	SYSTEM_CTRL_SET_SERIAL,		//串口设置
	SYSTEM_CTRL_SET_IP,
	SYSTEM_CTRL_SET_CID,
	SYSTEM_CTRL_SET_PORTON,			//端口使能                                                                                                                                                                                                                                                                                                                                                                                                                                            
	SYSTEM_CTRL_SET_BEATTIM,	//设置
	SYSTEM_CTRL_SET_PHONE,
	SYSTEM_CTRL_SET_CNMP,
	SYSTEM_CTRL_SET_MSG,
	SYSTEM_CTRL_GET_STATUS	//获取状态
}SYSTEM_CTRL;
typedef struct
{
	SYSTEM_CTRL systemCtrl;
	int (*fun)(WORD deviceID,char* buf,size_t size,char* outbuf );	
}tagSystemCtrlProcessMap;
#define SYSTEMCTRL_PROCESS_NAME(fun) 	  SystemCtrlProcess_##fun
#define SYSTEMCTRL_PROCESS_FUN(fun)	static int SYSTEMCTRL_PROCESS_NAME(fun)##(WORD deviceID,char* buf,size_t size,char* outbuf)

SYSTEMCTRL_PROCESS_FUN(SerialSet);
SYSTEMCTRL_PROCESS_FUN(IPSet);
SYSTEMCTRL_PROCESS_FUN(CIDSet);
SYSTEMCTRL_PROCESS_FUN(PortSet);
SYSTEMCTRL_PROCESS_FUN(BeatTimeSet);
SYSTEMCTRL_PROCESS_FUN(PhoneSet);
SYSTEMCTRL_PROCESS_FUN(CnmpSet);
SYSTEMCTRL_PROCESS_FUN(MsgSet);
SYSTEMCTRL_PROCESS_FUN(StatusGet);

const static tagSystemCtrlProcessMap _gSystemCtrlProcessMap[] =
{
	{SYSTEM_CTRL_SET_SERIAL,SYSTEMCTRL_PROCESS_NAME(SerialSet)},
 	{SYSTEM_CTRL_SET_IP,SYSTEMCTRL_PROCESS_NAME(IPSet)},
	{SYSTEM_CTRL_SET_CID,SYSTEMCTRL_PROCESS_NAME(CIDSet)},
	{SYSTEM_CTRL_SET_PORTON,SYSTEMCTRL_PROCESS_NAME(PortSet)},
	{SYSTEM_CTRL_SET_BEATTIM,SYSTEMCTRL_PROCESS_NAME(BeatTimeSet)},
	{SYSTEM_CTRL_SET_PHONE,SYSTEMCTRL_PROCESS_NAME(PhoneSet)},
	{SYSTEM_CTRL_SET_CNMP,SYSTEMCTRL_PROCESS_NAME(CnmpSet)},
	{SYSTEM_CTRL_SET_MSG,SYSTEMCTRL_PROCESS_NAME(MsgSet)},
	{SYSTEM_CTRL_GET_STATUS,SYSTEMCTRL_PROCESS_NAME(StatusGet)}
};

#define SYSTEM_CTRL_PROCESS_MAP_ARRAY_SIZE	sizeof(_gSystemCtrlProcessMap)/sizeof(tagSystemCtrlProcessMap)

typedef struct
{
	char comType;				 //类型
	char index;					//编号
	char baudRate;				//波特率
	char parityBit;					//校验位
	char dataBit;					//数据长度
	char stopBit;				//停止位
}tagSerialSet;

SYSTEMCTRL_PROCESS_FUN(SerialSet)
{
	char serialCount = 1;
	USART_FUNCTION uf = USART_FUNCTION_DEBUG;
	uint32_t baudRate;	 				//波特率
	uint16_t parity;					//校验位
	uint16_t wordLength;				//数据长度
	uint16_t stopBits;					//停止位
	tagSerialSet* serialSet = (tagSerialSet*)buf;

	if(size != sizeof(tagSerialSet) || buf == NULL || outbuf == NULL)
		return -1;

	switch(serialSet->comType)
	{
		case 'D':	//Debug		调试口
			uf =  USART_FUNCTION_DEBUG;
			break;
		case 'W':	//Wireless	无线口
			uf = USART_FUNCTION_WCDMA;
			return -1;
			//break;
		case 'C':	//Collect	采集口
			uf = USART_FUNCTION_MOUNT;
			serialCount = MOUNT_DEVICE_COUNT;
			break;
		case 'M':	//Message	信息口
			uf = USART_FUNCTION_MSG;
			break;
		case 'X':
			uf = USART_FUNCTION_MOUNT;
			serialSet->index = deviceID;
			serialCount = MOUNT_DEVICE_COUNT;
			break;
		default:
			return -1;
	}
	if(serialSet->index >= serialCount)
		return -1;

	switch(serialSet->baudRate)
	{
		case 0:// bps1200 
			baudRate = 1200;
			break;
		case 1:// bps2400
			baudRate = 2400;
			break;
		case 2:// bps4800
			baudRate = 4800; 
			break;
		case 3:// bps9600
			baudRate = 9600;
			break;
		case 4://bps14400
			baudRate = 14400;
			break;			
		case 5:// bps19200
			baudRate = 19200;
			break;
		case 6:// bps38400
			baudRate = 38400;
			break;
		case 7:// bps56000
			baudRate = 56000;
			break;
		case 8:// bps57600
			baudRate = 57600;
			break;
		case 9:// bps115200
			baudRate = 115200;
			break;
		default:// 默认为9600BPS
			baudRate = 9600;
			break;
	}
 
	switch(serialSet->parityBit)
	{
		case 0:/*N*/
			parity = USART_Parity_No;
			break;
		case 1:/*E*/
			parity = USART_Parity_Even;
			break;
		case 2:/*M*/
			return -1;
		case 3:/*O*/
			parity = USART_Parity_Odd;
			break;
		case 4:/*S*/
			return -1;
		default:/*N*/
			parity = USART_Parity_No;
			break;
	}

	switch(serialSet->dataBit)
	{
		case 0:/*6*/
			return -1;
		case 1:/*7*/
			return -1;
		case 2:/*8*/
			if(parity == USART_Parity_No)
			{
				wordLength = USART_WordLength_8b;
			}
			else if((parity == USART_Parity_Even)||(parity == USART_Parity_Odd))
			{
				wordLength = USART_WordLength_9b;
			}
			break;
		default:/*8*/
			wordLength = USART_WordLength_8b;
			break;	
	}

	switch(serialSet->stopBit)
	{
		case 0:/*1*/
			stopBits = USART_StopBits_1;
			break;
		case 1:/*1.5*/
			stopBits = USART_StopBits_1_5;
			break;
		case 2:/*2*/
			stopBits = USART_StopBits_2;
			break;
		default:/*1*/
			stopBits = USART_StopBits_1;
			break;		
	}
	DEBUG("baudRate:%d,parity:%d,wordLength:%d,stopBits:%d\n",baudRate,parity,wordLength,stopBits);
	return API_SetComParameter((USART_FUNCTION)(uf + serialSet->index),baudRate,parity,wordLength,stopBits);
}

typedef struct
{
	char deviceID;
	char IP[4];
	unsigned short port;
}tagIPSet;

SYSTEMCTRL_PROCESS_FUN(IPSet)
{
	tagIPSet* ipSet =  (tagIPSet*)buf;
	DEBUG("deviceID:%d"
			"IP:%d.%d.%d.%d"
			"port:%d\n",
			 ipSet->deviceID,
			 ipSet->IP[0],
			 ipSet->IP[1],
			 ipSet->IP[2],
			 ipSet->IP[3],
			 ipSet->port
	);
	DEBUG("%s==>OK\n",__FUNCTION__);

	return 0;
}
SYSTEMCTRL_PROCESS_FUN(CIDSet)
{
	DEBUG("%s==>OK\n",__FUNCTION__);

	return 0;
}
SYSTEMCTRL_PROCESS_FUN(PortSet)
{
	DEBUG("%s==>OK\n",__FUNCTION__);

	return 0;
}
SYSTEMCTRL_PROCESS_FUN(BeatTimeSet)
{
	DEBUG("%s==>OK\n",__FUNCTION__);

	return 0;
}
SYSTEMCTRL_PROCESS_FUN(PhoneSet)
{
	DEBUG("%s==>OK\n",__FUNCTION__);

	return 0;
}
SYSTEMCTRL_PROCESS_FUN(CnmpSet)
{
	DEBUG("%s==>OK\n",__FUNCTION__);

	return 0;
}
SYSTEMCTRL_PROCESS_FUN(MsgSet)
{
	DEBUG("%s==>OK\n",__FUNCTION__);

	return 0;
}
SYSTEMCTRL_PROCESS_FUN(StatusGet)
{
	DEBUG("%s==>OK\n",__FUNCTION__);

	return 0;
}

int SystemCtrl_Process(WORD deviceID,WORD type,char* buf,size_t size,char* outbuf)
{
	int i = 0;
	if(buf==NULL ||outbuf==NULL)
		return -1;
	for(i = 0; i < SYSTEM_CTRL_PROCESS_MAP_ARRAY_SIZE; i++)
	{
		if(_gSystemCtrlProcessMap[i].systemCtrl == type)
			break;
	}
	if(i >= SYSTEM_CTRL_PROCESS_MAP_ARRAY_SIZE)
	{ 	//找不到匹配的命令号
		return -1;
	}

	//找到一个匹配的项
	if(_gSystemCtrlProcessMap[i].fun == NULL)
	{
		return -1;
	}

	return _gSystemCtrlProcessMap[i].fun(deviceID,buf,size,outbuf);

}	

