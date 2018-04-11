
#include "config.h"
u8 AT_CPIN[]	="AT+CPIN?";		//查询SIM卡状态
u8 AT_CIPSTART[]="at+zipsetup=0,";//GPRS连接方式\GPRS连接地址(根据用户实际IP修改)\GPRS连接的端口
u8 AT_CIPCLOSE[]="at+zpppclose";		//TCP连接关闭
u8 AT_zpppopen[]	="at+zpppopen";		//打开网络
u8 AT_zpppstatus[]="at+zpppstatus";//查询连接状态
u8 AT_CIPSEND[]	="at+zipsend=0,";		//GPRS发送信息命令
u8 AT_GETICCID[]	="AT+GETICCID";		//查询SIM卡号
u8 AT_CIMI[]	="AT+CIMI";		//查询SIM卡号





u8 light_TCP[22]={"171.208.222.113,6037"};
u8 SIM_ID[20]={0};
u8 cdma_RESET_flag=0;

u8 g_flag=0;//G网标志
/*******************************************************************************
* 函 数 名         : CDMA_IO_Init
* 函数功能		   : LED初始化函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void CDMA_IO_Init()	  //端口初始化
{
	GPIO_InitTypeDef GPIO_InitStructure;	//声明一个结构体变量，用来初始化GPIO


	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE); /* 开启GPIO时钟 */

	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=CDMA_RESET|CDMA_ON_OFF;	 //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	  //设置推挽输出模式
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
	GPIO_Init(CDMA_IO,&GPIO_InitStructure); /* 初始化GPIO */	
	GPIO_ResetBits(CDMA_IO,CDMA_RESET);
	GPIO_SetBits(CDMA_IO,CDMA_ON_OFF);

}
/*********************************************************************
 ** 函数名称: strsearch_h ()
 ** 函数功能: 在指定的数组里连续找到相同的内容
 ** 入口参数: ptr2要查找的内容, ptr1当前数组
 ** 出口参数: 0-没有 找到   >1 查找到
 *********************************************************************/
u16 strsearch_h(u8 *ptr2,u8 *ptr1_at)//查字符串*ptr2在*ptr1中的位置
//本函数是用来检查字符串*ptr2是否完全包含在*ptr1中
//返回:  0  没有找到
//65535 从第N个字符开始相同
{
u16 i,j,k;
        
      
	if(Rx==0) return(0);
	
	for(i=0,j=0;i<Rx;i++)
	{
        	if(ptr1_at[i]==ptr2[j])
       		{	//第一个字符相同
        		for(k=i;k<Rx+1;k++,j++)
        		{

        			if(ptr2[j]==0)//比较正确
        			{                return(k);               //返回值是整数，不含0
                                }
        			if(ptr1_at[k]!=ptr2[j]) break;

        		}
        		j=0;
        	}
	}
	  ////   qwl_sendstring("-------------------\n");
	  //   qwl_sendstring(ptr1_at);
		//  qwl_sendstring("-------------------\n");
	return(0);
}

/****************************************************************************************************************/
///////////////////////////////////////void clear_SystemBuf();清除串口接收缓冲区/////////////////////////////////
/****************************************************************************************************************/
void clear_SystemBuf()
{

	Rx=0;
}



