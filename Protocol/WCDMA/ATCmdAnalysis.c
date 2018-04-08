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
#define MAX_ATCMD_LEN	30
#define CK(str1,str2)	 (StrNcmp((uchar*)str1,(uchar*)str2) == 0)

#define ATCMDRECV_NAME(fun) 	  ATCmdRecv_##fun
#define ATCMDRECV_FUN(fun)	static int ATCMDRECV_NAME(fun)##(uchar *Info,int len)
ATCMDRECV_FUN(CMGL);
ATCMDRECV_FUN(CSQ);
ATCMDRECV_FUN(CNSMOD);
ATCMDRECV_FUN(CPMS);
/*启动*/
ATCMDRECV_FUN(Start);
/*接收网络数据*/
ATCMDRECV_FUN(Receive);
/*网络连接打开了*/
ATCMDRECV_FUN(IpOpen);
/*网络连接关闭了*/
ATCMDRECV_FUN(IpClose);
/*SIM错误*/
ATCMDRECV_FUN(SIMFailed);
/*网络错误*/
ATCMDRECV_FUN(NetErr);
/*准备完毕*/
ATCMDRECV_FUN(PbDone);
/*SIM卡正常*/
ATCMDRECV_FUN(SmsDone);
ATCMDRECV_FUN(CMGRD);
ATCMDRECV_FUN(CIPOPEN);

/*ATCmd 初始化*/
static int AtCmdAnalysis_ATCmdRunCtrlArray_Init(void);

#define ATCMDGROUP_NAME(fun) 	  ATCmdGroup_##fun
#define ATCMDGROUP_FUN(fun)	static int ATCMDGROUP_NAME(fun)##(void)

#define ATCMDGROUP_CALLBACK_NAME(fun) 	  ATCmdGroupCallBack_##fun
#define ATCMDGROUP_CALLBACK_FUN(fun) static int ATCMDGROUP_CALLBACK_NAME(fun)(const tagATCmd* atCmd,WORD errType)
	
ATCMDGROUP_CALLBACK_FUN(DeviceSend);
ATCMDGROUP_CALLBACK_FUN(Init);

/*查询环*/
ATCMDGROUP_FUN(DoLoop);

ATCMDGROUP_FUN(Restart);

ATCMDGROUP_FUN(Init);

ATCMDGROUP_FUN(ReadMsg);

ATCMDGROUP_FUN(SendMsg);

ATCMDGROUP_FUN(ConnectNet);

ATCMDGROUP_FUN(DisConnectNet);

ATCMDGROUP_FUN(DeviceSend);

ATCMDGROUP_FUN(InfoSend);

#define TIMER_NAME(fun) 	  Timer_##fun
#define TIMER_FUN(fun)	static void TIMER_NAME(fun)##(void)
#define TIMER_RUN(fun)	  TIMER_NAME(fun)()

TIMER_FUN(StartToSendAtCmd);
/*定时器函数，用于向中心发送心跳数据*/
TIMER_FUN(DeviceBeat);

TIMER_FUN(StartOk);

TIMER_FUN(MainLoop);
		
typedef struct
{
	char*	ATcmd;	
	ATCmdRecvProcessFun	fun;
}tagATCmdRecvProcessItem;

static const tagATCmd	 _gATCmd[]={
//编号						发送头缓存						应答头缓存		失败应答头		等待应答时间	重发次数	响应函数
{AT_CMD_TEST_AT,         {"AT"},	                        {"OK",""},		{""},		    1000,	       	5,			NULL	,true},	//测试CDMA是否有返回
{AT_CMD_SET_CFUN,        {"AT+CFUN=1"},	                 	{"OK",""},		{""},    1000,	       	5,			NULL    		,true},	//设置运行模式
{AT_CMD_GET_CSQ,         {"AT+CSQ"},	                    {"+CSQ:",""},	{""},	1000,	       	5,			ATCMDRECV_NAME(CSQ)   ,true },	//查询网络信号
{AT_CMD_GET_CREG,        {"AT+CREG?"},	                 	{"+CREG: ",""}, {""},  1000,	       	5,			NULL    		,true},	//网络注册信息
{AT_CMD_GET_CICCID,      {"AT+CICCID"},	                 	{"+ICCID: ",""},{""},	1000,	       	5,			NULL    		,true},	//查询UIM序列号
{AT_CMD_GET_IPR,         {"AT+IPR?"},	                 	{"+IPR: ",""},	{""},    1000,	       	5,			NULL    		,true},	//查询波特率
{AT_CMD_SET_IPR,         {"AT+IPR="},	                 	{"OK",""},	    {""},    1000,	       	5,			NULL    		,true},	//设置波特率
//******************************************短信*******************************************************************
{AT_CMD_SET_CMGF,        {"AT+CMGF="},	                 	{"OK",""},		{""},    1000,	       	5,			NULL    		,true},	//设置短信格式 0,PDU模式 1,text模式
{AT_CMD_SET_CSCS,        {"AT+CSCS="},			            {"OK",""},		{""},    1000,	       	5,			NULL    		,true},	//选择TE字符集GSM、UCS2、IRA
{AT_CMD_STE_CPMS,       {"AT+CPMS=\"SM\",\"SM\",\"SM\""}, 	{"+CPMS",""},	{""},	1000,	       	5,			ATCMDRECV_NAME(CPMS)  ,true},	//短信存储位置选择命令
{AT_CMD_SET_CNMI,       {"AT+CNMI=2,1"},	  	            {"OK",""},		{""},    1000,	       	5,			NULL    		,true},	//设置新短消息提示功能 2,1:存储短信 1,2:不存储短信，新短信直接打印到串口
{AT_CMD_SET_CSMP,       {"AT+CSMP=17,167,0,"},         	{"OK",""},		{""},    1000,	       	5,			NULL    		,true},	//设置TEXT模式参数，最后一个为数据编码类型，0表示默认字符集(GSM) 241:字符 25:中文
{AT_CMD_SET_CMGSO,       {"AT+CMGSO="},	                 	{"OK",""},		{"ERROR"},    10000,	       	2,			NULL    		,true},	//快速发信息
{AT_CMD_SET_CMGS,		{"AT+CMGS="},						{">",""},		{""},	500,			1,			NULL			,true},
{AT_CMD_SET_CMGSDATA,	{""},								{"+CMGS","OK"},	{""},	5000,			1,			NULL			,true},
{AT_CMD_SET_CMGR,        {"AT+CMGR="},	                 	{"OK",""},		{""},    1000,	       	5,			NULL    		,true},	//读取短信，短信位置
{AT_CMD_SET_CMGRD,      {"AT+CMGRD="},	                 	{"+CMGRD:",""}, {""},   2000,	       	3,			ATCMDRECV_NAME(CMGRD) ,false},	//读取信息后从SIM 卡中将该信息删除，短信位置 
{AT_CMD_SET_CMGD,       {"AT+CMGD=,4"},	                 	{"OK",""},		{""},    3000,	       	5,			NULL    		,true},	//删除首选存储器上的短信，模式
{AT_CMD_SET_CMGL,       {"AT+CMGL="},		       			{"+CMGL:","OK"},		{""},    1000,	1,			ATCMDRECV_NAME(CMGL)    		,true},	//读取短信 "REC UNREAD"收到未读消息 "REC READ"收到已读消息 "STO UNSENT"未发送消息 "STO SENT"存储已发送消息 "ALL"全部消息
{AT_CMD_REV_MSDATA,      {""},	                         	{"+CMT:",""},	{""},	1000,	       	0,			NULL    		,true},	//收到短信数据 AT+CNMI=1,2
{AT_CMD_REV_MSMNOTE,    {""},                             	{"+CMTI:",""},  {""},   1000,         	0,			NULL    		,true},  //收到短信数据 AT+CNMI=2,1
{AT_CMD_GET_CMGRO,		{"AT+CMGRO="},						{"OK",""},		{""},	1000,			1,			NULL			,true},	//查询短信
//******************************************数据*******************************************************************
{AT_CMD_SET_CNMP,     	 {"AT+CNMP="},                     	{"OK",""},		{""},   1000,	       	5,			NULL    		,true},	//设置上网优先选择模式 2,自动 13,GSM 14,WCDMA
{AT_CMD_SET_CIPSRIP,    {"AT+CIPSRIP=0"},                 	{"OK",""},		{""},   1000,	       	5,			NULL    		,true},	//不显示接收IP
{AT_CMD_SET_CGSOCKCONT, {"AT+CGSOCKCONT=1,\"IP\","},      	{"OK",""},		{""},   1000,	       	5,			NULL    		,true},	//设置APN 
{AT_CMD_SET_NETOPEN,    {"AT+NETOPEN=,,1"},               	{"Network opened",""},{""},		    30000,	       	2,			NULL    ,true},	//多链接方式
{AT_CMD_SET_CIPOPEN,    {"AT+CIPOPEN="},	                {"Connect ok","+IP ERROR: Connection is already created"},{"Connect fail"},	20000,	       	1,			ATCMDRECV_NAME(CIPOPEN)    ,true},	//建立一路TCP/IP 连接 
{AT_CMD_SET_CIPSEND,    {"AT+CIPSEND="},	                {">",""},		  {""},  500,	       	1,			NULL    		,true},	//在一路TCP/IP 下发送数据(长度) 
{AT_CMD_SET_TCPWDATA,   {""},	                         	{"Send ok",""},	{""},	10000,	       	1,			NULL    	,true},	//在一路TCP/IP 下发送数据(数据体) 
{AT_CMD_SET_NETCLOSE,  {"AT+CIPCLOSE="},	             	{"OK",""},		{""},    1000,	       	5,			NULL    		,true},	//关闭一路TCP/IP连接
{AT_CMD_GET_CIPOPEN,    {"AT+CIPOPEN?"},	                {"OK",""},      {""},   500,	       	1,			NULL    		,false},	//查询网络连接状态 
{AT_CMD_REV_NETDATA,    {""},	                         	{"+IPD",""},	{""},	1000,	       	0,			NULL    		,true},	//收到网络数据
//******************************************************************************************************************
{AT_CMD_GET_CNUM,       {"AT+CNUM"},	                    {"+CNUM:",""},	{""},	1000,	       	5,			NULL    		,true},	//查询本机号码	 
{AT_CMD_SET_CNAOP,      {"AT+CNAOP=0"},	                 	{"OK",""},		{""},   1000,	       	5,			NULL    		,true},	//设置网络顺序0、Automatic	1、GSM,WCDMA  2、WCDMA,GSM  
{AT_CMD_GET_CNSMOD,     {"AT+CNSMOD?"},	                 	{"+CNSMOD:",""},{""},   1000,	       	5,			ATCMDRECV_NAME(CNSMOD),true},	//查询网络系统模式
{AT_CMD_SET_ATE,        {"ATE0"},	                     	{"OK",""},      {""},   1000,	       	5,			NULL    		,true},	//开启关闭回显数据
{AT_CMD_SET_CIPCCFG,    {"AT+CIPCCFG=3,0,1,1,1"},	     	{"OK",""},      {""},   1000,	       	5,			NULL    		,true},  //配置参数的套接字
//******************************************************************************************************************* 
};
#define AT_CMD_COUNT	sizeof(_gATCmd)/sizeof(tagATCmd)

