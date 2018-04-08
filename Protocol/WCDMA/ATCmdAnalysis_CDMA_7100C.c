#include "config.h"
#include "ATCmdAnalysis_Env.h"
#include "MsgUtf8String.h"
/******************************************************************************/
#define SYSTEM_MAP_ENABLE		1
static tagATCmdSetInfo* 		_gATCmdSetInfoPtr 	= NULL;
static tagModuleSetValStruct*	_gModuleSetVal 		= NULL;
#if SYSTEM_MAP_ENABLE
static tagSystemSetValStruct* 	_gSystemSetValPtr	= NULL;
#endif
static int 						_gModuleCount 		= 0;
static tagModeRunStateStruct*	_gModeRunStatePtr;
static tagAtCmdRunCtrlItem* 	_gAtCmdRunCtrlArray;
static LinkQueue*				_gLinkQueue;

static tagCnsModSet _gCnsModSet[] =
{
	{0,STRING_NO_SERVICE,"CMNET"},
	{2,"CDMA,","CMNET"},
	{4,"HDR","CMNET"},
	{8,"HYBRID","CMNET"}
};
#define CNSMODSET_SIZE	sizeof(_gCnsModSet)/sizeof(tagCnsModSet)

/******************************************************************************/

#define _gModeRunState		(*_gModeRunStatePtr)
#define _gATCmdSetInfo		(*_gATCmdSetInfoPtr) 
#define _gSystemSetVal		(*_gSystemSetValPtr) 
#define PResFreeQueue		_gLinkQueue[0]			//空闲的队列
#define	PResFullQueue		_gLinkQueue[1]	   		//繁忙的队列
#define	PResFreeQueuePtr	&PResFreeQueue
#define	PResFullQueuePtr	&PResFullQueue

/******************************************************************************/

ATCMDRECV_FUN(CMGL);
ATCMDRECV_FUN(CSQ);
ATCMDRECV_FUN(CNSMOD);
ATCMDRECV_FUN(CGMR);
ATCMDRECV_FUN(COPS);
//ATCMDRECV_FUN(CPMS);
//ATCMDRECV_FUN(CGSOCKCONT);
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
ATCMDRECV_FUN(CCLK);

/******************************************************************************/

ATCMDGROUP_CALLBACK_FUN(ConnectNet);
ATCMDGROUP_CALLBACK_FUN(DeviceSend);
ATCMDGROUP_CALLBACK_FUN(Init);

/******************************************************************************/

/*查询环*/
ATCMDGROUP_FUN(DoLoop);
/*重启模块*/
ATCMDGROUP_FUN(Restart);
/*获取模块的信息*/
ATCMDGROUP_FUN(GetModuleInfo);
/*模块初始化*/
ATCMDGROUP_FUN(Init);
/*读取短信*/
ATCMDGROUP_FUN(ReadMsg);
/*发送短信*/
ATCMDGROUP_FUN(SendMsg);
/*联网*/
ATCMDGROUP_FUN(ConnectNet);
/*断开网络*/
ATCMDGROUP_FUN(DisConnectNet);
/*从采集口获得信息并向服务端发送数据*/
ATCMDGROUP_FUN(DeviceSend);
/*发送简短的消息，一般是心跳*/
ATCMDGROUP_FUN(InfoSend);
/*设置3g版本号*/
ATCMDGROUP_FUN(Set3GNetVersion);

/******************************************************************************/

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
//	AT_CMD_SET_CNAOP 		=	 29,  //设置网络顺序
	AT_CMD_GET_CNSMOD 		=	 30,  //查询网络系统模式
	AT_CMD_REV_MSMNOTE		=	 31,  //收到短信数据
	AT_CMD_SET_ATE 			=	 32,  //开启关闭回显
	AT_CMD_SET_CIPCCFG		=	 33,  //配置参数的套接字
	AT_CMD_SET_CMGS			=	 34,  //发送短信+电话号码
	AT_CMD_SET_CMGSDATA		=	 35,  //发送短信数据体
	AT_CMD_GET_CMGRO		=	 36,  //查询短信
	AT_CMD_GET_CCLK			=	 37,  //查询时间
	AT_CMD_GET_CGMR			=	 38,  //查询版本号
	AT_CMD_SET_CSOCKSETPN	=	 39,  //改变PDP模式	
	AT_CMD_GET_COPS			=		40,	//查询注册网络状态
	AT_CMD_SET_CNVW			=	41,	//设置
	AT_CMD_GET_CGREG		=42,		// GPRS状态上报
	AT_CMD_GET_IPADDR		=43,		//查询IP地址
//	AT_CMD_GET_CGATT		=44,		//GPRS状态查询
	AT_CMD_SET_FSCD			=	45,	//配置当前目录
	AT_CMD_SET_FSMKDIR		=	46,	//创建目录
	AT_CMD_SET_FSRMDIR		=	47,	//删除目录
	AT_CMD_SET_FSLS			=	48,	//文件列表
	AT_CMD_SET_CSOCKAUTH		=	49,	//设置APN
	AT_CMD_SET_CIPMODE		=	50,	//设置透传模式
	AT_CMD_SET_CIPSENDMODE	=	51,	//设置发送模式
	AT_CMD_SET_CATR			=	52,	//设置通讯方式
	AT_CMD_GET_CSUB			= 	53,	//
	AT_CMD_SET_CGREG			=	54,	//设置GPRS上报状态
	AT_CMD_SET_IPREX			=	55,	//设定波特率
	AT_CMD_MAX
}AT_CMD;

typedef enum {
	WIRELESS_ERROR_SIM,		//SIM卡检测错误
	WIRELESS_ERROR_NET 		//网络检测错误
}WIRELESS_ERROR;	//无线模块错误

