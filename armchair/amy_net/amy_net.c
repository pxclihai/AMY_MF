
#include "config.h"

u16 eth_Len=0;

u8 eth_rec_buf[500];
u16 eth_rec_CHKSUM=0;

u8 USART3_rec_start_flag=0;//串口3接受开始标志
u16 USART3_rec_count=0;
	
	



u16 GATEWAY_heart_time_flag=59;   // 心跳发送延时计时，专用网关计时
u16 GATEWAY_heart_max_flag=10;   // 心跳发送延迟时间，默认60秒
u8 	GATEWAY_deal_time=0;				//心跳分时处理
u8  getwayheart_sendok_flag=1;//心跳有否发送成功，确认标志
	
u16 GATEWAY_list_time_flag=00;   // 列表发送延时计时，专用网关计时
u8 	GATEWAY_list_time=0;					//列表分时处理
u8  getwaylist_sendok_flag=0;//列表有否发送成功，确认标志
u8	getwaylist_send_flag=0;//列表发送标志，不等于0时，要求列表上传
u8	getwaylist_send_times=0;//列表上传次数

u16 GATEWAY_off_time_flag=00;   // 列表发送延时计时，专用网关计时
u8 	GATEWAY_off_time=0;					//列表分时处理
u8  getwayoff_sendok_flag=0;//列表有否发送成功，确认标志
u8	getwayoff_send_flag=0;//列表发送标志，不等于0时，要求列表上传
u8	getwayoff_send_times=0;//列表上传次数	

u8 amy_config_rec_flag=0;//按摩椅配置标志位
u8 amy_check_ID[16];//按摩椅要查询的终端先缓存
	
u8 amystate_send_flag[250]={0};//按摩椅状态发送标志
	
u8 plc_cofig_buf[16];//按摩椅配置暂时存储用于发送
u8 plc_cofig_buf_1[3];//按摩椅配置时间，模式
	
//u8 deal_process=0;//处理进程标志，邮电部要求一条一条发，所以用次标志

u8 amy_check_pointer=0x00;//按摩椅查询指针
u8 amy_storflash_pointer=0;//用于存储到flash，当超过250台查询，就存储
u8 amy_storflash_flag=0;//用于存储到flash，当超过250台查询，就存储，//标志，0读flash，1循环第一次然后更新flash，2，等待新终端，然后更新flash
u8 suipian_delay=0;
POLL_ZD_TYPE poll_zd={0};

