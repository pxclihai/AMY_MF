#include "config.h"

void DownLoad_DealPro(USART_FUNCTION uf,uchar *DaPtr,uint16 len);

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DownLoad_SendByte
// 功能描述  : 发送一字节数据
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void DownLoad_SendByte(USART_FUNCTION uf,uchar ChDa)
{
	API_AddProtocolBuffer(uf,(uchar *)&ChDa, 1);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DownLoad_SendHead
// 功能描述  : 发送数据帧头部分
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void DownLoad_SendHead(USART_FUNCTION uf,uchar Addr,uchar cmd)
{	
    API_ResetProtocolBuffer(uf,RESETRTBUFF_TYPE_TX);
	DownLoad_SendByte(uf,0xAA);		//协议头
	DownLoad_SendByte(uf,Addr); 
	DownLoad_SendByte(uf,cmd);   	
	DownLoad_SendByte(uf,0x00);   	//虚拟长度高字节
	DownLoad_SendByte(uf,0x00);   	//虚拟长度低字节
}


//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DownLoad_SendChksum
// 功能描述  : 回发的校验和函数
// 输入参数  : len 校验数据长度.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void DownLoad_SendChksum(USART_FUNCTION uf)  
{
	uchar *TxBuf;
	uint16 *Len;
	uchar *p;
	uint i,ChksumData=0;

	if(API_GetProtocolBufferOrLen(uf,&TxBuf,&Len)>-1)
	{
		/*填充数据体长度*/
		TxBuf[3] = (uchar)((*Len-5)>>8);
		TxBuf[4] = (uchar)(*Len-5);	

		/*计算校验和*/
		p = TxBuf+1;
		for(i=0;i<(*Len)-1;i++)
		{
			ChksumData+=*p++;
		}
	}
	DownLoad_SendByte(uf,(uchar)(ChksumData>>8));
	DownLoad_SendByte(uf,(uchar)ChksumData);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : LCDSendTail
// 功能描述  : 发送帧尾
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void DownLoad_SendTail(USART_FUNCTION uf)
{	
	DownLoad_SendByte(uf,0xCC);
	DownLoad_SendByte(uf,0x33);
	DownLoad_SendByte(uf,0xC3);
	DownLoad_SendByte(uf,0x3C);
	API_StartSendBuffer(uf);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DownLoad_DealProtocol
// 功能描述  : 通信协议解析
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
#define INFO_START 5
#define INFO_END   6
void DownLoad_DealProtocol(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
{
	uint16  chksum_len = 0;

	if((DaPtr[1] == ModAddress)||(DaPtr[1] == 0xff))
	{
		 chksum_len = (uint16)(DaPtr[3]<<8)+(uint16)DaPtr[4];
     if(chksum_len == (len - INFO_START - INFO_END))
		 {
				if(chk_LcdMode_chksum((uchar *)&DaPtr[1],len-7) == 1)
				{
					DownLoad_DealPro(uf,DaPtr,len);
				}
		}
	}
	else
	{
		API_ResetProtocolBuffer(uf,RESETRTBUFF_TYPE_ALL);
	}
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DownLoad_DealPro
// 功能描述  : 协议处理
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※ 
void DownLoad_DealPro(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
{
	switch(uf)
	{
		case USART_FUNCTION_WCDMA:
			DownLoad_Pro(uf,DaPtr,len);
			break;
		case USART_FUNCTION_MOUNT + 0:
			break;
		case USART_FUNCTION_MOUNT + 1:
			break;
		case USART_FUNCTION_MSG:
			break;
		case USART_FUNCTION_DEBUG:
			DownLoad_Pro(uf,DaPtr,len);
			break;
	}
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//                          程序结束(END)
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