/******************************************************************************/
static const tagATCmd	 _gATCmd[]={
//编号						发送头缓存						应答头缓存		失败应答头		等待应答时间	重发次数	响应函数
{AT_CMD_TEST_AT,         {"AT"},	                        {"OK",""},		{""},		    1000,	       	5,			NULL	,true},	//测试CDMA是否有返回
{AT_CMD_SET_CFUN,        {"AT+CFUN?"},	                 	{"OK",""},		{""},    1000,	       	5,			NULL    		,true},	//设置运行模式
{AT_CMD_GET_CSQ,         {"AT+CSQ"},	                    {"+CSQ:",""},	{""},	1000,	       	5,			ATCMDRECV_NAME(CSQ)   ,true },	//查询网络信号
{AT_CMD_GET_CREG,        {"AT+CREG?"},	                 	{"+CREG: ",""}, {""},  1000,	       	5,			NULL    		,true},	//网络注册信息
{AT_CMD_GET_CICCID,      {"AT+CICCID"},	                 	{"+ICCID: ",""},{""},	1000,	       	5,			NULL    		,true},	//查询UIM序列号
{AT_CMD_GET_IPR,         {"AT+IPR?"},	                 	{"+IPR: ",""},	{""},    1000,	       	5,			NULL    		,true},	//查询波特率
{AT_CMD_SET_IPR,         {"AT+IPR="},	                 	{"OK",""},	    {""},    1000,	       	5,			NULL    		,true},	//设置波特率
{AT_CMD_GET_CCLK,		 {"AT+CCLK?"},						{"+CCLK:","OK"},{""},		1000,		1,			ATCMDRECV_NAME(CCLK)			,true},	//读取系统时间
//******************************************短信*******************************************************************
{AT_CMD_SET_CMGF,        {"AT+CMGF="},	                 	{"OK",""},		{""},    1000,	       	5,			NULL    		,true},	//设置短信格式 0,PDU模式 1,text模式
{AT_CMD_SET_CSCS,        {"AT+CSCS="},			            {"OK",""},		{""},    1000,	       	5,			NULL    		,true},	//选择TE字符集GSM、UCS2、IRA
{AT_CMD_STE_CPMS,       {"AT+CPMS=\"ME\",\"ME\",\"ME\""}, 	{"+CPMS",""},	{""},	1000,	       	5,			/*ATCMDRECV_NAME(CPMS)*/NULL  ,true},	//短信存储位置选择命令
{AT_CMD_SET_CNMI,       {"AT+CNMI=2,1"},	  	            {"OK",""},		{""},    1000,	       	5,			NULL    		,true},	//设置新短消息提示功能 2,1:存储短信 1,2:不存储短信，新短信直接打印到串口
{AT_CMD_SET_CSMP,       {"AT+CSMP=17,167,0,8"},         	{"OK",""},		{""},    1000,	       	5,			NULL    		,true},	//设置TEXT模式参数，最后一个为数据编码类型，0表示默认字符集(GSM) 241:字符 25:中文
{AT_CMD_SET_CMGSO,       {"AT+CMGSO="},	                 	{"OK",""},		{"ERROR"},    10000,	       	2,			NULL    		,true},	//快速发信息
{AT_CMD_SET_CMGS,		{"AT+CMGS="},						{">",""},		{""},	500,			1,			NULL			,true},
{AT_CMD_SET_CMGSDATA,	{""},								{"+CMGS","OK"},	{""},	30000,			1,			NULL			,true},
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
{AT_CMD_SET_CGSOCKCONT, {"AT+CGSOCKCONT"},      				{"OK","+CGSOCKCONT:"},		{"ERROR"},   1000,	       	1,			NULL    		,true},	//设置APN 
//{AT_CMD_SET_NETOPEN,    {"AT+NETOPEN=,,1"},               	{"+NETOPEN: 0","Network opened"},{"ERROR"},		    30000,	       	2,			NULL    ,true},	//多链接方式
{AT_CMD_SET_NETOPEN,    {"AT+NETOPEN"},               	{"+NETOPEN: 0",""},{"ERROR"},		    20000,	       	2,			NULL    ,true},	//多链接方式
{AT_CMD_SET_CIPOPEN,    {"AT+CIPOPEN="},	                {"+CIPOPEN:","+IP ERROR: Connection is already created"},{"ERROR"},	5000,	       	1,			ATCMDRECV_NAME(CIPOPEN)    ,true},	//建立一路TCP/IP 连接 
{AT_CMD_SET_CIPSEND,    {"AT+CIPSEND="},	                {">",""},		  {""},  2500,	       	1,			NULL    		,true},	//在一路TCP/IP 下发送数据(长度) 
{AT_CMD_SET_TCPWDATA,   {""},	                         	{"+CIPSEND:",""},	{"ERROR"},	30000,	       	1,			NULL    	,true},	//在一路TCP/IP 下发送数据(数据体) 
{AT_CMD_SET_NETCLOSE,  {"AT+CIPCLOSE="},	             	{"OK",""},		{""},    1000,	       	1,			NULL    		,true},	//关闭一路TCP/IP连接
{AT_CMD_GET_CIPOPEN,    {"AT+CIPOPEN?"},	                {"OK",""},      {""},   500,	       	1,			NULL    		,false},	//查询网络连接状态 
{AT_CMD_REV_NETDATA,    {""},	                         	{"+IPD",""},	{""},	1000,	       	0,			NULL    		,true},	//收到网络数据
//******************************************************************************************************************
{AT_CMD_GET_CNUM,       {"AT+CNUM"},	                    {"+CNUM:",""},	{""},	1000,	       	5,			NULL    		,true},	//查询本机号码	 
//{AT_CMD_SET_CNAOP,      {"AT+CNAOP=0"},	                 	{"OK",""},		{""},   1000,	       	5,			NULL    		,true},	//设置网络顺序0、Automatic	1、GSM,WCDMA  2、WCDMA,GSM  
{AT_CMD_GET_CNSMOD,     {"AT+CNSMOD?"},	                 	{"+CNSMOD:",""},{""},   1000,	       	5,			ATCMDRECV_NAME(CNSMOD),true},	//查询网络系统模式
{AT_CMD_SET_ATE,        {"ATE0"},	                     	{"OK",""},      {""},   1000,	       	5,			NULL    		,true},	//开启关闭回显数据
{AT_CMD_SET_CIPCCFG,    {"AT+CIPCCFG=10,0,0,1,1,0,500"},	     	{"OK",""},      {""},   1000,	       	5,			NULL    		,true},  //配置参数的套接字
{AT_CMD_GET_CGMR,    	{"AT+CGMR"},	     				{"+CGMR:","OK"},      {""},   1000,	       	2,			ATCMDRECV_NAME(CGMR)   		,true},  //获取版本号
{AT_CMD_SET_CSOCKSETPN,	{"AT+CSOCKSETPN="},					{"OK",""},		{""},	1000,			2,			NULL			,true},
{AT_CMD_GET_COPS,				{"AT+COPS?"},								{"+COPS:",""},{""},	1000,			2,			ATCMDRECV_NAME(COPS),			true},
{AT_CMD_SET_CNVW,		{"AT+CNVW=1030,0,"},				{"+CNVW:1",""},	{"+CNVW:0"},	1000,	1,			NULL			,true},	//设置3G版本
{AT_CMD_GET_CGREG,		{"AT+CGREG"},				{"+CGREG: 1,1",""},{""},	10000,	4,		NULL,			true},	//获取GPRS状态
{AT_CMD_GET_IPADDR,		{"AT+IPADDR"},				{"+IPADDR:",""},{"+IP ERROR:"},	1000,	1,		NULL,			true},	//获取IP地址
//{AT_CMD_GET_CGATT,		{"AT+CGATT=1"},			{"OK",""},	{"ERROR"},			10000,		3,	NULL,		true}
{AT_CMD_SET_CSOCKAUTH,	{"AT+CSOCKAUTH"},			{"OK",""},	{"ERROR"},		1000,		1,			NULL,			true},//设置APN
{AT_CMD_SET_CIPMODE,		{"AT+CIPMODE"},				{"OK",""},	{"ERROR"},		1000,		1,	NULL,	true},		//设置透传模式 
{AT_CMD_SET_CIPSENDMODE,	{"AT+CIPSENDMODE"},			{"OK",""},	{"ERROR"},		1000,		1,	NULL,	true},
{AT_CMD_SET_CATR,			{"AT+CATR"},				{"OK",""},	{"ERROR"},		1000,		1,	NULL,	true},
{AT_CMD_GET_CSUB,			{"AT+CSUB"},				{"OK",""},	{"ERROR"},		1000,		1,	NULL,	true},
{AT_CMD_SET_CGREG,		{"AT+CGREG=1"},				{"OK",""},	{"ERROR"},	1000,	2,	NULL,	true},
{AT_CMD_SET_IPREX,		{"AT+IPREX="},				{"OK",""},	{"ERROR"},	1000,	2,	NULL,	true}
//******************************************************************************************************************* 
};

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

