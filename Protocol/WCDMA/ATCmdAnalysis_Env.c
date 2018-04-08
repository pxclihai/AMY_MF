/****************************************Copyright (c)****************************************************
**                                      
**                                 
**
**--------------文件信息---------------------------------------------------------------------------------
** 文件名:				ATCmdAnalysis.c
** 描述:				WCDMA命令解析系统
**
**--------------------------------------------------------------------------------------------------------
** 创建者:				张泽
** 创建日期:			2014-5-24
** 版本号:				v1.0
** 描述:				最初版本
**
**--------------------------------------------------------------------------------------------------------
** 修改者:             
** 修改日期:           
** 版本:                 
** 描述:            
**
*********************************************************************************************************/
 

#include "config.h"
#include "AtCmdAnalysis.h"

/******************************************************************************/

#define MAX_QUEUE_CALLBACK_SIZE		5 
#define MAX_QUEUE_SEND_BUFFER_SIZE	20


/******************************************************************************/
#define ERROR_PROCCESS_NAME(fun) 	  Error_Process_##fun
#define ERROR_PROCCESS(fun) static int ERROR_PROCCESS_NAME(fun)(char* addr, size_t size)
/******************************************************************************/
//函数定义区
/*ATCmd 初始化*/
static int AtCmdAnalysis_ATCmdRunCtrlArray_Init(void);

ERROR_PROCCESS(Check);
ERROR_PROCCESS(Init);
ERROR_PROCCESS(Device);
ERROR_PROCCESS(System);
ERROR_PROCCESS(SimCard);
ERROR_PROCCESS(Net);
ERROR_PROCCESS(Module);

/******************************************************************************/

static const tagATCmd*	 _gATCmd = NULL;
static int _gATCmdCount = 0;

static const tagATCmdRecvProcessItem*	_gtagATCmdRecvProcessItemArray = NULL;
static int _gATCmdRecvProcessItemSize = 0;
 
//初始化配置表
const static  tagAtCmdRunCtrlItem* _gAtCmdRunFunTypeArray = NULL;
static tagAtCmdRunCtrlItem _gAtCmdRunCtrlArray[ATCMD_RUN_TYPE_MAX];

static tagCallBackQueueItem	_gCallBackQueueItemArray[MAX_QUEUE_CALLBACK_SIZE] = {0};

static tagSendBufferQueueItem _gSendBufferQueueItemArray[MAX_QUEUE_SEND_BUFFER_SIZE] = {0};

tagATCmdSetInfo _gATCmdSetInfo = {0};
static 	tagModeRunStateStruct	_gModeRunState = {0};

static LinkQueue	_gLinkQueue[2]	= {0};
/*接收缓冲区结束*/
static LinkQueue	_gSendBufferLinkQueue[2]	= {0};

static tagModuleSetValStruct _gModuleSetVal[MOUNT_DEVICE_COUNT] =
 {
 	{	"10010",
		"10010",
		1,
		"192.168.0.2",
		8800,
		true,
		true,
		NETWORK_PRIORIYY_TYPE_AUTO,
		"",
		0,
		1,
		1,
	},
	{	"10010",
		"10010",
		1,
		"192.168.0.2",
		8800,
		true,
		true,
		NETWORK_PRIORIYY_TYPE_AUTO,
		"",
		0,
		1,
		0,
	}
 };
#if CONF_WARN_SEND_EN
static tagAlarmPhoneStruct	_gAlarmPhone =
{
	0,
	"123456",	
	"123456",	
	"123456",
	"123456",
	"123456",
	"123456",
	"help",
};
#endif
#if 1
/*
typedef __packed struct
{
	WORD  mapDeviceID;							//映射的设备ID
	WORD  controlPort;							//设置用于连接的采集口,不能与现有的采集口一致
	char  CID[MAX_CID_SIZE];					//CID编号
	char  IPAddress[MAX_PHONE_SIZE];   			//远程中心IP地址
	int32 Port;            						//远程中心端口号
	DWORD  timeOut;								//超时恢复时间，单位毫秒
	uchar deviceFun;							//设备功能
}tagSystemSetValCtrlStruct;

typedef __packed struct
{
	tagSystemSetValCtrlStruct sysSetValCtrl;	//值控制结构体
	SYSTEM_MOUNT_MAP_STATUS sysMountMapStatus;	//映射状态
}tagSystemSetValStruct;

*/
/*系统连接，用于日常维护*/
static tagSystemSetValStruct _gSystemSetVal =
{
	{
		0,								//映射的设备ID
		9,								//网络接口
		"123.159.193.22",   				//远程中心IP地址
		8383,            				//远程中心端口号
		60000							//超时10min
	},
	SYSTEM_MOUNT_MAP_STATUS_OFF,			//默认状态是OFF
	MAX_NET_IS_OPEN_NUM -1
};
#endif
const static tagErrorCallBack _gErrorCallBack[] = 
{
	{WCDMA_ERROR_CHECK,ERROR_PROCCESS_NAME(Check)},	//模块检测错误
	{WCDMA_ERROR_INIT,ERROR_PROCCESS_NAME(Init)},		//初始化没成功
	{WCDMA_ERROR_DEVICE,ERROR_PROCCESS_NAME(Device)},		//设备报的错误
	{WCDMA_ERROR_SYSTEM,ERROR_PROCCESS_NAME(System)},			//系统崩溃
	{WCDMA_ERROR_NO_SIM_CARD,ERROR_PROCCESS_NAME(SimCard)},	//没有SIM卡
	{WCDMA_ERROR_NET_DISCONNECT,ERROR_PROCCESS_NAME(Net)},	//网络连接不上
	{WCDMA_ERROR_MODULE_RESTART,ERROR_PROCCESS_NAME(Module)}	//模块错误

};

static tagCnsModSet* _gCnsMod = NULL;
static int _gCnsModSize = 0;
//static tagAPNCustom	_gAPNCustom = {
//	FALSE,			//开启自定义APN
//	"uninet",		//APN名字
//	"",				//代理服务器地址
//	0u,				//端口
//	"",				//用户名
//	""				//密码	
//};
/******************************************************************************/

#define AT_CMD_COUNT	_gATCmdCount	
#define RECV_AT_CMD_SIZE	_gATCmdRecvProcessItemSize
#define ERROR_CALL_BACK_SIZE	sizeof(_gErrorCallBack)/sizeof(tagErrorCallBack)

#define CLEAR_ERROR_TYPE()  _gATCmdSetInfo.errBits = SEND_ERROR_TYPE_NONE
#define SET_ERROR_TYPE(errCode)	 _gATCmdSetInfo.errBits |= 	errCode
#define CHECK_ERROR()	(_gATCmdSetInfo.errBits != SEND_ERROR_TYPE_NONE)
#define	_gRecvFunction _gATCmdSetInfo.RecvFunction
#define _gSendAtCmd	_gATCmdSetInfo.SendAtCmd	
#define AT_CALL_BACK_MODE()	(_gSendAtCmd != NULL)

#define PResFreeQueue	_gLinkQueue[0]		//空闲的队列
#define	PResFullQueue	_gLinkQueue[1]	   	//繁忙的队列
#define	PResFreeQueuePtr	&PResFreeQueue
#define	PResFullQueuePtr	&PResFullQueue

#define PResFreeSendBufferQueue	_gSendBufferLinkQueue[0]		//空闲的队列
#define	PResFullSendBufferQueue	_gSendBufferLinkQueue[1]	   	//繁忙的队列
#define	PResFreeSendBufferQueuePtr	&PResFreeSendBufferQueue
#define	PResFullSendBufferQueuePtr	&PResFullSendBufferQueue

#define CK(str1,str2)	 (StrNcmp((uchar*)str1,(uchar*)str2) == 0)
//内部函数区域开始
#define _INTR(fun)	fun