static const tagATCmdRecvProcessItem	_gtagATCmdRecvProcessItemArray[] =
{
	{"+CIPOPEN:",ATCMDRECV_NAME(IpOpen)},		  //连接状态
	{"+RECEIVE",ATCMDRECV_NAME(Receive)},		  //接收网络数据

	{"+IPCLOSE:",ATCMDRECV_NAME(IpClose)},		  //未连接状态
	{"+IP ERROR:",ATCMDRECV_NAME(NetErr)},	//网络故障
	{"+CME ERROR: SIM failure",ATCMDRECV_NAME(SIMFailed)}, 	//SIM卡有问题
	{"START",ATCMDRECV_NAME(Start)},	 	//启动
	{"PB DONE",ATCMDRECV_NAME(PbDone)}, 				//上电完毕
	{"SMS DONE",ATCMDRECV_NAME(SmsDone)}
};	
#define RECV_AT_CMD_SIZE	(sizeof(_gtagATCmdRecvProcessItemArray)/sizeof(tagATCmdRecvProcessItem))

typedef enum
{
	ATCMDGROUP_RUN_STATUS_ON,
	ATCMDGROUP_RUN_STATUS_OFF,
	ATCMDGROUP_RUN_STATUS_ALWAYSON 	
}ATCMDGROUP_RUN_STATUS;
typedef int (*ATCmd_Run_Fun)(void);
#define MAX_FUN_ARG_SIZE	100

typedef struct
{
	ATCMD_RUN_TYPE	type;
	ATCmd_Run_Fun fun;	//待运行的程序
	char	funArg[MAX_FUN_ARG_SIZE];		//待运行程序的参数
	WORD	funArgSize;						//参数大小
	ATCMDGROUP_RUN_STATUS status;	//运行的状态
	BOOL lock;		//状态锁	
}tagAtCmdRunCtrlItem;