//初始化配置表
const static  tagAtCmdRunCtrlItem _gAtCmdRunFunTypeArray[ATCMD_RUN_TYPE_MAX] =
{
	/*命令组编号										命令组执行函数						命令组参数	参数个数	命令组执行状态				状态锁*/
	{ATCMD_RUN_TYPE_RESTART,							ATCMDGROUP_NAME(Restart),		"",			0,			ATCMDGROUP_RUN_STATUS_OFF,  false},
	{ATCMD_RUN_TYPE_GET_MODULE_INFO,			ATCMDGROUP_NAME(GetModuleInfo),"",	0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{ATCMD_RUN_TYPE_INIT, 								ATCMDGROUP_NAME(Init),			"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{ATCMD_RUN_TYPE_SET3GNETVERSION,					ATCMDGROUP_NAME(Set3GNetVersion),	"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{ATCMD_RUN_TYPE_READ_MSG,							ATCMDGROUP_NAME(ReadMsg),			"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{ATCMD_RUN_TYPE_SEND_MSG,							ATCMDGROUP_NAME(SendMsg),			"",			0,			ATCMDGROUP_RUN_STATUS_OFF,  false},
	{ATCMD_RUN_TYPE_DISCONNECT_NET,						ATCMDGROUP_NAME(DisConnectNet),		"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{(ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_CONNECT_NET + 0),	ATCMDGROUP_NAME(ConnectNet),			"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{(ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_CONNECT_NET + 1),	ATCMDGROUP_NAME(ConnectNet),			"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{(ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_DEVICE_SEND +0), 	ATCMDGROUP_NAME(DeviceSend),			"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{(ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_DEVICE_SEND +1),	ATCMDGROUP_NAME(DeviceSend),			"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{(ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_INFO_SEND +0),		ATCMDGROUP_NAME(InfoSend),				"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
	{(ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_INFO_SEND +1),		ATCMDGROUP_NAME(InfoSend),				"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false},
 	{ATCMD_RUN_TYPE_LOOP,								ATCMDGROUP_NAME(DoLoop),				"",			0,			ATCMDGROUP_RUN_STATUS_OFF,	false}
};

/******************************************************************************/

#define AT_CMD_COUNT	sizeof(_gATCmd)/sizeof(tagATCmd)	
#define RECV_AT_CMD_SIZE	(sizeof(_gtagATCmdRecvProcessItemArray)/sizeof(tagATCmdRecvProcessItem))

/******************************************************************************/

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
	LINE_LOG();
	if(argc < 2)
		return -1;
	enable = (BOOL)atoi(argv[0]);
	port =  atoi(argv[1]);
	LINE_LOG();

	if(port < _gModuleCount)
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
					//AtCmdAnalysisi_DisConnect(port);
				}
			}
		}
	}
	
	if(_gSystemSetVal.sysMountMapStatus == SYSTEM_MOUNT_MAP_STATUS_ON && port == _gSystemSetVal.sysSetValCtrl.controlPort)
	{
		if(enable)
		{
			API_Led_Set(LED_TYPE_2, LED_STATE_ON);
	  		TIMER_RUN(DeviceBeat);
		}
		else
		{
			
		}
	}
	
	LINE_LOG();	
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
	LINE_LOG();
	if(argc < 1)
		return -1;
	for(index = 0; index < _gModuleCount; index++)
	{
		if(_gModeRunState.NetIsOpen[index] > 0)
		{
			_gModeRunState.NetIsOpen[index]--;
		}
	}
	LINE_LOG();

	API_SetTimer(20L,1,TIMER_TYPE_WCDMA_AT_SEND,NULL,TIMER_NAME(StartToSendAtCmd));
	LINE_LOG();
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
	LINE_LOG();
	_gModeRunState.SimCard = SIM_CARD_STATUS_ERROR;	
	API_SetTimer(60000,-1,TIMER_TYPE_WCDMA_AT_RESTART,TIMER_NAME(StartOk),NULL);
	AtCmdAnalysis_ErrorMsg(WCDMA_ERROR_NO_SIM_CARD,"",0);
	LINE_LOG();
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
	LINE_LOG();
	if(_gATCmdSetInfo.atCmdRunStatus != ATCMD_RUN_STATUS_NOT_RUN)
	{
		char* str = "Unkown";
		AtCmdAnalysis_ErrorMsg(WCDMA_ERROR_MODULE_RESTART,str,strlen(str));
	}
	_gATCmdSetInfo.atCmdRunStatus = ATCMD_RUN_STATUS_NOT_RUN;
	API_SetTimerFast(60000,-1,TIMER_TYPE_WCDMA_AT_RESTART,TIMER_NAME(StartOk),NULL);
	LINE_LOG();
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
	DEBUG("PbDone ==> Ok\n");
	LINE_LOG();
	if(_gModeRunState.SimCard == SIM_CARD_STATUS_OK
		&&_gATCmdSetInfo.atCmdRunStatus == ATCMD_RUN_STATUS_NOT_RUN)
	{
		RUN_ATCMD_GROUP(ATCMD_RUN_TYPE_GET_MODULE_INFO);
	}
	LINE_LOG();
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
	LINE_LOG();
	_gModeRunState.SimCard = SIM_CARD_STATUS_OK;
	LINE_LOG();	
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
	LINE_LOG();
	p = GetStrFromTwoKey((char*)Info, ",",",",buf ,sizeof(buf));
	LINE_LOG();
	if(p != NULL)
	{
		port = atoi(buf);	//接收的端口号
		p = InStr(p,",",32);
		p++;
		//p = GetStrFromTwoKey(p, ",","\0",buf ,sizeof(buf));
		if(p != NULL)
		{
			length= atoi(p);	//数据长度
			#if SYSTEM_MAP_ENABLE
			if(_gSystemSetVal.sysMountMapStatus == SYSTEM_MOUNT_MAP_STATUS_ON)
			{
				if(_gSystemSetVal.sysSetValCtrl.controlPort == port)
				{
					//功能被开启
					if(_gSystemSetVal.sysSetValCtrl.mapDeviceID < MOUNT_DEVICE_COUNT)
					{
						port = _gSystemSetVal.sysSetValCtrl.mapDeviceID;
						//_gSystemSetVal.sysMountMapStatus = SYSTEM_MOUNT_MAP_STATUS_ON;	//开启映射模式
						API_SetTimerFast(_gSystemSetVal.sysSetValCtrl.timeOut*1000,1,TIMER_TYPE_WCDMA_SYSTEM_MAP,TIMER_NAME(SystemMap),NULL);
					}
					else
					{
						return -1;
					}

				}
				else if(_gSystemSetVal.sysSetValCtrl.mapDeviceID == port)
				{
					return -1;
				}
			}
			#endif
			_gATCmdSetInfo.RecvFunction = 	(USART_FUNCTION)(port + USART_FUNCTION_MOUNT);
			SEL_ANLYSIS_MODE(PROTOCOL_ANALYSIS_MODE_RECEIVE,length);
			return 0;
		}
	}
	LINE_LOG();
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
	LINE_LOG();
	#if SYSTEM_MAP_ENABLE
	if(_gSystemSetVal.sysSetValCtrl.controlPort== portInt)
	{
		//if(_gSystemSetVal.sysMountMapStatus == SYSTEM_MOUNT_MAP_STATUS_ON)
		//检测到系统连接开启
		l = ReadStrLen((char*)(Info+10));
		if(l > 10)
		{
			if(_gSystemSetVal.sysMountMapStatus == SYSTEM_MOUNT_MAP_STATUS_ON
				&& _gSystemSetVal.NetIsOpen != MAX_NET_IS_OPEN_NUM )
			{
				_gSystemSetVal.NetIsOpen = MAX_NET_IS_OPEN_NUM;
				START_CMD(EnableHeartBeat);
				ADD_ARGV_BOOL(true);
				ADD_ARGV_INT(portInt);
				END_CMD();
			}
		}
		else if(l > 3 && l<= 10)
		{
			return 1;
		}
		else
		{
			if(_gSystemSetVal.NetIsOpen > 0)
			{
				_gSystemSetVal.NetIsOpen--;
			}
		}

	}
	else
	{
		if(portInt >= _gModuleCount || portInt < 0) {
			return -1;
		}
		
		LINE_LOG();
		 
		if(_gModuleSetVal[portInt].deviceFun != 1)
		{
			LINE_LOG();
			return -1;
		}
	}
	#else
	if(portInt >= _gModuleCount || portInt < 0)
		return -1;
	LINE_LOG();
	 
	if(_gModuleSetVal[portInt].deviceFun != 1)
	{
		return -1;
	}
	#endif
	LINE_LOG();
	/*将帧尾回车符更换成字符串结束符*/
	/*判断字符长度,如果字符长度大于2,说明此断开已经建立连接*/
	l = ReadStrLen((char*)(Info+10));
	LINE_LOG();
	if(l > 10)
	{ 
		ret = true;
	}
	else if(l > 3 && l<= 10)
	{
		return 1;
	}
	else
	{	
		ret = false;
	}
	LINE_LOG();
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
				//RUN_ATCMD_GROUP((ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_DISCONNECT_NET + portInt));	
				AtCmdAnalysisi_DisConnect(portInt);
				LINE_LOG();
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
			LINE_LOG();
			for(index = 0; index < _gModuleCount; index++)
			{
				if(_gModeRunState.NetIsOpen[index] > 0 && _gModuleSetVal[index].deviceFun == 1)
					break;
			}
			//如果
			if(index >= _gModuleCount)
			{
				char buf[30] = {0};
				_gModeRunState.NetIsOpen[portInt] = MAX_NET_IS_OPEN_NUM-1;
				sprintf(buf,"%d",portInt);
				AtCmdAnalysis_ErrorMsg(WCDMA_ERROR_NET_DISCONNECT,buf,strlen(buf));
				AtCmd_RECV_WCDMA_Init();
			}
			//重启设备			
		}
	}
	LINE_LOG();
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
	LINE_LOG();
	port=InStr((char*)Info, ",",len);
	if(port == NULL)
		return -1;
	port++;
	DEBUG("IpClose ==> Ok\n");
	START_CMD(EnableHeartBeat);
	ADD_ARGV_BOOL(false);
	ADD_ARGV(port);
	END_CMD();
	LINE_LOG();
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
	LINE_LOG();
	DEBUG("SIM Check ==>Failed\n");
	START_CMD(SIMFailed);
	ADD_ARGV_INT(WIRELESS_ERROR_SIM);
	END_CMD();
	LINE_LOG();
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
	LINE_LOG();
	if(InStr((char*)Info,"Connection is already created",len) == NULL)
	{	
	 	START_CMD(NoticeError);
		ADD_ARGV_INT(WIRELESS_ERROR_NET);
		END_CMD();
	}
	else
	{
		  
	}
	LINE_LOG();
	return 0;//API_Wireless_NoticeError(WIRELESS_ERROR_NET);
}

