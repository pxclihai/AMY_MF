#ifndef AT_CMD_ANALYSIS_H
#define   AT_CMD_ANALYSIS_H

#define 	MAX_RECV_BUFF_COUNT				2			//应答正确的正确协议个数
#define 	MAX_RECV_ERROR_BUFF_COUNT		1			//应答错误的协议个数
#define 	MAX_MSG_PHONE_SIZE				80			//短信电话号码最大长度
#define 	MAX_CALL_BACK_PARAMETER_COUNT 	4			//回调函数参数最大个数
#define		MAX_CALL_BACK_PARAMETER_SIZE	50			//回调函数参数大小
#define 	MAX_CID_SIZE					20			//CID大小
#define 	MAX_IPADDRESS_SIZE				20			//IP地址长度
#define 	MAX_PHONE_SIZE					20			//告警电话号码长度
#define 	MAX_BEATSTRING_SIZE				100			//心跳字符串长度
#define 	MAX_CARRIEROPERATOR_NAME_SIZE	50			//运营商名字长度
#define		MAX_APN_SIZE						20			//APN名长度


#define 	MAX_HUGE_BUFFER_SIZE			5			//发送大数据时参数最大个数
#define 	MAX_SEND_DATA_SIZE_ONCE			960			//单次发送文件数据大小

#define 	MAX_SEND_BUFFER_SIZE			1124		//发送字符缓存
#define 	MAX_AT_CMD_ERROR_COUNT			20			//最大ErrCode记录
#define 	MAX_FUN_ARG_SIZE				100			//参数尺寸

#define 	MAX_ERROR_CODE_SIZE				15			//错误记录个数
#define 	MAX_RUNERROR_CODE_SIZE			31			//实时错误记录个数

/******************************************************************************/

typedef int (*ATCmdRecvProcessFun)(uchar *Info,int len);
typedef int (*CallBackFun)(int argc, char argv[][MAX_CALL_BACK_PARAMETER_SIZE]);
typedef int(*ErrorProcFun)(char* addr, size_t size);
typedef int (*ATCmd_Run_Fun)(void);



/******************************************************************************/

typedef enum
{
	NETWORK_PRIORIYY_TYPE_AUTO,		 //自动选择
	NETWORK_PRIORIYY_TYPE_1G_2G,	//1,2G优先
	NETWORK_PRIORIYY_TYPE_2G_3G,	//2,3G优先
	NETWORK_PRIORIYY_TYPE_MAX		
}NETWORK_PRIORIYY_TYPE;

typedef enum
{
	SIM_CARD_STATUS_NO_DETECT,
	SIM_CARD_STATUS_OK,
	SIM_CARD_STATUS_ERROR
}SIM_CARD_STATUS;

typedef enum {
	PROTOCOL_ANALYSIS_MODE_NONE,			//不自行解析
	PROTOCOL_ANALYSIS_MODE_SHORT_MESSAGE,	//短信
	PROTOCOL_ANALYSIS_MODE_RECEIVE			//接收网络数据
}PROTOCOL_ANALYSIS_MODE; 

typedef enum
{
	ATCMD_RUN_TYPE_RESTART,		//设备重启，优先级最高
	ATCMD_RUN_TYPE_GET_MODULE_INFO,	//获取模块信息,包括运营商之类的
	ATCMD_RUN_TYPE_INIT,		//设备初始化,优先级次高的	
	ATCMD_RUN_TYPE_SET3GNETVERSION,	//设置3G的版本号
	ATCMD_RUN_TYPE_READ_MSG,	//读取短信
	ATCMD_RUN_TYPE_SEND_MSG,	//发送短信
	ATCMD_RUN_TYPE_DISCONNECT_NET,	//断开网络
	ATCMD_RUN_TYPE_CONNECT_NET,	//连接网络
	ATCMD_RUN_TYPE_DEVICE_SEND = ATCMD_RUN_TYPE_CONNECT_NET+ MOUNT_DEVICE_COUNT,	//设备信息发送
	ATCMD_RUN_TYPE_INFO_SEND = ATCMD_RUN_TYPE_DEVICE_SEND + MOUNT_DEVICE_COUNT,	//发送短信息，少于100个字节
	ATCMD_RUN_TYPE_LOOP = ATCMD_RUN_TYPE_INFO_SEND + MOUNT_DEVICE_COUNT,		//轮询查询,优先级最低的
	ATCMD_RUN_TYPE_MAX			//ATCMD运行方式数量		
}ATCMD_RUN_TYPE;