//初始化配置表
const static  tagAtCmdRunCtrlItem _gAtCmdRunFunTypeArray[ATCMD_RUN_TYPE_MAX] =
{
	/*命令组编号										命令组执行函数						命令组参数	参数个数	命令组执行状态				状态锁*/
	{ATCMD_RUN_TYPE_RESTART,							ATCMDGROUP_NAME(Restart),		"",			0,			ATCMDGROUP_RUN_STATUS_OFF,  false},
	{ATCMD_RUN_TYPE_INIT, 								ATCMDGROUP_NAME(Init),			"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{ATCMD_RUN_TYPE_READ_MSG,							ATCMDGROUP_NAME(ReadMsg),		"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{ATCMD_RUN_TYPE_SEND_MSG,							ATCMDGROUP_NAME(SendMsg),		"",			0,			ATCMDGROUP_RUN_STATUS_OFF,  false},
	{ATCMD_RUN_TYPE_DISCONNECT_NET,						ATCMDGROUP_NAME(DisConnectNet),	"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{(ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_CONNECT_NET + 0),	ATCMDGROUP_NAME(ConnectNet),		"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{(ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_CONNECT_NET + 1),	ATCMDGROUP_NAME(ConnectNet),		"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{(ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_DEVICE_SEND +0), 	ATCMDGROUP_NAME(DeviceSend),			"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{(ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_DEVICE_SEND +1),	ATCMDGROUP_NAME(DeviceSend),			"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{(ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_INFO_SEND +0),		ATCMDGROUP_NAME(InfoSend),				"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{(ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_INFO_SEND +1),		ATCMDGROUP_NAME(InfoSend),				"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
 	{ATCMD_RUN_TYPE_LOOP,								ATCMDGROUP_NAME(DoLoop),				"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false}
};
static tagAtCmdRunCtrlItem _gAtCmdRunCtrlArray[ATCMD_RUN_TYPE_MAX];

#define RUN_ATCMD_GROUP(ATCMD_RUN_TYPE)	  if(_gAtCmdRunCtrlArray[ATCMD_RUN_TYPE].status == ATCMDGROUP_RUN_STATUS_OFF && !_gAtCmdRunCtrlArray[ATCMD_RUN_TYPE].lock)	\
			 {_gAtCmdRunCtrlArray[ATCMD_RUN_TYPE].status = ATCMDGROUP_RUN_STATUS_ON,	   \
			 _gAtCmdRunCtrlArray[ATCMD_RUN_TYPE].lock = true;								\
			 }

#define MAX_QUEUE_CALLBACK_SIZE		5
static tagCallBackQueueItem	_gCallBackQueueItemArray[MAX_QUEUE_CALLBACK_SIZE] = {0};


#define MAX_SEND_DATA_SIZE_ONCE	960		//单次发送文件数据大小
#define MAX_QUEUE_SEND_BUFFER_SIZE	20

#define MAX_HUGE_BUFFER_SIZE	5
typedef struct
{
	int Count;
	char* Buffer[MAX_HUGE_BUFFER_SIZE];
	uint16_t size[MAX_HUGE_BUFFER_SIZE];
}tagHugeSendBuffer;
typedef enum
{
	SEND_BUFFER_TYPE_COMMON,
	SEND_BUFFER_TYPE_HUGE
}SEND_BUFFER_TYPE;
typedef struct
{
   const tagATCmd*	SendAtCmd; 		//AT命令发送
   uchar	sendBuffer[100]; 		//待发送的扩展数据,长度需要匹配心跳数据长度
   int 		sendBufferSize;			//待发送的扩展数据长度
   SEND_BUFFER_TYPE sendBufferType;	//发送数据类型，大数据，普通数据
   tagHugeSendBuffer hugeBuffer;	//大数据描述结构体

}tagSendBufferQueueItem;
static tagSendBufferQueueItem _gSendBufferQueueItemArray[MAX_QUEUE_SEND_BUFFER_SIZE] = {0};

typedef enum
{
	SEND_ERROR_TYPE_NONE	= 0x0000,	//没有任何错误
	SEND_ERROR_TYPE_UNKONWN	= 0x0001,	//未知原因导致的错误
}SEND_ERROR_TYPE;


tagATCmdSetInfo _gATCmdSetInfo = {0};
#define SetATCmdSendCallBack(fun)	_gATCmdSetInfo.aTCmdSendCallBack = fun	

#define CLEAR_ERROR_TYPE()  _gATCmdSetInfo.errBits = SEND_ERROR_TYPE_NONE
#define SET_ERROR_TYPE(errCode)	 _gATCmdSetInfo.errBits |= 	errCode
#define CHECK_ERROR()	(_gATCmdSetInfo.errBits != SEND_ERROR_TYPE_NONE)
#define	_gRecvFunction _gATCmdSetInfo.RecvFunction
#define _gSendAtCmd	_gATCmdSetInfo.SendAtCmd	
#define AT_CALL_BACK_MODE()	(_gSendAtCmd != NULL)
#define	SEL_ANLYSIS_MODE(mode,size)	{_gATCmdSetInfo.protocolAnlysisMode = mode;		 \
									_gATCmdSetInfo.RecvSize = size;	\
									API_SetTimerFast(10000L,1,TIMER_TYPE_WCDMA_ANLYSIS_MODE,NULL,AtCmdAnalysis_AtCmd_EndAnlysisMode);	\
									}

static LinkQueue	_gLinkQueue[2]	= {0};
#define PResFreeQueue	_gLinkQueue[0]		//空闲的队列
#define	PResFullQueue	_gLinkQueue[1]	   	//繁忙的队列
#define	PResFreeQueuePtr	&PResFreeQueue
#define	PResFullQueuePtr	&PResFullQueue
/*接收缓冲区结束*/

static LinkQueue	_gSendBufferLinkQueue[2]	= {0};
#define PResFreeSendBufferQueue	_gSendBufferLinkQueue[0]		//空闲的队列
#define	PResFullSendBufferQueue	_gSendBufferLinkQueue[1]	   	//繁忙的队列
#define	PResFreeSendBufferQueuePtr	&PResFreeSendBufferQueue
#define	PResFullSendBufferQueuePtr	&PResFullSendBufferQueue

#define PROCESS_NAME(fun) 	  ATCmdRecv_Process_##fun
#define ATCMDRECV_PROCESS(fun)	static int PROCESS_NAME(fun)##(int argc, char	argv[][MAX_CALL_BACK_PARAMETER_SIZE])

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
		1,
	}
 };


static 	tagModeRunStateStruct	_gModeRunState = {0};

/******************************************************************************/
//内部函数区域开始
#define _INTR(fun)	fun


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

	EepAddr =  _EEP_ADDR_MODULE_VAL; 
	  						
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

	EepAddr =  _EEP_ADDR_MODULE_VAL;

	memcpy(m,_gModuleSetVal,sizeof(tagModuleSetValStruct) * MOUNT_DEVICE_COUNT);  
	if(API_EEPROM_Read(EepAddr,sizeof(tagModuleSetValStruct) * MOUNT_DEVICE_COUNT,(uchar*)_gModuleSetVal,true) < 0)
	{
		memcpy(_gModuleSetVal,m,sizeof(tagModuleSetValStruct) * MOUNT_DEVICE_COUNT);
		return -1;  
	}
 						
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
	_gATCmdSetInfo.protocolAnlysisMode = PROTOCOL_ANALYSIS_MODE_NONE;
	
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_DeviceBeat_ReStart
// 功能描述  : 重启心跳
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static void _INTR(AtCmdAnalysis_DeviceBeat_ReStart)(void)
{
	API_SetTimer(_gModuleSetVal[0].beatTime *  30 * 1000,-1,TIMER_TYPE_WCDMA_HEART_BEAT,TIMER_NAME(DeviceBeat),NULL);	
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
static int _INTR(AtCmdAnalysis_AtCmd_GetIndex)(AT_CMD cmd)
{
	int i = 0;

	for(i=0; i < AT_CMD_COUNT;i++)
	{
		if(_gATCmd[i].ATCmd == cmd)
			return i; 
	}

	return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_AtCmd_Send
// 功能描述  : 发送AT命令数据
// 输入参数  : CmdNo,命令编号; Info,命令附加数据内容; len,附加数据长度;
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

static int _INTR(AtCmdAnalysis_AtCmd_Send)(AT_CMD cmd,char *buf,int len)
{
	tagSendBufferQueueItem* pItem = NULL;
	QueuePtr QPtr = NULL;
	int index = -1;

	index =  AtCmdAnalysis_AtCmd_GetIndex(cmd);
	if(index < 0)
		return -1;

	if((QPtr = DeQueue(PResFreeSendBufferQueuePtr)) == NULL )
		return -1;
	pItem = QPtr->data;
	pItem->SendAtCmd = &_gATCmd[index];
	if(buf != NULL)
	{
   		memcpy(pItem->sendBuffer,buf,len);
		pItem->sendBufferSize = len;
   	}
	pItem->sendBufferType = SEND_BUFFER_TYPE_COMMON;
	EnQueue(PResFullSendBufferQueuePtr,QPtr);

	return 0;			

}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_AtCmd_SendHugeBuffer
// 功能描述  : 发送网络大数据包
// 输入参数  : cmd,命令编号; hugeBuffer,大数据报描述结构体;
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static int _INTR(AtCmdAnalysis_AtCmd_SendHugeBuffer)(AT_CMD cmd,tagHugeSendBuffer hugeBuffer)
{
	tagSendBufferQueueItem* pItem = NULL;
	QueuePtr QPtr = NULL;
	int index = -1;

	index =  AtCmdAnalysis_AtCmd_GetIndex(cmd);
	if(index < 0)
		return -1;

	if((QPtr = DeQueue(PResFreeSendBufferQueuePtr)) == NULL )
		return -1;

	pItem = QPtr->data;
	pItem->SendAtCmd = &_gATCmd[index];
   	pItem->sendBufferType = SEND_BUFFER_TYPE_HUGE;
	pItem->hugeBuffer =  hugeBuffer;

	EnQueue(PResFullSendBufferQueuePtr,QPtr);

	return 0;			

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

	if(_gATCmdSetInfo.atCmdRunStatus ==  ATCMD_RUN_STATUS_RUN_BUSY)
		return -1;
	_gATCmdSetInfo.aTCmdSendCallBack = NULL;

	CLEAR_ERROR_TYPE();	
	for(i = 0; i < ATCMD_RUN_TYPE_MAX;i++)
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

	for(i=0; i < ATCMD_RUN_TYPE_MAX;i++)
	{
		memcpy(&_gAtCmdRunCtrlArray[_gAtCmdRunFunTypeArray[i].type],&_gAtCmdRunFunTypeArray[i],sizeof(tagAtCmdRunCtrlItem));
	}

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
	if(deviceID < MOUNT_DEVICE_COUNT )
	{
		tagProtocolAnalysis pa;
		char data[51] = "";

		char* pBuf = _gModuleSetVal[deviceID].beatString;
		char* pCid= NULL; 

		size_t dataSize = sizeof(data);
		int cidLen = ReadStrLen(_gModuleSetVal[deviceID].CID);

		memcpy(data +(20-cidLen),_gModuleSetVal[deviceID].CID,cidLen);
		pCid = data +(20-cidLen);
		while(cidLen--)
		{
			(*pCid) = (*pCid) - '0';
			pCid++;	
		}

	 	if(Netdata_ProtocolAnalysis_Packet(&pa,PROTOCOL_ANALYSIS_TYPE_DEVICE_BEAT,data, dataSize) > -1)
		{
			//改变当前的状态为未连接状态
			memcpy(pBuf,pa.headerSender,MAX_PROTOCOL_HEADER_SIZE);
			pBuf += MAX_PROTOCOL_HEADER_SIZE;
			memcpy(pBuf,data,dataSize);
			pBuf +=  dataSize;
			memcpy(pBuf,pa.tailSender,MAX_PROTOCOL_TAIL_SIZE);
			return 0;
		}
	}		

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

	memset(&_gModeRunState,NULL,sizeof(tagModeRunStateStruct));

	for(index =0; index < MOUNT_DEVICE_COUNT; index++)
	{
		_gModeRunState.NetIsOpen[index] = MAX_NET_IS_OPEN_NUM -1;
	}

	return 0;
}
//内部函数区域结束
/******************************************************************************/
#define ERROR_PROCCESS_NAME(fun) 	  Error_Process_##fun
#define ERROR_PROCCESS(fun) static int ERROR_PROCCESS_NAME(fun)(char* addr, size_t size)

ERROR_PROCCESS(Init);
ERROR_PROCCESS(Device);
ERROR_PROCCESS(System);
ERROR_PROCCESS(SimCard);
ERROR_PROCCESS(Net);
const static tagErrorCallBack _gErrorCallBack[] = 
{
	{WCDMA_ERROR_INIT,ERROR_PROCCESS_NAME(Init)},		//初始化没成功
	{WCDMA_ERROR_DEVICE,ERROR_PROCCESS_NAME(Device)},		//设备报的错误
	{WCDMA_ERROR_SYSTEM,ERROR_PROCCESS_NAME(System)},			//系统崩溃
	{WCDMA_ERROR_NO_SIM_CARD,ERROR_PROCCESS_NAME(SimCard)},	//没有SIM卡
	{WCDMA_ERROR_NET_DISCONNECT,ERROR_PROCCESS_NAME(Net)},	//网络连接不上

};
#define ERROR_CALL_BACK_SIZE	sizeof(_gErrorCallBack)/sizeof(tagErrorCallBack)

#define MAX_ERROR_CODE_SIZE	15

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
	Date_Time dataTime;
	char bufs[256] = {0};
	DWORD	address;
	if(API_EEPROM_Read(_EEP_ADDR_ERROR_RECORD_CONTEXT, sizeof(tagErrorEERomContext), (uchar*)(&menu), true) < 0)
	{
		memset(&menu,NULL,sizeof(menu));
	}
	if(menu.writePointer > menu.size)
	{
		menu.writePointer = 0;	
	}
	address = _EEP_ADDR_ERROR_RECORD + 256 * menu.writePointer;

	memset(&dataTime,NULL,sizeof(dataTime));
	sprintf(bufs,
		"%.2d-%.2d-%.2d %.2d:%.2d:%.2d %s %s\n",
		dataTime.StuDate.Year,
		dataTime.StuDate.Month,
		dataTime.StuDate.Day,
		dataTime.StuDate.Hour,
		dataTime.StuDate.Minute,
		dataTime.StuDate.Second,
		title,
		buf
		);//记录时间
	DEBUG("%s\n",bufs);
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
		return 0;
	}

	 return -1;
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
	return Error_EEROM_Record("Net Error",addr);
}
 
 /******************************************************************************/

typedef enum {
	WIRELESS_ERROR_SIM,		//SIM卡检测错误
	WIRELESS_ERROR_NET 		//网络检测错误
}WIRELESS_ERROR;	//无线模块错误



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

	if(argc < 1)
		return -1;		
	uf = (USART_FUNCTION)atoi(argv[0]);

	if(uf < USART_FUNCTION_MAX)
		return API_StartSendBuffer(uf);
	return -1;		
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

	if(argc < 1)
		return -1; 		
	uf = (USART_FUNCTION)atoi(argv[0]);
	if(uf < USART_FUNCTION_MOUNT ||uf >= USART_FUNCTION_MOUNT + MOUNT_DEVICE_COUNT)
		return -1;

	deviceID = uf - USART_FUNCTION_MOUNT;
	Netdata_System_Ctrl(deviceID);

	return 0;
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

	if(argc < 1)
		return -1; 		
	uf = (USART_FUNCTION)atoi(argv[0]);
	if(uf < USART_FUNCTION_MOUNT ||uf >= USART_FUNCTION_MOUNT + MOUNT_DEVICE_COUNT)
		return -1;

	deviceID = uf - USART_FUNCTION_MOUNT;
	Netdata_Program_Update(deviceID);

	return 0;		
}
// 函数名称  : ATCMDRECV_PROCESS(PbDone)
// 功能描述  : 模块启动完毕
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_PROCESS(PbDone)
{
	API_SetTimer(60000,-1,TIMER_TYPE_WCDMA_AT_RESTART,TIMER_NAME(StartOk),NULL);
	return 0;
}

// 函数名称  : ATCMDRECV_PROCESS(EnableHeartBeat)
// 功能描述  : 心跳控制
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_PROCESS(EnableHeartBeat)
{
	BOOL enable;
	WORD port;

	if(argc < 2)
		return -1;
	enable = (BOOL)atoi(argv[0]);
	port =  atoi(argv[1]);

	if(port < MOUNT_DEVICE_COUNT)
	{
		if(enable)	 //说明网络通了
		{
			API_Led_Set(LED_TYPE_2, LED_STATE_ON);
	  		TIMER_RUN(DeviceBeat);
		}
		else
		{
			if(_gModeRunState.NetIsOpen[port] > 0)
			{
				API_Led_Set(LED_TYPE_2, LED_STATE_OFF);
				if(_gModeRunState.NetIsOpen[port] == MAX_NET_IS_OPEN_NUM)
				{
					_gModeRunState.NetIsOpen[port]--;
					//在断网的一刹那执行下函数
					API_SetTimer(20L,1,TIMER_TYPE_WCDMA_AT_SEND,NULL,TIMER_NAME(StartToSendAtCmd));
				}
			}
		}
	}
		
	return 0;
}
// 函数名称  : ATCMDRECV_PROCESS(NoticeError)
// 功能描述  : 错误处理
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_PROCESS(NoticeError)
{
	int index = 0;

	if(argc < 1)
		return -1;
	for(index = 0; index < MOUNT_DEVICE_COUNT; index++)
	{
		if(_gModeRunState.NetIsOpen[index] > 0)
		{
			_gModeRunState.NetIsOpen[index]--;
		}
	}

	API_SetTimer(20L,1,TIMER_TYPE_WCDMA_AT_SEND,NULL,TIMER_NAME(StartToSendAtCmd));

	return 	0;
}
// 函数名称  : ATCMDRECV_PROCESS(SIMFailed)
// 功能描述  : sim卡错误处理
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_PROCESS(SIMFailed)
{
	_gModeRunState.SimCard = SIM_CARD_STATUS_ERROR;	
	API_SetTimer(60000,-1,TIMER_TYPE_WCDMA_AT_RESTART,TIMER_NAME(StartOk),NULL);
	AtCmdAnalysis_ErrorMsg(WCDMA_ERROR_NO_SIM_CARD,"",0);
	return 0;
}

/******************************************************************************/
// 函数名称  : ATCMDRECV_FUN(Start)
// 功能描述  : 启动
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(Start)
{
	return 0;
}

// 函数名称  : ATCMDRECV_FUN(PbDone)
// 功能描述  : 模块启动完成
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(PbDone)
{
	START_CMD(PbDone);
	END_CMD();
	DEBUG("PbDone Cmd Ok\n");

	if(_gModeRunState.SimCard == SIM_CARD_STATUS_OK)
	{
		RUN_ATCMD_GROUP(ATCMD_RUN_TYPE_INIT);
	}
	return 0;
}

// 函数名称  : ATCMDRECV_FUN(SmsDone)
// 功能描述  : Sim卡检测OK
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(SmsDone)
{
	_gModeRunState.SimCard = SIM_CARD_STATUS_OK;	
	return 0;
}

// 函数名称  : ATCMDRECV_FUN(Receive)
// 功能描述  : 有网络数据即将来临
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(Receive)
{
	char *p;
	int length;
	int port;
	char buf[50] = {0};
	/*获取数据接收的端口编号*/

	p = GetStrFromTwoKey((char*)Info, ",",",",buf ,sizeof(buf));

	if(p != NULL)
	{
		port = atoi(buf);
		p = GetStrFromTwoKey(p, ",",":",buf ,sizeof(buf));
		if(p != NULL)
		{
			length= atoi(buf);
			_gATCmdSetInfo.RecvFunction = 	(USART_FUNCTION)(port + USART_FUNCTION_MOUNT);
			SEL_ANLYSIS_MODE(PROTOCOL_ANALYSIS_MODE_RECEIVE,length);
			return 0;
		}
	}


	return -1;//API_Wireless_StartReceive(port,length);
}

// 函数名称  : ATCMDRECV_FUN(IpOpen)
// 功能描述  : 网络连接打开了
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(IpOpen)
{
	int l;
	char* p	;

	BOOL ret = false;
	WORD portInt = 0;

	p = (char*)(Info+10);                 
	portInt = (*p) - '0';

	if(portInt >= MOUNT_DEVICE_COUNT)
		return -1;

	if(_gModuleSetVal[portInt].deviceFun != 1)
		return -1;

	/*将帧尾回车符更换成字符串结束符*/
	/*判断字符长度,如果字符长度大于2,说明此断开已经建立连接*/
	l = ReadStrLen((char*)(Info+10));

	if(l > 5)
	{ 
		ret = true;
	}
	else
	{	
		ret = false;
	}


	if(ret)	 //说明网络通了
	{
		 if(_gModeRunState.NetIsOpen[portInt] != MAX_NET_IS_OPEN_NUM)	//立刻恢复
		 {
		  	_gModeRunState.NetIsOpen[portInt] = MAX_NET_IS_OPEN_NUM;
			START_CMD(EnableHeartBeat);
			ADD_ARGV_BOOL(true);
			ADD_ARGV_INT(portInt);
			END_CMD();
		 }	
	}
	else
	{
		if(_gModeRunState.NetIsOpen[portInt] > 0)
		{
		 	if(_gModeRunState.NetIsOpen[portInt] == MAX_NET_IS_OPEN_NUM)	//立刻恢复
		  	{
			 	API_SetTimerFast(20L,1,TIMER_TYPE_WCDMA_AT_SEND,NULL,TIMER_NAME(StartToSendAtCmd));
				
				//端口断网一刹那
		  		START_CMD(EnableHeartBeat);
				ADD_ARGV_BOOL(false);
				ADD_ARGV_INT(portInt);
				END_CMD();	
		  	}	
			_gModeRunState.NetIsOpen[portInt]--;
		}
		else
		{
			int index = 0;
	
			for(index = 0; index < MOUNT_DEVICE_COUNT; index++)
			{
				if(_gModeRunState.NetIsOpen[index] > 0)
					break;
			}
	
			if(index >= MOUNT_DEVICE_COUNT)
			{
				char buf[30] = {0};
				_gModeRunState.NetIsOpen[portInt] = MAX_NET_IS_OPEN_NUM-1;
				sprintf(buf,"Service %d disConnect",portInt);
				AtCmdAnalysis_ErrorMsg(WCDMA_ERROR_NET_DISCONNECT,buf,strlen(buf));
				AtCmd_RECV_WCDMA_Init();
			}
	
			//重启设备			
		}
	}

	return 0;
}

// 函数名称  : ATCMDRECV_FUN(IpClose)
// 功能描述  : 网络连接关闭了
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(IpClose)
{
	char *port;

	port=InStr((char*)Info, ",",len);
	if(port == NULL)
		return -1;
	port++;
	DEBUG("EnableHeartBeat Cmd Ok\n");
	START_CMD(EnableHeartBeat);
	ADD_ARGV_BOOL(false);
	ADD_ARGV(port);
	END_CMD();
	return 0;
}

// 函数名称  : ATCMDRECV_FUN(SIMFailed)
// 功能描述  : SIM错误
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(SIMFailed)
{
	DEBUG("SIMFailed Cmd Ok\n");
	START_CMD(SIMFailed);
	ADD_ARGV_INT(WIRELESS_ERROR_SIM);
	END_CMD();
	return 0;
}

// 函数名称  : ATCMDRECV_FUN(NetErr)
// 功能描述  : 网络错误
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(NetErr)
{
	if(InStr((char*)Info,"Connection is already created",len) == NULL)
	{	
	 	START_CMD(NoticeError);
		ADD_ARGV_INT(WIRELESS_ERROR_NET);
		END_CMD();
	}
	else
	{
		  
	}
	return 0;//API_Wireless_NoticeError(WIRELESS_ERROR_NET);
}

// 函数名称  : ATCMDRECV_FUN(CPMS)
// 功能描述  : 接收查询到有短信数据
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(CPMS)
{
	char* p = NULL;
	int count;

	p = (char*)(Info+7);
	count = String_To_Int(&p , ",");
	/*有短信息*/
	if(count>0 && _gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		RUN_ATCMD_GROUP(ATCMD_RUN_TYPE_READ_MSG);		
	}

	return 0;
}

// 函数名称  : ATCMDRECV_FUN(CNSMOD)
// 功能描述  : 查询网络系统模式
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(CNSMOD)
{
	char* p = NULL;

	p=InStr((char*)Info,",",len);
	if(p!=NULL)
	{
		p++;
		_gModeRunState.CnsMod  = String_To_Int(&p,"\0");

	}

	return 0;

}

// 函数名称  : ATCMDRECV_FUN(CSQ)
// 功能描述  : 查询网络信号
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(CSQ)
{
	int16 csq;
	char buf[20] = {0}; 

	if(GetStrFromTwoKey((char*)Info, ":",",",buf ,len) != NULL)
	{
		csq = atoi(buf);
		csq = csq > 98?0:csq;
		csq = csq < 0?0:csq;
		_gModeRunState.CSQ =  csq;
	}

	return 0;
}

// 函数名称  : ATCMDRECV_FUN(CMGL)
// 功能描述  : 读取信息
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(CMGL)
{
	if(!CK(Info,"OK"))
	{
		int Start = 30;
		while((*Info != '\"' || *(Info+1) != ',') &&  Start > 0)
		{
			Start--;
			Info++;
		}
		//找到起点;
		Info += 2;
		Start = 0;
		while(Info[Start] != ',' && Start < MAX_MSG_PHONE_SIZE)
		{
			Start++;
		}
		if(Start < MAX_MSG_PHONE_SIZE)
		{
			_gATCmdSetInfo.haveShortMsg = true;
			SEL_ANLYSIS_MODE(PROTOCOL_ANALYSIS_MODE_SHORT_MESSAGE,-1);
			memcpy(_gATCmdSetInfo.phoneNum,Info,Start);
	
		}
		//GetStrFromTwoKey((char*)Info, "\",",",",_gATCmdSetInfo.phoneNum ,len);
	}

	return 0;	
	
}
// 函数名称  : ATCMDRECV_FUN(CMGRD)
// 功能描述  : 读取信息后从SIM 卡中短信
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(CMGRD)
{
	SEL_ANLYSIS_MODE(PROTOCOL_ANALYSIS_MODE_SHORT_MESSAGE,-1);
	GetStrFromTwoKey((char*)Info, ",",",",_gATCmdSetInfo.phoneNum ,len);

	return 0;	
}

// 函数名称  : ATCMDRECV_FUN(CIPOPEN)
// 功能描述  : 端口连接状态
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(CIPOPEN)
{
	return 0;
}

/******************************************************************************/
/*定时器区域开始*/
// 函数名称  : TIMER_FUN(MainLoop)
// 功能描述  : 主循环体
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

TIMER_FUN(MainLoop)
{
	if(_gATCmdSetInfo.atCmdRunStatus ==  ATCMD_RUN_STATUS_RUN_FREE)
	{
		RUN_ATCMD_GROUP(ATCMD_RUN_TYPE_LOOP);
	}
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
			   		AtCmdAnalysis_Net_Connect(i);

				}
				else
				{
					_gModuleSetVal[i].tryCount--;	
				}
			}
		}
	
	}
	//一分钟发送一次 	

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
	AtCmdAnalysis_RunStatus_Init();
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		AtCmd_RECV_WCDMA_Init();
	}
	else
	{
		API_WCDMA_Init();
	}
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
	_gATCmdSetInfo.atCmdRunStatus =  ATCMD_RUN_STATUS_RUN_BUSY;
	if(_gATCmdSetInfo.SendAtCmd == NULL || _gATCmdSetInfo.sendRepeatCount <= 0)
	{
	 	QueuePtr QPtr = NULL;
	 	tagSendBufferQueueItem* pItem = NULL;

		if(CHECK_ERROR())
		{
			DEBUG("Check Error\n");
			while((QPtr = DeQueue(PResFullSendBufferQueuePtr)) != NULL)
			{
				EnQueue(PResFreeSendBufferQueuePtr,QPtr); 	//清空所有的Cmd	,接下来的命令不执行了
			}			
		}
		else
		{
			 _gModeRunState.AtErrorCount = 0;
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

	 if(_gATCmdSetInfo.SendAtCmd != NULL && _gATCmdSetInfo.sendRepeatCount > 0)
	 {
	 	 if(_gATCmdSetInfo.sendRepeatCount < _gATCmdSetInfo.SendAtCmd->RepeatSendCnt)
		{
			_gModeRunState.AtErrorCount++;
			if(_gModeRunState.AtErrorCount >= MAX_AT_CMD_ERROR_COUNT)
			{
				 TIMER_RUN(StartOk);
			}
			DEBUG("there is least %d count to try Sending Buffer:cmd:%d\n",_gATCmdSetInfo.sendRepeatCount,_gATCmdSetInfo.SendAtCmd->ATCmd);
		}
		_gATCmdSetInfo.cmpSuccess = false;
		API_ResetProtocolBuffer(USART_FUNCTION_WCDMA,RESETRTBUFF_TYPE_TX);
		API_AddProtocolBuffer(USART_FUNCTION_WCDMA,(uchar*)(_gATCmdSetInfo.SendAtCmd->ATCmdSendBuff),ReadStrLen((char*)_gATCmdSetInfo.SendAtCmd->ATCmdSendBuff));
		API_AddProtocolBuffer(USART_FUNCTION_WCDMA,_gATCmdSetInfo.sendBuffer,_gATCmdSetInfo.sendBufferSize);
		API_AddProtocolBuffer(USART_FUNCTION_WCDMA,"\r\n",2);
		API_StartSendBuffer(USART_FUNCTION_WCDMA);
		_gATCmdSetInfo.sendRepeatCount--;

	 }

	 API_SetTimer(_gATCmdSetInfo.SendAtCmd->WaitTimer,1,TIMER_TYPE_WCDMA_AT_SEND,NULL,TIMER_NAME(StartToSendAtCmd));
}
/*定时器区域结束*/
/******************************************************************************/

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_FUN(DisConnectNet)
// 功能描述  : 断网
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDGROUP_FUN(DisConnectNet)
{
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		char StrTempBuf[100] = {0};
		int deviceIndex = 0;
		char* buf =  _gAtCmdRunCtrlArray[ATCMD_RUN_TYPE_DISCONNECT_NET].funArg;
		if(ReadStrLen(buf) < 1)
			return -1;
		deviceIndex = atoi(buf);
		if(deviceIndex >= MOUNT_DEVICE_COUNT )
			return -1;
			  
		sprintf(StrTempBuf,"%d",deviceIndex);
		AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_NETCLOSE,StrTempBuf,strlen(StrTempBuf));
		AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CIPOPEN,"",0);
		TIMER_RUN(StartToSendAtCmd);

		return 0;

	}

	return -1;

}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_FUN(ConnectNet)
// 功能描述  : 联网
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDGROUP_FUN(ConnectNet)
{ 
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		char StrTempBuf[100] = {0};
		int deviceIndex = 0;

		if(_gATCmdSetInfo.atCmdGroup < ATCMD_RUN_TYPE_CONNECT_NET || _gATCmdSetInfo.atCmdGroup >= ATCMD_RUN_TYPE_CONNECT_NET + MOUNT_DEVICE_COUNT)
			return -1; 

		deviceIndex = _gATCmdSetInfo.atCmdGroup - ATCMD_RUN_TYPE_CONNECT_NET;

		if(deviceIndex >= MOUNT_DEVICE_COUNT )
			return -1;
	  
		sprintf(StrTempBuf,"%d,\"TCP\",\"%s\",%d",deviceIndex,_gModuleSetVal[deviceIndex].IPAddress,
		                                         _gModuleSetVal[deviceIndex].Port);
		AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CIPOPEN,StrTempBuf,ReadStrLen(StrTempBuf));
		AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CIPOPEN,"",0);
		TIMER_RUN(StartToSendAtCmd);
		return 0;
	   
	} 
	return -1;
}			 

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_FUN(ReadMsg)
// 功能描述  : 读取短信
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDGROUP_FUN(ReadMsg)
{
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CMGRD,"0",1);
		TIMER_RUN(StartToSendAtCmd);

		return 0;
	}

	return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_FUN(SendMsg)