// 函数名称  : ATCMDRECV_FUN(CPMS)
// 功能描述  : 接收查询到有短信数据
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

//ATCMDRECV_FUN(CPMS)
//{
//	char* p = NULL;
//	int count;
//	LINE_LOG();
//	p = (char*)(Info+7);
//	count = String_To_Int(&p , ",");
//	/*有短信息*/
//	if(count>0 && _gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
//	{
//		RUN_ATCMD_GROUP(ATCMD_RUN_TYPE_READ_MSG);		
//	}
//	LINE_LOG();
//	return 0;
//}

//// 函数名称  : ATCMDRECV_FUN(CGSOCKCONT)
//// 功能描述  : APN设置
//// 输入参数  : None
//// 输出参数  : None
//// 返回参数  : None
////※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

//ATCMDRECV_FUN(CGSOCKCONT)
//{
//	char* p = NULL;
//	LINE_LOG();
//	p = (char*)(Info+7);

//	if((StrNcmp((uchar*)Info,(uchar*)"+CGSOCKCONT:") == 0))
//	{
//		char apn[23];
//		p = InStr((char*)Info,",",30);
//		if(p != NULL)
//		{
//			p++;
//			p = GetStrFromTwoKey(p,",\"","\",",apn,20);
//			if(p != NULL)
//			{
//				if(_gModeRunState.CnsMod <  CNSMODSET_SIZE
//					&& _gModeRunState.CnsMod >= 0)
//				{
//					if(!StrNcmp((uchar*)apn,(uchar*)_gCnsModSet[_gModeRunState.CnsMod].apn) == 0)
//					{
//						//如果不相等
//						_gATCmdSetInfo.apnStatus = -1;
//						
//					}
//				}
//				//MAX_CARRIEROPERATOR_NAME_SIZE
//			}
//		}
//		
//	}
//	LINE_LOG();
//	return 0;
//}