/*******************************************************************************
* 函 数 名         : USART3_sendchar(u8 ch)
* 函数功能		   : 串口3发送数据位
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void USART3_sendchar(u8 ch)
{
			USART_SendData(USART3,ch);//通过外设USARTx发送单个数据
		
		while(USART_GetFlagStatus(USART3,USART_FLAG_TXE)==RESET);

}

/*******************************************************************************
* 函 数 名         : USART3_sendstring(u8 *p)
* 函数功能		   : 串口3发送数据位
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void USART3_sendstring(u8 *p)
{
  	while(*p)
  	{
  		USART3_sendchar(*p);
  		p++;
	}

}	

/*******************************************************************************
* 函 数 名         :USART3_sendstring1(u8 *p,u16 len)
* 函数功能		   : 串口3发送数据位
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void USART3_sendstring1(u8 *p,u16 len)
{
	u16 i;
  	for(i=0;i<len;i++)
  	{
  		USART3_sendchar(p[i]);
			//qwl_sendchar(p[i]);//测试用
	}

}	
/*******************************************************************************
* 函 数 名         : usart_init
* 函数功能		   : 串口初始化函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void usart3_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;	//声明一个结构体变量，用来初始化GPIO

	USART_InitTypeDef  USART_InitStructure;	  //串口结构体定义

	NVIC_InitTypeDef NVIC_InitStructure;//中断结构体定义
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);//  使能时钟   
   
  /*串口3使用GPIOB的10,11脚*/
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);//  使能时钟
	
  //USART3 TX
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
  //USART3 Rx  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOB, &GPIO_InitStructure);


	USART_InitStructure.USART_BaudRate=9600;   //波特率设置为9600	//波特率
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;		//数据长8位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;			//1位停止位
	USART_InitStructure.USART_Parity=USART_Parity_No;				//无效验
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None; //失能硬件流
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;	 //开启发送和接受模式
	USART_Init(USART3,&USART_InitStructure);	/* 初始化USART3 */
	USART_Cmd(USART3, ENABLE);		   /* 使能USART3 */
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//使能或者失能指定的USART中断 接收中断
	USART_ClearFlag(USART3,USART_FLAG_TC);//清除USARTx的待处理标志位


  //使能相应中断，
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : USART5_Init
// 功能描述  : 串口5初始化
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void usart5_init(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	USART_InitTypeDef  USART_InitStructure;	  //串口结构体定义
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOD|RCC_APB2Periph_AFIO, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); 

	/* USART5 Tx (PC.12),输出 USART5 Rx (PD.02)输入 */    
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; 
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
  	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOC, &GPIO_InitStructure);  
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  	GPIO_Init(GPIOD, &GPIO_InitStructure); 
	
	USART_InitStructure.USART_BaudRate=9600;   //波特率设置为9600	//波特率
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;		//数据长8位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;			//1位停止位
	USART_InitStructure.USART_Parity=USART_Parity_No;				//无效验
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None; //失能硬件流
	USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;	 //开启发送和接受模式
	USART_Init(UART5,&USART_InitStructure);	/* 初始化USART5 */
	USART_Cmd(UART5, ENABLE);		   /* 使能USART5 */
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//使能或者失能指定的USART中断 接收中断
	USART_ClearFlag(UART5,USART_FLAG_TC);//清除USARTx的待处理标志位

	/*开串口5接收中断*/
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;												  
	NVIC_Init(&NVIC_InitStructure);
}

