#include "config.h"



u8 cdma_or_nblot_flag=0;
u8 send_NSORF_flag=0;

CONNECT_TYPE net={0};

EXCHANGE_PLATFORM exchange_platform={0,0,180,0,10,PLATFORM_CHOICE_PUBLIC};
//ProtocolResource _qProtocol = {0};	/*协议缓存*/

//u8 USART3_rec_start_flag=0;//串口3接受开始标志
//u16 USART3_rec_count=0;
//u8 eth_rec_buf[1024];
//u8 eth_rec_buf_1[512];//转换成直接数据
//u16 USART3_rec_count_1=0;//直接数据计数
//u16 eth_rec_CHKSUM=0;
//u8 S1_deal_flag=0;
u8 connect_flag=0;

//u8 light_control_buf[512];//转换成直接数据
//u16 light_control_len=0;

u8 nblot_or_cdma_flag=0;//用于跳转用，识别各个模块
//u8 nblot_NRB[]="AT+NRB";

u16 Rx=0;
u8 SystemBuf[RxIn];  //储存出口接收数据

const u8 ASCII_hex[24]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};

u8 net_error_flag=0;//网络错误标志
u8 deal_process=0;

u8 update_ip[22]={"39.108.148.248,26001"};

u8 CSQ_rec_deal=0;//接受CSQ处理步骤
u8 CSQ_buf=0;
u8 CSQ=0;
u8 CSQ_DELAY=0;
	
void USART1_sendchar(u8 ch)
{
			USART_SendData(USART1,ch);//通过外设USARTx发送单个数据
		
		while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);

}


/*******************************************************************************
* 函 数 名         : sendchar()
* 函数功能		   : 串口1发送数据位
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void qwl_net_sendchar(u8 ch)
{
   	
	USART1_sendchar(ch);
}

/*******************************************************************************
* 函 数 名         : send字符串()
* 函数功能		   : 串口1发送数据位
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void qwl_net_sendstring(u8 *p)
{
  	while(*p)
  	{
  		qwl_net_sendchar(*p);
  		p++;
	}
	qwl_net_sendchar(0x0d);
	qwl_net_sendchar(0x0a);
}
/*******************************************************************************
* 函 数 名         : send字符串()
* 函数功能		   : 串口1发送数据位
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void qwl_net_sendstring1(u8 *p)
{
  	while(*p)
  	{
  		qwl_net_sendchar(*p);
  		p++;
	}
}

/*******************************************************************************
* 函 数 名         : net_state
* 函数功能		   	: 
* 输    入         :
* 输    出         :0表示网络正常
*******************************************************************************/
 void qwl_net_s_deal(void)
 {
	// 			light_heart_time_flag++;
		net.delay++;
	 ARMCHAIR_S_IQR();//按摩椅专用秒定时器
	 if(exchange_platform.Platform_Choice == PLATFORM_CHOICE_UPDATE)
		exchange_platform.UPDATE_link_overtime++;
//	nblot_read_flag1_delay++;
 }