// 函数名称  : ATCMDRECV_FUN(COPS)
// 功能描述  : 查询运行商
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
ATCMDRECV_FUN(COPS)
{
	char* p = NULL;

	LINE_LOG();

	p = GetStrFromTwoKey((char*)Info,"\"","\"",_gATCmdSetInfo.carrierOperator,MAX_CARRIEROPERATOR_NAME_SIZE);
	if(p != NULL)
	{
		//MAX_CARRIEROPERATOR_NAME_SIZE
	}	
	LINE_LOG();

	return 0;
}
// 函数名称  : ATCMDRECV_FUN(CGMR)
// 功能描述  : 查询固件版本号
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(CGMR)
{
	if(!(StrNcmp((uchar*)Info,(uchar*)"OK") == 0))
	{
		int Start = 30;
		while(*Info != ':' && Start > 0)
		{
			Start--;
			Info++;
		}
		//找到起点;
		if(*Info != ':')
			return -1;
		Info ++;
		Start = 0;
		while(Info[Start] != '\0' && Start < 49)
		{
			_gATCmdSetInfo.wirelssModuleVision[Start] =  Info[Start];
			Start++;
		}
		_gATCmdSetInfo.wirelssModuleVision[Start] ='\0';
		return 0;
	}
	return -1;
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
	LINE_LOG();
	p=InStr((char*)Info,",",len);
	if(p!=NULL)
	{
		p++;
		_gModeRunState.CnsMod  = String_To_Int(&p,"\0");
	}
	LINE_LOG();
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
	LINE_LOG();
	if(GetStrFromTwoKey((char*)Info, ":",",",buf ,len) != NULL)
	{
		csq = atoi(buf);
		csq = csq > 98?0:csq;
		csq = csq < 0?0:csq;
		_gModeRunState.CSQ =  csq;
	}
	LINE_LOG();
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
	LINE_LOG();
	if(!(StrNcmp((uchar*)Info,(uchar*)"OK") == 0))
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
			SEL_ANLYSIS_MODE(PROTOCOL_ANALYSIS_MODE_SHORT_MESSAGE,-1);
			memcpy(_gATCmdSetInfo.phoneNum,Info,Start);
		}
		_gATCmdSetInfo.haveShortMsg = true;
	}
	LINE_LOG();
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
	LINE_LOG();
	SEL_ANLYSIS_MODE(PROTOCOL_ANALYSIS_MODE_SHORT_MESSAGE,-1);
	GetStrFromTwoKey((char*)Info, ",",",",_gATCmdSetInfo.phoneNum ,len);
	LINE_LOG();
	return 0;	
}
// 函数名称  : ATCMDRECV_FUN(CCLK)
// 功能描述  : 端口连接状态
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDRECV_FUN(CCLK)
{
	LINE_LOG();
	GetStrFromTwoKey((char*)Info, "\"","\"",_gATCmdSetInfo.timeStr ,30);
	LINE_LOG();
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
	LINE_LOG();
	return 0;
}

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
	LINE_LOG();
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		char StrTempBuf[100] = {0};
		int deviceIndex = 0;
		char* buf =  _gAtCmdRunCtrlArray[ATCMD_RUN_TYPE_DISCONNECT_NET].funArg;

		if(ReadStrLen(buf) < 1)
			return -1;
		deviceIndex = atoi(buf);
		if(deviceIndex >= _gModuleCount )
			return -1;
		LINE_LOG();	  
		sprintf(StrTempBuf,"%d",deviceIndex);

#if SYSTEM_MAP_ENABLE
		if(_gSystemSetVal.sysSetValCtrl.mapDeviceID == deviceIndex
			&& _gSystemSetVal.sysMountMapStatus == SYSTEM_MOUNT_MAP_STATUS_OFF
			&& _gSystemSetVal.NetIsOpen  == MAX_NET_IS_OPEN_NUM
		)
		{
			//如果系统连接也断开了，那么同时连接系统配置
			sprintf(StrTempBuf,"%d",_gSystemSetVal.sysSetValCtrl.controlPort);
			AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_NETCLOSE,StrTempBuf,ReadStrLen(StrTempBuf));


			//在连接目标网络的同时连接
		}
		else
		{
			//只连接原先的设备
			if(_gModeRunState.NetIsOpen[deviceIndex] < MAX_NET_IS_OPEN_NUM)
			{
				AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_NETCLOSE,StrTempBuf,ReadStrLen(StrTempBuf));
			}
		}
#else
		AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_NETCLOSE,StrTempBuf,ReadStrLen(StrTempBuf));
#endif		
		
		//AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_NETCLOSE,StrTempBuf,strlen(StrTempBuf));
		AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CIPOPEN,"",0);
		TIMER_RUN(StartToSendAtCmd);
		return 0;

	}
	LINE_LOG();
	return -1;

}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_CALLBACK_FUN(ConnectNet)
// 功能描述  : 联网
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDGROUP_CALLBACK_FUN(ConnectNet)
{
	int deviceIndex = 0;
	LINE_LOG();
	deviceIndex = _gATCmdSetInfo.atCmdGroup - ATCMD_RUN_TYPE_CONNECT_NET;
	if(errType != SEND_ERROR_TYPE_NONE)
	{
		//如果检测到错误
	}
	else
	{

	 	//如果没有检测到错误,继续发送数据
	}
	if(deviceIndex < _gModuleCount && deviceIndex >= 0)
	{
		_gModuleSetVal[deviceIndex].tryCount = 2;
	//	DEBUG("set :%d count:%d\n",deviceIndex,_gModuleSetVal[deviceIndex].tryCount );
	}

	LINE_LOG();

	return 0;
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
	LINE_LOG(); 
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		char StrTempBuf[100] = {0};
		int deviceIndex = 0;

		if(_gATCmdSetInfo.atCmdGroup < ATCMD_RUN_TYPE_CONNECT_NET || _gATCmdSetInfo.atCmdGroup >= ATCMD_RUN_TYPE_CONNECT_NET + _gModuleCount)
			return -1; 
		deviceIndex = _gATCmdSetInfo.atCmdGroup - ATCMD_RUN_TYPE_CONNECT_NET;

		if(deviceIndex >= _gModuleCount )
			return -1;
	  	AtCmdAnalysis_SetATCmdSendCallBack(ATCMDGROUP_CALLBACK_NAME(ConnectNet));	//设置发送成功后的回调函数

		
#if SYSTEM_MAP_ENABLE
		if(_gSystemSetVal.sysSetValCtrl.mapDeviceID == deviceIndex
			&& _gSystemSetVal.sysMountMapStatus == SYSTEM_MOUNT_MAP_STATUS_ON)
		{
							//正常运行				

			if(_gSystemSetVal.NetIsOpen < MAX_NET_IS_OPEN_NUM)
			{
				//如果系统连接也断开了，那么同时连接系统配置
				sprintf(StrTempBuf,"%d,\"TCP\",\"%s\",%d",_gSystemSetVal.sysSetValCtrl.controlPort,_gSystemSetVal.sysSetValCtrl.IPAddress,
		                                         _gSystemSetVal.sysSetValCtrl.Port);
				AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CIPOPEN,StrTempBuf,ReadStrLen(StrTempBuf));
			}
			//如果映射端口和需要连接的端口一致
			//先连接原先的设备
			if(_gModeRunState.NetIsOpen[deviceIndex] < MAX_NET_IS_OPEN_NUM
					&& _gModuleSetVal[deviceIndex].deviceFun == 1)
			{
				sprintf(StrTempBuf,"%d,\"TCP\",\"%s\",%d",deviceIndex,_gModuleSetVal[deviceIndex].IPAddress,
		                                         _gModuleSetVal[deviceIndex].Port);
					//如果原先的端口还没有连接上，先连接原先的端口
				AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CIPOPEN,StrTempBuf,ReadStrLen(StrTempBuf));
			}


			//在连接目标网络的同时连接
		}
		else
		{
			sprintf(StrTempBuf,"%d,\"TCP\",\"%s\",%d",deviceIndex,_gModuleSetVal[deviceIndex].IPAddress,
		                                         _gModuleSetVal[deviceIndex].Port);
			//只连接原先的设备
			if(_gModeRunState.NetIsOpen[deviceIndex] < MAX_NET_IS_OPEN_NUM)
			{
				AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CIPOPEN,StrTempBuf,ReadStrLen(StrTempBuf));
			}
		}
