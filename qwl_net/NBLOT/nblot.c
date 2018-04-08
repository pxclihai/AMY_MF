#include "config.h"


u8 nblot_CFUN[]="AT+CFUN=1";
u8 nblot_NSOCL[]="AT+NSOCL=0"; 
u8 nblot_CEREG[]="AT+CEREG=1";
u8 nblot_CGDCONT[]="AT+CGDCONT=1,\"IP\",\"CTNB\"";
u8 nblot_COPS[]="AT+COPS=1, 2,\"46011\"";
u8 nblot_CGATT[]="AT+CGATT? ";
u8 nblot_NSOST[]="AT+NSOST=0,";
u8 nblot_NSOCR[]="AT+NSOCR= DGRAM,17,56852,1";
u8 nblot_NSORF[]="AT+NSORF=0,";
u8 nblot_NCCID[]="AT+NCCID";

u8 nblot_NBAND[]="AT+NBAND?";
u8 nblot_NBAND1[]="AT+NBAND=5";
u8 nblot_NCONFIG[]="AT+NCONFIG?";

u8 nblot_AUTOCONNECT[]="AT+NCONFIG=AUTOCONNECT,TRUE";

u8 nblot_SCRAMBL_true[]="AT+NCONFIG=CR_0354_0338_SCRAMBLING,TRUE";
u8 nblot_SCRAMBL_false[]="AT+NCONFIG=CR_0354_0338_SCRAMBLING,FALSE";
u8 nblot_AVOID_true[]="AT+NCONFIG=CR_0859_SI_AVOID,TRUE";
u8 nblot_AVOID_false[]="AT+NCONFIG=CR_0859_SI_AVOID,FALSE";
u8 nblot_rec_flag=0;
u8 nblot_read_flag=0;//nb读到需要接受的数据，需要主动读


u8 nblot_read_flag1=0;
//u8 nblot_read_flag1_delay=0;

//u8 nblot_rec_buf[500];
//u16 nblot_rec_times=0;

u8 nblot_RESET_flag=0;


u8 nblot_overtime_flag=0;




u8 light_UDP[22]={"39.108.148.248,26002"};;

u8 NBIOT_SET_FALG=0;

u8 NB_SET_FLAG=0;
/*********************************************************************
 ** 函数名称: strsearch_hq ()
 ** 函数功能: 在指定的数组里连续找到相同的内容
 ** 入口参数: ptr2要查找的内容, ptr1当前数组
 ** 出口参数: 0-没有 找到   >1 查找到
 *********************************************************************/
/*u16 strsearch_hq(u8 *ptr2,u8 *ptr1_at)//查字符串*ptr2在*ptr1中的位置
//本函数是用来检查字符串*ptr2是否完全包含在*ptr1中
//返回:  0  没有找到
//65535 从第N个字符开始相同
{
u16 i,j,k;
        
      
	if(nblot_rec_times==0) return(0);
	
	for(i=0,j=0;i<nblot_rec_times;i++)
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
	return(0);
}
*/
//*******************************************************
//TP链接
//******************************************************
void nblot_TP_GPRS(u8 *p,u8 *q)
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
}

//发送数据
//*********************************************************
/*******************************************************************************
* 函 数 名         :qwl_net_sendstring1(u8 *p,u16 len)
* 函数功能		   : 串口3发送数据位
* 输    入         : 无
* 输    出         : 无
********************************************************************************/
	
u8 nblot_send(u8 *p,u16 len)
{	u16 j=0;


clear_SystemBuf();

if(exchange_platform.Platform_Choice == PLATFORM_CHOICE_PUBLIC)
nblot_TP_GPRS(nblot_NSOST,light_UDP);
else
nblot_TP_GPRS(nblot_NSOST,update_ip);//连接升级IP

	qwl_net_sendchar(0x2c);
	qwl_sendchar(0x2c);

///	sprintf(i, "%d", (u8)len);
//	for(k=0;k<strlen(i);k++)
	j=len;
	if(len>99)
	{	j=len/100;
		qwl_net_sendchar(NO[j]);
		qwl_sendchar(NO[j]);
		j=len%100;
	}
	
	qwl_net_sendchar(NO[j/10]);
	qwl_sendchar(NO[j/10]);
	qwl_net_sendchar(NO[j%10]);
	qwl_sendchar(NO[j%10]);
	

	
	qwl_net_sendchar(0x2c);
	qwl_sendchar(0x2c);

for(j=0;j<len;j++)
  	{
  		qwl_sendchar(NO[p[j]>>4]);//测试用
			qwl_sendchar(NO[p[j]&0x0f]);//测试用
			//qwl_sendchar(p[j]);
			qwl_net_sendchar(NO[p[j]>>4]);
			qwl_net_sendchar(NO[p[j]&0x0f]);
	}
	
	
	qwl_sendchar(0x0D);
  	qwl_sendchar(0x0A);
	qwl_net_sendchar(0x0D);
  	qwl_net_sendchar(0x0A);
return 1;
} 