/*******************************************************************************
* 函 数 名         : read_csq
* 函数功能		   : 网关心跳组包
* 输    入         : 
* 输    出         : 无
*******************************************************************************/
void READ_CSQ()
{
if(exchange_platform.Platform_Choice != PLATFORM_CHOICE_UPDATE)
		{
					if(CSQ_DELAY>59)
					{
						CSQ_DELAY=0;
						qwl_net_sendstring("AT+CSQ");//查询信号强度
					}
		}
											//j=strsearch_h("+CSQ:",SystemBuf,Rx);								

}	
/*******************************************************************************
* 函 数 名         : net_state
* 函数功能		   	: 
* 输    入         :
* 输    出         :0表示网络正常
*******************************************************************************/
u8 net_state()
{	
	if(exchange_platform.Platform_Choice ==	PLATFORM_CHOICE_UPDATE)
	{
		if(exchange_platform.UPDATE_link_overtime >exchange_platform.MAX_UPDATE_link_overtime )
		{
			exchange_platform.UPDATE_link_overtime =0;
			qwl_sendstring("超时间未连上，回到正常平台2");
			exchange_platform.Platform_Choice =PLATFORM_CHOICE_PUBLIC;
			API_nblot_init_0();//初始化模块，复位开始
		}

		if(exchange_platform.UPDATE_overtime_readnb >30)
		{
			exchange_platform.UPDATE_overtime_readnb =0;
			if(net.mode_type ==MODE_TYPE_BC95)
				{	
					if(net.reconnect_setp==LINK_OK)
																			{
					qwl_sendstring("超时清NB1");//读清队列
					qwl_net_sendstring("AT+NSORF=0,512");//读清队列
				}
			}
		}	
	}
								
	if(net_error_flag==0)
	{
		send_NSORF_flag=0;
	}								
	if(net.reconnect_setp!=LINK_OK)//在没联网成功时，把设置心跳速上的标志启动，用于马上联网
	{
		connect_flag=0;
	}
	switch(net.reconnect_setp)
		{
			case LINK_OK:
			{//net.delay_times=0;
				
						if(connect_flag==0)//联网时必须先发个心跳，防止心跳设置时间太久，永远连接不上
		{
			connect_flag=1;
			net_error_flag=0;
			exchange_platform.UPDATE_overtime_readnb =0;
			//upload_link_send();
/*			if(heart_max_time<=3)
			light_heart_time_flag=0;//heart_max_time;
			else
			{
				light_heart_time_flag=heart_max_time-3;
			}*/
		}	
			
									

			if(net_error_flag>=5)
			{
				net_error_flag=0;
				//deal_process_overtimes=0;
				//deal_process=0;
				net.delay_times=0;
				if(net.mode_type ==MODE_TYPE_ZTE)
					net.reconnect_setp=OPEN_GPRS;
				else
					net.reconnect_setp=LINK_IP;
				net.delay=0;
				net.setp_setp =0;
			}
				else if(net_error_flag>0)
			{
				if(send_NSORF_flag!=net_error_flag)
					{
						send_NSORF_flag=net_error_flag;//用于每次超时发一次
								if(net.mode_type ==MODE_TYPE_BC95)
								{	
											if(net.reconnect_setp==LINK_OK)
											{
								qwl_net_sendstring1("AT+NSORF=0,");//清队列	NBIOT需要									
								qwl_net_sendchar(0x35);
								qwl_net_sendchar(0x31);
								qwl_net_sendchar(0x32);
								qwl_net_sendchar(0x0d);
								qwl_net_sendchar(0x0a);
											}
								}
					}
			}
			if(net.mode_type ==MODE_TYPE_ZTE)	
			READ_CSQ();
			
	if(net.mode_type ==MODE_TYPE_BC95)
	{	

		nblot_read();
		exchange_platform.UPDATE_overtime_readnb =0;
/*		if(nblot_read_flag1>0)
		{
			u8 k;
			if(nblot_read_flag1_delay>2)//等待超时，服务器问题
			{

				nblot_read_flag1_delay=0;//心跳发送成功否，确认标志
				qwl_net_sendstring1("AT+NSORF=0,");//清队列
				qwl_net_sendchar(0x35);
				qwl_net_sendchar(0x31);
				qwl_net_sendchar(0x32);
				qwl_net_sendchar(0x0d);
				qwl_net_sendchar(0x0a);


				qwl_sendstring("等待服务器超时4\r\n");
				nblot_read_flag1=0;//进行下一个进程处理

			}

			//	k=nblot_group();
			if(k==0)
			nblot_read_flag1=0;
		}*/
		
		
		
	}
				
	
			}break;
			case LINK_IP:
			{
				if(net.mode_type ==MODE_TYPE_ZTE)
				{
							switch(g_flag)
							{
								case 2:
								{
									M6312_reconnect();//移动模块为M6312
								}
								break;
								case 3:
								{
									G510_reconnect();//移动模块为G510
								}
								break;
								default:
								{
									cdma_reconnect();//移动模块为MG2618或者电信MC8618
								}break;
							}
					
					}
				else
				nblot_reconnect();
			}break;
			case OPEN_GPRS:
			{
				if(net.mode_type ==MODE_TYPE_ZTE)
				{
					
								switch(g_flag)
							{
								case 2:
								{
									M6312_open_GPRS();//移动模块为M6312
								}
								break;
								case 3:
								{
									G510_open_GPRS();//移动模块为G510
								}
								break;
								default:
								{
									cdma_open_GPRS();//移动模块为MG2618或者电信MC8618
								}break;
							}
					
				}
				else
				{
					nblot_open_GPRS();
				}
			}break;
			case MODE_INIT:
			{
				
				if(net.mode_type ==MODE_TYPE_ZTE)
				{
					switch(g_flag)
					{
						case 2:
						{
							M6312_init();//移动模块为M6312
						}
						break;
						case 3:
						{
							G510_init();//移动模块为G510
						}
						break;
						default:
						{
							cdma_init();//移动模块为MG2618或者电信MC8618
						}break;
					}

				}
				else
				nblot_init();
			}break;
			case MODE_CHOICE:
			{
			cdma_or_nblot();//选择是NBLOT还是CDMA	
			}
			default:break;
		}
		return net.reconnect_setp;
}


