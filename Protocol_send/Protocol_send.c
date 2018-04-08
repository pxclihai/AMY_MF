#include "config.h"


PROTOCOL_SEND_TYPE Net_Data_Send={5,0,5,SEND_READY,WAIT_REC};



/*******************************************************************************
* 函 数 名         : void process_F8()
* 函数功能		   : 接受12命令反馈
* 输    入         : 
* 输    出         : 无
*******************************************************************************/
void process_F8(u8 RTN)
{  


	u8 i;

	u8 post_buf1[200];
	u16 post_buf1_len=0;

				for(i=0;i<16;i++)
				{post_buf1[post_buf1_len]=AmyID[i];
					post_buf1_len++;
				}
				for(i=0;i<2;i++)
				{post_buf1[post_buf1_len]=software_version[i];
					post_buf1_len++;
				}
				for(i=0;i<22;i++)
				{post_buf1[post_buf1_len]=Upgrade_ip[i];
					post_buf1_len++;
				}

	
			post_group(post_buf1,post_buf1_len,RTN);
   net_send(post_buf,post_len);
} 
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : upload_link_send()
// 功能描述  : 升级握手
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 无
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void upload_link_send_POST()
{
			
	process_F8(0xf8);

}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : upload_link_send()
// 功能描述  : 升级握手
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 无
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void upload_link_send()
{
								switch(Net_Data_Send.upload_handshake.send_state )
											{
												case SEND_READY:
												{
													if(Net_Data_Send.upload_handshake.TIMES>0)
													{
														if(Net_Data_Send.upload_handshake.TIMES!=0xff)
															Net_Data_Send.upload_handshake.TIMES--;
														
														Net_Data_Send.upload_handshake.delays=0;
														Net_Data_Send.upload_handshake.send_state=WAIT_ACK;
													upload_link_send_POST();
													}
													
												}break;
												case WAIT_ACK:
												{
													if(Net_Data_Send.upload_handshake.delays>Net_Data_Send.upload_handshake.MAX_delays)
													{
														Net_Data_Send.upload_handshake.delays=0;
														Net_Data_Send.upload_handshake.send_state=SEND_READY;
														qwl_sendstring("等待握手超时");
														
													}
													if(Net_Data_Send.upload_handshake.REC_state == REC_OK)
													{
														Net_Data_Send.upload_handshake.TIMES=5;
														Net_Data_Send.upload_handshake.send_state=STOP_SEND;
														Net_Data_Send.upload_handshake.delays=0;

													}
												}break;
												case STOP_SEND:
												{
														//升级握手数据初始化
													Net_Data_Send.upload_handshake.delays =0;
													Net_Data_Send.upload_handshake.MAX_delays  =5;
													Net_Data_Send.upload_handshake.TIMES  =5;
													//Net_Data_Send.upload_handshake.send_state  =SEND_READY;
													Net_Data_Send.upload_handshake.REC_state = WAIT_REC;
												}break;
		
												default:break;
											}	 
}											
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : net_data_send_init()
// 功能描述  : 网络数据发送初始化参数
// 输入参数  : None
// 输出参数  : None
// 返回参数  :无
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void net_data_send_init()
{
	//升级握手数据初始化
	Net_Data_Send.upload_handshake.delays =0;
	Net_Data_Send.upload_handshake.MAX_delays  =5;
	Net_Data_Send.upload_handshake.TIMES  =5;
	Net_Data_Send.upload_handshake.send_state  =SEND_READY;
	Net_Data_Send.upload_handshake.REC_state = WAIT_REC;	
	
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : net_data_send_delay
// 功能描述  : 网络数据发送延时
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 无
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void net_data_send_delay()
{
	Net_Data_Send.upload_handshake.delays++;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : net_data_send()
// 功能描述  : 网络数据发送
// 输入参数  : None
// 输出参数  : None
// 返回参数  : 无
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void net_data_send()
{
	if(net.reconnect_setp==LINK_OK)
	{

		upload_link_send();
										
	}
}