// 功能描述  : 发送短信
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDGROUP_FUN(SendMsg)
{
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		const int shortMsgMaxSize = 70 * 4;		//单条短信最大长度是70个字符,一个字符占用4个HEX 
	   	int size = 0;
		int phoneNumSize = 0;
		int index = 0;
		static char MsgEndChar = 0x1a;
		phoneNumSize = strlen(_gATCmdSetInfo.sendPhoneNum);
		size = ReadStrLen(_gATCmdSetInfo.msgBuffer);

		while(size > 0)
		{
			int shortMsgSize = size > shortMsgMaxSize?shortMsgMaxSize:size;
			
			tagHugeSendBuffer hugeBuffer;
			hugeBuffer.Count = 0;
			hugeBuffer.Buffer[hugeBuffer.Count] = _gATCmdSetInfo.msgBuffer + (index *shortMsgMaxSize) ;
			hugeBuffer.size[hugeBuffer.Count] = shortMsgSize;
			hugeBuffer.Count++;
			hugeBuffer.Buffer[hugeBuffer.Count] = &MsgEndChar;
			hugeBuffer.size[hugeBuffer.Count] = 1;
			hugeBuffer.Count++;
			AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CMGS,_gATCmdSetInfo.sendPhoneNum,phoneNumSize);
			AtCmdAnalysis_AtCmd_SendHugeBuffer(AT_CMD_SET_CMGSDATA,hugeBuffer);
			size -= shortMsgSize;
			index++;
		}

		TIMER_RUN(StartToSendAtCmd);
		return 0;

	}

	return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_CALLBACK_FUN(DeviceSend)
