#include "config.h"


u8 amy_config=0;
u8 amy_config1=0;
u8 PLC_time_flag=0;
u8 PLC_time_tiqian=0;//终端没到定时时间就提前发送
u8 time_share_process_flag=0;//按摩椅分时处理标志标志
u8 ceshi=0;//用于测试状态卡死标志
u16 zd_overtime_flag=0;//终端超时标识，超时没有收到反馈，进行标志置位
u8 AmyID[16];



#define   MAX_AMY_BUF  5250			//存放250台按摩椅
u8 amy_record_buf[MAX_AMY_BUF]={0};		//按摩椅数据缓存区
u8 amy_rec_flag;//按摩椅接受标志位
u8 amy_total=0;//按摩椅总数
u8 amy_CHKSUM=0;//按摩椅校验码，所以终端ID相加和
u8 armchair_time_flag=0;//按摩椅状态读取防止超时标志

#define   MAX_AMY_BUF_1  50			//标志头 2位模块地址 1位频段地址 16终端ID 1位状态 1位校验和
u8 amy_record_buf_1[MAX_AMY_BUF_1]={0};		//按摩椅数据缓存区
u8 amy_rec_flag=0;//按摩椅接受标志位
u8 amy_rec_flag_1=0;//按摩椅接受标志位正数计数

u16 amy_cotrol_time=0;//按摩椅控制时间
u16 amy_cotrol_time_flag=0;//定时器中计时
u8 amy_cotrol_flag=0;//按摩椅控制标志
u8 IO_STATE=0;

u8 wg_ack_buf[200]={0};//网关收到数据后，进行反馈缓存，放入主程序内处理
u8 wg_stor_pointer=0;//网关存储指针
u8 wg_deal_pointer=0;//网关处理指针，当前处理到的位置
u8 wg_ack_times=0;//需要反馈的条数


u8 wg_state_flag=0;//查询网关智联按摩椅状态标志
u8 wg_set_flag=0;//网关直接启动按摩椅标志

u8 zd_update_flag=0;//终端升级标志
u8 armchair_fen_delay=0;
u16 armchair_s_delay=0;
void PLCtime_share_process(void);

u8 SIM_GET_FLAG=0;
Shared_TYPE Shared_Module;//设备选择

u8 shebei_flag=0;//设备状态报告