//*******************************************************
//TP链接
//******************************************************
void TP_GPRS(u8 *p,u8 *q)
{//u8 ip_init_word[]={"121.42.196.113,28070"};//112.74.98.39,8647出厂默认IP
u8 i=0;
while(*p)
  	{
  		qwl_net_sendchar(*p);
			qwl_sendchar(*p);
  		p++;
			i++;
			if(i>20)
				break;
	}
		i=0;
	//q=&ip_init_word[0];
	while(*q)
  	{
  		qwl_net_sendchar(*q);
			qwl_sendchar(*q);
  		q++;
			i++;
			if(i>22)
				break;
	}
	qwl_sendchar(0x0D);
  	qwl_sendchar(0x0A);
	qwl_net_sendchar(0x0D);
  	qwl_net_sendchar(0x0A);
}
//*******************************************************
//SIM800C初始化
//*********************************************************
/*u8 GSM_init()
{	u8 i=0;
	qwl_net_sendstring("ATE0V1");//关闭回显设置DCE为OK方式
	Delay_MS(1000);

	while(1)
	{

		led_reversal();
		clear_SystemBuf();
		qwl_net_sendstring(AT_CPIN);//查询SIM卡状态
		Delay_MS(1000);
		if(strsearch_h("READY",SystemBuf)==0)
		{ 
			i++;
			if(i>5)
			return 1;
		}
		else
		{break;
		}
	}
	clear_SystemBuf();
	qwl_net_sendstring(AT_CIPCLOSE);
	i=0;
	led_reversal();
		while(1)
			{
				Delay_MS(1000);
			if(strsearch_h("ERROR",SystemBuf)==0)
					{ 
						i++;
						if(i>5)
						return 2;
					}
					else
					{break;
					}
					
					if(strsearch_h("+ZPPPSTATUS: CLOSED",SystemBuf)==0)
					{ 
						i++;
						if(i>5)
						return 2;
					}
					else
					{break;
					}
			}
clear_SystemBuf();			
qwl_net_sendstring(AT_zpppopen);	
		i=0;
			led_reversal();
	while(1)
			{
				Delay_MS(1000);
			if(strsearch_h("OPENED",SystemBuf)==0)
					{ 
						i++;
						if(i>8)
						return 2;
					}
					else
					{break;
					}
			}	 
	clear_SystemBuf();
	TP_GPRS(AT_CIPSTART,light_ip);//连接IP
			i=0;
			led_reversal();
			while(1)
			{
				Delay_MS(1000);
			if(strsearch_h("+ZTCPESTABLISHED:0",SystemBuf)==0)
					{ 
						i++;
						if(i>5)
						return 2;
					}
					else
					{break;
					}
					
					if(strsearch_h("ERROR",SystemBuf)==0)
					{ 
						i++;
						if(i>5)
						return 2;
					}
					else
					{return 2;
					}
			}	 
	return 0;
}

*/

/*****************************************************************************************************************
实现功能:对SIM800C进行启动,开启SIM800C功能详述:单片机上是跟SIM800C的管脚(/IGT)加时长至少为1s的低电平信号
*****************************************************************************************************************/

void GSM_RESET(void)
{

	GPIO_ResetBits(CDMA_IO,CDMA_RESET);
	GPIO_SetBits(CDMA_IO,CDMA_ON_OFF);
	Delay_MS(1000);//手册只要20ms，我给它多点时间
	GPIO_SetBits(CDMA_IO,CDMA_RESET);
	GPIO_SetBits(CDMA_IO,CDMA_ON_OFF);
	Delay_MS(50);//手册只要10ms，我给它多点时间
	GPIO_SetBits(CDMA_IO,CDMA_RESET);
	GPIO_ResetBits(CDMA_IO,CDMA_ON_OFF);

}
/*****************************************************************************************************************
实现功能:对SIM800C进行启动,开启SIM800C功能详述:单片机上是跟SIM800C的管脚(/IGT)加时长至少为1s的低电平信号
*****************************************************************************************************************/

void Start_GSM(void)
{
//	GSM_RESET();
//	Delay_MS(3000);
//  GPIO_SetBits(CDMA_IO,CDMA_ON_OFF);
//	GPIO_SetBits(CDMA_IO,CDMA_RESET);
	
}