// 功能描述  : 发送设备的信息后的回调函数
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDGROUP_CALLBACK_FUN(DeviceSend)
{
	int deviceIndex = 0;

	deviceIndex = _gATCmdSetInfo.atCmdGroup - ATCMD_RUN_TYPE_DEVICE_SEND;
	if(errType != SEND_ERROR_TYPE_NONE)
	{
		API_ResetProtocolBuffer((USART_FUNCTION)(deviceIndex + USART_FUNCTION_MOUNT),RESETRTBUFF_TYPE_AND_INT_RX);

		//如果检测到错误
	}
	else
	{
		//Delay_MS(1000);
		AtCmdAnalysis_Net_DeviceSend(deviceIndex);
	 	//如果没有检测到错误,继续发送数据
	}

	return 0;

}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_FUN(DeviceSend)
// 功能描述  : 发送设备信息
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDGROUP_FUN(DeviceSend)
{
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		char StrTempBuf[100] = {0};
		int deviceIndex = 0;
		uint16_t size = 0;
		uint16_t retSize =0;
		tagHugeSendBuffer hugeBuffer;

		if(_gATCmdSetInfo.atCmdGroup < ATCMD_RUN_TYPE_DEVICE_SEND || _gATCmdSetInfo.atCmdGroup >= ATCMD_RUN_TYPE_DEVICE_SEND + MOUNT_DEVICE_COUNT)
			return -1; 

		deviceIndex = _gATCmdSetInfo.atCmdGroup - ATCMD_RUN_TYPE_DEVICE_SEND;
		if(_gModeRunState.NetIsOpen[deviceIndex] < MAX_NET_IS_OPEN_NUM)
			return -1;
	  	size = Netdata_Protocol_ReadSize(deviceIndex,MAX_SEND_DATA_SIZE_ONCE);
		if(size == 0) {
			API_ResetProtocolBuffer((USART_FUNCTION)(deviceIndex + USART_FUNCTION_MOUNT),RESETRTBUFF_TYPE_AND_INT_RX);
			return -1;
		}
		/*把数据读出来*/

		SetATCmdSendCallBack(ATCMDGROUP_CALLBACK_NAME(DeviceSend));	//设置发送成功后的回调函数
		hugeBuffer.Count = 0;
		size = 0;
		if(Netdata_Protocol_GetPostion(deviceIndex) == PROTOCOL_READ_POS_START)
		{
			size +=Netdata_Protocol_ReadAddressAndSize(deviceIndex,PROTOCOL_READ_POS_START, &hugeBuffer.Buffer[hugeBuffer.Count],&hugeBuffer.size[hugeBuffer.Count],20);
			hugeBuffer.Count++;
										
		}

		if((retSize = Netdata_Protocol_ReadAddressAndSize(deviceIndex,PROTOCOL_READ_POS_CENTER, &hugeBuffer.Buffer[hugeBuffer.Count],&hugeBuffer.size[hugeBuffer.Count],MAX_SEND_DATA_SIZE_ONCE)) < MAX_SEND_DATA_SIZE_ONCE)
		{
			hugeBuffer.Count++;
			size += Netdata_Protocol_ReadAddressAndSize(deviceIndex,PROTOCOL_READ_POS_END, &hugeBuffer.Buffer[hugeBuffer.Count],&hugeBuffer.size[hugeBuffer.Count],MAX_SEND_DATA_SIZE_ONCE);


		}

		hugeBuffer.Count++;
		size += retSize;
		//数据读完
		sprintf(StrTempBuf,"%d,%d",deviceIndex,size);
		AtCmdAnalysis_AtCmd_Send(AT_CMD_TEST_AT,"",0);
 		AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CIPSEND,StrTempBuf,ReadStrLen(StrTempBuf));

		AtCmdAnalysis_AtCmd_SendHugeBuffer(AT_CMD_SET_TCPWDATA,hugeBuffer);

		AtCmdAnalysis_DeviceBeat_ReStart();	//心跳暂停
		//		DEBUG("L:%d\n",__LINE__);
		TIMER_RUN(StartToSendAtCmd);

		return 0;

	}
	return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_FUN(InfoSend)
