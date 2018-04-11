#include "config.h"

/*网络口映射的串口端口*/
#define   ETH_MAP_PORT	 USART_FUNCTION_DEBUG

const PINFO _gProtocol_info[PROTOCOL_TYPE_MAX]=
{
	{1,{0x7E,0x00,0x00,0x00}, 9, 1,{0x0D,0x00,0x00,0x00},PROTOCOL_TYPE_POSTS},	//邮电部协议
//	{1,{0x2A,0x00,0x00,0x00}, 6, 1,{0x0D,0x00,0x00,0x00},PROTOCOL_TYPE_COMPANY},//公司协议
//	{1,{0xF0,0x00,0x00,0x00},18,1,{0x0D,0x00,0x00,0x00},PROTOCOL_TYPE_WIRELESS},//无线协议
	{1,{0xAA,0x00,0x00,0x00},11,4,{0xCC,0xEE,0xC3,0x3C},PROTOCOL_TYPE_DOWNLOAD},//程序升级协议,
	//注意，真正的结尾位{0xCC,0x33,0xC3,0x3C}，主程序执行时变回程序本身缺陷，想要远程升级而必须改
};
#if CONF_QWL_NET_EN
const PINFO_BC95 _gProtocol_info1[PROTOCOL_TYPE_MAX]=
{
	{2,{'7','E',0x00,0x00,0x00,0x00,0x00,0x00},16,2,{'0','D',0x00,0x00,0x00,0x00,0x00,0x00},PROTOCOL_TYPE_POSTS},	//邮电部协议
	{2,{'A','A',0x00,0x00,0x00,0x00,0x00,0x00},22,8,{'C','C','E','E','C','3','3','C'},PROTOCOL_TYPE_DOWNLOAD},//程序升级协议,
	//注意，真正的结尾位{0xCC,0x33,0xC3,0x3C}，主程序执行时变回程序本身缺陷，想要远程升级而必须改
};
#endif
#define PINFO_SIZE	  2//总协议数量

typedef __packed struct {
	ProtocolFun send;		//发送函数
	ProtocolFun recv;		//接收函数
	ProtocolFun doProtocol;	//接收到数据完毕后在主程序里面执行
	ProtocolResource* pRes;	//协议
	ReceiveFun rFun;		//接收数据后的回调函数
}ProtocolFunStruct;

static ProtocolFunStruct _gProtocolFun[USART_FUNCTION_MAX] ={
	0
};