#else
		sprintf(StrTempBuf,"%d,\"TCP\",\"%s\",%d",deviceIndex,_gModuleSetVal[deviceIndex].IPAddress,
		                                         _gModuleSetVal[deviceIndex].Port);
		AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CIPOPEN,StrTempBuf,ReadStrLen(StrTempBuf));
#endif
		AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CIPOPEN,"",0);
		TIMER_RUN(StartToSendAtCmd);
		LINE_LOG();
		return 0;
	}
	LINE_LOG(); 
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
	LINE_LOG();
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CMGRD,"0",1);
		TIMER_RUN(StartToSendAtCmd);
		LINE_LOG();
		return 0;
	}
	LINE_LOG();
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
	LINE_LOG();
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		const int shortMsgMaxSize = 70 * 4;		//单条短信最大长度是70个字符,一个字符占用4个HEX 
	   	int size = 0;
		int phoneNumSize = 0;
		int index = 0;
		static char MsgEndChar = 0x1a;
		phoneNumSize = strlen(_gATCmdSetInfo.sendPhoneNum);
		size = ReadStrLen(_gATCmdSetInfo.msgBuffer);
		LINE_LOG();
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
			Msg_SendStatus_Notice(1);
		}
		LINE_LOG();
		TIMER_RUN(StartToSendAtCmd);
		return 0;
	}
	LINE_LOG();
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
	LINE_LOG();
	deviceIndex = _gATCmdSetInfo.atCmdGroup - ATCMD_RUN_TYPE_DEVICE_SEND;
	if(errType != SEND_ERROR_TYPE_NONE)
	{
		API_ResetProtocolBuffer((USART_FUNCTION)(deviceIndex + USART_FUNCTION_MOUNT),RESETRTBUFF_TYPE_AND_INT_RX);
		//如果检测到错误
	}
	else
	{
		LINE_LOG();
		//让程序连续执行发送命令直到完成
		if(_gATCmdSetInfo.atCmdGroup > 0)
		{
			_gATCmdSetInfo.atCmdGroup--;
		}
		else
		{
			_gATCmdSetInfo.atCmdGroup = (ATCMD_RUN_TYPE)(ATCMD_RUN_TYPE_MAX - 1);
		}
		AtCmdAnalysis_Net_DeviceSend(deviceIndex);
	 	//如果没有检测到错误,继续发送数据
	}
	LINE_LOG();
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
	int deviceIndex = 0;
	if(_gATCmdSetInfo.atCmdGroup < ATCMD_RUN_TYPE_DEVICE_SEND || _gATCmdSetInfo.atCmdGroup >= ATCMD_RUN_TYPE_DEVICE_SEND + _gModuleCount)
		return -1; 

	deviceIndex = _gATCmdSetInfo.atCmdGroup - ATCMD_RUN_TYPE_DEVICE_SEND;	
	LINE_LOG();
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		char StrTempBuf[100] = {0};
		int sendPortID = 0;

		uint16_t size = 0;
		uint16_t retSize =0;
		tagHugeSendBuffer hugeBuffer;

		sendPortID = deviceIndex;
		if(_gModeRunState.NetIsOpen[deviceIndex] < MAX_NET_IS_OPEN_NUM
			#if SYSTEM_MAP_ENABLE
			&& (//_gSystemSetVal.sysSetValCtrl.deviceFun == 0 || 
				_gSystemSetVal.NetIsOpen < MAX_NET_IS_OPEN_NUM
				|| _gSystemSetVal.sysMountMapStatus == SYSTEM_MOUNT_MAP_STATUS_OFF
				|| _gSystemSetVal.sysSetValCtrl.mapDeviceID != deviceIndex
			
				)
			#endif 
			)
		{
			//在设备处于断开的状态，且这个口没有被映射，或者没有连接状态下，那么直接返回
			//API_KillTimer(TIMER_TYPE_WCDMA_SEND);
			//API_KillTimer((TIMER_TYPE)(TIMER_TYPE_WCDMA_DEVICE_SEND + deviceIndex));
			//API_ResetProtocolBuffer((USART_FUNCTION)(deviceIndex + USART_FUNCTION_MOUNT),RESETRTBUFF_TYPE_AND_INT_RX);
			goto DEVICE_SEND_ERROR;
			//return -1;
		}
		//存在发送数据的条件
		
		//读取待发送的数据长度
	  	size = Netdata_Protocol_ReadSize(deviceIndex,MAX_SEND_DATA_SIZE_ONCE);
		if(size == 0) {
			//数据已经清空了
			//API_KillTimer(TIMER_TYPE_WCDMA_SEND);
			//API_KillTimer((TIMER_TYPE)(TIMER_TYPE_WCDMA_DEVICE_SEND + deviceIndex));
			//API_ResetProtocolBuffer((USART_FUNCTION)(deviceIndex + USART_FUNCTION_MOUNT),RESETRTBUFF_TYPE_AND_INT_RX);
			goto DEVICE_SEND_ERROR;
			//return -1;
		}
		/*把数据读出来*/
		
		AtCmdAnalysis_SetATCmdSendCallBack(ATCMDGROUP_CALLBACK_NAME(DeviceSend));	//设置发送成功后的回调函数
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
		#if SYSTEM_MAP_ENABLE
		if(//_gSystemSetVal.sysSetValCtrl.deviceFun == 1&&
				_gSystemSetVal.NetIsOpen == MAX_NET_IS_OPEN_NUM
				&& _gSystemSetVal.sysMountMapStatus == SYSTEM_MOUNT_MAP_STATUS_ON
				&& _gSystemSetVal.sysSetValCtrl.mapDeviceID == deviceIndex			
				)
		{
			sendPortID = _gSystemSetVal.sysSetValCtrl.controlPort;
		}
		#endif
		sprintf(StrTempBuf,"%d,%d",sendPortID,size);
		AtCmdAnalysis_AtCmd_Send(AT_CMD_TEST_AT,"",0);
 		AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CIPSEND,StrTempBuf,ReadStrLen(StrTempBuf));

		AtCmdAnalysis_AtCmd_SendHugeBuffer(AT_CMD_SET_TCPWDATA,hugeBuffer);
		//AtCmdAnalysis_DeviceBeat_ReStartByDeviceID(deviceIndex,2);
		//AtCmdAnalysis_DeviceBeat_ReStart();	//心跳暂停
		TIMER_RUN(StartToSendAtCmd);
		LINE_LOG();
		return 0;

	}
