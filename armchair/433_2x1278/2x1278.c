#include "config.h"

u8 q2x1278_config_buf[6];//433配置参数
u8 q2x1278_config_buf1=0;//用于按键在调试模块地址时零时存储，方便按键超时退出使用

u8 config_recwait_buf[256]={0};//配置反馈等待区域
u16 config_recwait_time=0;//配置反馈等待计时

u8 sx1278_fb_flag=0;


u8 Uart_RecData_buf[MAX_RX_BUF];
u8 rx_pointer=0;


u8 Uart2_RecData_buf[MAX_RX2_BUF];
u8 rx2_pointer=0;

/*******************************************************************************
* 函 数 名         : USART2_sendchar(u8 ch)
* 函数功能		   : 串口2发送数据位
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

void USART2_sendchar(u8 ch)
{
			USART_SendData(USART2,ch);//通过外设USARTx发送单个数据
		
		while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);

}

/*******************************************************************************
* 函 数 名         : USART2_sendstring(u8 *p)
* 函数功能		   : 串口2发送数据位
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void USART2_sendstring(u8 *p)
{
  	while(*p)
  	{
  		USART2_sendchar(*p);
  		p++;
	}

}
/*******************************************************************************
* 函 数 名         : q2x1278_fb
* 函数功能		   : 433配置
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void q2x1278_fb()
{u8 i;
	if(sx1278_fb_flag)
	{
		sx1278_fb_flag=0;
		TM1618_DISPLAY_NO[0]=0;	
			amy_config=amy_config&0xf7;//进入运行状态
	
				i=q2x1278_set();//进行参数配置
			if(i==0)
			qwl_sendstring("433配置成功");
			else
				qwl_sendstring("433配置失败");
			
			if(q2x1278_config_buf[2]!=0xff)
			PLC_time_tiqian=1;//加速发送心跳//方便安装人员马上知道按摩椅状态	
	}
}
/*******************************************************************************
* 函 数 名         : q2x1278_init
* 函数功能		   : 433初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void q2x1278_init()	   //
{
	GPIO_InitTypeDef GPIO_InitStructure;	  //声明一个结构体变量，用来初始化GPIO

	/* 开启GPIO时钟 */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);

	
	/*  配置GPIO的模式和IO口 */
	GPIO_InitStructure.GPIO_Pin=q2x1278_M0|q2x1278_M1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	;	//推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOE,&GPIO_InitStructure);
				
}


