#include "config.h"
#include "Protocol.h"

#define USART_FUNCTION_TYPE	 USART_FUNCTION_DEBUG		/*串口功能类型*/

#define MAX_BUFFER_SIZE		1500

static uchar _gRxMessageBuffer[MAX_BUFFER_SIZE]= {0};
static uchar _gTxMessageBuffer[MAX_BUFFER_SIZE]= {0};
static ProtocolResource _gProtocol = {0};	/*协议缓存*/
static UartStruct* _gUartStruct =NULL;		/*串口映射结构体指针*/

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Protocol_Debug_Send
// 功能描述  : 调试发送函数
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static int Protocol_Send()
{
	if(_gUartStruct == NULL)
		return -1;
	if(_gProtocol.TxPro_Lenth==0)
	{
			
		Reset_Protocol(&_gProtocol);		//复位协议
		/*发送数据完成后关闭发送中断*/
		EnableCom(_gUartStruct->uCom,false);
		SetupComDir(_gUartStruct,true);	
	}
	else
	{
		if((_gProtocol.PTxDaBuf<_gProtocol.TxMessageBuffer)||(_gProtocol.PTxDaBuf>=(_gProtocol.TxMessageBuffer+_gProtocol.size)))
		{
			Reset_Protocol(&_gProtocol);		//复位协议
			/*发送数据完成后关闭发送中断*/
			EnableCom(_gUartStruct->uCom,false);
			SetupComDir(_gUartStruct,true);	
			return -1;
		}
		SendDataFromUartStruct(_gUartStruct,*(_gProtocol.PTxDaBuf));	
		_gProtocol.PTxDaBuf++;
		_gProtocol.TxPro_Lenth--;
		/*发送数据后开发送中断*/
		EnableCom(_gUartStruct->uCom,true);
	}
 	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ReceiveTimerOut
// 功能描述  : 接收数据超时处理
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static void ReceiveTimerOut(void)
{
	Reset_Protocol(&_gProtocol);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Protocol_Debug_Receive
// 功能描述  : 调试接收函数
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static int Protocol_Receive()
{
 	uchar chr;		
	if(_gProtocol.Port_Type==PORT_TYPE_SERIAL) //端口为串口方式
	{
        if((_gProtocol.ResState==RESOURCE_USE_STATUS_FREE)
			||(_gProtocol.ResState==RESOURCE_USE_STATUS_RECEIVE))
		{
			
			_gProtocol.Mode_Type = MODE_TYPE_NONE;
		    chr = ReceiveDataFromUartStruct(_gUartStruct);
			DoProtocolReceiveAnalysis(&_gProtocol,chr);
        }
	}
	else
	{ 
		if(_gProtocol.ResState==RESOURCE_USE_STATUS_FREE)
		{
			_gProtocol.Port_Type=PORT_TYPE_SERIAL;
			chr = ReceiveDataFromUartStruct(_gUartStruct);
			DoProtocolReceiveAnalysis(&_gProtocol,chr);
		}
		else
		{
		    //返回错误，网口正在接收
		}
	}
	API_SetTimer(200,1,(TIMER_TYPE)USART_FUNCTION_TYPE,ReceiveTimerOut,NULL);

	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Protocol_Debug_DoProtocol
// 功能描述  : 调试回调函数
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
static int Protocol_DoProtocol()
{
	if(_gProtocol.ResState == RESOURCE_USE_STATUS_END )
	{
		API_KillTimer((TIMER_TYPE)USART_FUNCTION_TYPE);
		ReceiveCallBack(USART_FUNCTION_TYPE,_gProtocol.Pro_Type,_gProtocol.PRxDaBuf,_gProtocol.RxPro_Lenth);
	}
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Init_Protocol_Debug
// 功能描述  : 调试初始化函数
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void Init_Protocol_Debug(void)
{
	_gProtocol.RxMessageBuffer = _gRxMessageBuffer;
	_gProtocol.TxMessageBuffer = _gTxMessageBuffer;
	_gProtocol.size	= MAX_BUFFER_SIZE;
	_gProtocol.RxMode = PROTOCOL_RX_MODE_ANALYSIS;

	Reset_Protocol(&_gProtocol);
	_gProtocol.Port_Type = PORT_TYPE_SERIAL;
	_gUartStruct = GetUartStructBaseFunction(USART_FUNCTION_TYPE);
	RegisterProtocolFun(USART_FUNCTION_TYPE,Protocol_Send,Protocol_Receive,Protocol_DoProtocol,&_gProtocol);

	DMA_Usart_Init(USART_FUNCTION_TYPE);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//                          程序结束(END)
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
