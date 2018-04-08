#include "config.h"


//#define    	POST_VER		   		0x20 //协议版本
#define    	SYS_DEVICE_TYPE		   	0xD0 //设备类型

uchar  		RTN = 0; 

uchar ChkPostDataSum(uchar *DaPtr,uint16 len);
void PostSendDefault(USART_FUNCTION uf);
void DealPostPro(USART_FUNCTION uf,uchar *DaPtr,uint16 len);

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : PostSendByte
// 功能描述  : 发送一字节数据
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void PostSendByte(USART_FUNCTION uf,uchar ChDa)
{
	API_AddProtocolBuffer(uf,(uchar *)&ChDa, 1);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : PostSendBytes
// 功能描述  : 发送n个字节，即n个ASCII码
// 输入参数  : pchar 发送数据 count 长度.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void PostSendBytes(USART_FUNCTION uf,uchar *pchar,uchar count)
{
	if(count){
		while(count--)	
			PostSendByte(uf,*pchar++);
	}
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : PostSendOneByte
// 功能描述  : 发送一字节数据,2字节ASC
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void PostSendByteASC(USART_FUNCTION uf,uchar ChDa)
{
	uchar TempH;
	uchar TempL;
	TempH = hi_hex2asc(ChDa);
	TempL = low_hex2asc(ChDa);

	PostSendByte(uf,TempH);
	PostSendByte(uf,TempL);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : PostSend_HexStr
// 功能描述  : 发送字符串(获取配置参数用)
// 输入参数  : None .
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void PostSend_HexStr(USART_FUNCTION uf,uchar *P,uchar Cnt)  
{	
	uchar  i;
	P += Cnt;
	for(i=0; i<Cnt; i++)            
	{	
		P--;
		PostSendByteASC(uf,hi_hex2asc(*P));
		PostSendByteASC(uf,low_hex2asc(*P));
	}
	PostSendByteASC(uf,0x2C);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : PostSend_Word
// 功能描述  : 送两个字符数
// 输入参数  : TemInt 发送数据.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void PostSend_Word(USART_FUNCTION uf,uint TemInt)  
{	uchar  Te;
	Te = (uchar )(TemInt>>8);
	PostSendByteASC(uf,Te);
	PostSendByteASC(uf,TemInt);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : PostSend_Str
// 功能描述  : 发送字符串
// 输入参数  : PS1字符串
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void PostSend_Str(USART_FUNCTION uf,char *PS1)
{
	while(*PS1 != 0x00)
	{
		PostSendByteASC(uf,*PS1);
		PS1++;
	}
	PostSendByteASC(uf,0x2C);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : PostSendHead
// 功能描述  : 邮电部协议发送数据帧头部分,应答使用
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void PostSendHead(USART_FUNCTION uf)
{
	uchar len_H = 0x00;
	uchar len_L = 0x00;
	uchar PostVer = POST_VER;
	uchar DeviceType = SYS_DEVICE_TYPE;
	/*复位收发缓存*/
	API_ResetProtocolBuffer(uf,RESETRTBUFF_TYPE_TX);
	PostSendByte(uf,'~');
	PostSendByteASC(uf,PostVer);
	PostSendByteASC(uf,ModAddress);
	PostSendByteASC(uf,DeviceType);
	PostSendByteASC(uf,RTN);	    //RTN
	PostSendByteASC(uf,len_H);	   			//虚拟发送数据体长度
	PostSendByteASC(uf,len_L);	   			//虚拟发送数据体长度
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : PostSendHeadCmd
// 功能描述  : 邮电部协议发送数据帧头部分，主发使用
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void PostSendHeadCmd(USART_FUNCTION uf,uchar Addr,uchar cmd)	    
{
	uchar len_H = 0x00;
	uchar len_L = 0x00;
	uchar PostVer = POST_VER;
	uchar DeviceType = SYS_DEVICE_TYPE;
	/*复位收发缓存*/
	API_ResetProtocolBuffer(uf,RESETRTBUFF_TYPE_TX);
	PostSendByte(uf,'~');
	PostSendByteASC(uf,PostVer);
	PostSendByteASC(uf,Addr);
	PostSendByteASC(uf,DeviceType);
	PostSendByteASC(uf,cmd);	    //RTN
	PostSendByteASC(uf,len_H);	   			//虚拟发送数据体长度
	PostSendByteASC(uf,len_L);	   			//虚拟发送数据体长度
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : PostSendTail
// 功能描述  : 邮电部协议发送帧尾
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void PostSendTail(USART_FUNCTION uf)
{	
	PostSendByte(uf,0x0D);
	API_StartSendBuffer(uf);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : SendPostChksum
// 功能描述  : 邮电部协议回发的校验和函数（要先HEX转成ASC再求校验和）
// 输入参数  : len 校验数据长度.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void PostSendPostChksum(USART_FUNCTION uf)  
{
	uchar *TxBuf;
	uint16 *Len;
	uchar *p;
	uint i,ChksumData=0;
	uchar tempSumH;
	uchar tempSumL;

	if(API_GetProtocolBufferOrLen(uf,&TxBuf,&Len)>-1)
	{
		p = TxBuf+1;
		for(i=0;i<(*Len)-1;i++)
		{
			ChksumData+=*p++;
		}
		ChksumData=~ChksumData+1;
		tempSumH = (uchar)(ChksumData>>8);
		tempSumL = (uchar)ChksumData;

		PostSendByteASC(uf,tempSumH);
		PostSendByteASC(uf,tempSumL);
	}
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : PostSendFixupByte
// 功能描述  : 修正发送缓冲固定位置参数
// 输入参数  : FixupNo 位置 valus 参数.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void PostSendFixupByte(USART_FUNCTION uf,uchar *TxBuf,uchar FixupNo,uchar valus)
{
	TxBuf[FixupNo] = hi_hex2asc(valus);
	TxBuf[FixupNo+1] = low_hex2asc(valus);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : PostSendDataLen
// 功能描述  : 邮电部协议发送数据长度
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void PostSendDataLen(USART_FUNCTION uf)
{
	uchar *TxBuf;
	uint16 *Len;
	Taglchksum  tmplen = {0};
	unsigned int word;
	unsigned char tempH;
	unsigned char tempL;

	if(API_GetProtocolBufferOrLen(uf,&TxBuf,&Len)>-1)
	{
		tmplen.word = *Len - 13;
		tmplen.div.lChksum = tmplen.div.hLenth+tmplen.div.mLenth+tmplen.div.lLenth;
		tmplen.div.lChksum = ~tmplen.div.lChksum+1;
		word = tmplen.word;
		tempH = word>>8;
		tempL =	(uchar)word;

		PostSendFixupByte(uf,TxBuf,9,tempH);	   
		PostSendFixupByte(uf,TxBuf,11,tempL);
	}
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ChkPostDataSum
// 功能描述  : 邮电部协议数据长度校验
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : 成功返回0  失败返回1
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
uchar ChkPostDataSum(uchar *DaPtr,uint16 len)
{
	Taglchksum  tmplen  = {0};
	unsigned int  word;

	word = TwocharToInt(DaPtr[5],DaPtr[6]);	//收到的信息体长度
	tmplen.word = len - 18;					//实际收到的信息体累加的长度,再对些计算校验 
	tmplen.div.lChksum = tmplen.div.hLenth+tmplen.div.mLenth+tmplen.div.lLenth;
	tmplen.div.lChksum=~tmplen.div.lChksum+1;

	if(word!=tmplen.word)
		return 1;	
	else
		return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : SendDefault
// 功能描述  : 邮电部协议应答的命令，无数据体
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void PostSendDefault(USART_FUNCTION uf)
{	
	PostSendHead(uf);
	PostSendByteASC(uf,1);   					// DataFlag
	PostSendByteASC(uf,1);   					// 此状态为协议转换器与PC的通信状态
	PostSendDataLen(uf);
	PostSendPostChksum(uf);
	PostSendTail(uf);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DealPostProtocol
// 功能描述  : 邮电部协议解析
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void DealPostProtocol(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
{ 		  
	unsigned char CallAddr;
	unsigned char utemp;	
	unsigned char Var;
	u16 i;
	for(i=0;i<len;i++)
	{
		qwl_sendchar(DaPtr[i]);
	}
	Var=twoascii2byte(DaPtr[1],DaPtr[2]);//协议版本	
	CallAddr=twoascii2byte(DaPtr[3],DaPtr[4]);				/*上位机呼叫地址*/
		
	if(CallAddr==ModAddress ||CallAddr==0xff||CallAddr==0x01)
	{	//判断出呼叫本机 6=SOI+EOI+SUM==4 ASCII码
		utemp=chk_post_chksum((uchar *)&DaPtr[1],len-6);	

		//判校验和是否正确
		if(!utemp)
		{  	 
            // 把ASCII转换成HEX
    		AscToHexFun(&DaPtr[1],len-2); 
            if(DaPtr[4]==0x50)	//获取地址 广播直接能通过
			{	
                RTN=0;		
				DealPostPro(uf,DaPtr,len);	
			}  	
            else
            {		 				    
    			if((Var==0x20)||(Var==0x21)||(Var==0x22))
    			{	 
    				if(1)//!ChkPostDataSum(DaPtr,len))
    				{  
    					RTN=0;								
    					DealPostPro(uf,DaPtr,len); 
    				}
    				else
    				{	//参数部分累加和检查不对
    					RTN=0x03;
    				}									
    			}
    			else
    			{
                    RTN=0x01;	
    			}
            }
		}
		else
		{
            RTN=0x02;//累加和检查不对
		}										
	}
	else
	{
		API_ResetProtocolBuffer(uf,RESETRTBUFF_TYPE_ALL);
	}

	if(RTN)
	{	//信息体错误
		PostSendDefault(uf);
	}
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DealPostPro
// 功能描述  : 邮电部协议处理
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※ 
void DealPostPro(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
{	
    switch(uf)
	{
		case USART_FUNCTION_WCDMA:
			Uploading_Pro(uf,DaPtr,len);
			break;
		case USART_FUNCTION_MOUNT + 0:
			break;
		case USART_FUNCTION_MOUNT + 1:
			break;
		case USART_FUNCTION_MSG:
			Uploading_Pro(uf,DaPtr,len);
			break;
		case USART_FUNCTION_DEBUG:
			Uploading_Pro(uf,DaPtr,len);
			break;
	}
}



//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//                          程序结束(END)
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※