typedef enum
{
	ATCMD_RUN_STATUS_NOT_RUN,	//未运行
	ATCMD_RUN_STATUS_RUN_BUSY,		//正在运行指令
	ATCMD_RUN_STATUS_RUN_FREE,		//指令运行完毕
	ATCMD_RUN_STATUS_MAX		//运行状态
}ATCMD_RUN_STATUS;	//ATCMD可运行状态

typedef enum
{
	WCDMA_ERROR_CHECK,		//模块检测
	WCDMA_ERROR_INIT,		//初始化没成功
	WCDMA_ERROR_DEVICE,		//设备报的错误
	WCDMA_ERROR_SYSTEM,			//系统崩溃
	WCDMA_ERROR_NO_SIM_CARD,	//没有SIM卡
	WCDMA_ERROR_NET_DISCONNECT,	//网络连接不上
	WCDMA_ERROR_MODULE_RESTART,	//模块自动重启了
	WCDMA_ERROR_MAX
}WCDMA_ERROR; 

typedef enum
{
	SEND_BUFFER_TYPE_COMMON,
	SEND_BUFFER_TYPE_HUGE
}SEND_BUFFER_TYPE;

typedef enum
{
	SEND_ERROR_TYPE_NONE	= 0x0000,	//没有任何错误
	SEND_ERROR_TYPE_UNKONWN	= 0x0001,	//未知原因导致的错误
}SEND_ERROR_TYPE;

typedef enum
{
	ATCMDGROUP_RUN_STATUS_ON,
	ATCMDGROUP_RUN_STATUS_OFF,
	ATCMDGROUP_RUN_STATUS_ALWAYSON 	
}ATCMDGROUP_RUN_STATUS;

/*系统映射口状态*/
typedef enum
{
	SYSTEM_MOUNT_MAP_STATUS_OFF,	//没处于map状态
	SYSTEM_MOUNT_MAP_STATUS_ON,		//处于map状态
	SYSTEM_MOUNT_MAP_STATUS_MAX		//map状态
}SYSTEM_MOUNT_MAP_STATUS;

/******************************************************************************/
typedef struct
{
	uint8	 ATCmd;								//ATCmd	
	uint8*   ATCmdSendBuff/*[MaxATCmdProlLen]*/;	//发送协议头缓存
	uint8*   ATCmdReceBuff[MAX_RECV_BUFF_COUNT]/*[MaxATCmdProlLen]*/;	//接收协议头缓存
	uint8*	 ATCmdRecvErrBuff[MAX_RECV_ERROR_BUFF_COUNT];				//接收错误协议头缓存
	uint16   WaitTimer;							//等待应答命令时间
	uint8    RepeatSendCnt;                     //失败重发命令次数
	ATCmdRecvProcessFun	fun;					//接收成功后响应函数
	BOOL callBackIfOk;							//接收成功后是否调用立刻返回	
}tagATCmd;	//CDMA命令结构

typedef  int(* ATCmdSendCallBack)(const tagATCmd* atCmd,WORD errType);
		
typedef struct
{
	char*	ATcmd;	
	ATCmdRecvProcessFun	fun;
}tagATCmdRecvProcessItem;

typedef struct
{
	CallBackFun callbackFun;		//回调函数
	int	argc;		//参数个数 
	/*参数数组*/
	char argv[MAX_CALL_BACK_PARAMETER_COUNT][MAX_CALL_BACK_PARAMETER_SIZE];

}tagCallBackQueueItem;

typedef __packed struct
{
	char  CID[MAX_CID_SIZE];				//CID编号
	char  phonelNum[MAX_IPADDRESS_SIZE]; 	//本机号码   
	uchar BeatTime;        					//向远程中心发送心跳间隔时间(分)	   
	char  IPAddress[MAX_PHONE_SIZE];   		//远程中心IP地址
	int32 Port;            					//远程中心端口号	
	BOOL SendRunValFlag;  					//是否向此端口下的采集口发送运行参数
	BOOL OpenFlag;	  	   					//是否开启自动连接 =false,不开启 =true,开启
	NETWORK_PRIORIYY_TYPE Cns_CNMP;	 		//优先当前网络配置
	char beatString[MAX_BEATSTRING_SIZE];	//向中心发送的心跳数据
	int tryCount;							//实验次数	
	DWORD beatTime;							//心跳时间 
	uchar deviceFun;						//设备功能
}tagModuleSetValStruct;
#if CONF_WARN_SEND_EN
typedef __packed struct
{
	BOOL		Warn_Function_Enable;
	char 	phonelNum1[MAX_IPADDRESS_SIZE]; 	//告警号码   
	char 	phonelNum2[MAX_IPADDRESS_SIZE]; 	//告警号码   
	char 	phonelNum3[MAX_IPADDRESS_SIZE]; 	//告警号码   
	char 	phonelNum4[MAX_IPADDRESS_SIZE]; 	//告警号码   
	char 	phonelNum5[MAX_IPADDRESS_SIZE]; 	//告警号码   
	char 	phonelNum6[MAX_IPADDRESS_SIZE]; 	//告警号码 
	char 	Warning_type[60];

}tagAlarmPhoneStruct;
#endif
typedef __packed struct
{
	WORD  mapDeviceID;							//映射的设备ID
	WORD  controlPort;							//设置用于连接的采集口,不能与现有的采集口一致
//	char  CID[MAX_CID_SIZE];					//CID编号
	char  IPAddress[MAX_IPADDRESS_SIZE];   			//远程中心IP地址
	int32 Port;            						//远程中心端口号
	DWORD  timeOut;								//超时恢复时间，单位毫秒
//	uchar deviceFun;							//设备功能
}tagSystemSetValCtrlStruct;

