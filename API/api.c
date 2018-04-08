#include "config.h"
 
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_WatchDog_Init
// 功能描述  : 看门狗初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_WatchDog_Init(void)
{
#if CONF_WATCHDOG_EN
 	WD_Init();
#endif
 	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_WatchDog_Init
// 功能描述  : 看门狗喂食
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_WatchDog_Feed(void)
{
#if CONF_WATCHDOG_EN
 	FeedWatchDog();
#endif
 	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_Led_Init(void)	
// 功能描述  : 协议初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_Led_Init(void)		  
{
#if CONF_LED_EN
 	Led_Init();
	
		/*进入led跑马灯间隔时间*/
//	API_SetTimer(100,-1,TIMER_TYPE_HORSE_LED,API_horse_race_lamp,NULL);
#endif
	return 0;
}


//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_SPI_Flash_Init
// 功能描述  : 协议初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_SPI_Flash_Init(void)		  
{

 	SPI_Flash_Init();

	return 0;
}
	
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_net_state()		  
// 功能描述  : 读取CDMA或者NB状态
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回1，失败0
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_net_rec(u8 ch)	  
{
#if CONF_QWL_NET_EN
 	net_rec(ch);
#endif	
	return 0;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_net_state()		  
// 功能描述  : 读取CDMA或者NB状态
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回1，失败0
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_qwl_net_init()	  
{
#if CONF_QWL_NET_EN
 	qwl_net_init();
#endif	
	return 0;	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_nblot_init_0
// 功能描述  : 联网模块初始化，从NB，zte模块选择开始
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void API_nblot_init_0()
{
	#if CONF_QWL_NET_EN
	nblot_init_0();
	#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_net_data_send() 
// 功能描述  : 用于数据发送
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回1，失败0
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_net_data_send() 
{
	#if CONF_QWL_NET_EN

	if(exchange_platform.Platform_Choice==PLATFORM_CHOICE_UPDATE)
		net_data_send();
	#endif	
	return 0;
}


//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : net_data_send_init() 
// 功能描述  : 用于数据发送
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回1，失败0
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_net_data_send_init() 
{
	#if CONF_QWL_NET_EN
net_data_send_init();
	
API_SetTimer(1000,-1,TIMER_TYPE_SEND_DELAY,net_data_send_delay,NULL);
	#endif	
	return 0;

}


//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_net_state()		  
// 功能描述  : 读取CDMA或者NB状态
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回1，失败0
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_net_state()	  
{
#if CONF_QWL_NET_EN
	if(Run_User_Program_Falg ==false)
 	return net_state();
	else 
		return 4;
#else
	return 4;
#endif	
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_qwl_sendstring(u8 *p)	  
// 功能描述  : 测试发送测试数据
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_qwl_sendstring(u8 *p)	  
{
#if CONF_CESHI_EN
 	qwl_sendstring(p);
	
#endif
	return 0;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_qwl_sendstring(u8 *p)	  
// 功能描述  : 测试发送测试数据
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_qwl_sendchar(u8 ch)	  
{
#if CONF_CESHI_EN
 	qwl_sendchar(ch);
	
#endif
	return 0;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_horse_race_lamp(void)	
// 功能描述  : 协议初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void API_horse_race_lamp(void)		  
{
#if CONF_LED_EN
 	horse_race_lamp();
#endif
}
 //※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_WatchDog_ClockPro
// 功能描述  : 看门狗延时
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_WatchDog_ClockPro(void)
{
#if CONF_WATCHDOG_EN
 	WD_ClockingPro();
#endif
 	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_Protocols_Init
// 功能描述  : 协议初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_Protocols_Init(void)		  
{
#if CONF_PROTOCOL_EN
 	Protocols_Init();
#endif
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_Protocols_Init
// 功能描述  : 主循环体调用的执行所有协议，用于分析接收过来的数据
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_DoProtocols(void)		  
{
#if CONF_PROTOCOL_EN
 	return DoProtocols();
#else
 	return 0;
#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_SendProtocolBuffer
// 功能描述  : 发送数据
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_SendProtocolBuffer(USART_FUNCTION uf,uchar* buffer, size_t size)		  
{
#if CONF_PROTOCOL_EN
 	return SendProtocolBuffer(uf,buffer,size);
#else
 	return 0;
#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_ResetProtocolBuffer
// 功能描述  : 复位收发缓存
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_ResetProtocolBuffer(USART_FUNCTION uf,RESETRTBUFFTYPE ResetType)
{
#if CONF_PROTOCOL_EN
 	return ResetProtocolBuffer(uf,ResetType);
#else
 	return 0;
#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_AddProtocolBuffer
// 功能描述  : 向指定的协议类型缓存添加发送数据
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_AddProtocolBuffer(USART_FUNCTION uf,uchar* buffer, size_t size)
{
#if CONF_PROTOCOL_EN
 	return AddProtocolBuffer(uf,buffer,size);
#else
 	return 0;
#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_GetProtocolBufferOrLen
// 功能描述  : 获取指定的协议发送缓存指针和长度
// 输入参数  : uf,协议类型
// 输出参数  : ptr发送缓存指针	len发送长度
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_GetProtocolBufferOrLen(USART_FUNCTION uf,uchar **ptr,uint16 **len)
{
#if CONF_PROTOCOL_EN
	return GetProtocolBufferOrLen(uf,ptr,len);
#else
 	return 0;
#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_StartSendBuffer
// 功能描述  : 指定的协议类型启动发送数据
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_StartSendBuffer(USART_FUNCTION uf)
{
#if CONF_PROTOCOL_EN
 	return StartSendBuffer(uf);
#else
 	return 0;
#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_SetProtocolReceiveCallBack
// 功能描述  : 设置接收回调函数
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_SetProtocolReceiveCallBack(USART_FUNCTION uf,ReceiveFun fun)
{
#if CONF_PROTOCOL_EN
 	return SetProtocolReceiveCallBack(uf,fun);
#else
 	return 0;
#endif
}



//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_SetComParameter
// 功能描述  : 串口参数设置
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_SetComParameter(
	USART_FUNCTION uf,					//串口协议
	uint32_t baudRate,	 				//波特率
	uint16_t parity,					//校验位
	uint16_t wordLength,				//数据长度
	uint16_t stopBits					//停止位
	)
{
#if CONF_PROTOCOL_EN
 	return SetUartParameter(uf,baudRate,parity,wordLength,stopBits);
#else
 	return 0;
#endif	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_Timers_Init
// 功能描述  : 定时器初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_Timers_Init(void)
{
#if CONF_TIMER_EN
	LowPrcisionTime_Init();
#endif
	return 0;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_SetTimer
// 功能描述  : 创建定时器
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_SetTimer(int time,int circleCount,TIMER_TYPE timerType,TimerEvent tEvent,TimerEvent cEvent)
{
#if CONF_TIMER_EN
   return SetTimer(time,circleCount,timerType,tEvent,cEvent);
#else
	return 0;
#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_KillTimer
// 功能描述  : 删除定时器
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_KillTimer(TIMER_TYPE timerType)
{
#if CONF_TIMER_EN
	return DelTimer(timerType);
#else
	return 0;
#endif
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_StartRunTimer
// 功能描述  : 启动定时器
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_StartRunTimer(void)
{
#if CONF_TIMER_EN
	LowPrcisionTime_Start();
#endif
	return 0;

}


//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_goto_iap
// 功能描述  : 跳转到引导程序
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void API_goto_iap(void)
{
#if CONF_IAP_EN
	goto_iap();
#endif
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_Get_Iap_Config
// 功能描述  : 获取iap升级参数
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_Get_Iap_Config(void)
{
#if CONF_IAP_EN
 	return Get_Iap_Config();
#else
	return -1;
#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_IAP_Update_Write
// 功能描述  : WCDMA程序更新写
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_IAP_Update_Write(char* buf,size_t size)
{
#if CONF_IAP_EN
 	return IAP_Update_Write(buf,size);
#else
	return -1;
#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_IAP_Update_Start
// 功能描述  : WCDMA程序更新开始
// 输入参数  : force,强制从零开始更新 buf,md5校验码 size,校验码长度
// 输出参数  : None
// 返回参数  : 已升级的长度
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
DWORD API_IAP_Update_Start(BOOL force,char* buf,size_t size)
{
#if CONF_IAP_EN
 	return IAP_Update_Start(force,buf,size);
#else
	return 0;
#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_IAP_Update_End
// 功能描述  : WCDMA程序更新结束
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_IAP_Update_End(void)
{
#if CONF_IAP_EN
 	return IAP_Update_End();
#else
	return -1;
#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_IAP_copyflash
// 功能描述  : IAP程序拷贝到用户区
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_IAP_copyflash(u32 src_add,u32 dest_add,u32 copy_long)
{
#if CONF_IAP_EN
 	return IAP_copyflash(src_add,dest_add,copy_long);
#else
	return -1;
#endif
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_IAP_copyflash
// 功能描述  : IAP程序拷贝到用户区
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_IAP_copyflash1(u32 src_add,u32 dest_add,u32 copy_long)
{
	
#if CONF_IAP_EN
	#if CONF_SPI_FLASH_EN
 	return IAP_copyflash1(src_add,dest_add,copy_long);
	#else
	return -1;
	#endif
#else
	return -1;
#endif
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_Jump_To_UserProgram
// 功能描述  : 跳转到用户程序
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 成功返回0，失败-1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int API_Jump_To_UserProgram(void)
{
#if CONF_IAP_EN
 	return Jump_To_UserProgram();
#else
	return -1;
#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_Select_Program_run
// 功能描述  : 选择进入用户或引导程序
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void API_Select_Program_run(void)
{
#if CONF_IAP_EN
 	Select_Program_run();
#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_Check_Run_User_Program_Falg
// 功能描述  : 检查进入用户或引导程序标志
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void API_Check_Run_User_Program_Falg(void)
{
#if CONF_IAP_EN
 	Check_Run_User_Program_Falg();
#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_Armchair_IO_Init
// 功能描述  : 按摩椅初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void API_Armchair_IO_Init(void)
{
#if CONF_ARMCHAIR_EN
 	Armchair_IO_Init();
#endif
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_Armchair_Init
// 功能描述  : 按摩椅初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void API_Armchair_Init(void)
{
#if CONF_ARMCHAIR_EN
 	Armchair_Init();
#endif
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : API_Armchair_Init
// 功能描述  : 按摩椅初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void API_ARMCHAIR_DEAL(void)
{
#if CONF_ARMCHAIR_EN
 	ARMCHAIR_DEAL();
#endif
}
/*******************************************************************************
* 函 数 名         : API_ARMCHAIR_TIME
* 函数功能		   : 超时延时标志放置处
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void API_ARMCHAIR_TIME()
{
	#if CONF_ARMCHAIR_EN
		ARMCHAIR_TIME();
	#endif
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//                          程序结束(END)
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※



