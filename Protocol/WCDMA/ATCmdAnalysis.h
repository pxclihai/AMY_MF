#if 1
#include "ATCmdAnalysis_Env.h"
#else

#ifndef AT_CMD_ANALYSIS_H
#define   AT_CMD_ANALYSIS_H

//AT命令定义编号
typedef enum { 
	AT_CMD_TEST_AT			=    0,	  //测试CDMA是否有返回
	AT_CMD_SET_CFUN			=    1,	  //设置运行模式
	AT_CMD_GET_CSQ  		=    2,	  //查询网络信号
	AT_CMD_GET_CREG  		=    3,	  //网络注册信息
	AT_CMD_GET_CICCID  		=    4,	  //查询UIM序列号
	AT_CMD_GET_IPR  		=    5,	  //查询波特率
	AT_CMD_SET_IPR  		=    6,	  //设置波特率
	AT_CMD_SET_CMGF  		=    7,   //设置短信格式 0,PDU模式 1,text模式
	AT_CMD_SET_CSCS   		=    8,	  //选择TE字符集GSM、UCS2、IRA
	AT_CMD_STE_CPMS   		=    9,	  //短信存储位置选择命令
	AT_CMD_SET_CNMI   		=    10,  //设置新短消息提示功能 2,1:存储短信 1,2:不存储短信，新短信直接打印到串口
	AT_CMD_SET_CSMP   		=    11,  //设置TEXT模式参数，最后一个为数据编码类型，0表示默认字符集(GSM) 241:字符 25:中文
	AT_CMD_SET_CMGSO  		=    12,  //快速发信息
	AT_CMD_SET_CMGR   		=    13,  //读取短信，短信位置
	AT_CMD_SET_CMGRD  		=    14,  //读取信息后从SIM 卡中将该信息删除，短信位置
	AT_CMD_SET_CMGD  		=    15,  //删除首选存储器上的短信，模式
	AT_CMD_SET_CMGL 		=    16,  //读取短信 "REC UNREAD"收到未读消息 "REC READ"收到已读消息 "STO UNSENT"未发送消息 "STO SENT"存储已发送消息 "ALL"全部消息
	AT_CMD_REV_MSDATA 		=    17,  //收到短信数据
	AT_CMD_SET_CNMP  		=    18,  //设置上网优先选择模式 2,自动 13,GSM 14,WCDMA
	AT_CMD_SET_CIPSRIP 		=    19,  //设置不显示接收IP
	AT_CMD_SET_CGSOCKCONT  	=    20,  //设置APN 
	AT_CMD_SET_NETOPEN  	=    21,  //设置多链接方式
	AT_CMD_SET_CIPOPEN  	=    22,  //建立一路TCP/IP 连接
	AT_CMD_SET_CIPSEND   	=    23,  //在一路TCP/IP 下发送数据(长度) 
	AT_CMD_SET_TCPWDATA		=	 24,  //在一路TCP/IP 下发送数据(数据体)
 	AT_CMD_SET_NETCLOSE  	=    25,  //关闭一路TCP/IP连接 
	AT_CMD_GET_CIPOPEN 		=	 26,  //查询网络连接状态
	AT_CMD_REV_NETDATA		=	 27,  //收到网络数据
	AT_CMD_GET_CNUM			=	 28,  //查询本机号码
	AT_CMD_SET_CNAOP 		=	 29,  //设置网络顺序
	AT_CMD_GET_CNSMOD 		=	 30,  //查询网络系统模式
	AT_CMD_REV_MSMNOTE		=	 31,  //收到短信数据
	AT_CMD_SET_ATE 			=	 32,  //开启关闭回显
	AT_CMD_SET_CIPCCFG		=	 33,  //配置参数的套接字
	AT_CMD_SET_CMGS			=	 34,  //发送短信+电话号码
	AT_CMD_SET_CMGSDATA		=	 35,  //发送短信数据体
	AT_CMD_GET_CMGRO		=	 36,  //查询短信
	AT_CMD_MAX
}AT_CMD;

typedef int (*ATCmdRecvProcessFun)(uchar *Info,int len);
//#define	     MaxATCmdProlLen			50		//AT协议的最大长度
#define 	MAX_RECV_BUFF_COUNT		2			//应答正确的正确协议个数
#define 	MAX_RECV_ERROR_BUFF_COUNT		1			//应答错误的协议个数
#define 	MAX_MSG_PHONE_SIZE		80

typedef struct
{
	AT_CMD	 ATCmd;								//ATCmd	
	uint8*    ATCmdSendBuff/*[MaxATCmdProlLen]*/;	//发送协议头缓存
	uint8*    ATCmdReceBuff[MAX_RECV_BUFF_COUNT]/*[MaxATCmdProlLen]*/;	//接收协议头缓存
	uint8*	  ATCmdRecvErrBuff[MAX_RECV_ERROR_BUFF_COUNT];				//接收错误协议头缓存
	uint16   WaitTimer;							//等待应答命令时间
	uint8    RepeatSendCnt;                     //失败重发命令次数
	ATCmdRecvProcessFun	fun;					//接收成功后响应函数
	BOOL callBackIfOk;							//接收成功后是否调用立刻返回	
}tagATCmd;	//CDMA命令结构