/******************************************************************************/
#if CONF_WARN_SEND_EN
static int _INTR(AtCmdAnalysis_WarnPhone_EERom_Story)(void)
{
		uint EepAddr;
		LINE_LOG();
		EepAddr =  _EEP_ADDR_ALARM_PHONE; 
		LINE_LOG();  						
		return API_EEPROM_Write(EepAddr,sizeof(tagAlarmPhoneStruct) ,(uchar*)&_gAlarmPhone,true);
}
#endif
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_EERom_Story
// 功能描述  : 存储变量到EEROM内
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static int _INTR(AtCmdAnalysis_EERom_Story)(void)
{
	uint EepAddr;
	LINE_LOG();
	EepAddr =  _EEP_ADDR_MODULE_VAL; 
	LINE_LOG();  						
	return API_EEPROM_Write(EepAddr,sizeof(tagModuleSetValStruct) * MOUNT_DEVICE_COUNT ,(uchar*)_gModuleSetVal,true);    
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_EERom_Read
// 功能描述  : 从EEROM内读取变量
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static int _INTR(AtCmdAnalysis_EERom_Read)(void)
{
	uint EepAddr;
	tagModuleSetValStruct m[MOUNT_DEVICE_COUNT];
	int i = 0;
#if CONF_WARN_SEND_EN
	uint EepAddr1;
	tagAlarmPhoneStruct n;
	EepAddr1 =  _EEP_ADDR_ALARM_PHONE;
#endif
	EepAddr =  _EEP_ADDR_MODULE_VAL;
	LINE_LOG();
	memcpy(m,_gModuleSetVal,sizeof(tagModuleSetValStruct) * MOUNT_DEVICE_COUNT);  
	if(API_EEPROM_Read(EepAddr,sizeof(tagModuleSetValStruct) * MOUNT_DEVICE_COUNT,(uchar*)_gModuleSetVal,true) < 0)
	{
		memcpy(_gModuleSetVal,m,sizeof(tagModuleSetValStruct) * MOUNT_DEVICE_COUNT);
		return -1;  
	}
#if CONF_WARN_SEND_EN
	memcpy(&n,&_gAlarmPhone,sizeof(tagAlarmPhoneStruct));  
	if(API_EEPROM_Read(EepAddr1,sizeof(tagAlarmPhoneStruct),(uchar*)&_gAlarmPhone,true) < 0)
	{
		memcpy(&_gAlarmPhone,&n,sizeof(tagAlarmPhoneStruct));
		return -1;  
	}
#endif
	for(i = 0; i < MOUNT_DEVICE_COUNT; i++)
	{
		_gModuleSetVal[i].tryCount = 0;
 	}
	LINE_LOG();					
	return 0;    
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_AtCmd_EndAnlysisMode
// 功能描述  : 改变分析模式回普通的模式
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static void _INTR(AtCmdAnalysis_AtCmd_EndAnlysisMode)(void)
{
	LINE_LOG();
	_gATCmdSetInfo.protocolAnlysisMode = PROTOCOL_ANALYSIS_MODE_NONE;
	LINE_LOG();
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCmdRecv_Queue_Init
// 功能描述  : 将中断运行转移到主函数体运行的过度队列初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static int _INTR(ATCmdRecv_Queue_Init)(void)
 {
 	LINE_LOG();
 	QueueInit(_gCallBackQueueItemArray,MAX_QUEUE_CALLBACK_SIZE,PResFreeQueuePtr,PResFullQueuePtr);
 }
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCmdRecv_SendBufferQueue_Init
// 功能描述  : ATCmd发送队列初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static int _INTR(ATCmdRecv_SendBufferQueue_Init)(void)
 {
 	LINE_LOG();
 	QueueInit(_gSendBufferQueueItemArray,MAX_QUEUE_SEND_BUFFER_SIZE,PResFreeSendBufferQueuePtr,PResFullSendBufferQueuePtr);
 }

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_Protocol_Analysis
// 功能描述  : 网络数据以及短信内容的识别
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static int _INTR(AtCmdAnalysis_Protocol_Analysis)(USART_FUNCTION uf,uchar c)
{
	int ret = 0;
	LINE_LOG();
	switch(_gATCmdSetInfo.protocolAnlysisMode)
	{
		case PROTOCOL_ANALYSIS_MODE_SHORT_MESSAGE: //短信
			//短信接收
			ret = Msg_Buffer_Receive(_gATCmdSetInfo.phoneNum,c);
			break;
		case PROTOCOL_ANALYSIS_MODE_RECEIVE: //接收网络数据
			//网络数据解析 	
			ret = Netdata_Protocol_Anlysis(_gATCmdSetInfo.RecvFunction,c);
			break;
		case PROTOCOL_ANALYSIS_MODE_NONE:
		default:
			return -1;
	}
	if(_gATCmdSetInfo.RecvSize >0)
		_gATCmdSetInfo.RecvSize--;	
	LINE_LOG();
	if(ret < 0 || _gATCmdSetInfo.RecvSize == 0)
	{	//结束或者任何其他意外，都可以导致外部解析模式结束
		_gATCmdSetInfo.RecvSize = 0;
   		_gATCmdSetInfo.protocolAnlysisMode = PROTOCOL_ANALYSIS_MODE_NONE;
		API_KillTimerFast(TIMER_TYPE_WCDMA_ANLYSIS_MODE);

	}
	return 0;
} 

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_AtCmd_GetIndex
// 功能描述  : 获取AtCmd的索引
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static int _INTR(AtCmdAnalysis_AtCmd_GetIndex)(uint8 cmd)
{
	int i = 0;
	LINE_LOG();
	for(i=0; i < AT_CMD_COUNT;i++)
	{
		if(_gATCmd[i].ATCmd == cmd)
			return i; 
	}
	LINE_LOG();
	return -1;
}


//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_AtCmdRun_Process
// 功能描述  : 主循环内调用，命令组的执行
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

static int _INTR(AtCmdAnalysis_AtCmdRun_Process)(void)
{
	int i = 0;
	int startGroupID = 0;
	LINE_LOG();
	if(_gATCmdSetInfo.atCmdRunStatus ==  ATCMD_RUN_STATUS_RUN_BUSY)
		return -1;
	_gATCmdSetInfo.aTCmdSendCallBack = NULL;
	//startGroupID = _gATCmdSetInfo.atCmdGroup < 0? 0:_gATCmdSetInfo.atCmdGroup;
	startGroupID = _gATCmdSetInfo.atCmdGroup >(ATCMD_RUN_TYPE_MAX-1)? (ATCMD_RUN_TYPE_MAX-1) : _gATCmdSetInfo.atCmdGroup;
	if(startGroupID <  ATCMD_RUN_TYPE_MAX -1)
	{
		startGroupID++;
	}
	else
	{
		startGroupID = 0;
	}
	CLEAR_ERROR_TYPE();	
	for(i = startGroupID; i < ATCMD_RUN_TYPE_MAX;i++)
	{
		if(_gAtCmdRunCtrlArray[i].status == ATCMDGROUP_RUN_STATUS_ON && _gAtCmdRunCtrlArray[i].lock)
		{
			_gATCmdSetInfo.atCmdGroup = (ATCMD_RUN_TYPE)i;
			if(_gAtCmdRunCtrlArray[i].fun != NULL)
			{
				_gAtCmdRunCtrlArray[i].fun();	
			}
			else
			{
				_gATCmdSetInfo.atCmdRunStatus =  ATCMD_RUN_STATUS_RUN_FREE;	
			}
			if(_gAtCmdRunCtrlArray[i].status == ATCMDGROUP_RUN_STATUS_ON)   //常态运行的只能有一个，而且只能在最后一个。
			{
				_gAtCmdRunCtrlArray[i].status = ATCMDGROUP_RUN_STATUS_OFF;
			}
			_gAtCmdRunCtrlArray[i].lock = false;
			return 0;
			//break;	//执行完一个后，立刻结束,下一个从头开始执行				
		}
	}
	
	for(i = 0; i < startGroupID;i++)
	{
		if(_gAtCmdRunCtrlArray[i].status == ATCMDGROUP_RUN_STATUS_ON && _gAtCmdRunCtrlArray[i].lock)
		{
			_gATCmdSetInfo.atCmdGroup = (ATCMD_RUN_TYPE)i;
			if(_gAtCmdRunCtrlArray[i].fun != NULL)
			{
				_gAtCmdRunCtrlArray[i].fun();	
			}
			else
			{
				_gATCmdSetInfo.atCmdRunStatus =  ATCMD_RUN_STATUS_RUN_FREE;	
			}
			if(_gAtCmdRunCtrlArray[i].status == ATCMDGROUP_RUN_STATUS_ON)   //常态运行的只能有一个，而且只能在最后一个。
			{
				_gAtCmdRunCtrlArray[i].status = ATCMDGROUP_RUN_STATUS_OFF;
			}
			_gAtCmdRunCtrlArray[i].lock = false;
			break;	//执行完一个后，立刻结束,下一个从头开始执行				
		}
	}
	LINE_LOG();
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_ATCmdRunCtrlArray_Init
// 功能描述  : 命令组的系统的初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

static int _INTR(AtCmdAnalysis_ATCmdRunCtrlArray_Init)(void)
{
	int i = 0;
	LINE_LOG();
	for(i=0; i < ATCMD_RUN_TYPE_MAX;i++)
	{
		memcpy(&_gAtCmdRunCtrlArray[_gAtCmdRunFunTypeArray[i].type],&_gAtCmdRunFunTypeArray[i],sizeof(tagAtCmdRunCtrlItem));
	}
	LINE_LOG();
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_BeatString_Build
// 功能描述  : 心跳字符串的建立
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

static int _INTR(AtCmdAnalysis_BeatString_Build)(WORD deviceID)
{
	LINE_LOG();
	if(deviceID < MOUNT_DEVICE_COUNT )
	{
		tagProtocolAnalysis pa;
		char data[51] = "";

		char* pBuf = _gModuleSetVal[deviceID].beatString;
		char* pCid= NULL; 

		size_t dataSize = sizeof(data);
		int cidLen = ReadStrLen(_gModuleSetVal[deviceID].CID);
		LINE_LOG();
		memcpy(data +(20-cidLen),_gModuleSetVal[deviceID].CID,cidLen);
		pCid = data +(20-cidLen);
		while(cidLen--)
		{
			(*pCid) = (*pCid) - '0';
			pCid++;	
		}
		LINE_LOG();
	 	if(Netdata_ProtocolAnalysis_Packet(&pa,PROTOCOL_ANALYSIS_TYPE_DEVICE_BEAT,data, dataSize) > -1)
		{
			//改变当前的状态为未连接状态
			memcpy(pBuf,pa.headerSender,MAX_PROTOCOL_HEADER_SIZE);
			pBuf += MAX_PROTOCOL_HEADER_SIZE;
			memcpy(pBuf,data,dataSize);
			pBuf +=  dataSize;
			memcpy(pBuf,pa.tailSender,MAX_PROTOCOL_TAIL_SIZE);
			LINE_LOG();
			return 0;
		}
	}		
	LINE_LOG();
	return -1;
} 

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_RunStatus_Init
// 功能描述  : 心跳字符串的建立
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

static int _INTR(AtCmdAnalysis_RunStatus_Init)(void)
{
	int index = 0;
	LINE_LOG();
	memset(&_gModeRunState,NULL,sizeof(tagModeRunStateStruct));

	for(index =0; index < MOUNT_DEVICE_COUNT; index++)
	{
		_gModeRunState.NetIsOpen[index] = MAX_NET_IS_OPEN_NUM -1;
	}
	LINE_LOG();
	return 0;
}
//内部函数区域结束
/******************************************************************************/

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Error_EEROM_Record
// 功能描述  : 错误记录
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

static int _INTR(Error_EEROM_Record)(char* title,char* buf)
{
	tagErrorEERomContext menu;
//	Date_Time dataTime;
	char bufs[256] = {0};
	DWORD	address;
	LINE_LOG();
	if(API_EEPROM_Read(_EEP_ADDR_ERROR_RECORD_CONTEXT, sizeof(tagErrorEERomContext), (uchar*)(&menu), true) < 0)
	{
		memset(&menu,NULL,sizeof(menu));
	}
	if(menu.writePointer > menu.size)
	{
		menu.writePointer = 0;	
	}
	address = _EEP_ADDR_ERROR_RECORD + 256 * menu.writePointer;
	LINE_LOG();
//	memset(&dataTime,NULL,sizeof(dataTime));
	sprintf(bufs,
		"%s %s %s\n",
//		dataTime.StuDate.Year,
//		dataTime.StuDate.Month,
//		dataTime.StuDate.Day,
//		dataTime.StuDate.Hour,
//		dataTime.StuDate.Minute,
//		dataTime.StuDate.Second,
		_gATCmdSetInfo.timeStr,
		title,
		buf
		);//记录时间
	DEBUG("%s\n",bufs);
	LINE_LOG();
	if(API_EEPROM_Write(address,200,(uchar*)bufs,true) > -1)
	{
		if(menu.size < MAX_ERROR_CODE_SIZE)
		{
			menu.size++;

		}
		menu.writePointer++;
		if(menu.writePointer >= MAX_ERROR_CODE_SIZE)
		{
			menu.writePointer = 0;	
		}

		DEBUG("Write error Address:%d\n",address);
		API_EEPROM_Write(_EEP_ADDR_ERROR_RECORD_CONTEXT, sizeof(tagErrorEERomContext), (uchar*)(&menu), true);
		LINE_LOG();
		return 0;
	}
	 LINE_LOG();
	 return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ERROR_PROCCESS(Check)
// 功能描述  : 检测错误
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ERROR_PROCCESS(Check)
{
	LINE_LOG();
	return Error_EEROM_Record("Check Error",addr);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ERROR_PROCCESS(Init)
// 功能描述  : 初始化错误
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ERROR_PROCCESS(Init)
{
	LINE_LOG();
	return Error_EEROM_Record("Init Error",addr);
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ERROR_PROCCESS(Device)
// 功能描述  : 设备错误
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ERROR_PROCCESS(Device)
{
	LINE_LOG();
	return Error_EEROM_Record("Device Error",addr);
}
// 函数名称  : ERROR_PROCCESS(System)
// 功能描述  : 系统错误
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ERROR_PROCCESS(System)
{
	LINE_LOG();
	return Error_EEROM_Record("SysErr",addr);
}
// 函数名称  : ERROR_PROCCESS(SimCard)
// 功能描述  : sim卡错误
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ERROR_PROCCESS(SimCard)
{
	return Error_EEROM_Record("SimCard Error",addr);
}
// 函数名称  : ERROR_PROCCESS(Net)
// 功能描述  : 网络错误
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ERROR_PROCCESS(Net)
{
	LINE_LOG();
	return Error_EEROM_Record("Net Error",addr);
}

// 函数名称  : ERROR_PROCCESS(Module)
// 功能描述  : 模块错误
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ERROR_PROCCESS(Module)
{
	LINE_LOG();
	return Error_EEROM_Record("Module Error",addr);
}



/******************************************************************************/

// 函数名称  : ATCMDRECV_PROCESS(StartSend)
// 功能描述  : 开始向某个功能口发送数据
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_PROCESS(StartSend)
{
	USART_FUNCTION uf;
	LINE_LOG();
	if(argc < 1)
		return -1;		
	uf = (USART_FUNCTION)atoi(argv[0]);
	LINE_LOG();
	if(uf < USART_FUNCTION_MAX)
		return API_StartSendBuffer(uf);
	return -1;		
}

// 函数名称  : ATCMDRECV_PROCESS(StartUpdate)
// 功能描述  : 开始升级
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None					 
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_PROCESS(StartUpdate)
{
	USART_FUNCTION uf;
	WORD deviceID = 0;
	LINE_LOG();
	if(argc < 1)
		return -1; 		
	uf = (USART_FUNCTION)atoi(argv[0]);
	if(uf < USART_FUNCTION_MOUNT ||uf >= USART_FUNCTION_MOUNT + MOUNT_DEVICE_COUNT)
		return -1;

	deviceID = uf - USART_FUNCTION_MOUNT;
	Netdata_Program_Update(deviceID);
	LINE_LOG();
	return 0;		
}

// 函数名称  : ATCMDRECV_PROCESS(StartWcdmaCtrl)
// 功能描述  : 开始WCDMA控制
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None					 
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_PROCESS(StartWcdmaCtrl)
{
	USART_FUNCTION uf;
	WORD deviceID = 0;
	LINE_LOG();
	if(argc < 1)
		return -1; 		
	uf = (USART_FUNCTION)atoi(argv[0]);
	if(uf < USART_FUNCTION_MOUNT ||uf >= USART_FUNCTION_MOUNT + MOUNT_DEVICE_COUNT)
		return -1;

	deviceID = uf - USART_FUNCTION_MOUNT;
	Netdata_System_Ctrl(deviceID);
	LINE_LOG();
	return 0;
}

/******************************************************************************/
/*定时器区域开始*/

////※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//// 函数名称  : TIMER_FUN(SystemReStart)
//// 功能描述  : 系统重启
//// 输入参数  : None
//// 输出参数  : None
//// 返回参数  : None
////※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//TIMER_FUN(SystemReStart)
//{
//	NVIC_SystemReset();	
//}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : TIMER_FUN(MainLoop)
// 功能描述  : 主循环体
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

TIMER_FUN(MainLoop)
{
	LINE_LOG();
	if(_gATCmdSetInfo.atCmdRunStatus ==  ATCMD_RUN_STATUS_RUN_FREE)
	{
		RUN_ATCMD_GROUP(ATCMD_RUN_TYPE_LOOP);
	}
	LINE_LOG();
}


//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : TIMER_FUN(DeviceBeat)
// 功能描述  : 发送心跳
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
TIMER_FUN(DeviceBeat)
{
	int  i = 0;
	LINE_LOG();
	for(i = 0; i < MOUNT_DEVICE_COUNT; i++)
	{
		if(_gModuleSetVal[i].deviceFun == 1)
		{
			if(_gModeRunState.NetIsOpen[i] == MAX_NET_IS_OPEN_NUM)
			{
				AtCmdAnalysis_Net_InfoSend(i,_gModuleSetVal[i].beatString,69);	//发送心跳数据	
			}
			else
			{
				if(_gModuleSetVal[i].tryCount < 1) {
					//没连接上。
					_gModuleSetVal[i].tryCount = 1;
					//DEBUG("start :%d count:%d\n",i,_gModuleSetVal[i].tryCount );
					
			   		AtCmdAnalysis_Net_Connect(i);

				}
				else
				{
					_gModuleSetVal[i].tryCount--;	
				}
			}
		}
	
	}
	if(_gSystemSetVal.sysMountMapStatus == SYSTEM_MOUNT_MAP_STATUS_ON)
	{
		//如果开启了系统控制连接
		if(_gSystemSetVal.NetIsOpen < MAX_NET_IS_OPEN_NUM)
		{
			AtCmdAnalysis_Net_Connect(_gSystemSetVal.sysSetValCtrl.mapDeviceID);//开始连接后台控制	
		}
		else
		{
			AtCmdAnalysis_Net_InfoSend(_gSystemSetVal.sysSetValCtrl.mapDeviceID,_gModuleSetVal[_gSystemSetVal.sysSetValCtrl.mapDeviceID].beatString,69);
		}
		
	}
	else
	{
		if(_gSystemSetVal.NetIsOpen == MAX_NET_IS_OPEN_NUM)
		{
			AtCmdAnalysisi_DisConnect(_gSystemSetVal.sysSetValCtrl.mapDeviceID);//开始连接后台控制	
		}
	}
	//一分钟发送一次 	
	LINE_LOG();
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : TIMER_FUN(StartOk)
// 功能描述  : 重启
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

TIMER_FUN(StartOk)
{
	LINE_LOG();
	AtCmdAnalysis_RunStatus_Init();
	LINE_LOG();
	DEBUG_RECORD("Re Start!");

	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
 		LINE_LOG();
		AtCmd_RECV_WCDMA_Init();

	}
	else
	{
		LINE_LOG();
		API_WCDMA_Init();
	}
	LINE_LOG();
	API_SetTimer(60000,-1,TIMER_TYPE_WCDMA_AT_RESTART,TIMER_NAME(StartOk),NULL);
	LINE_LOG();
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : TIMER_FUN(SystemMap)
// 功能描述  : 系统映射
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

TIMER_FUN(SystemMap)
{
	_gSystemSetVal.sysMountMapStatus = SYSTEM_MOUNT_MAP_STATUS_OFF;
	//TIMER_RUN(StartOk);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : TIMER_FUN(DelayStart)
// 功能描述  : 重启
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

TIMER_FUN(DelayStart)
{
	TIMER_RUN(StartOk);
}

int AtCmdAnalysis_AtcmdError_Record(void)
{
	LINE_LOG();
	if(!_gATCmdSetInfo.cmpSuccess && _gATCmdSetInfo.SendAtCmd != NULL)
	{
		char errBuf[30] = {0};
		_gModeRunState.AtErrorCount++;
		if(_gModeRunState.AtErrorCount >= MAX_AT_CMD_ERROR_COUNT)
		{
			char* str = "AtCmd Error";
			_gModeRunState.AtErrorCount = 0;
			AtCmdAnalysis_ErrorMsg(WCDMA_ERROR_MODULE_RESTART,str,strlen(str));
			TIMER_RUN(StartOk);
		}
		sprintf(errBuf,"No:%d cmd:%d\n",_gATCmdSetInfo.sendRepeatCount,_gATCmdSetInfo.SendAtCmd->ATCmd);
	//	DEBUG_RECORD(errBuf);
		LINE_LOG();
	}
	LINE_LOG();	
	return 0;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : TIMER_FUN(StartToSendAtCmd)
// 功能描述  : AtCmd发送
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

TIMER_FUN(StartToSendAtCmd)
{
	LINE_LOG();
	if(_gATCmdSetInfo.atCmdRunStatus == ATCMD_RUN_STATUS_NOT_RUN)
	{
		//不会继续执行下去了
	 	QueuePtr QPtr = NULL;

		DEBUG_RECORD("System must be Restart\n");
		while((QPtr = DeQueue(PResFullSendBufferQueuePtr)) != NULL)
		{
			EnQueue(PResFreeSendBufferQueuePtr,QPtr); 	//清空所有的Cmd	,接下来的命令不执行了
		}
		LINE_LOG();			
		API_WCDMA_Init();
		LINE_LOG();
		return;
	}
	LINE_LOG();
	_gATCmdSetInfo.atCmdRunStatus =  ATCMD_RUN_STATUS_RUN_BUSY;
	if(_gATCmdSetInfo.SendAtCmd == NULL || _gATCmdSetInfo.sendRepeatCount <= 0)
	{
	 	QueuePtr QPtr = NULL;
	 	tagSendBufferQueueItem* pItem = NULL;
		LINE_LOG();
		if(CHECK_ERROR())
		{
			AtCmdAnalysis_AtcmdError_Record();
			//DEBUG("Check Error\n");
			if(_gATCmdSetInfo.SendAtCmd != NULL)
			{
				char errBuf[30] = {0};
				sprintf(errBuf,"Check Cmd Error:%d",_gATCmdSetInfo.SendAtCmd->ATCmd);
				DEBUG_RECORD(errBuf); 
			}
			LINE_LOG();
			while((QPtr = DeQueue(PResFullSendBufferQueuePtr)) != NULL)
			{
				EnQueue(PResFreeSendBufferQueuePtr,QPtr); 	//清空所有的Cmd	,接下来的命令不执行了
			}			
		}
		else
		{
//			 if(_gATCmdSetInfo.SendAtCmd != NULL)
//			 {
//			 	if(_gATCmdSetInfo.SendAtCmd->RepeatSendCnt > 1)
//				{
//			 		_gModeRunState.AtErrorCount = 0;
//				}
//			 }
		}


		if((QPtr = DeQueue(PResFullSendBufferQueuePtr)) == NULL )
		{
			_gATCmdSetInfo.atCmdRunStatus=  ATCMD_RUN_STATUS_RUN_FREE;	//空闲;

			if(_gATCmdSetInfo.aTCmdSendCallBack != NULL)
			{
				//发送失败后的代码
				_gATCmdSetInfo.aTCmdSendCallBack(_gATCmdSetInfo.SendAtCmd,_gATCmdSetInfo.errBits);				
			}
	 		_gATCmdSetInfo.SendAtCmd = NULL;
			_gATCmdSetInfo.sendRepeatCount = 0;

			CLEAR_ERROR_TYPE();	
			API_KillTimer(TIMER_TYPE_WCDMA_AT_SEND);
			LINE_LOG();
			//一套命令全部结束
			return ;
		}
		else
		{
			int index = 0;
			pItem = QPtr->data;
			_gATCmdSetInfo.SendAtCmd = pItem->SendAtCmd;

			switch(pItem->sendBufferType)
			{
				case SEND_BUFFER_TYPE_COMMON:
					memcpy(_gATCmdSetInfo.sendBuffer,pItem->sendBuffer,pItem->sendBufferSize);
					_gATCmdSetInfo.sendBufferSize = pItem->sendBufferSize;
					break;
				case SEND_BUFFER_TYPE_HUGE:
					//发送大数据量	不能超过1250个数据
					pItem->hugeBuffer.Count = pItem->hugeBuffer.Count < MAX_HUGE_BUFFER_SIZE?pItem->hugeBuffer.Count: MAX_HUGE_BUFFER_SIZE;
					_gATCmdSetInfo.sendBufferSize = 0;
					for(index = 0; index < pItem->hugeBuffer.Count;index++)
					{
						if(pItem->hugeBuffer.Buffer[index] != NULL && _gATCmdSetInfo.sendBufferSize + pItem->hugeBuffer.size[index] <= MAX_SEND_BUFFER_SIZE )
						{
							//没有超过最大buffer容量，就继续添加数据
							memcpy(_gATCmdSetInfo.sendBuffer + _gATCmdSetInfo.sendBufferSize,pItem->hugeBuffer.Buffer[index],pItem->hugeBuffer.size[index]);
							_gATCmdSetInfo.sendBufferSize += pItem->hugeBuffer.size[index];
						}
	
					}
			}			
			_gATCmdSetInfo.sendRepeatCount = _gATCmdSetInfo.SendAtCmd->RepeatSendCnt;
			EnQueue(PResFreeSendBufferQueuePtr,QPtr);
		}
		
	}
	LINE_LOG();
	 if(_gATCmdSetInfo.SendAtCmd != NULL && _gATCmdSetInfo.sendRepeatCount > 0)
	 {
		AtCmdAnalysis_AtcmdError_Record();
		_gATCmdSetInfo.cmpSuccess = false;
		API_ResetProtocolBuffer(USART_FUNCTION_WCDMA,RESETRTBUFF_TYPE_TX);
		API_AddProtocolBuffer(USART_FUNCTION_WCDMA,(uchar*)(_gATCmdSetInfo.SendAtCmd->ATCmdSendBuff),ReadStrLen((char*)_gATCmdSetInfo.SendAtCmd->ATCmdSendBuff));
		API_AddProtocolBuffer(USART_FUNCTION_WCDMA,_gATCmdSetInfo.sendBuffer,_gATCmdSetInfo.sendBufferSize);
		API_AddProtocolBuffer(USART_FUNCTION_WCDMA,"\r\n",2);
		API_StartSendBuffer(USART_FUNCTION_WCDMA);
		_gATCmdSetInfo.sendRepeatCount--;

	 }
	 LINE_LOG();
	 API_SetTimer(_gATCmdSetInfo.SendAtCmd->WaitTimer,1,TIMER_TYPE_WCDMA_AT_SEND,NULL,TIMER_NAME(StartToSendAtCmd));
	LINE_LOG();
}
/*定时器区域结束*/

/******************************************************************************/
//全局函数区域

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_WCDMA_StartUpadate
// 功能描述  : 开始进行升级
// 输入参数  : uf,是哪个口发送过来的数据
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_WCDMA_StartUpadate(USART_FUNCTION uf)
{
	if(uf < USART_FUNCTION_MAX)
	{
	    LINE_LOG();
		START_CMD(StartUpdate);
		ADD_ARGV_INT(uf);
		END_CMD();
		LINE_LOG();
		return 0;
	}

	return 1;
}

int AtCmdAnalysis_WCDMA_StartWcdmaCtrl(USART_FUNCTION uf)
{
	if(uf < USART_FUNCTION_MAX)
	{
		LINE_LOG();
		START_CMD(StartWcdmaCtrl);
		ADD_ARGV_INT(uf);
		END_CMD();
		LINE_LOG();
		return 0;
	}

	return 1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_UartFuntion_StartSend
// 功能描述  : 开始向功能口发送数据
// 输入参数  : uf,向哪个口发送数据
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_UartFuntion_StartSend(USART_FUNCTION uf)
{

	if(uf < USART_FUNCTION_MAX)
	{
		LINE_LOG();
		START_CMD(StartSend);
		ADD_ARGV_INT(uf);
		END_CMD();
		LINE_LOG();
		return 0;
	}
	return 1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_ATCmd_Analysis
// 功能描述  : 接收3G模块AT命令并解析处理
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int AtCmdAnalysis_ATCmdRecv_Analysis(uchar *Info,int len)
{
	 int  i = 0;
	 const tagATCmdRecvProcessItem* pItem = NULL;
	 LINE_LOG();
	 if(Info == NULL || len < 1)
	 	return -1;
	 for(i= 0; i < RECV_AT_CMD_SIZE; i++)
	 {
	 	 if(CK(Info,_gtagATCmdRecvProcessItemArray[i].ATcmd))
		 {
		 	pItem = &_gtagATCmdRecvProcessItemArray[i];
		 	break;
		 }
	 }
	 LINE_LOG();
	 if(pItem != NULL)
	 {
	 	if(pItem->fun != NULL)
		{
			int ret = pItem->fun(Info,len);
			ret = ret < 0?-1:ret;
			if(ret <= 0)
			{
				return ret;
			} 
		}
		else
		{
			return 0;
		}
	 }
	 LINE_LOG();
	 if(AT_CALL_BACK_MODE())
	 {
	 	int i =  0;
		BOOL ret = false;
		LINE_LOG();
	 	for(i = 0; i < MAX_RECV_BUFF_COUNT; i++)
		{	
			if((ret = CK(Info,_gSendAtCmd->ATCmdReceBuff[i])) == TRUE)
				break;
		}

	 	if(ret)
		{
			LINE_LOG();
			_gModeRunState.AtErrorCount = 0;
			if(_gSendAtCmd->fun != NULL)
			{
				_gSendAtCmd->fun(Info,len);

			}

  			_gATCmdSetInfo.sendRepeatCount = 0;
			CLEAR_ERROR_TYPE();
			_gATCmdSetInfo.cmpSuccess  = true;
		
			API_SetTimerFast(10L,1,TIMER_TYPE_WCDMA_AT_SEND,NULL,TIMER_NAME(StartToSendAtCmd));
			LINE_LOG();
			return 0;	 
			//应答正确
		}
		else
		{
			LINE_LOG();
			//如果前面已经判断出来有匹配的值了，下面的就不进行判断了
			if(!_gATCmdSetInfo.cmpSuccess)
			{
				for(i = 0; i < MAX_RECV_ERROR_BUFF_COUNT; i++)
				{	
					if((ret = CK(Info,_gSendAtCmd->ATCmdRecvErrBuff[i])) == TRUE)
						break;
				}
				if(ret)	//接收到指定的错误信息，就直接返回
				{
				 
					API_SetTimerFast(10L,1,TIMER_TYPE_WCDMA_AT_SEND,NULL,TIMER_NAME(StartToSendAtCmd));

				}
				SET_ERROR_TYPE(SEND_ERROR_TYPE_UNKONWN);

			}
			//重新发送命令
			//应答与预期的结果不一致
		}
		LINE_LOG(); 
		return -1;
	 }
	 LINE_LOG();
	 return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_Queue_Callback
// 功能描述  : 回调函数执行
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_Queue_Callback(void)
{
	QueuePtr qPtr = NULL;

	while((qPtr = DeQueue(PResFullQueuePtr)) != NULL)
	{
		tagCallBackQueueItem* pItem = qPtr->data;
		if( pItem != NULL)
		{
			if(pItem->callbackFun != NULL)
			{
				pItem->callbackFun(pItem->argc,pItem->argv);

			}	
		}
		//全速执行缓存直到为空为止
		EnQueue(PResFreeQueuePtr,qPtr);
	}
	Msg_BufferQueue_Process();
	AtCmdAnalysis_AtCmdRun_Process();

	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmd_RECV_WCDMA_Init
// 功能描述  : 重启模块
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmd_RECV_WCDMA_Init(void)
{
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		RUN_ATCMD_GROUP(ATCMD_RUN_TYPE_RESTART);
	}		   

	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_Net_InfoSend
// 功能描述  : 网络数据发送
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_Net_InfoSend(WORD deviceID,char* buf, WORD size)	
{
LINE_LOG();
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		LINE_LOG();
		if(deviceID < MOUNT_DEVICE_COUNT )
		{	
		 
			WORD index = ATCMD_RUN_TYPE_INFO_SEND + deviceID;
			if(_gAtCmdRunCtrlArray[index].status == ATCMDGROUP_RUN_STATUS_OFF)
			{  LINE_LOG();
				memset(_gAtCmdRunCtrlArray[index].funArg,NULL,MAX_FUN_ARG_SIZE);
				memcpy(_gAtCmdRunCtrlArray[index].funArg,buf,size);
				_gAtCmdRunCtrlArray[index].funArgSize = size;
				RUN_ATCMD_GROUP((ATCMD_RUN_TYPE)index);
			}		
		}
	}
	LINE_LOG();
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_Net_DeviceSend
// 功能描述  : 设备数据发送
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_Net_DeviceSend(WORD deviceID)		//发送设备的返回的信息
{
	LINE_LOG();
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		LINE_LOG();
		if(deviceID < MOUNT_DEVICE_COUNT )
		{
			WORD index = ATCMD_RUN_TYPE_DEVICE_SEND + deviceID;
			if(_gAtCmdRunCtrlArray[index].status == ATCMDGROUP_RUN_STATUS_OFF)
			{
				RUN_ATCMD_GROUP((ATCMD_RUN_TYPE)index);
			}		
		}
	}
	LINE_LOG();
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysisi_DisConnect
// 功能描述  : 中断某个设备连接
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysisi_DisConnect(WORD deviceID)
{
	LINE_LOG();
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		LINE_LOG();
		if(deviceID < MOUNT_DEVICE_COUNT )
		{
			LINE_LOG();
			if(_gModeRunState.NetIsOpen[deviceID] != MAX_NET_IS_OPEN_NUM
				&& 	(deviceID != _gSystemSetVal.sysSetValCtrl.mapDeviceID
				|| _gSystemSetVal.sysMountMapStatus != SYSTEM_MOUNT_MAP_STATUS_OFF
				||_gSystemSetVal.NetIsOpen < MAX_NET_IS_OPEN_NUM)
				)
			{
				LINE_LOG();
				return 0;
			}
			else
			{
				if(_gAtCmdRunCtrlArray[ATCMD_RUN_TYPE_DISCONNECT_NET].status == ATCMDGROUP_RUN_STATUS_OFF)
				{
					sprintf(_gAtCmdRunCtrlArray[ATCMD_RUN_TYPE_DISCONNECT_NET].funArg,"%d",deviceID);
					RUN_ATCMD_GROUP(ATCMD_RUN_TYPE_DISCONNECT_NET);
					LINE_LOG();
					return 0;	
				}
			}		
		}
		
	}
	LINE_LOG(); 
	return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_PhoneNum_Set
// 功能描述  : 电话号码设置
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_PhoneNum_Set(WORD deviceID,uchar* phoneNum,size_t size)
{
	LINE_LOG();
	if(size < MAX_IPADDRESS_SIZE && deviceID < MOUNT_DEVICE_COUNT)
	{
		LINE_LOG();
		memset(_gModuleSetVal[0].phonelNum,NULL, MAX_PHONE_SIZE);
		memcpy(_gModuleSetVal[0].phonelNum,phoneNum,size);
		AtCmdAnalysis_EERom_Story();
		LINE_LOG();
		return 0;
	}
	LINE_LOG();
	return -1;	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_IP_Set
// 功能描述  : IP设置
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_IP_Set(WORD deviceID,uchar* ipAddress,size_t size,int32 port)
{
	LINE_LOG();
	if(size < MAX_PHONE_SIZE && deviceID < MOUNT_DEVICE_COUNT)
	{
		LINE_LOG();
		memset(_gModuleSetVal[deviceID].IPAddress,NULL, MAX_PHONE_SIZE);
		memcpy(_gModuleSetVal[deviceID].IPAddress,ipAddress,size);
		_gModuleSetVal[deviceID].Port = port;
		AtCmdAnalysis_EERom_Story();
		AtCmdAnalysisi_DisConnect(deviceID);
		AtCmdAnalysis_Net_Connect(deviceID);
		LINE_LOG();
		return 0;
	}
	LINE_LOG();
	return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_BeatTime_Set
// 功能描述  : 心跳设置
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_BeatTime_Set(DWORD beatTime,BOOL store)
{
	LINE_LOG();
	if(beatTime >= 1 && beatTime <= 10)
	{
		int i = 0;
		for(i = 0; i < MOUNT_DEVICE_COUNT;i++)
		{
			_gModuleSetVal[i].beatTime = beatTime;
		}
		LINE_LOG();
		if(store)
		{
			AtCmdAnalysis_EERom_Story();
		}
		LINE_LOG();
		AtCmdAnalysis_DeviceBeat_ReStart();
		LINE_LOG();
		return 0;
	}
	else
	{
		LINE_LOG();
		return -1;
	}	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_Device_Fun
// 功能描述  : 设备使能
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_Device_Fun(WORD deviceID,uchar deviceFun)
{
	LINE_LOG();
	if(deviceID < MOUNT_DEVICE_COUNT && deviceFun < 3)
	{
		_gModuleSetVal[deviceID].deviceFun = deviceFun;
		AtCmdAnalysis_EERom_Story();
		if(deviceFun == 1)
		{
			LINE_LOG();
			AtCmdAnalysis_Net_Connect(deviceID);	
		}
		else
		{
			LINE_LOG();
			AtCmdAnalysisi_DisConnect(deviceID);
		}
		
	}
	LINE_LOG();
	return 0;	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_CNMP_Set
// 功能描述  : 网络模式设置
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_CNMP_Set(WORD	cnmp)
{
	LINE_LOG();
	if(cnmp < NETWORK_PRIORIYY_TYPE_MAX)
	{
		LINE_LOG();
		_gModuleSetVal[0].Cns_CNMP = (NETWORK_PRIORIYY_TYPE)cnmp;
		AtCmdAnalysis_EERom_Story();
	   AtCmd_RECV_WCDMA_Init();		//重启设备
	   LINE_LOG();
	   return 0;
	}
	LINE_LOG();
	return -1;	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_Net_Connect
// 功能描述  : 网络连接
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_Net_Connect(WORD deviceID)
{
	LINE_LOG();

	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{

		LINE_LOG();
 		if(deviceID < MOUNT_DEVICE_COUNT )
		{

			LINE_LOG();
 			if(_gModeRunState.NetIsOpen[deviceID] == MAX_NET_IS_OPEN_NUM
			&& 	(deviceID != _gSystemSetVal.sysSetValCtrl.mapDeviceID
				|| _gSystemSetVal.sysMountMapStatus != SYSTEM_MOUNT_MAP_STATUS_ON
				||_gSystemSetVal.NetIsOpen == MAX_NET_IS_OPEN_NUM)
			)
			{
				LINE_LOG();
 				return 0;
			}

			if(_gAtCmdRunCtrlArray[ATCMD_RUN_TYPE_CONNECT_NET + deviceID].status == ATCMDGROUP_RUN_STATUS_OFF)
			{
				LINE_LOG();

				if(AtCmdAnalysis_BeatString_Build(deviceID) > -1)
				{
					RUN_ATCMD_GROUP((ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_CONNECT_NET + deviceID));	
				}	
			}
		}
	}
	LINE_LOG();
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_ModeRunState_Get
// 功能描述  : 运行状态获取
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_ModeRunState_Get(tagModeRunStateStruct* runState)
{
	LINE_LOG();
	if(runState != NULL)
	{
		LINE_LOG();
		memcpy(runState,&_gModeRunState,sizeof(tagModeRunStateStruct));
		return 0;
	}
	LINE_LOG();
	return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_AlarmPhone_Get
// 功能描述  : 某个设备的参数设置获取
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
#if CONF_WARN_SEND_EN
int AtCmdAnalysis_AlarmPhone_Get(tagAlarmPhoneStruct* AlarmPhone)
{
	LINE_LOG();
	memcpy(AlarmPhone,&_gAlarmPhone,sizeof(tagAlarmPhoneStruct));
	return 0;
}
#endif
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_ModuleSetVal_Get
// 功能描述  : 某个设备的参数设置获取
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_ModuleSetVal_Get(WORD deviceID,tagModuleSetValStruct* moduleSetVal)
{
	LINE_LOG();
	if(deviceID < MOUNT_DEVICE_COUNT && moduleSetVal != NULL)
	{
		LINE_LOG();
		memcpy(moduleSetVal,&_gModuleSetVal[deviceID],sizeof(tagModuleSetValStruct));
		return 0;
	}
	LINE_LOG();
	return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_ModuleSetVal_Read
// 功能描述  : 某个设备的参数设置获取
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

const tagModuleSetValStruct* AtCmdAnalysis_ModuleSetVal_Read(WORD deviceID)
{
	LINE_LOG();
	if(deviceID < MOUNT_DEVICE_COUNT)
	{
		LINE_LOG();
		return &_gModuleSetVal[deviceID];
	}
	LINE_LOG();
	return NULL;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_Msg_Send
// 功能描述  : 短信发送
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_Msg_Send(char* phoneNum, char* buf)
{
	LINE_LOG();
	if(phoneNum != NULL && buf != NULL)
	{
		int retSize = 0;
		strcpy(_gATCmdSetInfo.sendPhoneNum,phoneNum);
		retSize = Msg_Buffer_UTF8ToPDU(buf,strlen(buf),_gATCmdSetInfo.msgBuffer,sizeof(_gATCmdSetInfo.msgBuffer)-1);
		LINE_LOG();
		if(retSize > 0)
		{
			LINE_LOG();
			//_gATCmdSetInfo.msgBuffer[retSize] = 0x1a;
			_gATCmdSetInfo.msgBuffer[retSize] = 0x0;
			RUN_ATCMD_GROUP(ATCMD_RUN_TYPE_SEND_MSG);	
		}
		//sprintf(_gATCmdSetInfo.msgBuffer,"%s%c",buf,0x1a);
		LINE_LOG();
		return 0;
	}
	LINE_LOG();
	return -1;	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_Msg_Send_FromDevice
// 功能描述  : 短信发送
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_Msg_Send_FromDevice(WORD deviceID, char* buf)
{
	LINE_LOG();
	if(deviceID < MOUNT_DEVICE_COUNT)
	{
		char phoneNum[90] = {0};
		int retSize = 0;
		phoneNum[0] = '\"';
		retSize = Msg_Buffer_UTF8ToPDU(_gModuleSetVal[deviceID].phonelNum,strlen(_gModuleSetVal[deviceID].phonelNum),&phoneNum[1],80);
		if(retSize > 0)
		{
			LINE_LOG();
			phoneNum[retSize+1] = '\"';
			return AtCmdAnalysis_Msg_Send(phoneNum,buf);
		}
	}
	LINE_LOG();
	return -1;	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_Msg_Send_Direct
// 功能描述  : 短信直接发送，不必进行转码工作
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_Msg_Send_Direct(char* phoneNum, char* buf)
{
	LINE_LOG();
	if(phoneNum != NULL && buf != NULL)
	{
		int retSize = 0;
		strcpy(_gATCmdSetInfo.sendPhoneNum,phoneNum);
		retSize = strlen(buf);
		LINE_LOG();
		if(retSize > 0 && retSize < 600)
		{
			strcpy(_gATCmdSetInfo.msgBuffer,buf);
			_gATCmdSetInfo.msgBuffer[retSize] = 0x1a;
			RUN_ATCMD_GROUP(ATCMD_RUN_TYPE_SEND_MSG);	
		}
		//sprintf(_gATCmdSetInfo.msgBuffer,"%s%c",buf,0x1a);
		LINE_LOG();
		return 0;
	}
	return -1;	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_Init
// 功能描述  : 系统的初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_Init(void)
{
	LINE_LOG();
	AtCmdAnalysis_EERom_Read();
	AtCmdAnalysis_RunStatus_Init();

	AtCmd_Init(_gModuleSetVal, MOUNT_DEVICE_COUNT,&_gModeRunState,&_gATCmdSetInfo,_gLinkQueue,_gAtCmdRunCtrlArray,&_gSystemSetVal);
	LINE_LOG();
	ATCmdRecv_Queue_Init();
	ATCmdRecv_SendBufferQueue_Init();
	Netdata_Init();
	Msg_Init();
	API_SetOutsideAnalysisFun(USART_FUNCTION_WCDMA,AtCmdAnalysis_Protocol_Analysis);
	AtCmdAnalysis_ATCmdRunCtrlArray_Init();
	API_SetTimer(3000,-1,TIMER_TYPE_WCDMA_ATCMD_PROCESS_LOOP,TIMER_NAME(MainLoop),NULL);
	AtCmdAnalysis_BeatTime_Set(_gModuleSetVal[0].beatTime,false);
	API_SetTimer(60000,-1,TIMER_TYPE_WCDMA_AT_RESTART,TIMER_NAME(StartOk),NULL);
	DEBUG_RECORD("Start OK!");
	LINE_LOG();
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_CID_Set
// 功能描述  : CID设置 
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_CID_Set(WORD deviceID,uchar* cid,size_t size)
{
	LINE_LOG();
	if(size < MAX_IPADDRESS_SIZE && deviceID < MOUNT_DEVICE_COUNT)
	{	
		memset(_gModuleSetVal[deviceID].CID,NULL, MAX_CID_SIZE);
		memcpy(_gModuleSetVal[deviceID].CID,cid,size);
		AtCmdAnalysis_EERom_Story();
		LINE_LOG();
		return AtCmdAnalysis_BeatString_Build(deviceID);
	}
	LINE_LOG();
	return -1;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_AlarmPhone_Set
// 功能描述  : 报警号码设置 
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
#if CONF_WARN_SEND_EN
int AtCmdAnalysis_AlarmPhone_Set(tagAlarmPhoneStruct* val)
{
	LINE_LOG();
	memcpy(&_gAlarmPhone,val,sizeof(tagAlarmPhoneStruct));
	LINE_LOG();
	AtCmdAnalysis_WarnPhone_EERom_Story();
//	AtCmd_RECV_WCDMA_Init();		//重启设备
//	十秒后重启模块
	API_SetTimer(10000L,1,TIMER_TYPE_WCDMA_DELAY_RESTART,NULL,TIMER_NAME(DelayStart));	
	LINE_LOG();	
	return 0;
}
#endif
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_ModuleSetVal_Set
// 功能描述  : 设备参数的设置 
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_ModuleSetVal_Set(tagModuleSetValStruct val[MOUNT_DEVICE_COUNT])
{
	int i = 0;
	LINE_LOG();
	for(i = 0; i < MOUNT_DEVICE_COUNT; i++)
	{
		memcpy(&_gModuleSetVal[i],&val[i],sizeof(tagModuleSetValStruct));
	}
	LINE_LOG();
	AtCmdAnalysis_EERom_Story();
//	AtCmd_RECV_WCDMA_Init();		//重启设备
	//十秒后重启模块
	API_SetTimer(10000L,1,TIMER_TYPE_WCDMA_DELAY_RESTART,NULL,TIMER_NAME(DelayStart));

	
	LINE_LOG();	
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_ErrorMsg
// 功能描述  : 报错误信息 
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_ErrorMsg(WCDMA_ERROR errType,char* buf,size_t size)
{
	int i = 0;
	LINE_LOG();
	for(i = 0; i < ERROR_CALL_BACK_SIZE; i++)
	{
		if(errType == _gErrorCallBack[i].errorType)
		{
			if(_gErrorCallBack[i].fun != NULL)
			{
				_gErrorCallBack[i].fun(buf,size);
				break;
			}
		}
	}
	LINE_LOG();
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_AtCmd_Send
// 功能描述  : 发送AT命令数据
// 输入参数  : CmdNo,命令编号; Info,命令附加数据内容; len,附加数据长度;
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_AtCmd_Send(uint8 cmd,char *buf,int len)
{
	tagSendBufferQueueItem* pItem = NULL;
	QueuePtr QPtr = NULL;
	int index = -1;
	LINE_LOG();
	index =  AtCmdAnalysis_AtCmd_GetIndex(cmd);
	if(index < 0 && len > MAX_SEND_BUFFER_SIZE)
		return -1;
	LINE_LOG();
	if((QPtr = DeQueue(PResFreeSendBufferQueuePtr)) == NULL )
	{
		DEBUG("Error for Queue!\n");
		return -1;
	}
	pItem = QPtr->data;
	pItem->SendAtCmd = &_gATCmd[index];
	if(buf != NULL)
	{
   		memcpy(pItem->sendBuffer,buf,len);
		pItem->sendBufferSize = len;
   	}
	pItem->sendBufferType = SEND_BUFFER_TYPE_COMMON;
	EnQueue(PResFullSendBufferQueuePtr,QPtr);
	LINE_LOG();
	return 0;			

}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_AtCmd_SendHugeBuffer
// 功能描述  : 发送网络大数据包
// 输入参数  : cmd,命令编号; hugeBuffer,大数据报描述结构体;
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int AtCmdAnalysis_AtCmd_SendHugeBuffer(uint8 cmd,tagHugeSendBuffer hugeBuffer)
{
	tagSendBufferQueueItem* pItem = NULL;
	QueuePtr QPtr = NULL;
	int index = -1;
	LINE_LOG();
	index =  AtCmdAnalysis_AtCmd_GetIndex(cmd);
	if(index < 0)
		return -1;
	LINE_LOG();
	if((QPtr = DeQueue(PResFreeSendBufferQueuePtr)) == NULL )
		return -1;
	LINE_LOG();
	pItem = QPtr->data;
	pItem->SendAtCmd = &_gATCmd[index];
   	pItem->sendBufferType = SEND_BUFFER_TYPE_HUGE;
	pItem->hugeBuffer =  hugeBuffer;
	LINE_LOG();
	EnQueue(PResFullSendBufferQueuePtr,QPtr);
	LINE_LOG();
	return 0;			

}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_DeviceBeat_ReStart
// 功能描述  : 重启心跳
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int AtCmdAnalysis_DeviceBeat_ReStart(void)
{
	LINE_LOG();
	return API_SetTimer(_gModuleSetVal[0].beatTime *  30 * 1000,-1,TIMER_TYPE_WCDMA_HEART_BEAT,TIMER_NAME(DeviceBeat),NULL);	
}

int AtCmdAnalysis_DeviceBeat_ReStartByDeviceID(int deviceID,int tryCount)
{
	LINE_LOG();
	if(deviceID < MOUNT_DEVICE_COUNT && deviceID >=0)
	{
		_gModuleSetVal[deviceID].tryCount = tryCount;
		_gAtCmdRunCtrlArray[ATCMD_RUN_TYPE_INFO_SEND + deviceID].status = ATCMDGROUP_RUN_STATUS_OFF;
		return API_SetTimer(_gModuleSetVal[0].beatTime *  30 * 1000,-1,TIMER_TYPE_WCDMA_HEART_BEAT,TIMER_NAME(DeviceBeat),NULL);	
	}
	return -1;	
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_AtCmd_Resgister
// 功能描述  : 注册ATCMD解析体 
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int AtCmdAnalysis_AtCmd_Resgister(const tagATCmd* ATCmd,	 //ATCmd解析体
									int ATCmdCount,	//解析体命令数量
									const tagATCmdRecvProcessItem*	ATCmdRecvProcessItemArray,	//关键字解析体
									int ATCmdRecvProcessItemSize,	//关键解析体数量
									const tagAtCmdRunCtrlItem* AtCmdRunFunTypeArray
									)
{
	if(ATCmd == NULL || ATCmdRecvProcessItemArray == NULL)
	{
		return -1;
	}
	_gATCmd = ATCmd;
	_gATCmdCount = ATCmdCount;
	_gtagATCmdRecvProcessItemArray = ATCmdRecvProcessItemArray;
	_gATCmdRecvProcessItemSize = ATCmdRecvProcessItemSize;
	_gAtCmdRunFunTypeArray	 = AtCmdRunFunTypeArray;
	LINE_LOG();
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_SetATCmdSendCallBack
// 功能描述  :  设置组运行成功后的回调函数 
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int AtCmdAnalysis_SetATCmdSendCallBack(ATCmdSendCallBack fun)
{
	LINE_LOG();
	_gATCmdSetInfo.aTCmdSendCallBack = fun;
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_RunGroup
// 功能描述  : 运行组 
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int AtCmdAnalysis_RunGroup(ATCMD_RUN_TYPE runType)
{
	LINE_LOG();
	__disable_irq();
	 if(_gAtCmdRunCtrlArray[runType].status == ATCMDGROUP_RUN_STATUS_OFF
	  && !_gAtCmdRunCtrlArray[runType].lock)
	 {
	 	_gAtCmdRunCtrlArray[runType].status = ATCMDGROUP_RUN_STATUS_ON;
	 	_gAtCmdRunCtrlArray[runType].lock = true;
		__enable_irq();
		return 0;
	 }
	 __enable_irq();
	 LINE_LOG();
	return -1;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_SelectAnlysisMode
// 功能描述  : 选择分析模式 
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int AtCmdAnalysis_SelectAnlysisMode(PROTOCOL_ANALYSIS_MODE mode,int size)
{
	LINE_LOG();
	_gATCmdSetInfo.protocolAnlysisMode = mode;
	_gATCmdSetInfo.RecvSize = size;
	API_SetTimerFast(10000L,1,TIMER_TYPE_WCDMA_ANLYSIS_MODE,NULL,AtCmdAnalysis_AtCmd_EndAnlysisMode);
	LINE_LOG();
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_WirelessModuleVision_Get
// 功能描述  : 获取无线模块版本号
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
char* AtCmdAnalysis_WirelessModuleVision_Get(void)
{
	return _gATCmdSetInfo.wirelssModuleVision;	
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_CarrierOperator_Get
// 功能描述  : 获取运营商
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
char* AtCmdAnalysis_CarrierOperator_Get(void)
{
	return _gATCmdSetInfo.carrierOperator;	
}	

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_RunError_EEROM_Record
// 功能描述  : 实时错误记录
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_RunError_EEROM_Record(char* buf)
{
	tagErrorEERomContext menu;
	const int cellSize = 32;	//一个单元的尺寸
	const int maxSize = 30;	//32个字节为一个单元,一个校验位	,预留一位
	int size = 0;
	DWORD	address;
	if(buf == NULL)
		return -1;
	LINE_LOG();
	size = 	strlen(buf);

	size = size > maxSize?maxSize:size;

	if(API_EEPROM_Read(_EEP_ADDR_RUN_ERROR_RECORD_CONTEXT, sizeof(tagErrorEERomContext), (uchar*)(&menu), true) < 0)
	{
		memset(&menu,NULL,sizeof(menu));
	}
	if(menu.writePointer > menu.size)
	{
		menu.writePointer = 0;	
	}
	address = _EEP_ADDR_RUN_ERROR_RECORD + cellSize * menu.writePointer;

	if(API_EEPROM_Write(address,maxSize,(uchar*)buf,true) > -1)
	{
		if(menu.size < MAX_RUNERROR_CODE_SIZE)
		{
			menu.size++;
		}
		menu.writePointer++;
		if(menu.writePointer >= MAX_RUNERROR_CODE_SIZE)
		{
			menu.writePointer = 0;	
		}

		API_EEPROM_Write(_EEP_ADDR_RUN_ERROR_RECORD_CONTEXT, sizeof(tagErrorEERomContext), (uchar*)(&menu), true);
		LINE_LOG();
		return 0;
	}
		LINE_LOG();
	 return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_SystemSetValCtrl_Get
// 功能描述  : 获取后台控制设置
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
tagSystemSetValStruct*	AtCmdAnalysis_SystemSetValCtrl_Get(void)
{
	return (&_gSystemSetVal);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_CnsMod_Init
// 功能描述  : APN和网络模式初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int AtCmdAnalysis_CnsMod_Init(tagCnsModSet* mod, int size)
{
	uint EepAddr;
	char m[10*MAX_APN_SIZE];
	int i = 0;
	EepAddr =  _EEP_ADDR_APN_VAL;
	LINE_LOG();
	_gCnsMod = mod;
	_gCnsModSize = size;
	
	if(API_EEPROM_Read(EepAddr,MAX_APN_SIZE * size,(uchar*)m,true) >=0)
	{
		for(i = 0; i < _gCnsModSize; i++)
		{
			//改变APN的值
			memcpy(_gCnsMod[i].apn,&m[MAX_APN_SIZE * i],MAX_APN_SIZE);
		
		}
	}

	LINE_LOG();					

	Msg_CnsModInit(_gCnsMod,_gCnsModSize);
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_CnsMod_EERom_Story
// 功能描述  : APN和网络模式存储
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int AtCmdAnalysis_CnsMod_EERom_Story(void)
{
	uint EepAddr;
	char m[10*MAX_APN_SIZE];
	int i = 0;
	EepAddr =  _EEP_ADDR_APN_VAL;

	for(i = 0; i < _gCnsModSize; i++)
	{
		//改变APN的值
		memcpy(&m[MAX_APN_SIZE * i],_gCnsMod[i].apn,MAX_APN_SIZE);		
	
	}
	return API_EEPROM_Write(EepAddr,MAX_APN_SIZE * _gCnsModSize,(uchar*)m,true);

}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_CnsMod_Get
// 功能描述  : APN和网络模式获取
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int AtCmdAnalysis_CnsMod_Get(tagCnsModSet** mod, int* size)
{
	*mod = _gCnsMod ;
	*size = _gCnsModSize;
	return 0;
}
int AtCmdAnslysis_3GNetVersion_Set(int mode)
{
	char* buf = NULL;
	WORD index = ATCMD_RUN_TYPE_SET3GNETVERSION;
	WORD size;
	switch(mode)
	{
	case 0:
		buf ="\"00\"";
		break;
	case 1:
		buf = "\"01\"";
		break;
	default:	
		return -1;;
	}
	size = ReadStrLen(buf);
	size = size > MAX_FUN_ARG_SIZE? MAX_FUN_ARG_SIZE:size;
	memset(_gAtCmdRunCtrlArray[index].funArg,NULL,MAX_FUN_ARG_SIZE);
	memcpy(_gAtCmdRunCtrlArray[index].funArg,buf,size);
	_gAtCmdRunCtrlArray[index].funArgSize = size;
	RUN_ATCMD_GROUP((ATCMD_RUN_TYPE)index);

	return 0;
}

int AtCmdAnslysis_MsgAnalysis99(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
{
	
	return Msg_Buffer_Analysis99(uf,DaPtr,len);
}
/*分析9A协议*/
int AtCmdAnslysis_MsgAnalysis9A(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
{

	
	return Msg_Buffer_Analysis9A(uf,DaPtr,len);
}