/*******************************************************************************
* 函 数 名         : wg_amy_state()
* 函数功能		   : 网关直连按摩椅，查脚状态
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void wg_amy_state()
{u8 i;
	if(wg_state_flag==1)
	{
		wg_state_flag=0;
		for(i=0;i<16;i++)
		amy_check_ID[i]=AmyID[i];
							
		if(Shared_Module.Module==ruilaikesi)
		{
								i=armchair_io_state();
		}
		else
		{
			if(amy_cotrol_flag==1)
			{
			i=1;
			}
			else
				i=0;
		}
						if(i==0)//按摩椅状态读取成功
							{
								
							qwl_sendstring("控制成功\r\n");
								amystate_pack_send(0,0xff);//状态打包上传
							}
							else if(i==1)
							{	
							qwl_sendstring("控制失败\r\n");
							amystate_pack_send(1,0xff);//状态打包上传
							}	
							else if(i==2)
							{	
							qwl_sendstring("按摩椅坏了\r\n");
							amystate_pack_send(3,0xff);//状态打包上传
							}	
				
	}
	else if(wg_state_flag==2)
	{
		wg_state_flag=0;
		for(i=0;i<16;i++)
		amy_check_ID[i]=AmyID[i];
		if(Shared_Module.Module==ruilaikesi)
		{
								i=armchair_io_state_1();
		}
		else
			i=1;
						if(i==0)//按摩椅状态读取成功
							{
													amy_cotrol_time=0;//控制时间清0	
							qwl_sendstring("按摩椅启动失败，需要关闭按摩椅\r\n");
								amystate_pack_send(0,0xff);//状态打包上传
							}
							else if(i==1)
							{	
							qwl_sendstring("按摩椅启动成功\r\n");
							amystate_pack_send(1,0xff);//状态打包上传
							}	

	}
}


/*******************************************************************************
* 函 数 名         : amy_CHKSUM_produce()
* 函数功能		   : 刷新列表，去掉好久没连接的，10次心跳为一次剔除单位
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void amy_CHKSUM_produce()
{ u16 i;
	u8 j;
	amy_CHKSUM=0;
	for(i=0;i<250;i++)
		{
			
	if(amy_record_buf[i*21]>0)
			{
				for(j=0;j<16;j++)
				{amy_CHKSUM=amy_CHKSUM+amy_record_buf[(i*21)+4+j];
				}
			}
		}	
}

/*******************************************************************************
* 函 数 名         :GATEWAY_heard_link
* 函数功能		   	: 心跳连接
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void GATEWAY_heart_link()//心跳连接
{
	qwl_sendstring("发送心跳");
	qwl_sendchar(NO[CSQ>>4]);
	qwl_sendchar(NO[CSQ&0x0f]);
	if(poll_zd.poll_max_time >0)
	{
		qwl_sendchar(NO[(poll_zd.poll_time>>12)&0x0f]);
		qwl_sendchar(NO[(poll_zd.poll_time>>8)&0x0f]);
		qwl_sendchar(NO[(poll_zd.poll_time>>4)&0x0f]);
		qwl_sendchar(NO[(poll_zd.poll_time>>0)&0x0f]);
		qwl_sendchar(NO[(poll_zd.poll_max_time>>12)&0x0f]);
		qwl_sendchar(NO[(poll_zd.poll_max_time>>8)&0x0f]);
		qwl_sendchar(NO[(poll_zd.poll_max_time>>4)&0x0f]);
		qwl_sendchar(NO[(poll_zd.poll_max_time>>0)&0x0f]);
	}
	
		GATEWAY_heart_send_pack();//心跳发送组包
		USART3_sendstring1(post_buf,post_len);
		if(net.reconnect_setp==LINK_OK)
		{
			
			net_send(post_buf,post_len);
		}	
}
/*******************************************************************************
* 函 数 名         :GATEWAY_heart_send
* 函数功能		   	: 心跳发送并检验处理
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void GATEWAY_heart_send()//心跳发送并检验处理
{
	switch (GATEWAY_deal_time)
	{
		case 0:
		{
			if(GATEWAY_heart_time_flag>=GATEWAY_heart_max_flag)//达到预定的发送时间值，上传心跳
			{
				GATEWAY_heart_time_flag=0;//心跳时间重新计时
				GATEWAY_heart_link();//心跳连接
				GATEWAY_deal_time=1;//等待反馈
				getwayheart_sendok_flag=0;//心跳发送成功否，确认标志
			}
			else
				deal_process=0;//进行下一个进程处理
						
		}break;
		case 1:
		{
			if(GATEWAY_heart_time_flag>5)//等待超时，服务器问题
			{
				GATEWAY_deal_time=0;//等待失败
				getwayheart_sendok_flag=0;//心跳发送成功否，确认标志
				amy_config=amy_config|0x40;//置位数码管显示
				qwl_sendstring("等待服务器超时1\r\n");
				deal_process=0;//进行下一个进程处理
				GATEWAY_heart_max_flag=10;//超时，请求加速
				
				net_error_flag++;

			}
			
			if(getwayheart_sendok_flag==1)//心跳发送成功
			{//getwayheart_sendok_flag=0;//心跳发送成功否，确认标志
				amy_config=(amy_config&0xbf);//复位数码管显示
			GATEWAY_deal_time=0;//等待成功，重新定时发送心跳
				qwl_sendstring("服务器发送成功1\r\n");
				deal_process=0;//进行下一个进程处理
				net_error_flag=0;
			}
						

		}break;
		default:break;
	}
	
}
/*******************************************************************************
* 函 数 名         :GATEWAY_list_link
* 函数功能		   	: 列表上传
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
/*void GATEWAY_list_link()//心跳连接
{
	qwl_sendstring("列表组包后上传\r\n");	
	GATEWAY_list_send_pack();//心跳发送组包
	USART3_sendstring1(post_buf,post_len);
if(net.reconnect_setp==LINK_OK)
{
	net_send(post_buf,post_len);
}	

				
				
}

*/
/*******************************************************************************
* 函 数 名         :GATEWAY_list_send
* 函数功能		   	: 列表上传并自我检验处理
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
/*void GATEWAY_list_send()//列表上传并自我检验处理
{
	switch(GATEWAY_list_time)
	{
		case 0:
		{
			u8 i;

							if(getwaylist_send_flag>0)//有新设备，或者设备更新，要求上传
						{
										if((amy_total)==0)
										{qwl_sendstring("终端数量清零后也需要上传一下\r\n");
											getwaylist_send_times=0;
										}											
										else
										{
											i=amy_total/13;
											if((amy_total%13)>0)
												i++;
											
											getwaylist_send_times=i;
										}
													GATEWAY_list_time_flag=0;//列表时间重新计时

								
																
													GATEWAY_list_link();//列表上传

														GATEWAY_list_time=1;//等待反馈
														getwaylist_sendok_flag=0;//心跳发送成功否，确认标志

											
						}
						else
							deal_process=1;//进行下一个进程处理
		}break;
		case 1:
		{
			if(GATEWAY_list_time_flag>3)//等待超时，服务器问题
			{
				getwaylist_send_times=0;
				GATEWAY_list_time=0;//等待失败
				getwaylist_send_flag=0;//列表要求重新上传标志清除，已经发送
				//getwaylist_sendok_flag=0;//心跳发送成功否，确认标志
				amy_config=amy_config|0x40;//置位数码管显示
				qwl_sendstring("等待服务器超时2\r\n");
				deal_process=1;//进行下一个进程处理
			}

			if(getwaylist_sendok_flag==1)//列表发送成功
			{getwaylist_sendok_flag=0;//列表发送成功否，确认标志
				amy_config=(amy_config&0xbf);//复位数码管显示
			
				if(getwaylist_send_times==0)
				{getwaylist_send_flag--;//列表要求重新上传标志清除，已经发送完毕
					deal_process=1;//进行下一个进程处理
					GATEWAY_list_time=0;//等待成功，重新定时发送心跳
				}
				else
				{GATEWAY_list_time=2;
				}
				qwl_sendstring("服务器发送成功2\r\n");
				
			}
						

		}break;
		
		case 2:
		{

											
											
													GATEWAY_list_time_flag=0;//列表时间重新计时
								
																
													GATEWAY_list_link();//列表上传

														GATEWAY_list_time=1;//等待反馈
														getwaylist_sendok_flag=0;//心跳发送成功否，确认标志
			}break;
		default:break;
	}
	
}
*/
/*******************************************************************************
* 函 数 名         :POLL_list_link
* 函数功能		   	: 列表上传
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void POLL_list_link()//心跳连接
{
	qwl_sendstring("列表组包后上传\r\n");	
	poll_list_send_pack();//心跳发送组包
	USART3_sendstring1(post_buf,post_len);
if(net.reconnect_setp==LINK_OK)
{
	net_send(post_buf,post_len);
}	

				
				
}


/*******************************************************************************
* 函 数 名         :POLL_list_send
* 函数功能		   	: 列表上传并自我检验处理
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void POLL_list_send()//列表上传并自我检验处理
{
	switch(GATEWAY_list_time)
	{
		case 0:
		{

							if(poll_zd.state_send_flag>0)//有新设备，或者设备更新，要求上传
						{
GATEWAY_list_time_flag=0;//列表上传

																
													POLL_list_link();//列表上传

														GATEWAY_list_time=1;//等待反馈
														getwaylist_sendok_flag=2;//心跳发送成功否，确认标志

											
						}
						else
							deal_process=1;//进行下一个进程处理
		}break;
		case 1:
		{
			if(GATEWAY_list_time_flag>5)//等待超时，服务器问题
			{
GATEWAY_list_time_flag=0;//列表上传

				GATEWAY_list_time=0;//等待失败
				poll_zd.state_send_flag=0;
				//poll_zd.state_send_flag=0;//列表要求重新上传标志清除，已经发送
				//getwaylist_sendok_flag=0;//心跳发送成功否，确认标志
				amy_config=amy_config|0x40;//置位数码管显示
				qwl_sendstring("等待服务器超时2\r\n");
				deal_process=1;//进行下一个进程处理
			}

			if(getwaylist_sendok_flag==1)//列表发送成功
			{getwaylist_sendok_flag=0;//列表发送成功否，确认标志
				amy_config=(amy_config&0xbf);//复位数码管显示
							
					getwaylist_send_flag=0;//gatewaylist_send_flag--;//列表要求重新上传标志清除，已经发送完毕
					deal_process=1;//进行下一个进程处理
					GATEWAY_list_time=0;//等待成功，重新定时发送心跳

					poll_zd.state_send_flag=0;
				qwl_sendstring("服务器发送成功2\r\n");
				
			}
						

		}break;
		
		
		default:break;
	}
	
}

/*******************************************************************************
* 函 数 名         :POLL_list_link
* 函数功能		   	: 列表上传
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void amy_off_link()//心跳连接
{
	qwl_sendstring("按摩椅工作停止上传\r\n");	
	amy_off_send_pack();//心跳发送组包
	USART3_sendstring1(post_buf,post_len);
if(net.reconnect_setp==LINK_OK)
{
	net_send(post_buf,post_len);
}	

				
				
}


/*******************************************************************************
* 函 数 名         :POLL_list_send
* 函数功能		   	: 列表上传并自我检验处理
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void amy_off_send()//列表上传并自我检验处理
{
	switch(GATEWAY_off_time)
	{
		case 0:
		{

				if((getwayoff_send_flag&0x01)>0)
				{
						

																
													amy_off_link();//列表上传

														GATEWAY_off_time=1;//等待反馈
														getwayoff_sendok_flag=2;//心跳发送成功否，确认标志

									GATEWAY_off_time_flag=0;//列表上传	
				}
		}break;
		case 1:
		{
			if(GATEWAY_off_time_flag>5)//等待超时，服务器问题
			{
				GATEWAY_off_time_flag=0;//列表上传

				GATEWAY_off_time=0;//等待失败
				qwl_sendstring("等待服务器超时3\r\n");
				
			}

			if(getwayoff_sendok_flag==1)//列表发送成功
			{getwayoff_sendok_flag=0;//列表发送成功否，确认标志
				amy_config=(amy_config&0xbf);//复位数码管显示
							
					getwayoff_send_flag=0;//gatewaylist_send_flag--;//列表要求重新上传标志清除，已经发送完毕
					GATEWAY_off_time=0;//等待成功，重新定时发送心跳
				qwl_sendstring("服务器发送成功3\r\n");
				
			}
						

		}break;
		
		
		default:break;
	}
	
}
/*******************************************************************************
* 函 数 名         :GATEWAY_heart_send
* 函数功能		   	: 心跳发送并检验处理
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void SIM_ID_SEND()//心跳发送并检验处理
{
	if(SIM_GET_FLAG>0)
	{SIM_GET_FLAG=0;
		SIM_ID_send_pack();
	USART3_sendstring1(post_buf,post_len);
if(net.reconnect_setp==LINK_OK)
{
	net_send(post_buf,post_len);
}	
	}
}
/*******************************************************************************
* 函 数 名         : suibian_send()
* 函数功能		   : 
* 输    入         : 
* 输    出         : 
*******************************************************************************/
void suipian_send()
{
	if(suipian_delay>15)
	{
		suipian_delay=0;
		if(exchange_platform.Platform_Choice != PLATFORM_CHOICE_UPDATE)
		{
		if(net.reconnect_setp==LINK_OK)
			if(net.mode_type ==MODE_TYPE_ZTE)
				{
							switch(g_flag)
								{
									case 2:
									{
										
									}
									break;
									case 3:
									{
										
									}
									break;
									default:
									{
										qwl_sendstring("发送随便数据，为了保持快速反应");
										net_send(0x00,1);//移动模块为MG2618或者电信MC8618
									}break;
								}
				}
		}
	}

}
/*******************************************************************************
* 函 数 名         :GATEWAY_send()
* 函数功能		   	: 网关发送处理
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void GATEWAY_send()//对网关组网发送打包处理
{

	amy_CHKSUM_produce();
	GATEWAY_heart_send();//心跳发送，并自我验证是否发送成功
	POLL_list_send();
	SIM_ID_SEND();
	suipian_send();//为了保持快速反应
	amy_off_send();//发送按摩椅停止信号
/*	switch(deal_process)//心跳正在传送，等待
		{	case 0:
			{

			
			GATEWAY_list_send();//列表上传，并自我验证是否发送成功
			}break;
			case 1: 
			{
			GATEWAY_heart_send();//心跳发送，并自我验证是否发送成功
			}break;
			default: break;
		}*/

}
/*******************************************************************************
* 函 数 名         :amystate_pack_send
* 函数功能		   	: 按摩椅打包上传
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void amystate_pack_send(u8 state,u8 addr)
{

	amystate_send_pack(state,addr);//状态发送组包
	USART3_sendstring1(post_buf,post_len);
	if(net.reconnect_setp==LINK_OK)
{
	net_send(post_buf,post_len);
}	
}
/*******************************************************************************
* 函 数 名         :amy_state_send()
* 函数功能		   	: 终端状态发送
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void amy_state_send()
{
	u16 i;
	u8 j;
	u8 n;
	u16 k;
	u8 jj=0x03;
	u8 addr[3];
//	u8 *read_flash;

	//u32 AMYID_adr=0x0803D000;//按摩椅存储ID地址
	//u8 *AmyID;
	for(i=0;i<amy_max_total;i++)
		{if(config_recwait_buf[i]>0)//查询有没需要等待反馈的终端
			break;
		}
if(i>=amy_max_total)//没有需要反馈的终端，自我查询
		{		
				switch (poll_zd.poll_deal )
				{
					case 0://列表状态不请求状态下
					{
						if(poll_zd.list_please_flag >0)//请求到列表了再进行查询，不然不进行轮询
						{
								if(poll_zd.poll_max_time>0)
								{
									if(poll_zd.poll_time >=poll_zd.poll_max_time )
									{
										poll_zd.poll_time=0;
										poll_zd.poll_deal =1;
									}
								}
								else
								{
									poll_zd.list_please_flag=0;
									poll_zd.poll_time=0;
									poll_zd.poll_deal =1;
								}
						}
						
					}break;
					case 1://请求列表中的终端状态，轮询方式
					{
						amy_check_pointer++;
								if(amy_check_pointer>=amy_max_total)
									amy_check_pointer=0;
						for(;amy_check_pointer<amy_max_total;amy_check_pointer++)
						{j=poll_zd.zd_list[(amy_check_pointer/8)];
							//qwl_sendchar(NO[j>>4]);
							//qwl_sendchar(NO[j&0x0f]);
							
							n=amy_check_pointer%8;
							//qwl_sendchar(NO[n>>4]);
							//qwl_sendchar(NO[n&0x0f]);
							j=j>>n;
							//qwl_sendchar(NO[j>>4]);
							//qwl_sendchar(NO[j&0x0f]);
							if((j&0x01)!=0)
							{	
								
								break;
								
							}

						}
						if(amy_check_pointer>=amy_max_total)
							{
								poll_zd.poll_deal =0;
								poll_zd.state_send_flag =1;
									amy_check_pointer=0;
								qwl_sendstring("轮询一次完毕");
							}
							else{

									config_recwait_buf[amy_check_pointer]=0x01;
									addr[0]=0;
									addr[1]=amy_check_pointer;
									addr[2]=q2x1278_config_buf[4];
									amy_config_please(&addr[0]);
									config_recwait_time=0;
								
									}
					}break;
				
					default:break;
				}
			
					
					
					
					
		}
		
/*		if(i>=amy_max_total)//没有需要反馈的终端，自我查询
		{
				if(amy_storflash_flag==0)//开机后优先查flash内数据
					{
						read_flash=(u8*)(flash_AMY_adr1);
										for(i=amy_storflash_pointer;i<amy_max_total;i++)
										{
											amy_storflash_pointer++;//查询标志
											if(amy_storflash_pointer>=amy_max_total)
											{amy_storflash_flag=1;
												amy_storflash_pointer=0;
											}
											
											if(read_flash[i*21])
											{
												qwl_sendstring("先查FLASH数据");
											addr[0]=read_flash[i*21+1];
											addr[1]=read_flash[i*21+2];
											addr[2]=q2x1278_config_buf[4];
											amy_config_please(&addr[0]);
											config_recwait_time=0;	
												break;
											}
										}
					}
			

	
	
		
			else
				{
					amy_check_pointer++;
					amy_storflash_pointer++;	
					
					if(amy_check_pointer>=amy_max_total)
					amy_check_pointer=0;
					
										if(amy_storflash_pointer>=amy_max_total)//第一次轮循后，存储进flash
												{
													amy_storflash_pointer=0;
													
													if(amy_storflash_flag==1)//将列表存储到flash
													{
														if(amy_total>0)//有终端数量再存入FLASH
														{
																qwl_sendstring("存入FLASH");
															//for(k=0;k<5250;k++)
															//	{
															//		qwl_sendchar(NO[amy_record_buf[k]>>4]);
															//			qwl_sendchar(NO[amy_record_buf[k]&0x0f]);
															//	}
																amy_storflash_flag=2;
																
																Write_Flash_ALLAMY();
														}
													 }	
												 }

					addr[0]=0;
					addr[1]=amy_check_pointer;
					addr[2]=q2x1278_config_buf[4];
					amy_config_please(&addr[0]);
					config_recwait_time=0;
				}
		}
		
	*/	
	amy_storflash_flag=2;
	
	if(config_recwait_time>1100)//超时1100ms,加1
	{config_recwait_time=0;
		
			for(i=0;i<amy_max_total;i++)
		{
			if(config_recwait_buf[i]>0)
			{
				if((config_recwait_buf[i]&0x0f)>5)
				{qwl_sendstring("状态上传处理时间太慢1");
					config_recwait_buf[i]=6;
				}
				if((config_recwait_buf[i]&0xf0)==0)
				{
					config_recwait_buf[i]++;
//					qwl_sendstring("超时加一");
//					qwl_sendchar(NO[(i>>4)]);
//					qwl_sendchar(NO[i&0x0f]);
				}
			}
		}
	}
	
	for(i=0;i<amy_max_total;i++)
	{
		if(config_recwait_buf[i]>0)
		{																	
			if(config_recwait_buf[i]&0xf0)
			{				
				config_recwait_buf[i]=0;
							k=i*21;
							for(n=0;n<16;n++)
							{
								amy_check_ID[n]=amy_record_buf[k+4+n];
								
							}
									j=amy_record_buf[k+20];
											if(j&0x04)//转换成平台能识别的故障码
													{j=1;
													}
													else if(j&0x20)//转换成平台能识别的故障码
													{j=3;
													}
												else if(amy_record_buf[k]>2)//表示连续有3个终端上传时间内没收到终端数据
												{j=2;
													qwl_sendstring("状态上传处理时间太慢2");
												}
												else if(1)
												{j=0;
												}
												
												poll_zd.zd_state[(i*2/8)]=(poll_zd.zd_state[(i*2/8)]&(~(jj<<(i*2%8)))|(j<<(i*2%8)));//存取终端状态
												
												if(amystate_send_flag[i]==1)//按摩椅状态发送标志
												{amystate_pack_send(j,i);//状态打包上传
													amystate_send_flag[i]=0;//按摩椅状态发送标志
												}
			}
			else if((config_recwait_buf[i]&0x0f)>1)
				{
//					qwl_sendstring("超时读取");
//					qwl_sendchar(NO[(i>>4)]);
//					qwl_sendchar(NO[i&0x0f]);
					k=i*21;
							

												if(amy_record_buf[k]>0)//表示连续有3个终端上传时间内没收到终端数据
												{
														j=2;//超时
												poll_zd.zd_state[(i*2/8)]=(poll_zd.zd_state[(i*2/8)]&(~(jj<<(i*2%8)))|(j<<(i*2%8)));//存取终端状态
													qwl_sendstring("状态上传处理更新问题3");
													qwl_sendchar(NO[i>>4]);
													qwl_sendchar(NO[i&0x0f]);
													if(amy_storflash_flag!=0)//查询flash时不进行终端加减
													{
														//if(amystate_send_flag[i]==0)//按摩椅状态需要发送时，不进行终端加减处理
														//{
																	if(amy_record_buf[k]>=3)
																			{
																				for(k=0;k<21;k++)
																				{	qwl_sendchar(amy_record_buf[i*21+k]);
																					amy_record_buf[i*21+k]=0;//超时没有显示，剔除
																				
																				}
																				getwaylist_send_flag=1;//列表要求重新上传标志
																				getwaylist_send_times=0;
																								if(amy_total==0)
																								{qwl_sendstring("终端计数问题1\r\n");
																								}
																				qwl_sendstring("终端计数减少\r\n");				
																				amy_total--;//总数下降

																			}
																			else
																			{
																				
																				//if(amy_check_pointer==0)//再次请求，还不在线清除
																				//amy_check_pointer=249;
																				//else
																				//amy_check_pointer--;
																					
																			amy_record_buf[k]++;
																			}
															//}
														}
												}
												
										
													
											else
												{qwl_sendstring("指针");
													
													qwl_sendchar(NO[amy_check_pointer>>4]);
													qwl_sendchar(NO[amy_check_pointer&0x0f]);
													qwl_sendchar(NO[i>>4]);
													qwl_sendchar(NO[i&0x0f]);												

													qwl_sendchar(0x0d);
													qwl_sendchar(0x0a);
													j=2;//超时
												poll_zd.zd_state[(i*2/8)]=(poll_zd.zd_state[(i*2/8)]&(~(jj<<(i*2%8)))|(j<<(i*2%8)));//存取终端状态
												}

												
						
												j=2;//超时
												//poll_zd.zd_state[(i*2/8)]=(poll_zd.zd_state[(i*2/8)]&(~(jj<<(i*2%8)))|(j<<(i*2%8)));//存取终端状态
											if(amystate_send_flag[i]==1)//按摩椅状态发送标志
												{

													for(n=0;n<16;n++)
														{
															amy_check_ID[n]=amy_record_buf[k+4+n];

														}
													qwl_sendstring("读状态超时");
													amystate_pack_send(j,i);//状态打包上传
													amystate_send_flag[i]=0;//按摩椅状态发送标志
												}
												config_recwait_buf[i]=0;
			}
			
		}
	}
	
		
}

/*******************************************************************************
* 函 数 名         :amyack_send
* 函数功能		   	: 按摩椅打包上传
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void amystate_send(u8 state)
{

	amyack_send_pack(state);
	USART3_sendstring1(post_buf,post_len);
			if(net.reconnect_setp==LINK_OK)
		{
			net_send(post_buf,post_len);
		}	
}