#define MAX_CALL_BACK_PARAMETER_COUNT 	4
#define	MAX_CALL_BACK_PARAMETER_SIZE	50
typedef int (*CallBackFun)(int argc, char argv[][MAX_CALL_BACK_PARAMETER_SIZE]);

typedef struct
{
	CallBackFun callbackFun;		//回调函数
	int	argc;		//参数个数 
	/*参数数组*/
	char argv[MAX_CALL_BACK_PARAMETER_COUNT][MAX_CALL_BACK_PARAMETER_SIZE];

}tagCallBackQueueItem;
typedef enum
{
	NETWORK_PRIORIYY_TYPE_AUTO,		 //自动选择
	NETWORK_PRIORIYY_TYPE_1G_2G,	//1,2G优先
	NETWORK_PRIORIYY_TYPE_2G_3G,	//2,3G优先
	NETWORK_PRIORIYY_TYPE_MAX		
}NETWORK_PRIORIYY_TYPE;

#define MAX_CID_SIZE		20
#define MAX_IPADDRESS_SIZE	20
#define MAX_PHONE_SIZE		20
#define MAX_BEATSTRING_SIZE	100	
typedef __packed struct
{
	char  CID[MAX_CID_SIZE];				//CID编号
	char  phonelNum[MAX_IPADDRESS_SIZE]; 	//本机号码   
	uchar BeatTime;        //向远程中心发送心跳间隔时间(分)	   
	char  IPAddress[MAX_PHONE_SIZE];   //远程中心IP地址
	int32 Port;            //远程中心端口号	
	BOOL SendRunValFlag;  //是否向此端口下的采集口发送运行参数
	BOOL OpenFlag;	  	   //是否开启自动连接 =false,不开启 =true,开启
	NETWORK_PRIORIYY_TYPE Cns_CNMP;	 //优先当前网络配置
	char beatString[MAX_BEATSTRING_SIZE];		//向中心发送的心跳数据
	int tryCount;				//实验次数	
	DWORD beatTime;			//心跳时间 
	uchar deviceFun;			//设备功能选择
}tagModuleSetValStruct;

typedef enum
{
	SIM_CARD_STATUS_NO_DETECT,
	SIM_CARD_STATUS_OK,
	SIM_CARD_STATUS_ERROR
}SIM_CARD_STATUS;
#define MAX_AT_CMD_ERROR_COUNT	100
typedef __packed struct
{
	int16 CSQ;             //信号强度
	int16 NetIsOpen[MOUNT_DEVICE_COUNT];  	   //网络连接
	int16 CnsMod;	   	   //网络模式
	SIM_CARD_STATUS SimCard;			//SIMcard是否ok
	int16 AtErrorCount;					//错误计数器，当AT命令连续出错达到指定值时，自动重启
}tagModeRunStateStruct;   	   //无线模块运行参数结构

typedef enum {
	PROTOCOL_ANALYSIS_MODE_NONE,			//不自行解析
	PROTOCOL_ANALYSIS_MODE_SHORT_MESSAGE,	//短信
	PROTOCOL_ANALYSIS_MODE_RECEIVE			//接收网络数据
}PROTOCOL_ANALYSIS_MODE; 

typedef enum
{
	ATCMD_RUN_TYPE_RESTART,		//设备重启，优先级最高
	ATCMD_RUN_TYPE_INIT,		//设备初始化,优先级次高的	
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

typedef  int(* ATCmdSendCallBack)(const tagATCmd* atCmd,WORD errType);
#define MAX_SEND_BUFFER_SIZE	1124		
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
}tagATCmdSetInfo;

typedef enum
{	WCDMA_ERROR_INIT,		//初始化没成功
	WCDMA_ERROR_DEVICE,		//设备报的错误
	WCDMA_ERROR_SYSTEM,			//系统崩溃
	WCDMA_ERROR_NO_SIM_CARD,	//没有SIM卡
	WCDMA_ERROR_NET_DISCONNECT,	//网络连接不上
	WCDMA_ERROR_MAX
}WCDMA_ERROR; 
typedef int(*ErrorProcFun)(char* addr, size_t size); 
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
const tagModuleSetValStruct* AtCmdAnalysis_ModuleSetVal_Read(WORD deviceID);


int AtCmdAnalysis_Msg_Send(char* phoneNum, char* buf);
int AtCmdAnalysis_Msg_Send_FromDevice(WORD deviceID, char* buf);
int AtCmdAnalysis_Msg_Send_Direct(char* phoneNum, char* buf);
int AtCmdAnalysis_BeatTime_Set(DWORD beatTime,BOOL store);
int AtCmdAnalysis_Device_Fun(WORD deviceID,uchar deviceFun);
int AtCmdAnalysis_CNMP_Set(WORD	cnmp);
int AtCmd_RECV_WCDMA_Init(void);
int AtCmdAnalysis_ModuleSetVal_Set(tagModuleSetValStruct val[MOUNT_DEVICE_COUNT]);

#include "3GProDa.h"
#include "SendShortMsg.h"
#endif
#endif