/*******************************************************************************
* 函 数 名         : cdma_init()
* 函数功能		   : cdma初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void cdma_init()
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
				qwl_net_sendstring(AT_CPIN);//查询SIM卡状态
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
									Delay_MS(500);
								}
							
						
			}break;
			case 4:
			{
				qwl_sendstring("查sim卡号");
				clear_SystemBuf();
				if(g_flag==0)
				qwl_net_sendstring(AT_GETICCID);//查询SIM卡号码
				else
					qwl_net_sendstring(AT_CIMI);
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
									
									net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=OPEN_GPRS;//进入网络重新连接
								net.delay=0;
								}
							
						
			}break;
		
		default:break;
		}
		

/*
Start_GSM();
i=GSM_init();
	if(i==1)
			qwl_sendstring("查询SIM卡状态失败");
	else if(i==2)
			qwl_sendstring("网络连接失败");
	else if(i==0)
			qwl_sendstring("成功联网");	
*/
//send_data();//ip_init_word);

}

/*******************************************************************************
* 函 数 名         : void light_eth_reconnect()
* 函数功能		   : cdma初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

u8 light_TCP_test[22]={"171.208.222.113,6037"};
void cdma_reconnect()
{
	switch (net.setp_setp)
	{
		case 0:
		{
			qwl_sendstring("重连IP");
			clear_SystemBuf();
			if(exchange_platform.Platform_Choice == PLATFORM_CHOICE_PUBLIC)			
			TP_GPRS(AT_CIPSTART,light_TCP_test);//连接IP
			else
			TP_GPRS(AT_CIPSTART,update_ip);//连接升级IP

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
			

				if(strsearch_h("+ZTCPESTABLISHED:0",SystemBuf)>0)
				{net.reconnect_setp=LINK_OK;
					net.setp_setp=0;

//					light_heart_time_flag=heart_max_time;
					qwl_sendstring("重连成功1");
				}
				
					if(strsearch_h("+ZIPSETUP:CONNECTED",SystemBuf)>0)
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
void cdma_open_GPRS()
{
	switch (net.setp_setp)
	{
		case 0:
		{
			
			cdma_RESET_flag++;
			if(cdma_RESET_flag>3)
		{
			net.delay_times=0;
					net.setp_setp=0;
					net.reconnect_setp=MODE_CHOICE;//进入CDMA重启
					net.delay=0;
			cdma_RESET_flag=0;
					qwl_sendstring("进入cdma复位");
		}
			clear_SystemBuf();
			qwl_net_sendstring(AT_CIPCLOSE);
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
				}
				if(strsearch_h("ERROR",SystemBuf)>0)
				{
					net.reconnect_setp=OPEN_GPRS;
					net.setp_setp=3;
					net.delay_times=0;
					net.delay=0;

				qwl_sendstring("网络关闭错误");
				}
			
		}break;
		case 3:
		{
			clear_SystemBuf();			
			qwl_net_sendstring(AT_zpppopen);
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
			

				if(strsearch_h("OPENED",SystemBuf)>0)
				{net.reconnect_setp=LINK_IP;//打开完毕，进入联网
					net.setp_setp=0;
					net.delay_times=0;
					net.delay=0;
					qwl_sendstring("打开网络连接成功");
				}
				if(strsearch_h("+ZPPPOPEN:CONNECTED",SystemBuf)>0)
				{net.reconnect_setp=LINK_IP;//打开完毕，进入联网
					net.setp_setp=0;
					net.delay_times=0;
					net.delay=0;
					qwl_sendstring("打开网络连接成功");
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
				}
			
		}break;
		default:break;
	}
}
/*******************************************************************************
* 函 数 名         : cdma_init_0()
* 函数功能		   : 网络重连
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void cdma_init_0()
{
								net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=MODE_CHOICE;//先进入NBLOT还是CDMA选择
								net.delay=0;
}
/*******************************************************************************
* 函 数 名         :cdma_send(u8 *p,u16 len)
* 函数功能		   : 
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
	
u8 cdma_send(u8 *p,u16 len)
{
	

	u16 j=0;
	u16 g_delay_us=0;
	u16 g_delay_ms=0;
clear_SystemBuf();
qwl_net_sendstring1(AT_CIPSEND);
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
		if(g_flag==0)
		{
		qwl_net_sendchar(0x20);	
		qwl_net_sendchar(0x0d);	
		}
		else
		{
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