typedef __packed struct
{
	tagSystemSetValCtrlStruct sysSetValCtrl;	//值控制结构体
	SYSTEM_MOUNT_MAP_STATUS sysMountMapStatus;	//映射状态
	int16 NetIsOpen;  	   						//网络连接
}tagSystemSetValStruct;

typedef __packed struct
{
	int16 CSQ;             //信号强度
	int16 NetIsOpen[MOUNT_DEVICE_COUNT];  	   //网络连接
	int16 CnsMod;	   	   //网络模式
	SIM_CARD_STATUS SimCard;			//SIMcard是否ok
	int16 AtErrorCount;					//错误计数器，当AT命令连续出错达到指定值时，自动重启
	BOOL	sendLock;					//发送锁
}tagModeRunStateStruct;   	   //无线模块运行参数结构

typedef struct
{
	const tagATCmd*	SendAtCmd; 	//AT命令发送
	uint8		sendRepeatCount;	//失败后重发次数
	uchar	sendBuffer[MAX_SEND_BUFFER_SIZE];		//待发送的AT命令附加数据
 	int		sendBufferSize;			//待发送AT命令附加数据的长度

	char	phoneNum[MAX_MSG_PHONE_SIZE];			//待接收短息的发送的手机号码
	char	msgBuffer[600];			//待发送短信的缓冲
	char	sendPhoneNum[MAX_MSG_PHONE_SIZE];		//待发送短信的电话号码
	PROTOCOL_ANALYSIS_MODE	protocolAnlysisMode;	//解析模式
	USART_FUNCTION	RecvFunction; 	//接收的端口
	int RecvSize;				//接收数据的大小
	BOOL cmpSuccess;			//匹配成功	
	ATCMD_RUN_TYPE atCmdGroup;		//当前运行的atCmdGroup

	ATCmdSendCallBack aTCmdSendCallBack;	//如果指令组为空就执行该条指令 
	WORD errBits;				//错误发送的次数
	
	ATCMD_RUN_STATUS atCmdRunStatus;		//AT命令可运行的状态
	BOOL	haveShortMsg;
	int 	apnStatus;							//APN设置与实际不符
	char	timeStr[256];
	char	wirelssModuleVision[50];		//模块版本号
	char 	carrierOperator[MAX_CARRIEROPERATOR_NAME_SIZE];			//运营商		
}tagATCmdSetInfo;
 
typedef struct
{
	WCDMA_ERROR	errorType;
	ErrorProcFun fun;	
}tagErrorCallBack;

typedef __packed struct
{
	uchar writePointer;
	uchar size;
}tagErrorEERomContext;

typedef struct
{
	int Count;
	char* Buffer[MAX_HUGE_BUFFER_SIZE];
	uint16_t size[MAX_HUGE_BUFFER_SIZE];
}tagHugeSendBuffer;

typedef struct
{
   const tagATCmd*	SendAtCmd; 		//AT命令发送
   uchar	sendBuffer[100]; 		//待发送的扩展数据,长度需要匹配心跳数据长度
   int 		sendBufferSize;			//待发送的扩展数据长度
   SEND_BUFFER_TYPE sendBufferType;	//发送数据类型，大数据，普通数据
   tagHugeSendBuffer hugeBuffer;	//大数据描述结构体

}tagSendBufferQueueItem;