// 功能描述  : 发送网络信息
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDGROUP_FUN(InfoSend)
{
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		char StrTempBuf[100] = {0};
		int deviceIndex = 0;
		uint16_t size = 0;

		if(_gATCmdSetInfo.atCmdGroup < ATCMD_RUN_TYPE_INFO_SEND || _gATCmdSetInfo.atCmdGroup >= ATCMD_RUN_TYPE_INFO_SEND + MOUNT_DEVICE_COUNT)
			return -1; 


		deviceIndex = _gATCmdSetInfo.atCmdGroup - ATCMD_RUN_TYPE_INFO_SEND;

		if(_gModeRunState.NetIsOpen[deviceIndex] < MAX_NET_IS_OPEN_NUM)
			return -1;
		//数据读完
		size = _gAtCmdRunCtrlArray[_gATCmdSetInfo.atCmdGroup].funArgSize;// strlen(_gAtCmdRunFunTypeArray[_gATCmdSetInfo.atCmdGroup].funArg);
		

		AtCmdAnalysis_DeviceBeat_ReStart();	//心跳暂停
		sprintf(StrTempBuf,"%d,%d",deviceIndex,size);
 		AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CIPSEND,StrTempBuf,strlen(StrTempBuf));

		AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_TCPWDATA,
			_gAtCmdRunCtrlArray[_gATCmdSetInfo.atCmdGroup].funArg,
			size);

		//		DEBUG("L:%d\n",__LINE__);
		TIMER_RUN(StartToSendAtCmd);

		return 0;

	}
	return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_FUN(Restart)