/*******************************************************************************
* 函 数 名         : q2x1278_rec_cofig
* 函数功能		   : 433参数读取
* 输    入         : 无
* 输    出         : 0，正常，1不正常。全局函数，433配置
*******************************************************************************/
u8 q2x1278_rec_cofig()	   //
{u16 j=0;
u8 read_cofig_command[3]= {0xc1,0xc1,0xc1};

	GPIO_SetBits(GPIOE,q2x1278_M0);	//对M0输出1
	GPIO_SetBits(GPIOE,q2x1278_M1);	//对M1输出1,进入设置状态
	rx2_pointer=0;
	//Tm618_Smg_clear();//清数码管
	while(1)//等待接受数据
	{

		u8 i;
		u8 k;
		u8 w[2]={0xc0,0x00};
		u8 n=0;
		USART2_sendstring(read_cofig_command);
			Delay_MS(500);
		API_WatchDog_Feed();
	//Tm618_Smg_Line(n);//此段只用于数码管显示检测的---
		n++;
	if(n>4)
		n=0;
	
		if(rx2_pointer>5)
		{
			
			


			i=strsearch(w,Uart2_RecData_buf,rx2_pointer);

		
 			if(i>0)
			{
				for(k=0;k<6;k++)
				{
					q2x1278_config_buf[k]=Uart2_RecData_buf[i+k-1];
					qwl_sendchar(NO[Uart2_RecData_buf[k]>>4]);
				qwl_sendchar(NO[Uart2_RecData_buf[k]&0x0f]);
				}
				
				break;
			}
		}
 
		j++;
		
		if(j>5)
		{	GPIO_ResetBits(GPIOE,q2x1278_M0);	//对M0输出0
			GPIO_ResetBits(GPIOE,q2x1278_M1);	//对M1输出0,进入发送状态
			return 1;
		}
		
	}
	
	GPIO_ResetBits(GPIOE,q2x1278_M0);	//对M0输出0
	GPIO_ResetBits(GPIOE,q2x1278_M1);	//对M1输出0,进入发送状态
return 0;	
}
/*******************************************************************************
* 函 数 名         : q2x1278_set()
* 函数功能		   : 配置数
* 输    入         : 无
* 输    出         : 0，正常，1不正常。全局函数，433配置
*******************************************************************************/
u8 q2x1278_set()	   //
{u16 j=0;
		u8 k;
u8 i;
	u8 rec_no=0;
	GPIO_SetBits(GPIOE,q2x1278_M0);	//对M0输出1
	GPIO_SetBits(GPIOE,q2x1278_M1);	//对M1输出1,进入设置状态
	rx2_pointer=0;
	Tm618_Smg_clear();//清数码管

	while(1)//等待接受数据
	{
		u8 n=0;
		

		for(k=0;k<6;k++)
		{USART2_sendchar(q2x1278_config_buf[k]);
//			qwl_sendchar(NO[q2x1278_config_buf[k]>>4]);
//			qwl_sendchar(NO[q2x1278_config_buf[k]&0x0f]);
		}
		
			Delay_MS(500);
		API_WatchDog_Feed();
	Tm618_Smg_Line(n);//此段只用于数码管显示检测的---
		n++;
	if(n>2)
		n=0;
	
		if(rx2_pointer>5)
		{
						
			i=strsearch("OK",Uart2_RecData_buf,rx2_pointer);

 			if(i>0)
			{
				qwl_sendstring("配置返回OK");
				break;
			}
		}
 
		j++;
		
		if(j>5)
		{GPIO_ResetBits(GPIOE,q2x1278_M0);	//对M0输出0
	GPIO_ResetBits(GPIOE,q2x1278_M1);	//对M1输出0,进入发送状态
			
			rec_no=1;
			break;
		}
		
	}
	
	amy_config=0;
	amy_config1=0;
	
	i=q2x1278_rec_cofig();//433模块检查，读取配置参数并显示
	if(i==0)
		{

		qwl_sendstring("433存在");
		

			TM1618_DISPLAY_NO[0]=0x00;
			TM1618_DISPLAY_NO[2]=(q2x1278_config_buf[2]&0x0f);//数码管显示赋值
			
			TM1618_DISPLAY_NO[1]=(q2x1278_config_buf[2]>>4);//数码管显示赋值
			
			amy_config=amy_config|0x01;//网关存在否
			
			if(q2x1278_config_buf[2]==0xff)//
			amy_config=amy_config|0x02;//终端还是网关	
					amy_config=amy_config|0x10;//默认网络断线
		}
	else
		{
			qwl_sendstring("433不存在");
			amy_config=amy_config&0xfe;//网关存在否
			amy_config=amy_config&0xfc;//终端还是网关
		}	
	GPIO_ResetBits(GPIOE,q2x1278_M0);	//对M0输出0
	GPIO_ResetBits(GPIOE,q2x1278_M1);	//对M1输出0,进入发送状态
		

	
return rec_no;	
}