/*******************************************************************************
* 函 数 名         : cdma_init()
* 函数功能		   : cdma初始化
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void nblot_init()
{
	
		switch (net.setp_setp)
	{

			case 0:
			{qwl_sendstring("查询频段");
				clear_SystemBuf();
				qwl_net_sendstring(nblot_NBAND);//复位
				net.delay=0;
				net.setp_setp=1;
			}break;
			case 1:
			{
				u8 addr=0;
							if(net.delay>2)//等待超时，服务器问题
						{net.delay=0;
							qwl_sendstring("查询频段超时");
									net.setp_setp=0;
								net.delay=0;
							net.delay_times++;
							if(net.delay_times>5)
							{
								net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=MODE_CHOICE;//进入网络重新连接
								net.delay=0;
							}
						}	
							addr=strsearch_h("+NBAND:",SystemBuf);
							if(addr>0)
								{ if(SystemBuf[addr]=='5')
									{qwl_sendstring("频段正确");
										NB_SET_FLAG=0;
									}
									else if(SystemBuf[addr]=='8')
									{qwl_sendstring("频段正确");
										NB_SET_FLAG=0;
									}
									else
									{
										qwl_sendstring("频段错误");
										NB_SET_FLAG=1;
									}
									net.delay_times=0;
								net.setp_setp=2;
								net.reconnect_setp=MODE_INIT;//进入网络重新连接
								net.delay=0;
								}
							
						
			}break;
			case 2:
			{qwl_sendstring("查询配置");
				clear_SystemBuf();
				qwl_net_sendstring(nblot_NCONFIG);//复位
				net.delay=0;
				net.setp_setp=3;
			}break;
			case 3:
			{
				u8 addr=0;
							if(net.delay>2)//等待超时，服务器问题
						{net.delay=0;
							qwl_sendstring("查询配置超时");
								net.setp_setp=2;
								net.delay=0;
							net.delay_times++;
							if(net.delay_times>5)
							{
								net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=MODE_CHOICE;//进入网络重新连接
								net.delay=0;
							}	
	
						}
						addr=strsearch_h("AVOID,FALSE",SystemBuf);
						if(addr==0)
						addr=strsearch_h("AVOID,TRUE",SystemBuf);
						if(addr)
						{
							NB_SET_FLAG=0;
							addr=strsearch_h("AUTOCONNECT,FALSE",SystemBuf);
							if(addr>0)
								{ NB_SET_FLAG=NB_SET_FLAG|0x02;
									qwl_sendstring("自动连接错误");
								}
								
							addr=strsearch_h("SCRAMBLING,FALSE",SystemBuf);
							if(addr>0)
								{ NB_SET_FLAG=NB_SET_FLAG|0x04;
									qwl_sendstring("SCRAMB连接错误");
								}
								
							addr=strsearch_h("AVOID,FALSE",SystemBuf);
							if(addr>0)
								{ NB_SET_FLAG=NB_SET_FLAG|0x08;
									qwl_sendstring("AVOID连接错误");
								}
								qwl_sendstring("切饶玛次数");
								qwl_sendchar(NO[NBIOT_SET_FALG>>4]);
								qwl_sendchar(NO[NBIOT_SET_FALG&0x0f]);
								if(NB_SET_FLAG&0x01)
								{
								net.delay_times=0;
								net.setp_setp=4;
								net.reconnect_setp=MODE_INIT;//进入网络重新连接
								net.delay=0;
								}
								else if(NB_SET_FLAG&0x02)
								{
								net.delay_times=0;
								net.setp_setp=6;
								net.reconnect_setp=MODE_INIT;//进入网络重新连接
								net.delay=0;
								}
								else if(1)
								{

										if(NBIOT_SET_FALG>3)//3次连接不上，就进行模式切换
										{

									net.delay_times=0;
									net.setp_setp=8;//0;//net.setp_setp=8;//饶玛自动识别关闭
									net.reconnect_setp=MODE_INIT;//2;//net_reconnection_flag=3;//进入网络重新连接
									net.delay=0;											
										}
										else
										{
										NBIOT_SET_FALG++;
									net.delay_times=0;
									net.setp_setp=0;
									net.reconnect_setp=OPEN_GPRS;//进入网络重新连接
									net.delay=0;		
										}
								}
							}

			}break;		
			
			
			case 4:
			{qwl_sendstring("设置第五频段");
				clear_SystemBuf();
				qwl_net_sendstring(nblot_NBAND1);//设置频段为5
				net.delay=0;
				net.setp_setp=5;
			}break;
			case 5:
			{
				u8 addr=0;
							if(net.delay>2)//等待超时，服务器问题
						{net.delay=0;
							qwl_sendstring("设置频段超时");
								net.setp_setp=4;
								net.delay=0;
							net.delay_times++;
							if(net.delay_times>5)
							{
								net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=MODE_CHOICE;//进入网络重新连接
								net.delay=0;
							}
						}
							
							addr=strsearch_h("OK",SystemBuf);
							if(addr>0)
								{ 
									qwl_sendstring("设置频段成功");
									if(NB_SET_FLAG&0x02)
									{
								net.delay_times=0;
								net.setp_setp=6;
								net.reconnect_setp=MODE_INIT;//进入网络重新连接
								net.delay=0;		
									}
									else if(NBIOT_SET_FALG>3)
									{
								net.delay_times=0;
								net.setp_setp=8;
								net.reconnect_setp=MODE_INIT;//进入网络重新连接
								net.delay=0;	
									}
									else if(1)
									{
								net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=MODE_CHOICE;//进入网络重新连接，设置完频段必须重启
								net.delay=0;	
									}
									
								}
		
			}break;

			case 6:
			{qwl_sendstring("设置自动联网模式");
				clear_SystemBuf();
				qwl_net_sendstring(nblot_AUTOCONNECT);//设置频段为5
				net.delay=0;
				net.setp_setp=7;
			}break;
			case 7:
			{
				u8 addr=0;
							if(net.delay>2)//等待超时，服务器问题
						{net.delay=0;
							qwl_sendstring("设置自动联网超时");
								net.setp_setp=6;
								net.delay=0;
							net.delay_times++;
							if(net.delay_times>5)
							{
								net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=MODE_CHOICE;//进入网络重新连接
								net.delay=0;
							}
						}
							
							addr=strsearch_h("OK",SystemBuf);
							if(addr>0)
								{ 
									qwl_sendstring("设置自动联网成功");
									if(NBIOT_SET_FALG>3)
									{
								net.delay_times=0;
								net.setp_setp=8;
								net.reconnect_setp=MODE_INIT;//进入网络重新连接
								net.delay=0;	
									}
									else if(1)
									{
								net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=MODE_CHOICE;//进入网络重新连接
								net.delay=0;	
									}
									
								}
		
			}break;	

			case 8:
			{
				if((NB_SET_FLAG&0x0c)>0)
				qwl_sendstring("设置SCRAMBL=true");
				else
				qwl_sendstring("设置SCRAMBL=false");	
				clear_SystemBuf();
				if((NB_SET_FLAG&0x0c)>0)
				qwl_net_sendstring(nblot_SCRAMBL_true);//设置为true
				else
				qwl_net_sendstring(nblot_SCRAMBL_false);//设置为false
				net.delay=0;
				net.setp_setp=9;
			}break;
			case 9:
			{
				u8 addr=0;
							if(net.delay>2)//等待超时，服务器问题
						{net.delay=0;
							qwl_sendstring("设置SCRAMBL超时");
								net.setp_setp=8;
								net.delay=0;
							net.delay_times++;
							if(net.delay_times>5)
							{
								net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=MODE_CHOICE;//进入网络重新连接
								net.delay=0;
							}
						}
							
							addr=strsearch_h("OK",SystemBuf);
							if(addr>0)
								{ 
									qwl_sendstring("设置SCRAMBL成功");
								net.delay_times=0;
								net.setp_setp=10;
								net.reconnect_setp=MODE_INIT;//进入网络重新连接
								net.delay=0;	
								}
		
			}break;	

					
				case 10:
			{
				if(NB_SET_FLAG&0x0c)
				qwl_sendstring("设置AVOID=true");
				else
				qwl_sendstring("设置AVOID=false");	
				clear_SystemBuf();
				if(NB_SET_FLAG&0x0c)
				qwl_net_sendstring(nblot_AVOID_true);//设置为true
				else
				qwl_net_sendstring(nblot_AVOID_false);//设置为false
				net.delay=0;
				net.setp_setp=11;
			}break;
			case 11:
			{
				u8 addr=0;
							if(net.delay>2)//等待超时，服务器问题
						{net.delay=0;
							qwl_sendstring("设置AVOID超时");
								net.setp_setp=10;
								net.delay=0;
							net.delay_times++;
							if(net.delay_times>5)
							{
								net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=MODE_CHOICE;//进入网络重新连接
								net.delay=0;
							}
						}
							
							addr=strsearch_h("OK",SystemBuf);
							if(addr>0)
								{ 
									NBIOT_SET_FALG=0;
									qwl_sendstring("设置AVOID成功");
								net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=MODE_CHOICE;//进入网络重新连接,设置完需要重启
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
void nblot_reconnect()
{
	
		switch (net.setp_setp)
	{
		
		case 0:
		{
			clear_SystemBuf();			
			qwl_net_sendstring(nblot_NSOCR);
			net.setp_setp=1;
			net.delay=0;
			qwl_sendstring("创建包");
			nblot_RESET_flag++;
			if(nblot_RESET_flag>3)
		{
			net.delay_times=0;
					net.setp_setp=0;
					net.reconnect_setp=MODE_CHOICE;//进入CDMA重启
					net.delay=0;
			nblot_RESET_flag=0;
					qwl_sendstring("进入NBLOT复位");
	}
		}break;
		case 1:
		{
			if(net.delay>=6)//等待超时，服务器问题
			{net.delay=0;
				net.setp_setp=0;//等待失败
				net.reconnect_setp=LINK_IP;
				
				
				qwl_sendstring("创建包超时");
				net.delay_times++;
				if(net.delay_times>7)
				{
					net.delay_times=0;
					net.setp_setp=0;
					net.reconnect_setp=OPEN_GPRS;//进入CDMA重启
					net.delay=0;
					qwl_sendstring("进入NBLOT重启");
				}
			}
			

				if(strsearch_h("OK",SystemBuf)>0)
				{net.reconnect_setp=LINK_OK;//打开完毕，进入联网
					net.setp_setp=0;
					net.delay_times=0;
					net.delay=0;
					deal_process=0;
					qwl_sendstring("创建包成功");
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
void nblot_open_GPRS()
{
	switch (net.setp_setp)
	{
		case 0:
		{
			clear_SystemBuf();			
			qwl_net_sendstring(nblot_NCCID);
			net.setp_setp=1;
			net.delay=0;
			qwl_sendstring("读取NB卡号");
		}break;
			case 1:
		{u8 i;
			u16 j;
						if(net.delay>3)//等待超时，服务器问题
			{net.delay=0;
				net.setp_setp=0;//等待失败
				net.reconnect_setp=OPEN_GPRS;
				
				
				qwl_sendstring("读卡超时");
				net.delay_times++;
				if(net.delay_times>3)
				{
					net.setp_setp=2;
					net.delay_times=0;
					net.delay=0;
				}
			}
			

				j=strsearch_h("+NCCID:",SystemBuf);
				if(j>0)
				{
					
					qwl_sendstring("查NB卡号成功");
									for(i=0;i<20;i++)
									{SIM_ID[i]=SystemBuf[j+i];
										qwl_sendchar(SIM_ID[i]);
									}
					net.setp_setp=2;
					net.delay_times=0;
					net.delay=0;
				}
				
			
		}break;

				case 2:
		{
			clear_SystemBuf();			
			qwl_net_sendstring(nblot_CGATT);
			net.setp_setp=3;
			net.delay=0;
			qwl_sendstring("附着查询");
		}break;
		case 3:
		{
			if(net.delay>6)//等待超时，服务器问题
			{net.delay=0;
				net.setp_setp=2;//等待失败
				net.reconnect_setp=OPEN_GPRS;
				
				
				qwl_sendstring("附着超时");
				net.delay_times++;
				if(net.delay_times>7)
				{
					net.delay_times=0;
					net.setp_setp=0;
					net.reconnect_setp=MODE_CHOICE;//进入CDMA重启
					net.delay=0;
					qwl_sendstring("进入NBLOT重启");
				}
			}
			

				if(strsearch_h("+CGATT:1",SystemBuf)>0)
				{
					net.reconnect_setp=LINK_IP;//打开包
					net.setp_setp=0;
					net.delay_times=0;
					net.delay=0;
					qwl_sendstring("NBLOT附着成功");
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
void nblot_init_0()
{
								net.delay_times=0;
								net.setp_setp=0;
								net.reconnect_setp=MODE_CHOICE;//进入网络重新连接
								net.delay=0;
}



/*******************************************************************************
* 函 数 名         : void nblot_rec(void)
* 函数功能		   : 网络重连
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
void nblot_rec(u8 k)
{
	if(k=='+')
	{
		nblot_rec_flag=1;
	}
	switch(nblot_rec_flag)
	{
		case 1:
		{
			if(k=='N')
				nblot_rec_flag++;
		}
		break;
		case 2:
		{
			if(k=='S')
				nblot_rec_flag++;
		}
		break;
		case 3:
		{
			if(k=='O')
				nblot_rec_flag++;
		}
		break;
		case 4:
		{
			if(k=='N')
				nblot_rec_flag++;
		}
		break;
		case 5:
		{
			if(k=='M')
				nblot_rec_flag++;
		}
		break;
		case 6:
		{
			if(k=='I')
			{
				nblot_rec_flag=0;
				nblot_read_flag=1;
			}
		}
		break;
	
		default:break;
	}

}
/*******************************************************************************
* 函 数 名         : nblot_read()
* 函数功能		   : 
* 输    入         : 
* 输    出         : 
*******************************************************************************/
void nblot_read()
{

	if(nblot_read_flag>0)
	{
		
//		nblot_read_flag1_delay=0;
		nblot_read_flag1=1;
		nblot_RESET_flag=0;//切换开关饶玛用
//		nblot_rec_times=0;
	NBIOT_SET_FALG=0;
qwl_sendstring("AT+NSORF=0,512");//读清队列
	qwl_net_sendstring("AT+NSORF=0,512");//读清队列
		
				nblot_read_flag=0;
	}
}
/*******************************************************************************
* 函 数 名         : u8 nblot_group()
* 函数功能		   : 
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/
/*u8 nblot_group(void)
{
	u16 i=0;
	u16 j=0;
	u16 len=0;
	u8 ch=0;
	u8 k;
	k=1;

		
		len=nblot_rec_times;
		Delay_MS(3);
		if(len==nblot_rec_times);
		{

	qwl_sendchar(NO[nblot_rec_times>>4]);
	qwl_sendchar(NO[nblot_rec_times&0x0f]);
			
			i=strsearch_hq("7E",nblot_rec_buf);
			if(i>1)
			{
				i=i-2;
				
							for(j=i;j<nblot_rec_times;j++)
			{
				qwl_sendchar(nblot_rec_buf[j]);
			}				
			for(j=i;j<nblot_rec_times;j++)
			{ch=(ASCII_hex[nblot_rec_buf[j]-0x30]<<4)+ASCII_hex[nblot_rec_buf[j+1]-0x30];
				j++;
				eth_rec_deal(ch);
//				qwl_sendchar(ch);


								
			}
									
	
			k=0;
			nblot_rec_times=0;
			
			
			}
				
		}
	
		//i=strsearch_h("+NSORF=0,",SystemBuf);
		//{
		//	len=
			//nblot_read_flag=0;
		//}
	
	return k;
}

*/