// 功能描述  : 模块重启
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDGROUP_FUN(Restart)
{
	_gATCmdSetInfo.atCmdRunStatus =  ATCMD_RUN_STATUS_NOT_RUN;
	_gATCmdSetInfo.sendRepeatCount = 0;

	API_WCDMA_Init();

	return 0;	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_CALLBACK_FUN(Init)
// 功能描述  : 初始化的回调函数
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDGROUP_CALLBACK_FUN(Init)
{
	if(errType != SEND_ERROR_TYPE_NONE)
	{
		char buf[20] = {0};

		_gATCmdSetInfo.atCmdRunStatus =  ATCMD_RUN_STATUS_NOT_RUN;	//标志成Not Run状态
		DEBUG("Error detect to Init setting ev,errType:%d!\n",errType);
		sprintf(buf,"errorType:%d,atCmd:%d",errType,atCmd->ATCmd);
		AtCmdAnalysis_ErrorMsg(WCDMA_ERROR_INIT,buf,strlen(buf));
		AtCmd_RECV_WCDMA_Init();
		//如果检测到错误
	}
	else
	{

		DEBUG("Success to Init setting ev!\n");

		TIMER_RUN(DeviceBeat);

	 	//如果没有检测到错误
	}
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_FUN(Init)
// 功能描述  : 初始化的函数
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDGROUP_FUN(Init)
{
	char* netPriBuf = NULL;
	char* apnBuffer = NULL;

	DEBUG("Start to Init setting ev!\n");
	SetATCmdSendCallBack(ATCMDGROUP_CALLBACK_NAME(Init));	//设置初始化成功后的回调函数

	switch(_gModuleSetVal[0].Cns_CNMP)
	{
		case NETWORK_PRIORIYY_TYPE_1G_2G:	//1,2G优先
			 netPriBuf = "13";
			 apnBuffer= "\"3gwap\"";
			 break;
		case NETWORK_PRIORIYY_TYPE_2G_3G:	//2,3G优先
			netPriBuf = "14";
			apnBuffer = "\"3gnet\"";
			break;
		case NETWORK_PRIORIYY_TYPE_AUTO:	//自动选择
		default:
			netPriBuf = "2";
			apnBuffer = "\"3gnet\"";
	}

	AtCmdAnalysis_AtCmd_Send(AT_CMD_TEST_AT,"",0);
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CNMP,netPriBuf,ReadStrLen(netPriBuf));
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CNAOP,"",0);  //设置网络顺序
	AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CNSMOD,"",0);  //设置网络顺序
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CFUN,"",0);  //设置网络顺序	
	AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CSQ,"",0);  //设置网络顺序
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CMGF,"1",1);  //设置短信格式 

	//支持中文		
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CSMP,"25",2);  //设置TEXT模式参数
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CSCS,"\"UCS2\"",6);  //选择TE字符集GSM、UCS2、IRA

 	AtCmdAnalysis_AtCmd_Send(AT_CMD_STE_CPMS,"",0);  //设置网络顺序
 	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CNMI,"",0);  //设置网络顺序

 	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CIPSRIP,"",0);  //设置网络顺序
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CGSOCKCONT,apnBuffer,ReadStrLen(apnBuffer));  //设置网络顺序
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CMGD,"",0);
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_NETOPEN,"",0);  //设置网络顺序
 	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CIPCCFG,"",0);  //设置网络顺序	
 //	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_ATE,"",0);  //设置网络顺序

	//		DEBUG("L:%d\n",__LINE__);
	TIMER_RUN(StartToSendAtCmd);
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_FUN(DoLoop)
// 功能描述  : 轮流发送命令，进行查询
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDGROUP_FUN(DoLoop)
{
	if(_gATCmdSetInfo.atCmdRunStatus ==  ATCMD_RUN_STATUS_RUN_FREE)
	{
		API_SetTimer(600000,-1,TIMER_TYPE_WCDMA_AT_RESTART,TIMER_NAME(StartOk),NULL);
		//只有在空闲状态下才会运行

		AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CSQ,"",0);
		AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CIPOPEN,"",0);
		AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CNSMOD,"",0);
		if(_gATCmdSetInfo.haveShortMsg)
		{
			AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CMGD,"",0);
			_gATCmdSetInfo.haveShortMsg = false;
		}
		else
		{
			AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CMGL,"\"ALL\"",5);

		}
		//		DEBUG("L:%d\n",__LINE__);
		TIMER_RUN(StartToSendAtCmd);

		return 0;

	}
	return -1;
	 
}


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

		START_CMD(StartUpdate);
		ADD_ARGV_INT(uf);
		END_CMD();
		return 0;
	}

	return 1;
}

