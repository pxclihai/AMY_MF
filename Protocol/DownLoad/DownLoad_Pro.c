#include "config.h"


//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Set_Comm_baud
// 功能描述  : 设置波特率
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void Set_Comm_baud(USART_FUNCTION uf,uchar _baudRate)
{	
	uint32_t baudRate;	 				//波特率
	uint16_t parity;					//校验位
	uint16_t wordLength;				//数据长度
	uint16_t stopBits;					//停止位

	switch(_baudRate)
	{
		case 0:// bps1200 
			baudRate = 1200;
			break;
		case 1:// bps2400
			baudRate = 2400;
			break;
		case 2:// bps4800
			baudRate = 4800; 
			break;
		case 3:// bps9600
			baudRate = 9600;
			break;
		case 4:// bps14400
			baudRate = 14400;
			break;
		case 5:// bps19200
			baudRate = 19200;
			break;
		case 6:// bps38400
			baudRate = 38400;
			break;
		case 7:// bps56000
			baudRate = 56000;
			break;
		case 8:// bps57600
			baudRate = 57600;
			break;
		case 9:// bps115200
			baudRate = 115200;
			break;
		default:// 默认为9600BPS
			baudRate = 9600;
			break;
	}

	parity = USART_Parity_No;
	wordLength = USART_WordLength_8b;
	stopBits = USART_StopBits_1;

	API_SetComParameter(uf,baudRate,parity,wordLength,stopBits);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Program_Updata
// 功能描述  : 升级程序
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
typedef __packed struct
{
	char type;
	WORD length;
}tagWCDMAUpdataHead;

/*上一次收到的数据包编号*/
static uchar Last_Packet_Index = 0;

void Program_Updata_timeout(void)
{
	__disable_fault_irq();

	__set_FAULTMASK(1);      // 关闭所有中端
	NVIC_SystemReset();      // 复位
}

static void Program_Updata(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
{
	tagWCDMAUpdataHead* pHead = NULL; 
	DWORD dataLength = 0;
	int ret = -1;
	/*当前数据包索引*/
	uchar packetIndex;
	char *ps;
	uchar baudIndex = 0xff;

	if((len - 12)<sizeof(tagWCDMAUpdataHead))
	{
		return;
	}

	packetIndex = DaPtr[5];
	ps = (char *)&DaPtr[6];

	pHead = (tagWCDMAUpdataHead*)ps;	

	/*超时无升级命令,重启恢复参数*/
	API_SetTimer(90000,1,TIMER_TYPE_IAP_REV_TIMEOUT,NULL,Program_Updata_timeout);

	DownLoad_SendHead(uf,ModAddress,DaPtr[2]);
	DownLoad_SendByte(uf,0x00); 
	DownLoad_SendByte(uf,packetIndex); 

	switch(pHead->type)
	{
		case 1:
			dataLength = API_IAP_Update_Start(pHead->length < 8,ps + sizeof(tagWCDMAUpdataHead),8);						
			baudIndex = *(ps + sizeof(tagWCDMAUpdataHead) + 8);
			iap_data.InitUserParFlag = 	*(ps + sizeof(tagWCDMAUpdataHead) + 9);

			ret = 0;
			
			DownLoad_SendByte(uf,ret > -1?1:0);
			DownLoad_SendByte(uf,pHead->type); 
			DownLoad_SendByte(uf,((uchar*)&dataLength)[3]); 
			DownLoad_SendByte(uf,((uchar*)&dataLength)[2]); 
			DownLoad_SendByte(uf,((uchar*)&dataLength)[1]);
			DownLoad_SendByte(uf,((uchar*)&dataLength)[0]);  

			Set_Comm_baud(uf,baudIndex);
			break;
		case 2:
			InvertedOrder((uchar*)&(pHead->length),2);
			if(packetIndex != Last_Packet_Index)
			{
				ret = API_IAP_Update_Write(ps + sizeof(tagWCDMAUpdataHead),pHead->length);
				if(ret==-1)
				{
						packetIndex = Last_Packet_Index ;
				}
			}
			else
			{
				ret = 0;
			}

			DownLoad_SendByte(uf,ret > -1?1:0);
			DownLoad_SendByte(uf,pHead->type); 
			break;
		case 3:
			ret = API_IAP_Update_End();

			DownLoad_SendByte(uf,ret > -1?1:0);
			DownLoad_SendByte(uf,pHead->type); 
			break;
	}

	Last_Packet_Index = packetIndex;

	/**************************************************************************/
	/*开始发送应答数据*/
	DownLoad_SendChksum(uf);
	DownLoad_SendTail(uf);
}


//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : DownLoad_Pro
// 功能描述  : 程序升级协议处理
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void DownLoad_Pro(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
{
	uchar Cmd = 0;

	Cmd = DaPtr[2];
	switch(Cmd)
	{
		case 0xF8:
			Run_User_Program_Falg = true;
			DownLoad_SendHead(uf,ModAddress,Cmd);
			DownLoad_SendChksum(uf);
			DownLoad_SendTail(uf);
		exchange_platform.UPDATE_link_overtime=0;//有数据收到，超时标志清0
			break;
		case 0xF9:
			Program_Updata(uf,DaPtr,len);
		exchange_platform.UPDATE_link_overtime=0;//有数据收到，超时标志清0
			break;
		default:
			break;
	}
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//                          程序结束(END)
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※