/*******************************************************************************
* 函 数 名         : q2x1278_check_send()
* 函数功能		   : 终端发送到网关是否成功，进行读取
* 输    入         : 无
* 输    出         : 
*******************************************************************************/
u8 q2x1278_check_send()
{u8 i;
 u8 j=3;

	if(rx2_pointer>5)
		{

			i=strsearch("SEND_OK",Uart2_RecData_buf,rx2_pointer);
 			if(i>0)
			{
				j=0;
				rx2_pointer=0;
			}
			
			i=strsearch("ERROR01",Uart2_RecData_buf,rx2_pointer);
 			if(i>0)
			{
				j=1;//频段相同
				rx2_pointer=0;
			}
			
			i=strsearch("ERROR02",Uart2_RecData_buf,rx2_pointer);
			
 			if(i>0)
			{
				j=2;//ID相同
				rx2_pointer=0;
			}			
		}
 	

return j;
}
/*******************************************************************************
* 函 数 名         : q2x1278_plc_send()
* 函数功能		   : 终端组网发送数据
* 输    入         : 无
* 输    出         : 
*******************************************************************************/
void q2x1278_plc_send()
{
	u32 flash_adr=0x0803e800;//flash存储地址
	u8 i;
	u8 send_buf[28]={0};
	u8 *read_flash;
			read_flash=(u8*)(flash_adr);
			send_buf[0]=0xff;//包头
			send_buf[1]=26;			//包长度不包含头包尾，从数据长度开始数
			send_buf[2]=plc_state;//发送状态命令，
			
		send_buf[3]=(q2x1278_config_buf[1]>>4)+0x30;//模块地址高位，转成ASCII发送
		send_buf[4]=(q2x1278_config_buf[1]&0x0f)+0x30;//模块地址高位，转成ASCII发送
	
		send_buf[5]=(q2x1278_config_buf[2]>>4)+0x30;//模块地址低位，转成ASCII发送
		send_buf[6]=(q2x1278_config_buf[2]&0x0f)+0x30;//模块地址低位，转成ASCII发送
	
		send_buf[7]=(q2x1278_config_buf[4]>>4)+0x30;//频段地址，转成ASCII发送
		send_buf[8]=(q2x1278_config_buf[4]&0x0f)+0x30;//频段地址，转成ASCII发送
			
	
			for(i=0;i<16;i++)
				{send_buf[i+9]=*read_flash;
				 read_flash++;
				}				
		send_buf[25]=(amy_config>>4)+0x30;//状态，转成ASCII发送
		send_buf[26]=(amy_config&0x0f)+0x30;//状态，转成ASCII发送
		send_buf[27]=0xfe;//结尾符号
				
				

				USART2_sendchar(0x00);//网关模块地址
				USART2_sendchar(0xff);//
				USART2_sendchar(q2x1278_config_buf[4]);//频段
				for(i=0;i<28;i++)//发送给网关数据
				USART2_sendchar(send_buf[i]);
				
				qwl_sendstring("终端ID");
				for(i=9;i<27;i++)//发送给网关数据
				qwl_sendchar(send_buf[i]);
		
		
}