//***************************************************
//发送数据
//*********************************************************
/*******************************************************************************
* 函 数 名         :net_send(u8 *p,u16 len)
* 函数功能		   : 
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
	
u8 net_send(u8 *p,u16 len)
{

	if(net.mode_type ==MODE_TYPE_ZTE)
	{
		
		switch(g_flag)
					{
						case 2:
						{
							M6312_send(p,len);//移动模块为M6312
						}
						break;
						case 3:
						{
							G510_send(p,len);//移动模块为G510
						}
						break;
						default:
						{
							cdma_send(p,len);//移动模块为MG2618或者电信MC8618
						}break;
					}
					

	}
	else
	{
		nblot_send(p,len);
		
	}

return 0;
} 



/*******************************************************************************
* 函 数 名         : REC_CSQ
* 函数功能		   	: 
* 输    入         :
* 输    出         :
*******************************************************************************/
void REC_CSQ(u8 k)
{
	if(k=='+')
	{
		CSQ_rec_deal=1;
	}
	switch(CSQ_rec_deal)
	{
		case 1:
		{
			if(k=='C')
				CSQ_rec_deal++;
		}
		break;
		case 2:
		{
			if(k=='S')
				CSQ_rec_deal++;
		}
		break;
		case 3:
		{
			if(k=='Q')
				CSQ_rec_deal++;
		}
		break;
		case 4:
		{
			if(k==':')
				CSQ_rec_deal++;
		}
		break;
		case 5:
		{
			if(k>=0x30)
				CSQ_buf=k;
			else
				CSQ_buf=0;
			CSQ_rec_deal++;
		}break;
				case 6:
		{
				if(CSQ_buf>=0x30)
				{
					if(k>=0x30)
					CSQ_buf=((CSQ_buf-0x30)*10)+(k-0x30);
					else
					CSQ_buf=(CSQ_buf-0x30);	
					
					CSQ=CSQ_buf;
					CSQ_buf=0xff;
				}
				else
				CSQ_buf=k;
				
				CSQ_rec_deal++;
		}
		break;
				case 7:
		{
			if(CSQ_buf!=0xff)
			{
				if(k>=0x30)
				CSQ_buf=((CSQ_buf-0x30)*10)+(k-0x30);
				else
					{
						CSQ_buf=(CSQ_buf-0x30);
					}
				CSQ=CSQ_buf;
					CSQ_rec_deal=0;
			}
		}
		break;
		default:break;
	}
}
/*******************************************************************************
* 函 数 名         : net_rec
* 函数功能		   	: 
* 输    入         :
* 输    出         :
*******************************************************************************/
void net_rec(u8 k)
{
	//qwl_sendchar(k);
	if(Rx<RxIn)
		{
			SystemBuf[Rx]=k;
 			Rx++;
			
		}
		
		if(net.mode_type ==MODE_TYPE_BC95)
		{
			if(net.reconnect_setp ==LINK_OK)
			nblot_rec(k);//读NB是否有需要读取的数

		}

		REC_CSQ(k);

		/*
		if(nblot_rec_times<500)
		{
		nblot_rec_buf[nblot_rec_times]=k;
			nblot_rec_times++;
		}
		if(net.mode_type ==MODE_TYPE_BC95)
		{nblot_rec(k);

		}
		else
		{
//				if(qwl_peizhi==0)
//				{
				eth_rec_deal(k);
//				}
		}
		*/
		
		
}


