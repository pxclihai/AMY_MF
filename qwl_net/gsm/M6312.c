
#include "config.h"
u8 AT_CPIN1[]	="AT+CPIN?";		//查询SIM卡状态
u8 AT_CIPSTART1[]="AT+IPSTART=\"TCP\",";//GPRS连接方式\GPRS连接地址(根据用户实际IP修改)\GPRS连接的端口
u8 AT_CIPCLOSE1[]="AT+IPCLOSE";		//TCP连接关闭
u8 AT_zpppopen1[]	="AT+CGACT=1,1";		//打开网络
//u8 AT_zpppstatus1[]="at+zpppstatus";//查询连接状态
u8 AT_CIPSEND1[]	="AT+IPSEND=";		//GPRS发送信息命令
u8 AT_GETICCID1[]	="AT+CCID";		//查询SIM卡号
u8 AT_CIMI1[]	="AT+CIMI";		//查询SIM卡号


u8 M6312_RESET_flag=0;



/*****************************************************************************************************************
实现功能:对SIM800C进行启动,开启SIM800C功能详述:单片机上是跟SIM800C的管脚(/IGT)加时长至少为1s的低电平信号
*****************************************************************************************************************/
/*
void GSM_RESET(void)
{

	GPIO_ResetBits(CDMA_IO,CDMA_RESET);
	GPIO_SetBits(CDMA_IO,CDMA_ON_OFF);
	Delay_MS(100);//手册只要20ms，我给它多点时间
	GPIO_SetBits(CDMA_IO,CDMA_RESET);
	GPIO_SetBits(CDMA_IO,CDMA_ON_OFF);
	Delay_MS(50);//手册只要10ms，我给它多点时间
	GPIO_SetBits(CDMA_IO,CDMA_RESET);
	GPIO_ResetBits(CDMA_IO,CDMA_ON_OFF);

}*/
/*****************************************************************************************************************
实现功能:对SIM800C进行启动,开启SIM800C功能详述:单片机上是跟SIM800C的管脚(/IGT)加时长至少为1s的低电平信号
*****************************************************************************************************************/
/*
void Start_GSM(void)
{
//	GSM_RESET();
//	Delay_MS(3000);
//  GPIO_SetBits(CDMA_IO,CDMA_ON_OFF);
//	GPIO_SetBits(CDMA_IO,CDMA_RESET);
	
}
*/
/*******************************************************************************
* 函 数 名         : M6312_init()
* 函数功能		   : M6312初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void M6312_init()
{
	//u8 i;
		switch (net.setp_setp)
	{
			case 0:
			{
					clear_SystemBuf();
				qwl_net_sendstring("ATE0V1");//查询SIM卡状态
				net.delay=0;
				net.setp_setp=1;
				net.delay=0;
				net.delay_times=0;
			}break;
			case 1:
			{
							if(net.delay>2)//等待超时，服务器问题
						{net.delay=0;
								net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=MODE_CHOICE;//进入网络重新连接
								net.delay=0;
						}
							if(strsearch_h("OK",SystemBuf)>0)
								{ qwl_sendstring("回显关闭成功");
									net.delay_times=0;
								net.setp_setp=2;
								net.reconnect_setp=MODE_INIT;//进入网络重新连接
								net.delay=0;
								}
			}break;
			case 2:
			{
				clear_SystemBuf();
				qwl_net_sendstring(AT_CPIN1);//查询SIM卡状态
				net.delay=0;
				net.setp_setp=3;
				net.delay=0;
				//net.delay_times=0;
			}break;
			case 3:
			{
							if(net.delay>2)//等待超时，服务器问题
						{net.delay=0;
							net.setp_setp=2;
							qwl_sendstring("查sim卡状态超时");
							net.delay_times++;
							if(net.delay_times>12)
							{qwl_sendstring("sim卡不存在");
								net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=MODE_CHOICE;//进入网络重新连接
								net.delay=0;
							}
						}	
							if(strsearch_h("READY",SystemBuf)>0)
								{ qwl_sendstring("查sim卡成功");
									net.delay_times=0;
								net.setp_setp=4;
								net.reconnect_setp=MODE_INIT;//进入网络连接
								net.delay=0;
									Delay_MS(1000);
								}
							
						
			}break;
			case 4:
			{
				qwl_sendstring("查sim卡号");
				clear_SystemBuf();
				qwl_net_sendstring(AT_GETICCID1);//查询SIM卡号码

				net.delay=0;
				net.setp_setp=5;
				net.delay=0;
				//net.delay_times=0;
			}break;
			case 5:
			{u16 j;
				u16 i;
							if(net.delay>2)//等待超时，服务器问题
						{net.delay=0;
							net.setp_setp=4;
							qwl_sendstring("查sim卡号超时");
							net.delay_times++;
							if(net.delay_times>5)
							{qwl_sendstring("sim卡号不存在");
								net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=MODE_CHOICE;//进入网络重新连接
								net.delay=0;
							}
						}	
						if(g_flag==0)
						j=strsearch_h("+GETICCID:0x",SystemBuf);
						else
						{u8 qqq=0;
						j=strsearch_h("OK",SystemBuf);
							if(j>0)
							{
								for(i=0;i<Rx;i++)
								{
									if(SystemBuf[i]>=0x30)
										if(SystemBuf[i]<=0x39)
										{
											if(qqq>=5)
												break;
											qqq++;
										}
								}
								if(i>=5)
								j=i-5;
								else
									j=0;
							}
							
						}
							if(j>0)
								{ qwl_sendstring("查sim卡号成功");
									for(i=0;i<20;i++)
									{SIM_ID[i]=SystemBuf[j+i];
										qwl_sendchar(SIM_ID[i]);
									}
									Delay_MS(1000);
									net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=OPEN_GPRS;//进入网络重新连接
								net.delay=0;
								}
							
						
			}break;
		
		default:break;
		}
		



}

/*******************************************************************************
* 函 数 名         : void light_eth_reconnect()
* 函数功能		   : cdma初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void M6312_reconnect()
{
	switch (net.setp_setp)
	{
		case 0:
		{
			qwl_sendstring("重连IP");
		clear_SystemBuf();
if(exchange_platform.Platform_Choice == PLATFORM_CHOICE_PUBLIC)			
	TP_GPRS(AT_CIPSTART1,light_TCP);//连接IP
else
	TP_GPRS(AT_CIPSTART1,update_ip);//连接升级IP

			net.setp_setp=1;
			net.delay=0;
		

						
		}break;
		case 1:
		{
			if(net.delay>=5)//等待超时，服务器问题
			{net.delay=0;
				net.setp_setp=0;//等待失败
				qwl_sendstring("重连超时");
				net.delay_times++;
				if(net.delay_times++>15)
				{net.delay_times=0;
					net.setp_setp=0;
					net.reconnect_setp=OPEN_GPRS;//进入网络重新连接
					net.delay=0;
				}
			}
			

				if(strsearch_h("ALREADY CONNECT",SystemBuf)>0)
				{net.reconnect_setp=LINK_OK;
					net.setp_setp=0;

//					light_heart_time_flag=heart_max_time;
					qwl_sendstring("重连成功1");
				}
				
					if(strsearch_h("CONNECT OK",SystemBuf)>0)
				{net.reconnect_setp=LINK_OK;
					net.setp_setp=0;

//					light_heart_time_flag=heart_max_time;
					qwl_sendstring("重连成功2");
				}
				
				if(strsearch_h("ERROR",SystemBuf)>0)
				{
					net.delay_times=0;
					net.setp_setp=0;
					net.reconnect_setp=OPEN_GPRS;//进入网络重新连接
					net.delay=0;
					
				qwl_sendstring("重连错误");
					Delay_MS(1000);
				}
			
		}break;
		default:break;
	}
}
/*******************************************************************************
* 函 数 名         : void cdma_open_GPRS()
* 函数功能		   : 网络重连
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void M6312_open_GPRS()
{
	switch (net.setp_setp)
	{
		case 0:
		{
			
			M6312_RESET_flag++;
			if(M6312_RESET_flag>3)
		{
			net.delay_times=0;
					net.setp_setp=0;
					net.reconnect_setp=MODE_CHOICE;//进入CDMA重启
					net.delay=0;
			M6312_RESET_flag=0;
					qwl_sendstring("进入cdma复位");
		}
			clear_SystemBuf();
			qwl_net_sendstring(AT_CIPCLOSE1);
			net.setp_setp=1;
			net.delay=0;
			qwl_sendstring("关闭连接");
		}break;
		case 1:
		{
			if(net.delay>=6)//等待超时，服务器问题
			{net.delay=0;
				net.setp_setp=0;//等待失败
				qwl_sendstring("网络关闭超时");
				net.delay_times++;
				if(net.delay_times>7)
				{net.delay_times=0;
					net.setp_setp=0;
					net.reconnect_setp=MODE_INIT;//进入CDMA重启
					net.delay=0;
					qwl_sendstring("进入CDMA重启");
				}
			}
			

				if(strsearch_h("+ZPPPSTATUS: CLOSED",SystemBuf)>0)
				{net.reconnect_setp=OPEN_GPRS;
					net.setp_setp=3;
					net.delay_times=0;
					net.delay=0;
					qwl_sendstring("网络关闭成功");
					Delay_MS(1000);
				}
				if(strsearch_h("ERROR",SystemBuf)>0)
				{
					net.reconnect_setp=OPEN_GPRS;
					net.setp_setp=3;
					net.delay_times=0;
					net.delay=0;

				qwl_sendstring("网络关闭错误");
					Delay_MS(1000);
				}
			
		}break;
		case 3:
		{
			clear_SystemBuf();			
			qwl_net_sendstring(AT_zpppopen1);
			net.setp_setp=4;
			net.delay=0;
			qwl_sendstring("打开网络连接");
		}break;
		case 4:
		{
			if(net.delay>=6)//等待超时，服务器问题
			{net.delay=0;
				net.setp_setp=0;//等待失败
				qwl_sendstring("网络连接超时");
				net.delay_times++;
				if(net.delay_times>7)
				{net.delay_times=0;
					net.setp_setp=0;
					net.reconnect_setp=MODE_INIT;//进入CDMA重启
					net.delay=0;
					qwl_sendstring("进入CDMA重启");
				}
			}
			

				if(strsearch_h("OK",SystemBuf)>0)
				{net.reconnect_setp=LINK_IP;//打开完毕，进入联网
					net.setp_setp=0;
					net.delay_times=0;
					net.delay=0;
					qwl_sendstring("打开网络连接成功");
					Delay_MS(1000);
				}
				if(strsearch_h("+ZPPPOPEN:CONNECTED",SystemBuf)>0)
				{net.reconnect_setp=LINK_IP;//打开完毕，进入联网
					net.setp_setp=0;
					net.delay_times=0;
					net.delay=0;
					qwl_sendstring("打开网络连接成功");
					Delay_MS(1000);
				}
				if(strsearch_h("ERROR",SystemBuf)>0)
				{
					net.setp_setp=0;//等待失败					
					net.delay_times++;
				if(net.delay_times++>7)
				{net.delay_times=0;
					net.setp_setp=0;
					net.reconnect_setp=MODE_INIT;//进入CDMA重启
					net.delay=0;
					qwl_sendstring("进入CDMA重启");
				}

				qwl_sendstring("打开网络错误");
					Delay_MS(1000);				
				}
			
		}break;
		default:break;
	}
}

/*******************************************************************************
* 函 数 名         :M6312_send(u8 *p,u16 len)
* 函数功能		   : 
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
	
u8 M6312_send(u8 *p,u16 len)
{
	

	u16 j=0;
	u16 g_delay_us=0;
	u16 g_delay_ms=0;
clear_SystemBuf();
qwl_net_sendstring1(AT_CIPSEND1);
j=len;
if(len>1000)
{
	qwl_net_sendchar(NO[j/1000]);
	qwl_sendchar(NO[j/1000]);
	j=j%1000;
}
if(len>100)
{
	qwl_net_sendchar(NO[j/100]);
	qwl_sendchar(NO[j/100]);
	j=j%100;
}

qwl_net_sendchar(NO[j/10]);
qwl_net_sendchar(NO[j%10]);
qwl_sendchar(NO[j/10]);
qwl_sendchar(NO[j%10]);

			qwl_net_sendchar(0x0d);	
  	qwl_net_sendchar(0x0A);
				g_delay_us=0;
				g_delay_ms=0;
				while(strsearch_h(">",SystemBuf)==0)
				{
					API_WatchDog_Feed();
					Delay_2us();
					g_delay_us++;
					if(g_delay_us>400)
					{
						g_delay_us=0;
						g_delay_ms++;
						if(g_delay_ms>1000)
						{
							
							break;
						}
					}
				}


for(j=0;j<len;j++)
  	{
  		qwl_sendchar(p[j]);//测试用
			qwl_net_sendchar(p[j]);
	}
		qwl_sendchar(0x0D);
  	qwl_sendchar(0x0A);
	qwl_net_sendchar(0x0D);
  	qwl_net_sendchar(0x0A);


return 0;
} 