/*******************************************************************************
* 函 数 名         :amy_config_send_1()
* 函数功能		   	: 终端配置发送
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
u8 amy_config_send_1(u8 addr)
{
		u8 send_buf[13]={0};
		u16 i;
		u8 j;
if(amy_config_rec_flag==1)
{amy_config_rec_flag=0;
	addr=addr+1;
		i=addr*21-1;
	if(i>0)
	{qwl_sendstring("找到终端地址");
	}
	if(i==0)
	{qwl_sendstring("没找到终端地址");
		amystate_send(0x06);
		return 1;
	}
	j=(i/21);
	
	config_recwait_buf[j]=0x01;
amystate_send_flag[j]=1;//按摩椅状态发送标志
			send_buf[0]=amy_record_buf[i-19];
		send_buf[1]=amy_record_buf[i-18];
		send_buf[2]=amy_record_buf[i-17];
					send_buf[3]=0xff;//包头
				send_buf[4]=8 ;//长度
		send_buf[5]=0xb0;// 命令类型，配置时间，模式
		
		for(j=0;j<3;j++)
		{send_buf[6+j*2]=((plc_cofig_buf_1[j]>>4)+0x30);
		send_buf[7+j*2]=((plc_cofig_buf_1[j]&0x0f)+0x30);
		}
		send_buf[12]=0xfe;//结尾帧
		for(i=0;i<13;i++)//发送给网关数据
		USART2_sendchar(send_buf[i]);
		
		qwl_sendstring("发送配置数据888");
			for(i=0;i<13;i++)//发送给网关数据
		{qwl_sendchar(NO[send_buf[i]>>4]);
		qwl_sendchar(NO[send_buf[i]&0x0f]);
		}
		config_recwait_time=0;
}
return 0;
}
/*******************************************************************************
* 函 数 名         :amy_config_send()
* 函数功能		   	: 终端配置发送
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
u8 amy_config_send()
{
		u8 send_buf[13]={0};
		u16 i;
		u8 j;
if(amy_config_rec_flag==1)
{amy_config_rec_flag=0;
		i=strsearch_qwl(plc_cofig_buf,amy_record_buf,5250,16);
	if(i>0)
	{qwl_sendstring("找到终端地址");
	}
	if(i==0)
	{qwl_sendstring("没找到终端地址");
		amystate_send(0x06);
		return 1;
	}
	j=(i/21);
	
	config_recwait_buf[j]=0x01;
amystate_send_flag[j]=1;//按摩椅状态发送标志
			send_buf[0]=amy_record_buf[i-19];
		send_buf[1]=amy_record_buf[i-18];
		send_buf[2]=amy_record_buf[i-17];
					send_buf[3]=0xff;//包头
				send_buf[4]=8 ;//长度
		send_buf[5]=0xb0;// 命令类型，配置时间，模式
		
		for(j=0;j<3;j++)
		{send_buf[6+j*2]=((plc_cofig_buf_1[j]>>4)+0x30);
		send_buf[7+j*2]=((plc_cofig_buf_1[j]&0x0f)+0x30);
		}
		send_buf[12]=0xfe;//结尾帧
		for(i=0;i<13;i++)//发送给网关数据
		USART2_sendchar(send_buf[i]);
		
		qwl_sendstring("发送配置数据888");
			for(i=0;i<13;i++)//发送给网关数据
		qwl_sendchar(send_buf[i]);
		config_recwait_time=0;
}
return 0;
}
/*******************************************************************************
* 函 数 名         :amy_config_please()
* 函数功能		   	: 按摩椅配置请求
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
u8 amy_config_please(u8 *addr)
{

	u8 i;
	u8 send_buf[13];
	
	config_recwait_buf[addr[1]]=0x01;

			send_buf[0]=addr[0];
		send_buf[1]=addr[1];
		send_buf[2]=addr[2];
					send_buf[3]=0xff;//包头
				send_buf[4]=2 ;//长度
		send_buf[5]=0xb1;// 命令类型，配置时间，模式
		
		
		send_buf[6]=0xfe;//结尾帧
		for(i=0;i<7;i++)//发送给网关数据
		USART2_sendchar(send_buf[i]);
		config_recwait_time=0;
		qwl_sendstring("请求状态");
//			for(i=0;i<7;i++)//发送给网关数据
//		{qwl_sendchar(NO[send_buf[i]>>4]);
//			qwl_sendchar(NO[send_buf[i]&0x0f]);
//		}
		

return 0;
}
/*******************************************************************************
* 函 数 名         :u8 amy_amy_please(u8 *send_buf,u8 len,u8 command);
* 函数功能		   	: 网关互联，通过rola
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
u8 amy_amy_please(u8 *send_buf,u8 len,u8 command,u8 *addr)
{

	u8 i;
	u8 n=0;
	u8 checksums=0;//累加和
u8 send_buf1[258];
			send_buf1[n]=addr[0];
				n++;
		send_buf1[n]=addr[1];
				n++;
		send_buf1[n]=addr[2];
				n++;
		send_buf1[n]=0xff;//包头
	n++;
		send_buf1[n]=(len+4);//长度
	n++;
		send_buf1[n]=command;// 命令类型，配置时间，模式
	n++;
	for(i=0;i<len;i++,n++)
	{
		send_buf1[n]=send_buf[i];
		checksums=checksums+send_buf[i];
	}
	send_buf1[n]=checksums;//校验码
	n++;
		send_buf1[n]=0xfe;//结尾帧
	n++;
		for(i=0;i<n;i++)//发送给网关数据
		USART2_sendchar(send_buf1[i]);

//			for(i=0;i<7;i++)//发送给网关数据
//		{qwl_sendchar(NO[send_buf[i]>>4]);
//			qwl_sendchar(NO[send_buf[i]&0x0f]);
//		}
		

return 0;
}
/*******************************************************************************
* 函 数 名         :amy_ack_send()
* 函数功能		   	: 终端配置发送
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
u8 amy_ack_send()
{
u8 i;
u8 send_buf[10];
while(wg_ack_times>0)//假如网关有需要处理的条数，进入处理
{
	qwl_sendstring("集中反馈数据");
//	qwl_sendchar(NO[(wg_ack_times>>4)]);
//	qwl_sendchar(NO[(wg_ack_times&0x0f)]);
//	qwl_sendchar(NO[(wg_deal_pointer>>4)]);
//	qwl_sendchar(NO[(wg_deal_pointer&0x0f)]);
	for(i=0;i<10;i++)
	{send_buf[i]=wg_ack_buf[wg_deal_pointer];

	wg_deal_pointer++;
	if(wg_deal_pointer>=200)
				wg_deal_pointer=0;
	}
	for(i=0;i<10;i++)
	{
			USART2_sendchar(send_buf[i]);
		
	}
		for(i=0;i<3;i++)
	{	
		qwl_sendchar(NO[send_buf[i]>>4]);
		qwl_sendchar(NO[send_buf[i]&0x0f]);
	}
			for(i=3;i<10;i++)
	{	
		qwl_sendchar(send_buf[i]);
	}
	Delay_MS(1);

	wg_ack_times--;
}
return 0;
}