NetSendNum gNetSend;			   //网络分包发送标志

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Protocols_Init
// 功能描述  : 协议初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void Protocols_Init(void)
{
	Uart_Init();
	Init_Protocol_Debug();
	#if CONF_QWL_NET_EN
	Init_Protocol_Wcdma();
		#endif
	#if CONF_ARMCHAIR_EN
	Init_Protocol_MSG();
		#endif
	/*注册debug协议端口数据接收处理回调函数*/
	SetProtocolReceiveCallBack(USART_FUNCTION_DEBUG,ReceivePro_Debug_Dispose);
	#if CONF_QWL_NET_EN
		/*注册wcdma协议端口数据接收处理回调函数*/
	SetProtocolReceiveCallBack(USART_FUNCTION_WCDMA,ReceivePro_Debug_Dispose);
	#endif
	#if CONF_ARMCHAIR_EN
		/*注册wcdma协议端口数据接收处理回调函数*/
	SetProtocolReceiveCallBack(USART_FUNCTION_MSG,ReceivePro_Debug_Dispose);
	#endif
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : RegisterProtocolFun
// 功能描述  : 注册一个协议的相关参数
// 输入参数  : uf,协议类型 send,发送函数 recv,接收函数  doProtocol,回调函数 pRes,协议缓存
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int RegisterProtocolFun(USART_FUNCTION uf,ProtocolFun send,ProtocolFun recv,ProtocolFun doProtocol,ProtocolResource* pRes)
{
	 if(uf < USART_FUNCTION_MAX)
	 {
	 	_gProtocolFun[uf].send = send;
		_gProtocolFun[uf].recv = recv;
		_gProtocolFun[uf].doProtocol = doProtocol;
		_gProtocolFun[uf].pRes = pRes;

	  	return 0;
	 }
	 return -1;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : getPINFO
// 功能描述  : 获取协议结构
// 输入参数  : pt,协议类型
// 输出参数  : None
// 返回参数  : 协议结构
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
#if CONF_QWL_NET_EN
const PINFO_BC95* getPINFO_BC95(PROTOCOL_TYPE pt)
{
	int i = 0;
	for(i = 0; i < PINFO_SIZE; i++)
	{
		if(_gProtocol_info1[i].ProtcolID == pt )
		{
			return &_gProtocol_info1[i];
		}
	}
	return NULL;
}
#endif
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : getPINFO
// 功能描述  : 获取协议结构
// 输入参数  : pt,协议类型
// 输出参数  : None
// 返回参数  : 协议结构
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
const PINFO* getPINFO(PROTOCOL_TYPE pt)
{
	int i = 0;
	for(i = 0; i < PINFO_SIZE; i++)
	{
		if(_gProtocol_info[i].ProtcolID == pt )
		{
			return &_gProtocol_info[i];
		}
	}
	return NULL;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : SendProtocolBuffer
// 功能描述  : 向指定的协议类型发送数据
// 输入参数  : uf,协议类型 buffer,数据 size,数据大小
// 输出参数  : None
// 返回参数  : 0:成功 1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int SendProtocolBuffer(USART_FUNCTION uf,uchar* buffer, size_t size)
{
	if(AddProtocolBuffer(uf,buffer,size)>-1) 
	{
		return StartSendBuffer(uf);
	}
	return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ResetProtocolBuffer
// 功能描述  : 复位收发缓存
// 输入参数  : uf,协议类型	 ResetType复位选择
// 输出参数  : None
// 返回参数  : 0:成功 -1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int ResetProtocolBuffer(USART_FUNCTION uf,RESETRTBUFFTYPE ResetType)
{
	ProtocolResource *Protocol = NULL;
	if(uf >=USART_FUNCTION_MAX)
		return -1;
	Protocol = _gProtocolFun[uf].pRes;
	if(Protocol == NULL)
		return -1;
	switch(ResetType)
	{
		case RESETRTBUFF_TYPE_ALL:
			Reset_Protocol(Protocol);
			break;
		case RESETRTBUFF_TYPE_RX:
			Reset_Protocol_Rx(Protocol);
			break;
		case RESETRTBUFF_TYPE_TX:
			Reset_Protocol_Tx(Protocol);
			break;
	}
	return 0;
}


//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AddProtocolBuffer
// 功能描述  : 向指定的协议类型缓存添加发送数据
// 输入参数  : uf,协议类型 buffer,数据 size,数据大小
// 输出参数  : None
// 返回参数  : 0:成功 1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int AddProtocolBuffer(USART_FUNCTION uf,uchar* buffer, size_t size)
{
	ProtocolResource *Protocol = NULL;
	if(uf >=USART_FUNCTION_MAX || buffer == NULL || size < 1)
		return -1;
	Protocol = _gProtocolFun[uf].pRes;
	if(Protocol == NULL)
		return -1;
	if(((Protocol->PTxDaBuf+size)<Protocol->TxMessageBuffer)||((Protocol->PTxDaBuf+size)>=(Protocol->TxMessageBuffer+Protocol->size)))
	{
		Reset_Protocol(Protocol);
		return -1;
	}
	memcpy(Protocol->PTxDaBuf,buffer,size);
	Protocol->PTxDaBuf += size;
	Protocol->TxPro_Lenth += size;

	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : GetProtocolBufferOrLen
// 功能描述  : 获取指定的协议发送缓存指针和长度
// 输入参数  : uf,协议类型
// 输出参数  : ptr发送缓存指针	len发送长度
// 返回参数  : 0:成功 -1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int GetProtocolBufferOrLen(USART_FUNCTION uf,uchar **ptr,uint16 **len)
{
	ProtocolResource *Protocol = NULL;
	if(uf >=USART_FUNCTION_MAX)
		return -1;
	Protocol = _gProtocolFun[uf].pRes;
	if(Protocol == NULL)
		return -1;
	*ptr = (uchar *)&(Protocol->TxMessageBuffer[0]);
	*len = (uint16 *)&(Protocol->TxPro_Lenth);

	return 0;
}





//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : StartSendBuffer
// 功能描述  : 指定的协议类型启动发送数据
// 输入参数  : uf,协议类型
// 输出参数  : None
// 返回参数  : 0:成功 1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int StartSendBuffer(USART_FUNCTION uf)
{
	ProtocolResource *Protocol = NULL;
	if(uf >=USART_FUNCTION_MAX)
		return -1;
	Protocol = _gProtocolFun[uf].pRes;
	if(Protocol == NULL)
		return -1;

	Protocol->ResState = RESOURCE_USE_STATUS_SEND;
	
	switch(Protocol->Port_Type)	  
	{
		case PORT_TYPE_SERIAL:
			/*发送数据前先将RS485方向控制脚置位发送状态*/
			SetupComDir(GetUartStructBaseFunction(uf),false);
			Init_ProSendPrt(Protocol);
			ProtocolSendByte(uf);
			break;
		
		case PORT_TYPE_WCDMA:
			/*发送数据前先将RS485方向控制脚置位发送状态*/
			//SetupComDir(GetUartStructBaseFunction(uf),false);
			//Init_ProSendPrt(Protocol);
		#if CONF_QWL_NET_EN
		net_send(Protocol->TxMessageBuffer,Protocol->TxPro_Lenth);
		#endif
			//ProtocolSendByte(uf);
			break;
		default:break;

	}

	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ProtocolSendByte
// 功能描述  : 向指定的协议类型发送单个数据
// 输入参数  : uf,协议类型
// 输出参数  : None
// 返回参数  : 0:成功 1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int ProtocolSendByte(USART_FUNCTION uf)
{
	if(uf < USART_FUNCTION_MAX)
	{
		if(_gProtocolFun[uf].send != NULL)
		{
			return _gProtocolFun[uf].send();
		}
	}
	
	return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ProtocolReceiveByte
// 功能描述  : 根据指定的协议类型获取单个数据
// 输入参数  : uf,协议类型
// 输出参数  : None
// 返回参数  : 0:成功 1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int ProtocolReceiveByte(USART_FUNCTION uf)
{
	if(uf < USART_FUNCTION_MAX)
	{
		if(_gProtocolFun[uf].send != NULL)
		{
			return _gProtocolFun[uf].recv();
		}
	}
	
	return -1;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : CheckProtocolType
// 功能描述  : 判断协议类型
// 输入参数  : Protocol,协议缓存
// 输出参数  : None
// 返回参数  : 协议类型
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
#if CONF_QWL_NET_EN
static PROTOCOL_TYPE CheckProtocolType_BC95(ProtocolResource *Protocol)
{
	int index = 0;
	PROTOCOL_TYPE pt = 	PROTOCOL_TYPE_NONE;
	if(Protocol->RxPro_Lenth < 1)
	{
		 return PROTOCOL_TYPE_NONE;	// 接收到的数据为空，那么无从判断，只能为空
	}
	for(index = 0; index <PINFO_SIZE; index++)
	{
		int RHIndex = 0; 
		uchar RHLenth = _gProtocol_info1[index].RHLenth;	//头字节的长度
		uchar minLen = Protocol->RxPro_Lenth > RHLenth?RHLenth:Protocol->RxPro_Lenth;	//取两个长度的最小值，防止溢出
		for(RHIndex = 0; RHIndex < minLen; RHIndex++) {

			if(Protocol->PRxDaBuf[RHIndex] != _gProtocol_info1[index].RHead[RHIndex])
				break;

		}

		if(RHIndex < RHLenth && RHIndex == minLen && pt == PROTOCOL_TYPE_NONE)
			pt = PROTOCOL_TYPE_CHECKING;
		if(RHIndex == RHLenth) {
			pt =  _gProtocol_info1[index].ProtcolID;	//读取完全匹配的协议
			break;
		} 
	}
	
	return pt;
}
#endif
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : CheckProtocolType
// 功能描述  : 判断协议类型
// 输入参数  : Protocol,协议缓存
// 输出参数  : None
// 返回参数  : 协议类型
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static PROTOCOL_TYPE CheckProtocolType(ProtocolResource *Protocol)
{
	int index = 0;
	PROTOCOL_TYPE pt = 	PROTOCOL_TYPE_NONE;
	if(Protocol->RxPro_Lenth < 1)
	{
		 return PROTOCOL_TYPE_NONE;	// 接收到的数据为空，那么无从判断，只能为空
	}
	for(index = 0; index <PINFO_SIZE; index++)
	{
		int RHIndex = 0; 
		uchar RHLenth = _gProtocol_info[index].RHLenth;	//头字节的长度
		uchar minLen = Protocol->RxPro_Lenth > RHLenth ? RHLenth:Protocol->RxPro_Lenth;	//取两个长度的最小值，防止溢出
		for(RHIndex = 0; RHIndex < minLen; RHIndex++) {

			if(Protocol->PRxDaBuf[RHIndex] != _gProtocol_info[index].RHead[RHIndex])
				break;

		}

		if(RHIndex < RHLenth && RHIndex == minLen && pt == PROTOCOL_TYPE_NONE)
			pt = PROTOCOL_TYPE_CHECKING;
		if(RHIndex == RHLenth) {
			pt =  _gProtocol_info[index].ProtcolID;	//读取完全匹配的协议
			break;
		} 
	}
	
	return pt;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DoProtocols
// 功能描述  : 在主循环体内执行回调函数
// 输入参数  : None
// 输出参数  : None
// 返回参数  :  0:成功 1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int DoProtocols(void)
{
	int index = 0;
	for(index = 0; index <USART_FUNCTION_MAX; index++)
	{

		if(_gProtocolFun[index].doProtocol != NULL)
		{
				
			_gProtocolFun[index].doProtocol();
		}
	
	}	

	return 0;
	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : GetProtocolResource
// 功能描述  : 获取协议缓冲
// 输入参数  : uf协议类型
// 输出参数  : None
// 返回参数  : 协议缓冲
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
ProtocolResource * GetProtocolResource(USART_FUNCTION uf)
{
	if(uf < USART_FUNCTION_MAX)
	{
		return _gProtocolFun[uf].pRes; 		
	}
	return NULL;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ReceiveCallBack
// 功能描述  : 执行接收回调函数
// 输入参数  : uf协议类型 buffer接收数据缓存  size长度
// 输出参数  : None
// 返回参数  :  0:成功 -1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int ReceiveCallBack(USART_FUNCTION uf,PROTOCOL_TYPE Pro_Type,uchar* buffer, size_t size)
{
	if(uf < USART_FUNCTION_MAX)
	{
		if(_gProtocolFun[uf].rFun != NULL)
		{

			return _gProtocolFun[uf].rFun(uf,Pro_Type,buffer,size);
		} 		
	}
	
	return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : SetProtocolReceiveCallBack
// 功能描述  : 设置接收回调函数
// 输入参数  : uf协议类型 fun回调函数
// 输出参数  : None
// 返回参数  :  0:成功 -1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int SetProtocolReceiveCallBack(USART_FUNCTION uf,ReceiveFun fun)
{
	if(uf < USART_FUNCTION_MAX)
	{
		_gProtocolFun[uf].rFun = fun;
		return 0;
	}
	
	return -1;	
}



//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DoProtocolAnalysis_BC95
// 功能描述  : 协议数据接收解析处理,专为NB开通处理通道
// 输入参数  : Protocol,协议 ChrData,接收的一个字节.
// 输出参数  : None
// 返回参数  :  0:成功 1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
#if CONF_QWL_NET_EN
int DoProtocolReceiveAnalysis_BC95(ProtocolResource *Protocol,uchar ChrData)  // 接收一条全的主机数据
{
	PROTOCOL_TYPE pt = PROTOCOL_TYPE_NONE;
	if(Protocol == NULL)
		return -1;
	if((Protocol->ResState == RESOURCE_USE_STATUS_FREE)||(Protocol->RxPro_Lenth<1))
	{
		Reset_Protocol(Protocol);		//需要先进行复位		
		Protocol->ResState = RESOURCE_USE_STATUS_RECEIVE;	//抢占协议类型			
	}
	if(Protocol->ResState != RESOURCE_USE_STATUS_RECEIVE) //协议处于非接收状态，
		return -1;
	Protocol->PRxDaBuf[Protocol->RxPro_Lenth] = ChrData;
	Protocol->RxPro_Lenth++;

	if(Protocol->RxPro_Lenth + (Protocol->PRxDaBuf - (uchar*)Protocol->RxMessageBuffer) >= Protocol->size)	  //MAXPCReLen
	{
		//超出了最大缓存值
		Reset_Protocol(Protocol);
	}
	if(Protocol->Pro_Type < PROTOCOL_TYPE_MAX)
	{
		//如果协议已经被赋值
		pt =  Protocol->Pro_Type;
	} else {
		//没有被赋值，开始进行协议检测
		while((pt = CheckProtocolType_BC95(Protocol)) == PROTOCOL_TYPE_NONE && Protocol->RxPro_Lenth > 0) {
			Protocol->RxPro_Lenth--;
			Protocol->PRxDaBuf++;
		}
		Protocol->Pro_Type = pt;		
	}
	if(pt < PROTOCOL_TYPE_MAX)	//检测到标准的协议了
	{
		
		const PINFO_BC95* pinf = getPINFO_BC95(pt);
		if(pinf != NULL)
		{
			int k = 0;
			uchar* pChar = (Protocol->PRxDaBuf + (Protocol->RxPro_Lenth - pinf->RTLenth));/*最后一个字符*/
			u8 qwl_RTail[8]={'C','C','E','E','C','3','3','C'};
			if(pinf->RTLenth==8)
			{
				for(k = 0; k < pinf->RTLenth; k++)
				{
				if(qwl_RTail[k] != pinf->RTail[k])
					break;
				}
				
				if(k >= pinf->RTLenth)
				{
					qwl_RTail[2]='3';
					qwl_RTail[3]='3';
				}
			}
			else
			{
					for(k = 0; k < pinf->RTLenth; k++)
				{
				qwl_RTail[k] = pinf->RTail[k];
				}
			}

			

			for(k = 0; k <  pinf->RTLenth; k++)
			{
				if(pChar[k] != qwl_RTail[k]) {
					break;
				}	
			}
			if(k >= pinf->RTLenth)
			{
				if(Protocol->RxPro_Lenth >= pinf->RSLenth )
				{
					u8 BC95_rec_buf[512]={0};
					u16 BC95_rec_len=0;
					u16 i;

					for(i=0;i<(Protocol->RxPro_Lenth);i=i+2)
					{
						BC95_rec_buf[BC95_rec_len]=(ASCII_hex[(Protocol->PRxDaBuf[i]-0x30)]<<4)+ASCII_hex[(Protocol->PRxDaBuf[i+1]-0x30)];
						BC95_rec_len++;
					}

					for(i=0;i<BC95_rec_len;i++)
					{
					Protocol->PRxDaBuf[i]=BC95_rec_buf[i];	
					}

					
					Protocol->RxPro_Lenth=BC95_rec_len;
					
					Protocol->ResState = RESOURCE_USE_STATUS_END;	//直接结束
				}
				else
				{
					Reset_Protocol(Protocol);		//需要先进行复位		
				}
			}  
		}
	}
	return 0;
}
#endif
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DoProtocolAnalysis
// 功能描述  : 协议数据接收解析处理
// 输入参数  : Protocol,协议 ChrData,接收的一个字节.
// 输出参数  : None
// 返回参数  :  0:成功 1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int DoProtocolReceiveAnalysis_ZTE(ProtocolResource *Protocol,uchar ChrData)  // 接收一条全的主机数据
{
	PROTOCOL_TYPE pt = PROTOCOL_TYPE_NONE;
	
	if(Protocol == NULL)
		return -1;
	if((Protocol->ResState == RESOURCE_USE_STATUS_FREE)||(Protocol->RxPro_Lenth<1))
	{
		Reset_Protocol(Protocol);		//需要先进行复位		
		Protocol->ResState = RESOURCE_USE_STATUS_RECEIVE;	//抢占协议类型			
	}
	if(Protocol->ResState != RESOURCE_USE_STATUS_RECEIVE) //协议处于非接收状态，
		return -1;
	Protocol->PRxDaBuf[Protocol->RxPro_Lenth] = ChrData;
	Protocol->RxPro_Lenth++;
	

	if(Protocol->RxPro_Lenth + (Protocol->PRxDaBuf - (uchar*)Protocol->RxMessageBuffer) >= Protocol->size)	  //MAXPCReLen
	{
		//超出了最大缓存值
		Reset_Protocol(Protocol);
	}
	if(Protocol->Pro_Type < PROTOCOL_TYPE_MAX)
	{
		//如果协议已经被赋值
		pt =  Protocol->Pro_Type;
	} else {
		//没有被赋值，开始进行协议检测
		while((pt = CheckProtocolType(Protocol)) == PROTOCOL_TYPE_NONE && Protocol->RxPro_Lenth > 0) {
			Protocol->RxPro_Lenth--;
			Protocol->PRxDaBuf++;
		}
		Protocol->Pro_Type = pt;		
	}
	if(pt < PROTOCOL_TYPE_MAX)	//检测到标准的协议了
	{
		
		const PINFO* pinf = getPINFO(pt);
		if(pinf != NULL)
		{
			int k = 0;
			uchar* pChar = (Protocol->PRxDaBuf + (Protocol->RxPro_Lenth - pinf->RTLenth));/*最后一个字符*/
			u8 qwl_RTail[4]={0xCC,0xEE,0xC3,0x3C};
			if(pinf->RTLenth==4)
			{
				for(k = 0; k < pinf->RTLenth; k++)
				{
				if(qwl_RTail[k] != pinf->RTail[k])
					break;
				}
				
				if(k >= pinf->RTLenth)
				{
					qwl_RTail[1]=0x33;
				}
			}
			else
			{
					for(k = 0; k < pinf->RTLenth; k++)
				{
				qwl_RTail[k] = pinf->RTail[k];
				}
			}

			

			for(k = 0; k <  pinf->RTLenth; k++)
			{
				if(pChar[k] != qwl_RTail[k]) {
					break;
				}	
			}
			if(k >= pinf->RTLenth)
			{
				if(Protocol->RxPro_Lenth >= pinf->RSLenth )
				{
					Protocol->ResState = RESOURCE_USE_STATUS_END;	//直接结束
					
				}
				else
				{
					Reset_Protocol(Protocol);		//需要先进行复位		
				}
			}  
		}
	}
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DoProtocolAnalysis
// 功能描述  : 协议数据接收解析处理
// 输入参数  : Protocol,协议 ChrData,接收的一个字节.
// 输出参数  : None
// 返回参数  :  0:成功 1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int DoProtocolReceiveAnalysis(ProtocolResource *Protocol,uchar ChrData)  // 接收一条全的主机数据
{
	
		#if CONF_QWL_NET_EN
	if(Protocol->Mode_Type ==MODE_TYPE_BC95)
	{
		return DoProtocolReceiveAnalysis_BC95(Protocol,ChrData);
	}
	else
		#endif
	{
		return DoProtocolReceiveAnalysis_ZTE(Protocol,ChrData);
	}
		
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DoProtocol_DMA_ReceiveAnalysis
// 功能描述  : 协议数据DMA接收解析处理
// 输入参数  : Protocol,协议 len,DMA接收长度.
// 输出参数  : None
// 返回参数  :  0:成功 1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int DoProtocol_DMA_ReceiveAnalysis(ProtocolResource *Protocol,int len)  // 接收一条全的主机数据
{	
	int i = 0;
	PROTOCOL_TYPE pt = PROTOCOL_TYPE_NONE;
	if(Protocol == NULL)
		return -1;
	for(i=0;i<len;i++)
	{
		if((Protocol->ResState == RESOURCE_USE_STATUS_FREE)||(Protocol->RxPro_Lenth<1))
		{
			Reset_Protocol(Protocol);		//需要先进行复位		
			Protocol->ResState = RESOURCE_USE_STATUS_RECEIVE;	//抢占协议类型			
		}
		if(Protocol->ResState != RESOURCE_USE_STATUS_RECEIVE) //协议处于非接收状态，
			return -1;
		Protocol->RxPro_Lenth++;
	
		if(Protocol->RxPro_Lenth + (Protocol->PRxDaBuf - (uchar*)Protocol->RxMessageBuffer) >= Protocol->size)	  //MAXPCReLen
		{
			//超出了最大缓存值
			Reset_Protocol(Protocol);
		}
		if(Protocol->Pro_Type < PROTOCOL_TYPE_MAX)
		{
			//如果协议已经被赋值
			pt =  Protocol->Pro_Type;
		} 
		else 
		{
			//没有被赋值，开始进行协议检测
			while((pt = CheckProtocolType(Protocol)) == PROTOCOL_TYPE_NONE && Protocol->RxPro_Lenth > 0) 
			{
				Protocol->RxPro_Lenth--;
				Protocol->PRxDaBuf++;
			}
			Protocol->Pro_Type = pt;		
		}

		if(pt < PROTOCOL_TYPE_MAX)	//检测到标准的协议了
		{
			
			const PINFO* pinf = getPINFO(pt);
			if(pinf != NULL)
			{
				int k = 0;
				uchar* pChar = (Protocol->PRxDaBuf + (Protocol->RxPro_Lenth - pinf->RTLenth));/*最后一个字符*/
				u8 qwl_RTail[4]={0xCC,0xEE,0xC3,0x3C};
			if(pinf->RTLenth==4)
			{
				for(k = 0; k < pinf->RTLenth; k++)
				{
				if(qwl_RTail[k] != pinf->RTail[k])
					break;
				}
				
				if(k >= pinf->RTLenth)
				{
					qwl_RTail[1]=0x33;
				}
			}
			else
			{
					for(k = 0; k < pinf->RTLenth; k++)
				{
				qwl_RTail[k] = pinf->RTail[k];
				}
			}	
				
				for(k = 0; k <  pinf->RTLenth; k++)
				{
					if(pChar[k] != qwl_RTail[k]) {
						break;
					}	
				}
				if(k >= pinf->RTLenth)
				{
					if(Protocol->RxPro_Lenth >= pinf->RSLenth )
					{
						Protocol->ResState = RESOURCE_USE_STATUS_END;	//直接结束
					}
					else
					{
						Reset_Protocol(Protocol);		//需要先进行复位		
					}
				}  
			}
		}
	}
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : SettingCommBps
// 功能描述  : 生效串口参数设置
// 输入参数  : None
// 输出参数  : None
// 返回参数  :  0:成功 1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
USART_FUNCTION _guf;
void SettingCommBps(void)
{
	if(_guf < USART_FUNCTION_MAX)
	{
		SettingSaveByUSART_FUNCTION(_guf);
	}
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : SetUartParameter
// 功能描述  : 串口参数设置
// 输入参数  : None
// 输出参数  : None
// 返回参数  :  0:成功 1:失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int SetUartParameter(
	USART_FUNCTION uf,					//串口协议
	uint32_t baudRate,	 				//波特率
	uint16_t parity,					//校验位
	uint16_t wordLength,				//数据长度
	uint16_t stopBits					//停止位
	)
{
	UartStruct* us = GetUartStructBaseFunction(uf);
	if(us != NULL)
	{
	   UartCom * uCom =us->uCom;
	   if(uCom != NULL)
	   {
	   		uCom->UartCommAag.baudRate = baudRate;
			uCom->UartCommAag.parity = parity;
			uCom->UartCommAag.wordLength = 	wordLength;
			uCom->UartCommAag.stopBits = stopBits;

			_guf = uf;
			API_SetTimer(2000,1,TIMER_TYPE_SETCOMMBPS,SettingCommBps,NULL);

		    return 0;
	   }
	}
	return -1;	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//                          程序结束(END)
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