int AtCmdAnalysis_WCDMA_StartWcdmaCtrl(USART_FUNCTION uf)
{
	if(uf < USART_FUNCTION_MAX)
	{

		START_CMD(StartWcdmaCtrl);
		ADD_ARGV_INT(uf);
		END_CMD();
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
		START_CMD(StartSend);
		ADD_ARGV_INT(uf);
		END_CMD();

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

	 if(pItem != NULL)
	 {
	 	if(pItem->fun != NULL)
		{
			if(pItem->fun(Info,len) < 0)
				return -1;	
		}
		return 0;
	 }

	 if(AT_CALL_BACK_MODE())
	 {
	 	int i =  0;
		BOOL ret = false;

	 	for(i = 0; i < MAX_RECV_BUFF_COUNT; i++)
		{	
			if((ret = CK(Info,_gSendAtCmd->ATCmdReceBuff[i])) == TRUE)
				break;
		}

	 	if(ret)
		{
			if(_gSendAtCmd->fun != NULL)
			{
				_gSendAtCmd->fun(Info,len);

			}

  			_gATCmdSetInfo.sendRepeatCount = 0;
			CLEAR_ERROR_TYPE();
			_gATCmdSetInfo.cmpSuccess  = true;
		
			API_SetTimerFast(10L,1,TIMER_TYPE_WCDMA_AT_SEND,NULL,TIMER_NAME(StartToSendAtCmd));
		
			return 0;	 
			//应答正确
		}
		else
		{
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
		return -1;
	 }
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
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		if(deviceID < MOUNT_DEVICE_COUNT )
		{	
		 
			WORD index = ATCMD_RUN_TYPE_INFO_SEND + deviceID;
			if(_gAtCmdRunCtrlArray[index].status == ATCMDGROUP_RUN_STATUS_OFF)
			{
				memset(_gAtCmdRunCtrlArray[index].funArg,NULL,MAX_FUN_ARG_SIZE);
				memcpy(_gAtCmdRunCtrlArray[index].funArg,buf,size);
				_gAtCmdRunCtrlArray[index].funArgSize = size;
				RUN_ATCMD_GROUP((ATCMD_RUN_TYPE)index);
			}		
		}
	}
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

	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{

		if(deviceID < MOUNT_DEVICE_COUNT )
		{
			WORD index = ATCMD_RUN_TYPE_DEVICE_SEND + deviceID;
			if(_gAtCmdRunCtrlArray[index].status == ATCMDGROUP_RUN_STATUS_OFF)
			{
				RUN_ATCMD_GROUP((ATCMD_RUN_TYPE)index);
			}		
		}
	}
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
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		if(deviceID < MOUNT_DEVICE_COUNT )
		{
			if(_gModeRunState.NetIsOpen[deviceID] != MAX_NET_IS_OPEN_NUM)
			{
				return 0;
			}
			else
			{
				if(_gAtCmdRunCtrlArray[ATCMD_RUN_TYPE_DISCONNECT_NET].status == ATCMDGROUP_RUN_STATUS_OFF)
				{
					sprintf(_gAtCmdRunCtrlArray[ATCMD_RUN_TYPE_DISCONNECT_NET].funArg,"%d",deviceID);
					RUN_ATCMD_GROUP(ATCMD_RUN_TYPE_DISCONNECT_NET);
					return 0;	
				}
			}		
		}
	} 
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
	if(size < MAX_IPADDRESS_SIZE && deviceID < MOUNT_DEVICE_COUNT)
	{
		memset(_gModuleSetVal[0].phonelNum,NULL, MAX_PHONE_SIZE);
		memcpy(_gModuleSetVal[0].phonelNum,phoneNum,size);
		AtCmdAnalysis_EERom_Story();

		return 0;
	}

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
	if(size < MAX_PHONE_SIZE && deviceID < MOUNT_DEVICE_COUNT)
	{
		memset(_gModuleSetVal[deviceID].IPAddress,NULL, MAX_PHONE_SIZE);
		memcpy(_gModuleSetVal[deviceID].IPAddress,ipAddress,size);
		_gModuleSetVal[deviceID].Port = port;
		AtCmdAnalysis_EERom_Story();
		AtCmdAnalysisi_DisConnect(deviceID);
		AtCmdAnalysis_Net_Connect(deviceID);
		return 0;
	}

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
	if(beatTime >= 1 && beatTime <= 10)
	{
		int i = 0;
		for(i = 0; i < MOUNT_DEVICE_COUNT;i++)
		{
			_gModuleSetVal[i].beatTime = beatTime;
		}
		if(store)
		{
			AtCmdAnalysis_EERom_Story();
		}
		AtCmdAnalysis_DeviceBeat_ReStart();

		return 0;
	}
	else
	{
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
	if(deviceID < MOUNT_DEVICE_COUNT )
	{
		_gModuleSetVal[deviceID].deviceFun = deviceFun;
		AtCmdAnalysis_EERom_Story();
		if(deviceFun == 1)
		{
			
			AtCmdAnalysis_Net_Connect(deviceID);	
		}
		else
		{
			AtCmdAnalysisi_DisConnect(deviceID);
		}
		
	}
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
	if(cnmp < NETWORK_PRIORIYY_TYPE_MAX)
	{
		_gModuleSetVal[0].Cns_CNMP = (NETWORK_PRIORIYY_TYPE)cnmp;
		AtCmdAnalysis_EERom_Story();
	   AtCmd_RECV_WCDMA_Init();		//重启设备
	   return 0;
	}

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

	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
 		if(deviceID < MOUNT_DEVICE_COUNT )
		{
 			if(_gModeRunState.NetIsOpen[deviceID] == MAX_NET_IS_OPEN_NUM)
			{
 				return 0;
			}
			if(_gAtCmdRunCtrlArray[ATCMD_RUN_TYPE_CONNECT_NET + deviceID].status == ATCMDGROUP_RUN_STATUS_OFF)
			{
				if(AtCmdAnalysis_BeatString_Build(deviceID) > -1)
				{
					RUN_ATCMD_GROUP((ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_CONNECT_NET + deviceID));	
				}	
			}
		}
	}

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
	if(runState != NULL)
	{
		memcpy(runState,&_gModeRunState,sizeof(tagModeRunStateStruct));
		return 0;
	}
	return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_ModuleSetVal_Get
// 功能描述  : 某个设备的参数设置获取
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int AtCmdAnalysis_ModuleSetVal_Get(WORD deviceID,tagModuleSetValStruct* moduleSetVal)
{
	if(deviceID < MOUNT_DEVICE_COUNT && moduleSetVal != NULL)
	{
		memcpy(moduleSetVal,&_gModuleSetVal[deviceID],sizeof(tagModuleSetValStruct));
		return 0;
	}

	return -1;
}

const tagModuleSetValStruct* AtCmdAnalysis_ModuleSetVal_Read(WORD deviceID)
{
	if(deviceID < MOUNT_DEVICE_COUNT)
	{
		return &_gModuleSetVal[deviceID];
	}

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
	if(phoneNum != NULL && buf != NULL)
	{
		int retSize = 0;
		strcpy(_gATCmdSetInfo.sendPhoneNum,phoneNum);
		retSize = Msg_Buffer_UTF8ToPDU(buf,strlen(buf),_gATCmdSetInfo.msgBuffer,sizeof(_gATCmdSetInfo.msgBuffer)-1);
		if(retSize > 0)
		{
			//_gATCmdSetInfo.msgBuffer[retSize] = 0x1a;
			_gATCmdSetInfo.msgBuffer[retSize] = 0x0;
			RUN_ATCMD_GROUP(ATCMD_RUN_TYPE_SEND_MSG);	
		}
		
		return 0;
	}
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
	if(deviceID < MOUNT_DEVICE_COUNT)
	{
		char phoneNum[90] = {0};
		int retSize = 0;
		phoneNum[0] = '\"';
		retSize = Msg_Buffer_UTF8ToPDU(_gModuleSetVal[deviceID].phonelNum,strlen(_gModuleSetVal[deviceID].phonelNum),&phoneNum[1],80);
		if(retSize > 0)
		{
			phoneNum[retSize+1] = '\"';
			return AtCmdAnalysis_Msg_Send(phoneNum,buf);
		}
	}
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
	if(phoneNum != NULL && buf != NULL)
	{
		int retSize = 0;
		strcpy(_gATCmdSetInfo.sendPhoneNum,phoneNum);
		retSize = strlen(buf);

		if(retSize > 0 && retSize < 600)
		{
			strcpy(_gATCmdSetInfo.msgBuffer,buf);
			_gATCmdSetInfo.msgBuffer[retSize] = 0x1a;
			RUN_ATCMD_GROUP(ATCMD_RUN_TYPE_SEND_MSG);	
		}
		//sprintf(_gATCmdSetInfo.msgBuffer,"%s%c",buf,0x1a);
		
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
	AtCmdAnalysis_EERom_Read();
	AtCmdAnalysis_RunStatus_Init();
	ATCmdRecv_Queue_Init();
	ATCmdRecv_SendBufferQueue_Init();
	Netdata_Init();
	Msg_Init();
	API_SetOutsideAnalysisFun(USART_FUNCTION_WCDMA,AtCmdAnalysis_Protocol_Analysis);
	AtCmdAnalysis_ATCmdRunCtrlArray_Init();
	API_SetTimer(3000,-1,TIMER_TYPE_WCDMA_ATCMD_PROCESS_LOOP,TIMER_NAME(MainLoop),NULL);
	AtCmdAnalysis_BeatTime_Set(_gModuleSetVal[0].beatTime,false);
	API_SetTimer(60000,-1,TIMER_TYPE_WCDMA_AT_RESTART,TIMER_NAME(StartOk),NULL);

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
	if(size < MAX_IPADDRESS_SIZE && deviceID < MOUNT_DEVICE_COUNT)
	{	
		memset(_gModuleSetVal[deviceID].CID,NULL, MAX_CID_SIZE);
		memcpy(_gModuleSetVal[deviceID].CID,cid,size);
		AtCmdAnalysis_EERom_Story();

		return AtCmdAnalysis_BeatString_Build(deviceID);
	}

	return -1;
}

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
	for(i = 0; i < MOUNT_DEVICE_COUNT; i++)
	{
		memcpy(&_gModuleSetVal[i],&val[i],sizeof(tagModuleSetValStruct));
	}
	AtCmdAnalysis_EERom_Story();
	AtCmd_RECV_WCDMA_Init();		//重启设备
		
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
	return 0;
}