/*******************************************************************************
* 函 数 名         : qwl_net_init()
* 函数功能		   	: 
* 输    入         :
* 输    出         :
*******************************************************************************/
void qwl_net_init()
{
	CDMA_IO_Init();
	nblot_init_0();
	
		API_SetTimer(1000,-1,TIMER_TYPE_QWL_NET_DEAL,qwl_net_s_deal,NULL);
}
/*******************************************************************************
* 函 数 名         : cdma_or_nblot_setp_init()
* 函数功能		   : 模块选择完后进入下一步统一初始化参数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void cdma_or_nblot_setp_init()
{
										net.delay_times=0;
										net.reconnect_setp=MODE_INIT;
										net.delay=0;
										net.setp_setp=0;

									if(exchange_platform.Platform_Choice ==PLATFORM_CHOICE_UPDATE)//超过十次连接不上，重回正常平台,专用升级
									{
											exchange_platform.UPDATE_link_times ++;
										if(exchange_platform.UPDATE_link_times>exchange_platform.MAX_UPDATE_link_times )
										{
											qwl_sendstring("超次数未连上，回到正常平台1");
											exchange_platform.UPDATE_link_times=0;
											exchange_platform.Platform_Choice =PLATFORM_CHOICE_PUBLIC;
											API_nblot_init_0();//初始化模块，复位开始
										}
									}

}

/*******************************************************************************
* 函 数 名         : usart_init
* 函数功能		   : 串口初始化函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void usart_init(u32 BaudRate)
{

	GPIO_InitTypeDef GPIO_InitStructure;	//声明一个结构体变量，用来初始化GPIO

	USART_InitTypeDef  USART_InitStructure;	  //串口结构体定义

	NVIC_InitTypeDef NVIC_InitStructure;//中断结构体定义
	
	
	USART_ClearFlag(USART1,USART_FLAG_TC);//清除USARTx的待处理标志位	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	 //打开时钟
	
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;//TX			   //串口输出PA9
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;	    //复用推挽输出
	GPIO_Init(GPIOA,&GPIO_InitStructure);  /* 初始化串口输入IO */
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_10;//RX			 //串口输入PA10
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;		  //模拟输入
	GPIO_Init(GPIOA,&GPIO_InitStructure); /* 初始化GPIO */


	USART_InitStructure.USART_BaudRate=BaudRate;   //波特率设置为9600	//波特率
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;		//数据长8位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;			//1位停止位
	USART_InitStructure.USART_Parity=USART_Parity_No;				//无效验
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None; //失能硬件流
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;	 //开启发送和接受模式
	USART_Init(USART1,&USART_InitStructure);	/* 初始化USART1 */
	USART_Cmd(USART1, ENABLE);		   /* 使能USART1 */
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//使能或者失能指定的USART中断 接收中断
	USART_ClearFlag(USART1,USART_FLAG_TC);//清除USARTx的待处理标志位


	/* 设置NVIC参数 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);    
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 	   //打开USART1的全局中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 	 //抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 			//响应优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			 //使能
	NVIC_Init(&NVIC_InitStructure);

}
/*******************************************************************************
* 函 数 名         : cdma_or_nblot()
* 函数功能		   : 网络重连
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void cdma_or_nblot()
{
		switch (net.setp_setp)
	{
			case 0:
			{
				clear_SystemBuf();	
				net.setp_setp=1;
				net.delay=0;
				net.mode_type =MODE_TYPE_NONE;//模块未选择前的状态
				//qwl_sendstring("AT+CGMM");	
				qwl_net_sendstring("AT+NRB");//复位	
				GSM_RESET();//复位CDMA
				nblot_or_cdma_flag=0;
				g_flag=0;//g网标志清除
			}break;
			case 1:
			{
							if(strsearch_h("ZTE",SystemBuf)>0)
								{ 
									qwl_sendstring("模块是CDMA");
									net.mode_type =MODE_TYPE_ZTE;//是CDMA
								cdma_or_nblot_setp_init();
										
								}
				if(strsearch_h("MC",SystemBuf)>0)
								{ 
									qwl_sendstring("模块是CDMA");
									net.mode_type =MODE_TYPE_ZTE;//是CDMA
								
										cdma_or_nblot_setp_init();

								}	
		
			if(strsearch_h("M6312",SystemBuf)>0)
								{ 
									qwl_sendstring("模块是移动M6312");
									net.mode_type =MODE_TYPE_ZTE;//是CDMA
									g_flag=2;
										cdma_or_nblot_setp_init();
								}	
			if(strsearch_h("G510",SystemBuf)>0)
								{ 
									qwl_sendstring("模块是移动G510");
									net.mode_type =MODE_TYPE_ZTE;//是CDMA
									g_flag=3;
										cdma_or_nblot_setp_init();
								}					
			if(strsearch_h("MG",SystemBuf)>0)
								{ 
									qwl_sendstring("模块是G网");
									net.mode_type =MODE_TYPE_ZTE;//是CDMA
									g_flag=1;
										cdma_or_nblot_setp_init();
								}										
				if(strsearch_h("REBOOTING",SystemBuf)>0)
								{ 
									qwl_sendstring("模块是NBLOT");
									net.mode_type =MODE_TYPE_BC95;//是NBLOT
								/*	if(heart_max_time>170)
									{
									heart_max_time=170;//nb超时3分钟就会连接不上，所以最久3分内连接
									}
									*/
										cdma_or_nblot_setp_init();
								}
				if(net.delay>=2)//等待超时，服务器问题
					{net.delay=0;
						qwl_sendstring("识别模块超时");
						
						if(nblot_or_cdma_flag==0)
							net.setp_setp=0;
							else
							{
								net.setp_setp=2;
								
							}
						
						net.delay_times++;
						if(net.delay_times>3)
						{net.delay_times=0;
							if(nblot_or_cdma_flag==0)
							net.setp_setp=2;
							else
							{
								qwl_sendstring("切换至115200");
								usart_init(115200);
								Delay_MS(500);
								API_WatchDog_Feed();
	

							
								qwl_net_sendstring("AT+IPR=9600");
								Delay_MS(500);
								API_WatchDog_Feed();
				
								usart_init(9600);
								
								Delay_MS(500);
								API_WatchDog_Feed();
								net.setp_setp=0;
							}
						}
					}
				
			}break;
	case 2:
			{
/*				qwl_sendstring("开启CDMA");
				Start_GSM();
				API_WatchDog_Feed();
						delay_ms(1000);
						API_WatchDog_Feed();
						qwl_net_sendstring("ATE0");//关闭回显设置DCE为OK方式
						delay_ms(1000);
						API_WatchDog_Feed();
				*/
				//Start_GSM();
				clear_SystemBuf();	
				net.setp_setp=1;
				net.delay=0;
				net.mode_type =MODE_TYPE_NONE;//模块未选择前的状态
				qwl_net_sendstring("AT+CGMM");	
			//qwl_net_sendstring(nblot_NRB);//复位	
				g_flag=0;//g网标志清除
				nblot_or_cdma_flag=1;
				}break;
		
		default:break;
		}

}