DEVICE_SEND_ERROR:
	API_KillTimer(TIMER_TYPE_WCDMA_SEND);
	API_KillTimer((TIMER_TYPE)(TIMER_TYPE_WCDMA_DEVICE_SEND + deviceIndex));
	API_ResetProtocolBuffer((USART_FUNCTION)(deviceIndex + USART_FUNCTION_MOUNT),RESETRTBUFF_TYPE_AND_INT_RX);

	LINE_LOG();
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
	LINE_LOG();
	if(_gATCmdSetInfo.atCmdRunStatus !=  ATCMD_RUN_STATUS_NOT_RUN)
	{
		char StrTempBuf[100] = {0};
		int deviceIndex = 0;
		uint16_t size = 0;
		int sendPortID = 0;
		
		if(_gATCmdSetInfo.atCmdGroup < ATCMD_RUN_TYPE_INFO_SEND || _gATCmdSetInfo.atCmdGroup >= ATCMD_RUN_TYPE_INFO_SEND + _gModuleCount)
			return -1; 

		deviceIndex = _gATCmdSetInfo.atCmdGroup - ATCMD_RUN_TYPE_INFO_SEND;
		sendPortID = deviceIndex;
		if(_gModeRunState.NetIsOpen[deviceIndex] < MAX_NET_IS_OPEN_NUM
			#if SYSTEM_MAP_ENABLE
			&& (//_gSystemSetVal.sysSetValCtrl.deviceFun == 0 
				_gSystemSetVal.NetIsOpen < MAX_NET_IS_OPEN_NUM
				|| _gSystemSetVal.sysMountMapStatus == SYSTEM_MOUNT_MAP_STATUS_OFF
				|| _gSystemSetVal.sysSetValCtrl.mapDeviceID != deviceIndex
				) 
			#endif
			)
		{
			return -1;
		}
		//数据读完
		size = _gAtCmdRunCtrlArray[_gATCmdSetInfo.atCmdGroup].funArgSize;// strlen(_gAtCmdRunFunTypeArray[_gATCmdSetInfo.atCmdGroup].funArg);

		//AtCmdAnalysis_DeviceBeat_ReStartByDeviceID(deviceIndex,2);
		#if SYSTEM_MAP_ENABLE
		if(//_gSystemSetVal.sysSetValCtrl.deviceFun == 1 && 
				_gSystemSetVal.NetIsOpen == MAX_NET_IS_OPEN_NUM
				&& _gSystemSetVal.sysMountMapStatus == SYSTEM_MOUNT_MAP_STATUS_ON
				&& _gSystemSetVal.sysSetValCtrl.mapDeviceID == deviceIndex
				)
		{
			sendPortID = _gSystemSetVal.sysSetValCtrl.controlPort;	//改变采集口发送数据
		}
		#endif	
		AtCmdAnalysis_AtCmd_Send(AT_CMD_TEST_AT,"",0);		
		sprintf(StrTempBuf,"%d,%d",sendPortID,size);
 		AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CIPSEND,StrTempBuf,strlen(StrTempBuf));

		AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_TCPWDATA,
			_gAtCmdRunCtrlArray[_gATCmdSetInfo.atCmdGroup].funArg,
			size);

		TIMER_RUN(StartToSendAtCmd);
		LINE_LOG();
		return 0;
	}
	LINE_LOG();

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
	LINE_LOG();
	_gATCmdSetInfo.atCmdRunStatus =  ATCMD_RUN_STATUS_NOT_RUN;
	_gATCmdSetInfo.sendRepeatCount = 0;

	API_WCDMA_Init();
	LINE_LOG();
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
	LINE_LOG();
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
		API_SetTimer(600000,-1,TIMER_TYPE_WCDMA_AT_RESTART,TIMER_NAME(StartOk),NULL);

		TIMER_RUN(DeviceBeat);
	 	//如果没有检测到错误
	}
	LINE_LOG();
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
	char apnBuffer[50];

	DEBUG("Start to Init setting ev!\n");

	//_gATCmdSetInfo.atCmdRunStatus = ATCMD_RUN_STATUS_RUN_FREE;
	AtCmdAnalysis_SetATCmdSendCallBack(ATCMDGROUP_CALLBACK_NAME(Init));	//设置初始化成功后的回调函数

	switch(_gModuleSetVal[0].Cns_CNMP)
	{
		case NETWORK_PRIORIYY_TYPE_1G_2G:	//1,2G优先
			 netPriBuf = "4";
			 break;
		case NETWORK_PRIORIYY_TYPE_2G_3G:	//2,3G优先
			netPriBuf = "8";
			break;
		case NETWORK_PRIORIYY_TYPE_AUTO:	//自动选择
		default:
			netPriBuf = "2";
	}
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_IPREX,"115200",6);
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CATR,"=1",2);
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CNMP,"2",2);
	
	AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CSQ,"",0);  //查询网络信号
	AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_COPS,"",0);	//查询注册网络状态
	AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CNSMOD,"",0);  //查询网络系统模式

	sprintf(apnBuffer,"=1,\"IP\",\"%s\"",_gCnsModSet[0].apn);
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CGSOCKCONT,apnBuffer,ReadStrLen(apnBuffer));  //设置APN 
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CSOCKSETPN,"1",1);
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CGREG,"",0);
	AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CGREG,"?",1);	//GPRS	

	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CMGF,"1",1);  //设置短信格式 
	//支持中文		
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CSMP,"",0);  //设置TEXT模式参数
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CSCS,"\"UCS2\"",6);  //选择TE字符集GSM、UCS2、IRA

 	AtCmdAnalysis_AtCmd_Send(AT_CMD_STE_CPMS,"",0);  //短信存储位置选择命令

 	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CNMI,"",0);  //设置新短消息提示功能 2,1:存储短信 1,2:不存储短信，新短信直接打印到串口

 	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CIPSRIP,"",0);  //设置不显示接收IP 
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CIPCCFG,"",0);  //配置参数的套接字
/**/
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_NETOPEN,"",0);  //设置多链接方式
	AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_IPADDR,"",0);	//获取IP地址

 //	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_ATE,"",0);  //开启关闭回显





	TIMER_RUN(StartToSendAtCmd);
	LINE_LOG();
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_CALLBACK_FUN(Set3GNetVersion)
// 功能描述  : 初始化的回调函数
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDGROUP_CALLBACK_FUN(Set3GNetVersion)
{
	LINE_LOG();
	if(errType != SEND_ERROR_TYPE_NONE)
	{
		//如果检测到错误
	}
	else
	{

	}
	DEBUG("Success to Reset!\n");
	API_SetTimer(10000,1,TIMER_TYPE_SYSTEM_RESTART,NULL,TIMER_NAME(SystemReStart));
	LINE_LOG();
	return 0;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_FUN(Set3GNetVersion)
// 功能描述  : 设置3G版本号
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
ATCMDGROUP_FUN(Set3GNetVersion)
{
	char* buf = NULL;
	int len = NULL;
	LINE_LOG();
	buf = _gAtCmdRunCtrlArray[ATCMD_RUN_TYPE_SET3GNETVERSION].funArg;
	len = _gAtCmdRunCtrlArray[ATCMD_RUN_TYPE_SET3GNETVERSION].funArgSize;
	DEBUG("Set 3GNet Version!\n");
	
	AtCmdAnalysis_SetATCmdSendCallBack(ATCMDGROUP_CALLBACK_NAME(Set3GNetVersion));	//设置初始化成功后的回调函数
	
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CNVW,buf,len);	//设置3G版本号

	TIMER_RUN(StartToSendAtCmd);

	LINE_LOG();

	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_CALLBACK_FUN(GetModuleInfo)
// 功能描述  : 获取模块信息回调函数
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

ATCMDGROUP_CALLBACK_FUN(GetModuleInfo)
{
	LINE_LOG();
	if(errType != SEND_ERROR_TYPE_NONE)
	{
		char buf[20] = {0};

		_gATCmdSetInfo.atCmdRunStatus =  ATCMD_RUN_STATUS_NOT_RUN;	//标志成Not Run状态
		DEBUG("Check Module ==>failed ,errType:%d!\n",errType);
		sprintf(buf,"errorType:%d,atCmd:%d",errType,atCmd->ATCmd);
		AtCmdAnalysis_ErrorMsg(WCDMA_ERROR_CHECK,buf,strlen(buf));
		AtCmd_RECV_WCDMA_Init();
		//如果检测到错误
	}
	else
	{
		DEBUG("Check Module ==>OK!\n");
		API_SetTimer(80000,-1,TIMER_TYPE_WCDMA_AT_RESTART,TIMER_NAME(StartOk),NULL);	//八十秒内启动不起来直接重启

		RUN_ATCMD_GROUP(ATCMD_RUN_TYPE_INIT);
	 	//如果没有检测到错误
	}
	LINE_LOG();
	return 0;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ATCMDGROUP_FUN(GetModuleInfo)
// 功能描述  : 获取模块信息
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
ATCMDGROUP_FUN(GetModuleInfo)
{
	DEBUG("Check Module Info!\n");
	LINE_LOG();

	_gATCmdSetInfo.atCmdRunStatus = ATCMD_RUN_STATUS_RUN_FREE;
	AtCmdAnalysis_SetATCmdSendCallBack(ATCMDGROUP_CALLBACK_NAME(GetModuleInfo));	//设置初始化成功后的回调函数
	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CFUN,"",0);
	//AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CSQ,"",0);  //查询网络信号
	AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_COPS,"",0);	//查询注册网络状态

	AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CGMR,"",0); //查询版本号
	AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CSUB,"",0);