typedef struct
{
	ATCMD_RUN_TYPE	type;
	ATCmd_Run_Fun fun;	//待运行的程序
	char	funArg[MAX_FUN_ARG_SIZE];		//待运行程序的参数
	WORD	funArgSize;						//参数大小
	ATCMDGROUP_RUN_STATUS status;	//运行的状态
	BOOL lock;		//状态锁	
}tagAtCmdRunCtrlItem;

typedef __packed struct
{
	uchar index;
	char* CnsModStr;
	char apn[MAX_APN_SIZE];
}tagCnsModSet;

/*自定义APN开启 */
typedef __packed struct
{
	BOOL enable;			//允许自定义APN
	char apnName[20];		//APN名字
	//BOOL enableProxy;		//开启代理
	char proxy[MAX_IPADDRESS_SIZE];			//代理服务器地址
	unsigned short port;	//代理服务器端口
	//BOOL enableUserCheck;	//开启用户验证
	char username[20];		//用户名
	char password[20];		//密码
}tagAPNCustom;

/******************************************************************************/

#define START_CMD(fun)	do{	\
							QueuePtr qCmdPtr = NULL;  	\
							int argc = 0;		 \
							tagCallBackQueueItem* pItem;	\
							char buf[20] = {0};	\
							if((qCmdPtr = DeQueue(PResFreeQueuePtr)) == NULL)	break;	\
							pItem = qCmdPtr->data;	\
							pItem->callbackFun = PROCESS_NAME(fun);

#define ADD_ARGV(arg1)		if(argc < MAX_CALL_BACK_PARAMETER_COUNT)	\
							{	\
								strcpy(pItem->argv[argc],arg1);	\
								argc++;	\
								pItem->argc = argc;	\
							}

#define ADD_ARGV_INT(arg1)	sprintf(buf,"%d",arg1);	\
							ADD_ARGV(buf)							

#define ADD_ARGV_BOOL(arg1)	ADD_ARGV_INT(arg1)

#define END_CMD()			if(argc == 0)	\
								buf[0] = buf[1];	\
							EnQueue(PResFullQueuePtr,qCmdPtr);	\
						}while(0);

/******************************************************************************/

#define ATCMDGROUP_NAME(fun) 	  ATCmdGroup_##fun
#define ATCMDGROUP_FUN(fun)	 int ATCMDGROUP_NAME(fun)##(void)

#define ATCMDGROUP_CALLBACK_NAME(fun) 	  ATCmdGroupCallBack_##fun
#define ATCMDGROUP_CALLBACK_FUN(fun)  int ATCMDGROUP_CALLBACK_NAME(fun)(const tagATCmd* atCmd,WORD errType)

#define PROCESS_NAME(fun) 	  ATCmdRecv_Process_##fun
#define ATCMDRECV_PROCESS(fun)	static int PROCESS_NAME(fun)##(int argc, char	argv[][MAX_CALL_BACK_PARAMETER_SIZE])

#define ATCMDRECV_NAME(fun) 	  ATCmdRecv_##fun
#define ATCMDRECV_FUN(fun)	static int ATCMDRECV_NAME(fun)##(uchar *Info,int len)


/******************************************************************************/
TIMER_FUN(StartToSendAtCmd);
/*定时器函数，用于向中心发送心跳数据*/
TIMER_FUN(DeviceBeat);

TIMER_FUN(StartOk);

TIMER_FUN(MainLoop);

TIMER_FUN(DelayStart);

TIMER_FUN(SystemMap);

//TIMER_FUN(SystemReStart);

/******************************************************************************/

int AtCmdAnalysis_Init(void);
int AtCmdAnalysis_ATCmdRecv_Analysis(uchar *Info,int len);
int AtCmdAnalysis_Queue_Callback(void);
//在主函数内启动功能函数的发送功能
int AtCmdAnalysis_WCDMA_StartWcdmaCtrl(USART_FUNCTION uf);
int AtCmdAnalysis_WCDMA_StartUpadate(USART_FUNCTION uf);
int AtCmdAnalysis_UartFuntion_StartSend(USART_FUNCTION uf);
int AtCmdAnalysis_Net_Connect(WORD deviceID);
int AtCmdAnalysis_Net_DeviceSend(WORD deviceID);
int AtCmdAnalysis_Net_InfoSend(WORD deviceID,char* buf, WORD size);
int AtCmdAnalysisi_DisConnect(WORD deviceID);
int AtCmdAnalysis_PhoneNum_Set(WORD deviceID,uchar* phoneNum,size_t size);
int AtCmdAnalysis_CID_Set(WORD deviceID,uchar* phoneNum,size_t size);
int AtCmdAnalysis_IP_Set(WORD deviceID,uchar* ipAddress,size_t size,int32 port);