void ac_switch_IN(void);
void ac_switch_OUT(void);
/*******************************************************************************
* 函 数 名         : armchair_io_Init
* 函数功能		   : armchair_io初始化函数
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Armchair_IO_Init()	  //端口初始化
{
	GPIO_armchair_INIT();
	Tm618_IO_Init();
}
/*******************************************************************************
* 函 数 名         : slot_machines_data_send()
* 函数功能		   : 每分钟发一个低电平，间隔150ms，每次拉低1ms
* 输    入         : 0启动，1关闭
* 输    出         : 无
*******************************************************************************/
void slot_machines_data_send()
{
	qwl_sendstring("启动按摩椅");
	armchair_status_OUT=0;
	Delay_MS(40);
	armchair_status_OUT=1;
}
/*******************************************************************************
* 函 数 名         : void Shared_Module_switch(u8 module_state)
* 函数功能		   : 启动共享模块
* 输    入         : 0启动，1关闭
* 输    出         : 无
*******************************************************************************/
void Shared_Module_switch(u8 module_state)
{
	switch(Shared_Module.Module)
	{
		case oxygen_plant:
		{
			if(module_state==0)
			armchair_status_OUT=0;//制氧机启动
			else
			armchair_status_OUT=1;//制氧机关闭
		}break;
		case slot_machines:
		{
			qwl_sendstring("启动投币机");
			if(amy_cotrol_time>0)
			{u8 i;
				i=amy_cotrol_time/60;
				if(amy_cotrol_time%60)
				{
					i++;
				}
				if(amy_cotrol_flag!=1)
				API_SetTimer(150,i,TIMER_TYPE_QWL_TOUBIJI_DEAL,slot_machines_data_send,NULL);
			}
		}break;
		default:
		{
		}break;
	}
	
}
/*******************************************************************************
* 函 数 名         : API_ARMCHAIR_TIME
* 函数功能		   : 超时延时标志放置处
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void ARMCHAIR_TIME()
{
	config_recwait_time++;
	
	armchair_s_delay++;
	if(armchair_s_delay>1000)//秒计时
	{armchair_s_delay=0;
			armchair_fen_delay++;
			if(armchair_fen_delay>59)//分钟计时
			{
				armchair_fen_delay=0;
				poll_zd.poll_time ++;//分钟加一
			}
	}
}
/*******************************************************************************
* 函 数 名         :key_q2x1278_set();
* 函数功能		   	: 433设置
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void key_q2x1278_set()
{
	
		switch(TM1618_DISPLAY_NO[0])
	{
		case 0://标志位0，正常显示
		{
			if(TM1618_DISPLAY_NO[0]==0)
			TM1618_DISPLAY_NO[0]++;
			amy_config=amy_config|0x08;//进入调试状态
			TM1618_DISPLAY_NO[2]=(q2x1278_config_buf[2]&0x0f);//数码管显示赋值
			TM1618_DISPLAY_NO[1]=(q2x1278_config_buf[2]>>4);//数码管显示赋值
			};break;
		case 1://设置模块地址低位
		{u8 i;
			i=(TM1618_DISPLAY_NO[1]<<4)+TM1618_DISPLAY_NO[2];			
			q2x1278_config_buf1=i;//零时存储模块地址低位
			
			TM1618_DISPLAY_NO[0]++;
			TM1618_DISPLAY_NO[1]=(q2x1278_config_buf[4]>>4);
			TM1618_DISPLAY_NO[2]=(q2x1278_config_buf[4]&0x0f);

		};break;
		case 2://设置模块地址低位
		{u8 i;

			i=(TM1618_DISPLAY_NO[1]<<4)+TM1618_DISPLAY_NO[2];			
			q2x1278_config_buf[4]=i;
			q2x1278_config_buf[2]=q2x1278_config_buf1;
			q2x1278_config_buf[1]=0x00;//地址高位初始化默认为00	
			q2x1278_config_buf[0]=0xc0;//433配置命令
			q2x1278_config_buf[3]=0x1A;//波特率
			q2x1278_config_buf[5]=0xc0;//定点发送
			TM1618_DISPLAY_NO[0]=0;	
			amy_config=amy_config&0xf7;//进入运行状态
	
				i=q2x1278_set();//进行参数配置
			if(i==0)
			qwl_sendstring("433配置成功");
			else
				qwl_sendstring("433配置失败");
			
			if(q2x1278_config_buf[2]!=0xff)
			PLC_time_tiqian=1;//加速发送心跳//方便安装人员马上知道按摩椅状态
		};break;
	default:break;
	}
	

}
/*******************************************************************************
* 函 数 名         :key_process()
* 函数功能		   	: 按键处理
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void key_process()
{
	u8 key;
if(TM1618_DISPLAY_NO[0]!=0)//超时自动退出设置界面
{
	if(key_delay_out>59)
	{key_delay_out=0;
			TM1618_DISPLAY_NO[0]=0;	
			amy_config=amy_config&0xf7;//进入运行状态
			TM1618_DISPLAY_NO[2]=(q2x1278_config_buf[2]&0x0f);//数码管显示赋值
			TM1618_DISPLAY_NO[1]=(q2x1278_config_buf[2]>>4);//数码管显示赋值
	}
}
		key=read_key();
	if(key>0)//有按键才进行处理，不然放弃
	{key_delay_out=0;
	switch(key)
	{
		case 1:
		{
			key_q2x1278_set();//433设置
			
		}break;
		case 2:
		{u8 i;
			if(TM1618_DISPLAY_NO[0]!=0)//进入设置状态下才可以调试
			{
			i=(TM1618_DISPLAY_NO[1]<<4)+TM1618_DISPLAY_NO[2];			
			i++;//数据加
				if(TM1618_DISPLAY_NO[0]==0x02)
				{
					if(i>31)
					i=0x0;
				}
				else
				{
				if(i==0xff)
					i=0;
				if(i>=amy_max_total)
					i=0xff;
				}
			TM1618_DISPLAY_NO[2]=(i&0x0f);
			TM1618_DISPLAY_NO[1]=(i>>4);
				
			}
			
		}break;
		case 3:
		{u8 i;
				if(TM1618_DISPLAY_NO[0]!=0)//进入设置状态下才可以调试
			{
			i=(TM1618_DISPLAY_NO[1]<<4)+TM1618_DISPLAY_NO[2];
			i--;//数据减
				if(i==0xfe)
				{
					if(amy_max_total>0)
						i=amy_max_total-1;
					else
						i=0;
				}
				
				if(TM1618_DISPLAY_NO[0]==0x02)
				{
					if(i==0)
					{
					i=31;
					}
				}
					
			TM1618_DISPLAY_NO[2]=(i&0x0f);
			TM1618_DISPLAY_NO[1]=(i>>4);
			}
		}break;
	default:break;
	}
	if(amy_cotrol_time==0)
		Delay_MS(500);
}
	
	
	
}
/*******************************************************************************
* 函 数 名         :q2x1278_rec_or_send()
* 函数功能		   	: 433自我网关或者终端发送或者接受进行判断处理
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void q2x1278_rec_or_send()
{

if(exchange_platform.Platform_Choice==PLATFORM_CHOICE_PUBLIC)
	{
         GATEWAY_send();//对网关组网发送打包处理	
	}
				amy_ack_send();
		if((amy_config&0x02)==0x02)//假如按摩椅是网关，
		{

	if(exchange_platform.Platform_Choice==PLATFORM_CHOICE_PUBLIC)
	{
			

			amy_state_send();
			amy_config_send();

	LED=!LED;
	}
		}	
		
//------		
		else if((amy_config&0x02)==0x00)//假如按摩椅是终端
		{
				if((amy_config&0x01)==0x01)//假如按摩椅有，进行处理，不然直接退出
				{
					
							
						PLCtime_share_process();//对读取按摩椅状态和终端发送，进行分时处理
						
				}
			
		}
		
		wg_amy_state();//功能：用于网关直连按摩椅，直接启动查询按摩椅状态

		
}
/*******************************************************************************
* 函 数 名         : ARMCHAIR_DEAL
* 函数功能		   : 按摩椅处理
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void ARMCHAIR_DEAL()
{
//	config_recwait_time++；
		ceshi=1;
	Tm618_Display();//数码管显示，内部包含当在设定时，数码管闪烁显示
		ceshi=2;
	key_process();	//按键处理
		ceshi=3;
			
	q2x1278_rec_or_send();//进行自我判断是终端还是网关进行处理
		ceshi=4;
	//S1_rec_DEAL();	//串口1接受处理
	//	ceshi=6;
	q2x1278_fb();	//放中断里设置不成功，所以放这处理
}
/*******************************************************************************
* 函 数 名         : Self_Check()
* 函数功能		   	: 系统自检
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Self_Check()
{u8 i;
u8 j;
	
	qwl_sendstring("瑞莱克斯输入输出端口自检：\r\n");
				i=armchair_io_state();
			if(i==0)//按摩椅状态读取成功
							{	
							qwl_sendstring("IO端口正常\r\n");
							shebei_flag=shebei_flag&0xfe;//瑞莱克斯IO口正常
							}
							else if(i==1)
							{	
							qwl_sendstring("IO端口失败\r\n");
							shebei_flag=shebei_flag|0x01;//瑞莱克斯IO口问题
							}	
							else if(i==2)
							{	
							qwl_sendstring("端口或按摩椅坏了\r\n");
							shebei_flag=shebei_flag|0x01;//瑞莱克斯IO口问题
							}	
							
								ac_switch_IN();
	
							if(GPIO_ReadInputDataBit(GPIOE,ac_status_out))
							{	
								qwl_sendstring("投币端口检测高\r\n");
							shebei_flag=shebei_flag&0xFD;//投币端口
							}
							else
							{
								qwl_sendstring("投币端口检测低\r\n");
							shebei_flag=shebei_flag|0x02;//投币端口
							}
							ac_switch_OUT();
	qwl_sendstring("flashID:");
	qwl_sendchar(NO[dev_id>>4]);
	qwl_sendchar(NO[dev_id&0x0f]);
							if(dev_id>0)
							{
								qwl_sendstring("flash正常");
								shebei_flag=shebei_flag&0xF7;//flash正常
							}
							else
							{qwl_sendstring("flash失败");
								shebei_flag=shebei_flag|0x08;//flash正常
							}
							
				if((shebei_flag&0x09)==0)
				{
					if(shebei_flag&0x02)
					{
						Tm618_Smg_8882();//显示数码管无点
						
					}
					else
					{
						Tm618_Smg_8881();//显示数码管有点
					}
				}
					
	i=q2x1278_rec_cofig();//433模块检查，读取配置参数并显示
	if(i==0)
		{

		qwl_sendstring("433存在");
		shebei_flag=shebei_flag|0x04;//433存在
			TM1618_DISPLAY_NO[0]=0x00;
			TM1618_DISPLAY_NO[2]=(q2x1278_config_buf[2]&0x0f);//数码管显示赋值
			
			TM1618_DISPLAY_NO[1]=(q2x1278_config_buf[2]>>4);//数码管显示赋值
			
			amy_config=amy_config|0x01;//网关存在否
			
			if(q2x1278_config_buf[2]==0xff)//
			amy_config=amy_config|0x02;//终端还是网关	
			else
			{
				
				j=q2x1278_config_buf[2];
			
			TM1618_DISPLAY_NO[0]=0;

			TM1618_DISPLAY_NO[2]=(j&0x0f);//数码管显示赋值
			
			TM1618_DISPLAY_NO[1]=(j>>4);//数码管显示赋值
			}
					amy_config=amy_config|0x10;//默认网络通信不上
		}
	else
		{
			qwl_sendstring("433不存在");
			shebei_flag=shebei_flag&0xfb;//433不存在，故障
			amy_config=amy_config&0xfe;//网关存在否
			amy_config=amy_config&0xfd;//终端还是网关
		}	

		
		
//		amy_state();//如果还按摩椅终端，进行按摩椅状态查询,实时会读，这里不进行初始化了
		
		rx_pointer=0;//串口1接受区清空
		rx2_pointer=0;//串口2接受区清空
		
		

}
/*******************************************************************************
* 函 数 名         : armchair_Init
* 函数功能		   : armchair初始化函数,启动定时
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Armchair_Init()	 
{
	q2x1278_init();
	Tm618_Smg_On();
	Init_Amy_Id();
	Self_Check();//系统自检	
	/*设置按摩椅超时时间*/
	API_SetTimer(1,-1,TIMER_TYPE_ARMCHAIR_TIME,API_ARMCHAIR_TIME,NULL);

}
/*******************************************************************************
* 函 数 名         : ac_switch_IN
* 函数功能		   : AC_SWITCH输入配置	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void ac_switch_IN()
{
	GPIO_InitTypeDef GPIO_InitStructure;
   			//  配置GPIO的模式和IO口 
		GPIO_InitStructure.GPIO_Pin=ac_status_out;	 //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPD;	  //设置上拉输入
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
	GPIO_Init(GPIOE,&GPIO_InitStructure); // 初始化GPIO 

}

/*******************************************************************************
* 函 数 名         : ac_switch_OUT
* 函数功能		   : AC_SWITCH输出配置	   
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void ac_switch_OUT()
{GPIO_InitTypeDef GPIO_InitStructure;
   			//  配置GPIO的模式和IO口 
		GPIO_InitStructure.GPIO_Pin=ac_status_out;	 //选择你要设置的IO口
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	  //推挽输出
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;	  //设置传输速率
	GPIO_Init(GPIOE,&GPIO_InitStructure); // 初始化GPIO 
	armchair_status_OUT=1;
}
/*******************************************************************************
* 函 数 名         : armchair_change_outIO()
* 函数功能		   : 改变与按摩椅输出端不同的输出IO口状态，让按摩椅端口跟随
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
u8 armchair_change_outIO(void)
{
	u8 i=0;
	
	armchair_switch=0;//按摩椅初始化，高停止，低启动
	Delay_MS(100);
	if(!armchair_status_IN)//==IO_STATE)
		{	
			i=0;
		}
		else
		{
			i=2;
		}
		armchair_switch=1;//按摩椅初始化，高停止，低启动
		
//	ac_switch_IN();
	
//	if(GPIO_ReadInputDataBit(GPIOE,ac_switch))
//	{		ac_switch_OUT();

	
//		GPIO_ResetBits(GPIOE,ac_switch);
//		GPIO_ResetBits(GPIOE,ac_status_in);//用于测试IO口正不正常留出的口
//		IO_STATE=0;
//	}
//			else
//			{
//				ac_switch_OUT();
	//			GPIO_SetBits(GPIOE,ac_switch);
		//		GPIO_SetBits(GPIOE,ac_status_in);//用于测试IO口正不正常留出的口
			//	IO_STATE=1;

//			}
	
	
	return i;
}
/*******************************************************************************
* 函 数 名         : armchair_io_state
* 函数功能		   : 读取按摩椅状态，正常或者不正常
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
u8 armchair_io_state_1()
{ u8 i;
//	ac_switch_IN();

		if(armchair_status_IN)//==IO_STATE)
		{	
			i=0;//查询按摩椅状态先
		}
		else
		{	i=1;
		}
	//	ac_switch_OUT();
	//	if(IO_STATE)
	//		GPIO_SetBits(GPIOE,ac_switch);
	//	else
	//		GPIO_ResetBits(GPIOE,ac_switch);
			

return i;
}
/*******************************************************************************
* 函 数 名         : armchair_io_state
* 函数功能		   : 读取按摩椅状态，正常或者不正常
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
u8 armchair_io_state()
{ u8 i;
//	ac_switch_IN();
		if(armchair_status_IN)//==IO_STATE)
		{	
			i=armchair_change_outIO();//查询按摩椅状态先
		}
		else
		{	i=1;
		}
	//	ac_switch_OUT();
	//	if(IO_STATE)
	//		GPIO_SetBits(GPIOE,ac_switch);
	//	else
	//		GPIO_ResetBits(GPIOE,ac_switch);
			

return i;
}

/*******************************************************************************
* 函 数 名         :PLCtime_share_process()
* 函数功能		   	: 终端分时处理标志
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void PLCtime_share_process()
{
	if(zd_overtime_flag>1000)
	{zd_overtime_flag=0;
		amy_config=amy_config|0x10;//
	}
	if(armchair_io_state_1()==0)
	{
		LED=!LED;//状态灯常亮，按摩椅在工作
	}
	
		switch(time_share_process_flag)
	{
		
		case 0: 
		{u8 i;
			if(PLC_time_tiqian>0)//标志发送
					{
						PLC_time_tiqian=0;
						PLC_time_flag=0;
							i=armchair_io_state();
							if(i==0)//按摩椅状态读取成功
							{
								
							qwl_sendstring("控制成功\r\n");
								amy_config=amy_config&0xdb;//	
							}
							else if(i==1)
							{	
							qwl_sendstring("控制失败\r\n");
							amy_config=amy_config|0x04;
							}	
							else if(i==2)
							{	
							qwl_sendstring("按摩椅坏了\r\n");
							amy_config=amy_config|0x20;
							}	
								q2x1278_plc_send();		
								time_share_process_flag=3;					
							rx2_pointer=0;//清除接受缓存区
							armchair_time_flag=0;

				}
		}break;
		
		case 1: 
		{u8 i;
			if(PLC_time_tiqian>0)//标志发送
					{
						PLC_time_tiqian=0;
						PLC_time_flag=0;
							i=armchair_io_state_1();
							if(i==0)//按摩椅状态读取成功
							{
								
							qwl_sendstring("控制成功\r\n");
								amy_config=amy_config&0xdb;//	
							}
							else if(i==1)
							{	
							qwl_sendstring("控制失败\r\n");
							amy_config=amy_config|0x04;
							}	
						
								q2x1278_plc_send();		
								time_share_process_flag=3;					
							rx2_pointer=0;//清除接受缓存区
							armchair_time_flag=0;

				}
		}break;
		
		case 3: 
		{ u8 i;
			i=q2x1278_check_send();
			if(i==0)
							{
								time_share_process_flag=0;
								armchair_time_flag=0;
								qwl_sendstring("终端发送OK");
								amy_config=amy_config&0xef;//
								amy_config1=amy_config1&0xfc;//
								zd_overtime_flag=0;//超时未有通信标志
								
							}
			else if(i==1)
			{
				time_share_process_flag=0;
				armchair_time_flag=0;
				qwl_sendstring("频段相同");
				amy_config1=amy_config1|0x02;//
				zd_overtime_flag=0;//超时未有通信标志
			}
			else if(i==2)
			{
				time_share_process_flag=0;
				armchair_time_flag=0;
				qwl_sendstring("ID相同");
				amy_config1=amy_config1|0x01;//
				zd_overtime_flag=0;//超时未有通信标志
			}
							else if(armchair_time_flag>3)//超时
							{	time_share_process_flag=0;
								armchair_time_flag=0;
//								PLC_time_tiqian=1;//加速发送心跳
								qwl_sendstring("终端发送超时ERROR");
								amy_config=amy_config|0x10;//
								
								//进行绝对被动，超时也不再发
						/*		//超时，再发一次
								qwl_sendstring("超时再发一次");
								i=armchair_io_state();
							if(i==0)//按摩椅状态读取成功
							{
								
								amy_config=amy_config&0xfb;//	
							}
							else
							{	
							amy_config=amy_config|0x04;
							}	
								q2x1278_plc_send();	*/	
							}
								


		}break;

		default: break;
	}
}
/*******************************************************************************
* 函 数 名         : zd_rec_deal
* 函数功能		   : 终端接受处理
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void zd_rec_deal()
{
				u8 i;
				u8 j;

//				u8	ZD_MODE=0;//终端模式
	switch(amy_record_buf_1[1])
			{
						case 0xb0://终端状态读取命令
						{
							i=((amy_record_buf_1[2]-0x30)<<4)+(amy_record_buf_1[3]-0x30);//ASCII转成BCD,模地址
							j=((amy_record_buf_1[4]-0x30)<<4)+(amy_record_buf_1[5]-0x30);
							amy_cotrol_time=(u16)(i<<8)+j;
							if(amy_cotrol_time>0)//当发来有时间才启动，
							{
							amy_cotrol_flag=1;
							amy_cotrol_time_flag=0;//按摩椅计时清零
							armchair_switch=0;//按摩椅初始化，高停止，低启动
								Shared_Module_switch(0x00);//0启动 1关闭
							qwl_sendstring("按摩椅开始工作88");
							GPIO_ResetBits(GPIOD,LED);
							}
//							ZD_MODE=((amy_record_buf_1[21]-0x30)<<4)+(amy_record_buf_1[22]-0x30);//ASCII转成BCD,频段
//					USART2_sendchar(0x00);//网关模块地址
//				USART2_sendchar(0xff);//
//				USART2_sendchar(q2x1278_config_buf[4]);//频段
//							USART2_sendstring("SEND_OK");
							PLC_time_tiqian=1;
							time_share_process_flag=1;							
							PLCtime_share_process();

						}break;
						
				case 0xb1://终端状态读取命令
						{

							PLC_time_tiqian=1;
							time_share_process_flag=0;							
							PLCtime_share_process();

						}break;
				case 0xf8:
				{
					zd_update_flag=1;
				}
				default:
					break;
			}
}
/*******************************************************************************
* 函 数 名         : wg_rec_deal
* 函数功能		   : 网关接受处理
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void wg_rec_deal()
{				u8 i;
				u16 k;
				u8 n;
				u8 PLC_ADR[3]={0};
				u8 rec_id_buf[16];//按摩椅ID零时存储
//				u8 *read_flash;
				switch(amy_record_buf_1[1])
				{

					case 0xa0:
						{
							for(i=0;i<16;i++)
							rec_id_buf[i]=amy_record_buf_1[i+8];//存储按摩椅数据，放入相应位置
							
							PLC_ADR[0]=((amy_record_buf_1[2]-0x30)<<4)+(amy_record_buf_1[3]-0x30);//ASCII转成BCD,模地址
							PLC_ADR[1]=((amy_record_buf_1[4]-0x30)<<4)+(amy_record_buf_1[5]-0x30);
							PLC_ADR[2]=((amy_record_buf_1[6]-0x30)<<4)+(amy_record_buf_1[7]-0x30);//ASCII转成BCD,频段
							
							
							k=PLC_ADR[1];
							k=k*21;

							if(amy_record_buf[k]>0)//存储前查表是否已经存在
							{
								
			if(config_recwait_buf[PLC_ADR[1]]>0)
			{
				config_recwait_buf[PLC_ADR[1]]=config_recwait_buf[PLC_ADR[1]]|0x80;//相应的状态标志位变1，表示状态反馈成功，要发送状态
			}
					
								for(i=0;i<16;i++)
								{	
									if(rec_id_buf[i]!=amy_record_buf[k+i+4])
									break;

								}
										
								if(i==16)
								{
								amy_record_buf[k]=1;//已经存在，标志位置1，初始，标识已经读到
									
								//qwl_sendstring("此终端已经有");
								amy_record_buf[k+20]=((amy_record_buf_1[24]-0x30)<<4)+(amy_record_buf_1[25]-0x30);
									
									if(wg_ack_times>=20)
									{qwl_sendstring("数量太多，处理不过来");
									}
									
									wg_ack_times++;//处理条数增加
							if(wg_stor_pointer>=200)
								wg_stor_pointer=0;
							
							//暂存，等待处理
							wg_ack_buf[wg_stor_pointer]=PLC_ADR[0];//模块地址//提醒终端发送成功
							wg_stor_pointer++;		
							wg_ack_buf[wg_stor_pointer]=PLC_ADR[1];
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]=PLC_ADR[2];//频段
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='S';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='E';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='N';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='D';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='_';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='O';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='K';
							wg_stor_pointer++;
							
								}
								else
								{
									if(wg_ack_times>=20)
									{qwl_sendstring("数量太多，处理不过来");
									}
									amy_record_buf[k+20]=((amy_record_buf_1[24]-0x30)<<4)+(amy_record_buf_1[25]-0x30);//实时状态还是要更新，新方式对ID不看重
									wg_ack_times++;//处理条数增加
							if(wg_stor_pointer>=200)
								wg_stor_pointer=0;
									//暂存，等待处理
							wg_ack_buf[wg_stor_pointer]=PLC_ADR[0];//模块地址//提醒终端发送成功
							wg_stor_pointer++;		
							wg_ack_buf[wg_stor_pointer]=PLC_ADR[1];
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]=PLC_ADR[2];//频段
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='E';//与附近一台机器频段相同
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='R';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='R';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='O';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='R';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='0';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='1';
							wg_stor_pointer++;
								}
							}
							else
							{	
								n=strsearch_qwl(rec_id_buf,amy_record_buf,5250,16);//读取，看是否包含此台
								if(n==0)
								{

									if(PLC_ADR[2]==q2x1278_config_buf[4])//判断在同频段内的进行存储
									{
										if(config_recwait_buf[PLC_ADR[1]]>0)//判断是不是自己请求的终端，不是自己请求的剔除
										{
							for(i=1;i<4;i++)
							amy_record_buf[k+i]=PLC_ADR[i-1];//存储按摩椅数据，放入相应位置
							for(i=4;i<20;i++)
							amy_record_buf[k+i]=amy_record_buf_1[i+4];//存储按摩椅数据，放入相应位置
							amy_record_buf[k+20]=((amy_record_buf_1[24]-0x30)<<4)+(amy_record_buf_1[25]-0x30);
								amy_record_buf[k]=1;//标志位置1，初始
									config_recwait_buf[PLC_ADR[1]]=config_recwait_buf[PLC_ADR[1]]|0x80;//相应的状态标志位变1，表示状态反馈成功，要发送状态
							amy_total++;//上线总数增加
							getwaylist_send_flag++;//列表要求重新上传标志
							amy_rec_flag=0;//重新开始读取标志位
								qwl_sendstring("新终端存取成功666");

								for(i=0;i<21;i++)
							{
								qwl_sendchar(NO[amy_record_buf[k+i]>>4]);
								qwl_sendchar(NO[amy_record_buf[k+i]&0x0f]);
							}
									
//									read_flash=(u8*)(flash_AMY_adr1);
									
//												if(amy_storflash_flag==2)
//												{
//														for(i=4;i<20;i++)
	//													{
//															if(read_flash[k+i]!=amy_record_buf[k+i])
//																break;
//																
	//													}
//														if(i==20)//判断表内是否已经有，有加入，没有剔除
//														{qwl_sendstring("有新，更新flash");
//														Write_Flash_ALLAMY();//更新FLASH表
//														}
//												}
								
										if(wg_ack_times>=20)
									{qwl_sendstring("数量太多，处理不过来");
									}
									
									wg_ack_times++;//处理条数增加
							if(wg_stor_pointer>=200)
								wg_stor_pointer=0;
							//暂存，等待处理
							wg_ack_buf[wg_stor_pointer]=PLC_ADR[0];//模块地址//提醒终端发送成功
							wg_stor_pointer++;		
							wg_ack_buf[wg_stor_pointer]=PLC_ADR[1];
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]=PLC_ADR[2];//频段
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='S';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='E';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='N';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='D';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='_';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='O';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='K';
							wg_stor_pointer++;
						}
										else
										{
											qwl_sendstring("不是自己请求的终端");
											qwl_sendchar(NO[PLC_ADR[0]>>4]);
											qwl_sendchar(NO[PLC_ADR[0]&0x0f]);
											qwl_sendchar(NO[PLC_ADR[1]>>4]);
											qwl_sendchar(NO[PLC_ADR[1]&0x0f]);
											qwl_sendchar(NO[PLC_ADR[2]>>4]);
											qwl_sendchar(NO[PLC_ADR[2]&0x0f]);
										}
					}
									else
										{
											qwl_sendstring("不在同频段内的终端");
											qwl_sendchar(NO[PLC_ADR[0]>>4]);
											qwl_sendchar(NO[PLC_ADR[0]&0x0f]);
											qwl_sendchar(NO[PLC_ADR[1]>>4]);
											qwl_sendchar(NO[PLC_ADR[1]&0x0f]);
											qwl_sendchar(NO[PLC_ADR[2]>>4]);
											qwl_sendchar(NO[PLC_ADR[2]&0x0f]);
										}
								}
								else
								{	
									
									if(wg_ack_times>=20)
									{qwl_sendstring("数量太多，处理不过来");
									}
									
									wg_ack_times++;//处理条数增加
							if(wg_stor_pointer>=200)
								wg_stor_pointer=0;
																		//暂存，等待处理
							wg_ack_buf[wg_stor_pointer]=PLC_ADR[0];//模块地址//提醒终端发送成功
							wg_stor_pointer++;		
							wg_ack_buf[wg_stor_pointer]=PLC_ADR[1];
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]=PLC_ADR[2];//频段
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='E';//终端重复
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='R';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='R';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='O';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='R';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='0';
							wg_stor_pointer++;
							wg_ack_buf[wg_stor_pointer]='2';
							wg_stor_pointer++;
								}
							}
						}break;
					case 0xc1:
					{
						qwl_sendstring("隔壁网关请求");

					}break;
					default:break;
					}
					

				

}
/*******************************************************************************
* 函 数 名         : ARMCHAIR_S_IQR()	  //秒中断定时器中放入
* 函数功能		   : 按摩椅数据存储
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void ARMCHAIR_S_IQR()	  //秒中断定时器中放入
{
	
/*	time_flag++;
	if(time_flag>60)
	{
		amy_print_string("按摩椅卡死");
		amy_print_char((ceshi>>4)+0x30);
		amy_print_char((ceshi&0x0f)+0x30);
	}*/
	PLC_time_flag++;
	armchair_time_flag++;
	GATEWAY_heart_time_flag++;//心跳超时反馈标志
	GATEWAY_list_time_flag++;//列表超时反馈标志
	key_delay_out++;//按键超时未有动作自动退出界面
	zd_overtime_flag++;//终端超时标识，超时没有收到反馈，进行标志置位
	suipian_delay++;//为了保持快速反应的延迟时间发送00数据
	GATEWAY_off_time_flag++;
	CSQ_DELAY++;//CSQ读取，每10S读
	//nblot_read_flag1_delay++;
	
	if(amy_cotrol_flag==1)
	{
		
	amy_cotrol_time_flag++;
	if(amy_cotrol_time_flag>=amy_cotrol_time)
			{amy_cotrol_flag=0;
			LED=1;
			qwl_sendstring("按摩椅工作停止");
				armchair_switch=1;//按摩椅初始化，高停止，低启动
				getwayoff_send_flag=getwayoff_send_flag|0x01;//按摩椅停止工作上报
				if(Shared_Module.Module == oxygen_plant)
				Shared_Module_switch(0x01);//0启动 1关闭
				}
			else
			{
				if(getwayoff_send_flag==0)
				{
					u8 i;
					i=armchair_io_state_1();
					if(i==0)
					{
						getwayoff_send_flag=getwayoff_send_flag|0x04;//运行中按摩椅状态不对
					}
				}
							armchair_switch=0;;//按摩椅初始化，高停止，低启动
				if(Shared_Module.Module == oxygen_plant)
				Shared_Module_switch(0x00);//0启动 1关闭
						LED=0;
			}
			
	}
	
}
/*******************************************************************************
* 函 数 名         : Amy_Record_Store
* 函数功能		   : 按摩椅数据存储
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void Amy_Record_Store(u8 ch)
{

	if(ch==0xff)
	{
			amy_rec_flag=1;
	}
		
	if((amy_config&0x01)==0x01)//433存在，并且是网关
	{
		switch (amy_rec_flag)
		{

			case 1:
			{				
					amy_rec_flag=2;//读到标志位，开始存储
				 amy_rec_flag_1=0;//开始计数读取	
			}break;
				case 2:
			{u8 i;

				if(ch==0xfe)//读到结尾符号
				{
					if(amy_record_buf_1[0]==amy_rec_flag_1)//查询与传输过来的校验和是否一致，一致就存储，不然放弃
					{	qwl_sendstring("S2包接受正常");
	//							qwl_sendchar(NO[config_recwait_time>>12]);//原来用于测试433回馈时间
//						qwl_sendchar(NO[(config_recwait_time>>8)&0x000f]);
///						qwl_sendchar(NO[(config_recwait_time>>4)&0x000f]);
//						qwl_sendchar(NO[(config_recwait_time)&0x000f]);
						if((amy_config&0x02)==0x02)//是网关
						{
							
							wg_rec_deal();
						}
						else
						{
							zd_rec_deal();
						}
						amy_rec_flag=0;
					}	
					else
					{
						amy_rec_flag=0;//
						qwl_sendstring("S2包长度有问题，丢包,丢包数据：");
						for(i=0;i<amy_rec_flag_1;i++)
						qwl_sendchar(amy_record_buf_1[i]);
					}
				}
				else
				{amy_record_buf_1[amy_rec_flag_1]=ch;//存入暂时缓存区，等待检验
					amy_rec_flag_1++;
				}
				
			}break;
			default: break;
		}
	}
	

}