//	AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CNAOP,"",0);  //设置网络顺序
	//AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CNSMOD,"",0);  //查询网络系统模式

	TIMER_RUN(StartToSendAtCmd);

	LINE_LOG();

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
	LINE_LOG();
	if(_gATCmdSetInfo.atCmdRunStatus ==  ATCMD_RUN_STATUS_RUN_FREE)
	{
		API_SetTimer(600000,-1,TIMER_TYPE_WCDMA_AT_RESTART,TIMER_NAME(StartOk),NULL);
		//只有在空闲状态下才会运行

		AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CSQ,"",0);
		AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CGREG,"?",1);	//GPRS
		AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CNSMOD,"",0);
		AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CCLK,"",0);
		if(_gATCmdSetInfo.haveShortMsg)
		{
			AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CMGD,"",0);
			_gATCmdSetInfo.haveShortMsg = false;
		}
		else
		{
			AtCmdAnalysis_AtCmd_Send(AT_CMD_SET_CMGL,"\"ALL\"",5);
		}
		AtCmdAnalysis_AtCmd_Send(AT_CMD_GET_CIPOPEN,"",0);
		TIMER_RUN(StartToSendAtCmd);
		LINE_LOG();
		return 0;

	}
	LINE_LOG();
	return -1;
	 
}

int AtCmd_Init(tagModuleSetValStruct* module,
				int moduleCount,
				tagModeRunStateStruct* runState,
				tagATCmdSetInfo* ATCmdSetInfo,
				LinkQueue* cmdLinkQueue,
				tagAtCmdRunCtrlItem* GroupCtrlArray,
				tagSystemSetValStruct*	systemSetVal)
{
	LINE_LOG();
	if(module == NULL || moduleCount < 1 || moduleCount > 10 || runState == NULL || cmdLinkQueue == NULL)
		return -1;
	_gAtCmdRunCtrlArray = GroupCtrlArray;
	_gModuleSetVal = module;
	_gModuleCount =  moduleCount;
	_gModeRunStatePtr = runState;
	_gATCmdSetInfoPtr = ATCmdSetInfo;
	#if SYSTEM_MAP_ENABLE
	_gSystemSetValPtr = systemSetVal;
	#endif
	_gLinkQueue = cmdLinkQueue;
	AtCmdAnalysis_CnsMod_Init(_gCnsModSet,CNSMODSET_SIZE);
	LINE_LOG();
	return AtCmdAnalysis_AtCmd_Resgister(_gATCmd,	 //ATCmd解析体
									AT_CMD_COUNT,	//解析体命令数量
									_gtagATCmdRecvProcessItemArray,	//关键字解析体
									RECV_AT_CMD_SIZE,	//关键解析体数量
									_gAtCmdRunFunTypeArray
									);
}