int AtCmdAnalysis_ErrorMsg(WCDMA_ERROR errType,char* buf,size_t size);
int AtCmdAnalysis_ModeRunState_Get(tagModeRunStateStruct* runState);
int AtCmdAnalysis_ModuleSetVal_Get(WORD deviceID,tagModuleSetValStruct* moduleSetVal);
#if CONF_WARN_SEND_EN
int AtCmdAnalysis_AlarmPhone_Get(tagAlarmPhoneStruct* AlarmPhone);
#endif
const tagModuleSetValStruct* AtCmdAnalysis_ModuleSetVal_Read(WORD deviceID);


int AtCmdAnalysis_Msg_Send(char* phoneNum, char* buf);
int AtCmdAnalysis_Msg_Send_Direct(char* phoneNum, char* buf);
int AtCmdAnalysis_Msg_Send_FromDevice(WORD deviceID, char* buf);

int AtCmdAnalysis_BeatTime_Set(DWORD beatTime,BOOL store);
int AtCmdAnalysis_Device_Fun(WORD deviceID,uchar deviceFun);
int AtCmdAnalysis_CNMP_Set(WORD	cnmp);
int AtCmd_RECV_WCDMA_Init(void);
int AtCmdAnalysis_ModuleSetVal_Set(tagModuleSetValStruct val[MOUNT_DEVICE_COUNT]);
#if CONF_WARN_SEND_EN
int AtCmdAnalysis_AlarmPhone_Set(tagAlarmPhoneStruct val[MOUNT_DEVICE_COUNT]);
#endif
int AtCmdAnalysis_DeviceBeat_ReStartByDeviceID(int deviceID,int tryCount);
int AtCmdAnalysis_DeviceBeat_ReStart(void);
int AtCmdAnalysis_AtCmd_Send(uint8 cmd,char *buf,int len);
int AtCmdAnalysis_AtCmd_SendHugeBuffer(uint8 cmd,tagHugeSendBuffer hugeBuffer);

int AtCmdAnalysis_AtCmd_Resgister(const tagATCmd* ATCmd,	 //ATCmd解析体
									int ATCmdCount,	//解析体命令数量
									const tagATCmdRecvProcessItem*	ATCmdRecvProcessItemArray,	//关键字解析体
									int ATCmdRecvProcessItemSize,	//关键解析体数量
									const tagAtCmdRunCtrlItem* AtCmdRunFunTypeArray
									);
int AtCmdAnalysis_RunGroup(ATCMD_RUN_TYPE runType);
int AtCmdAnalysis_SelectAnlysisMode(PROTOCOL_ANALYSIS_MODE mode,int size);
int AtCmdAnalysis_SetATCmdSendCallBack(ATCmdSendCallBack fun);
char* AtCmdAnalysis_WirelessModuleVision_Get(void);
char* AtCmdAnalysis_CarrierOperator_Get(void);
int AtCmdAnalysis_RunError_EEROM_Record(char* buf);
																		
tagSystemSetValStruct*	AtCmdAnalysis_SystemSetValCtrl_Get(void);

int AtCmdAnalysis_CnsMod_Init(tagCnsModSet* mod, int size);
									
int AtCmdAnalysis_CnsMod_Get(tagCnsModSet** mod, int* size);
									
int AtCmdAnalysis_CnsMod_EERom_Story(void);
int AtCmdAnslysis_3GNetVersion_Set(int mode);	
int AtCmdAnslysis_MsgAnalysis99(USART_FUNCTION uf,uchar *DaPtr,uint16 len);
int AtCmdAnslysis_MsgAnalysis9A(USART_FUNCTION uf,uchar *DaPtr,uint16 len);									
/******************************************************************************/
int AtCmd_Init(tagModuleSetValStruct* module,
				int moduleCount,
				tagModeRunStateStruct* runState,
				tagATCmdSetInfo* ATCmdSetInfo,
				LinkQueue* cmdLinkQueue,
				tagAtCmdRunCtrlItem* GroupCtrlArray,
				tagSystemSetValStruct*	systemSetVal
				);

/******************************************************************************/
#define RUN_ATCMD_GROUP(RUN_TYPE) AtCmdAnalysis_RunGroup(RUN_TYPE)
#define	SEL_ANLYSIS_MODE(mode,size)	AtCmdAnalysis_SelectAnlysisMode(mode,size)

#define DEBUG_RECORD(BUF) DEBUG(BUF);	\
	AtCmdAnalysis_RunError_EEROM_Record(BUF);
/******************************************************************************/

#include "3GProDa.h"
#include "SendShortMsg.h"
#endif

