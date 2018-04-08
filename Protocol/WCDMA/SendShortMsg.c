/****************************************Copyright (c)****************************************************
**                                      
**                                 
**
**--------------文件信息---------------------------------------------------------------------------------
** 文件名:				SendShortMsg.c
** 描述:				短信命令解析系统
**
**--------------------------------------------------------------------------------------------------------
** 创建者:				张泽
** 创建日期:			2014-5-24
** 版本号:				v1.0
** 描述:				最初版本
**
**--------------------------------------------------------------------------------------------------------
** 修改者:             
** 修改日期:           
** 版本:                 
** 描述:            
**
*********************************************************************************************************/
 
#include "config.h"
#if CONF_WCDMA_EN
#include "AtCmdAnalysis.h"
#include "SendShortMsg.h"
#include "MsgUtf8String.h"

/******************************************************************************/

#define MAX_MSG_BUFFER_SIZE		600
#define	MAX_MSG_BUFFER_COUNT	3

/******************************************************************************/

typedef int (*MsgProcessFun)(int argc,char**);
typedef __packed struct
{
	char buf[MAX_MSG_BUFFER_SIZE];
	DWORD count;
	char phoneNum[MAX_MSG_PHONE_SIZE];	
}tagMsgBuffer;

typedef __packed struct
{
	char reSendPhone[MAX_MSG_PHONE_SIZE];
	char sendPhoneNum[MAX_MSG_PHONE_SIZE];
	BOOL recallStatus;
}tagMsgSet;

typedef struct
{
	char*	cmd;
	MsgProcessFun	fun;
}tagMsgProcessInfo;


/******************************************************************************/

#define MSGPROCESS_NAME(fun) 	  Msg_Process_##fun
#define MSGPROCESS_FUN(fun)	static int MSGPROCESS_NAME(fun)##(int argc, char** argv)

static BOOL IsHex(char c);

MSGPROCESS_FUN(IP);
MSGPROCESS_FUN(CID);
MSGPROCESS_FUN(BEATTIM);
MSGPROCESS_FUN(DISPCOM);
MSGPROCESS_FUN(PORTON);
MSGPROCESS_FUN(SETCNMP);
MSGPROCESS_FUN(STATUS);
MSGPROCESS_FUN(PHONE);
MSGPROCESS_FUN(ERROR);
MSGPROCESS_FUN(MSG);
MSGPROCESS_FUN(VIS);
MSGPROCESS_FUN(RESET);
MSGPROCESS_FUN(SYS);
MSGPROCESS_FUN(APN);
MSGPROCESS_FUN(MODE);
/******************************************************************************/
//短信处理队列,主要是将中断里面的处理交到主线程内进行处理
static tagMsgSet	_gMsgSet 							= {0};
static LinkQueue	_gLinkQueue[2]						= {0};
static tagMsgBuffer	_gMsgBuffer[MAX_MSG_BUFFER_COUNT] 	= {0};

static QueuePtr 	_gMsgQueueBufferNodePtr 			= NULL;


static const tagCnsModSet* _gCnsMod = NULL;
static int _gCnsModSize = 0;	

const static tagMsgProcessInfo _gMsgProcessInfoArray[] 	= {
	  {"IP",		MSGPROCESS_NAME(IP)},
	  {"CID",		MSGPROCESS_NAME(CID)},
	  {"BEATTIM",	MSGPROCESS_NAME(BEATTIM)},
	  {"DISPCOM",	MSGPROCESS_NAME(DISPCOM)},
	  {"PORTON",	MSGPROCESS_NAME(PORTON)},
	  {"SETCNMP",	MSGPROCESS_NAME(SETCNMP)},
	  {"PHONE",		MSGPROCESS_NAME(PHONE)},
	  {"STATUS",	MSGPROCESS_NAME(STATUS)},
	  {"ERROR",		MSGPROCESS_NAME(ERROR)},
	  {"MSG",		MSGPROCESS_NAME(MSG)},
	  {"VIS",		MSGPROCESS_NAME(VIS)},
	  {"RESET",		MSGPROCESS_NAME(RESET)},
	  {"SYS",		MSGPROCESS_NAME(SYS)},
	  {"APN",		MSGPROCESS_NAME(APN)},
	  {"MODE",		MSGPROCESS_NAME(MODE)}
		  
};

/******************************************************************************/

#define CK(str1,str2)	  					(!StrNcmp((uchar*)str1,(uchar*)str2))
#define PResFreeQueue						_gLinkQueue[0]		//空闲的队列
#define	PResFullQueue						_gLinkQueue[1]	   	//繁忙的队列
#define	PResFreeQueuePtr					&PResFreeQueue
#define	PResFullQueuePtr					&PResFullQueue
#define _gMsgQueueBufferPtr  				((tagMsgBuffer*)(_gMsgQueueBufferNodePtr->data))
#define MAX_MSG_PROCESS_INFO_ARRAY_SIZE		(sizeof(_gMsgProcessInfoArray)/sizeof(tagMsgProcessInfo))

/******************************************************************************/

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_EERom_Story
// 功能描述  : 存储变量到EEROM内
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

static int MsgBuffer_Queue_Init(void)
 {
  	QueueInit(_gMsgBuffer,MAX_MSG_BUFFER_COUNT,PResFreeQueuePtr,PResFullQueuePtr);
 }
 //※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_EERom_Story
// 功能描述  : 存储变量到EEROM内
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

static void Msg_TimerOut(void)
{
	_gMsgSet.recallStatus = false;	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Msg_Buffer_Process
// 功能描述  : 解析短信的内容
// 输入参数  : msgBuf,短信缓冲
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

static int Msg_Buffer_Process(tagMsgBuffer* msgInputBuf)
{
 	u8 i;
	char *p;
	char Command[20]={0};
	char Parameter[80]={0};
	char*	argv[20] ={0};
	int count = 0;

	tagMsgBuffer msgBuffer;
	tagMsgBuffer* msgBuf = &msgBuffer;
	int retSize = 0;

	if((retSize = Msg_Buffer_PDUToUTF8(msgInputBuf->buf,msgInputBuf->count,msgBuf->buf)) < 0)
		return -1;
	msgBuf->count = retSize;
	memcpy(msgBuf->phoneNum,msgInputBuf->phoneNum,MAX_MSG_PHONE_SIZE);
	

	if(msgBuf->buf[0] == '*')//判断协议头
	{	
	   for(i=0; i<msgBuf->count; i++) 
	   {
			if((msgBuf->buf[i] == '#')) //判断短信完整性
			{
				p = strtok((char*)(msgBuf->buf+1), ","); //跳过“*”
				sprintf(Command,"%s",p);   //取命令
				p = strtok(NULL, "#");
	            sprintf(Parameter,"%s",p); //取命令参数
				p = Parameter;
				p = strtok(Parameter, "," );
				argv[count] =  msgBuf->phoneNum;
				count++;
			   	while( p != NULL ) {
			       	argv[count] = p;
			       	p = strtok( NULL, "," );
					count++;
			  	}

				for(i = 0; i < MAX_MSG_PROCESS_INFO_ARRAY_SIZE; i++)
				{
					 if(CK(Command,_gMsgProcessInfoArray[i].cmd))
					 {
					 	 if(_gMsgProcessInfoArray[i].fun != NULL)
						 {
						 	return _gMsgProcessInfoArray[i].fun(count,argv);
						 }
					 }
				}
				//执行解析好的命令和参数	
			}	
	   }		
	}
	else
	{
		if(_gMsgSet.recallStatus && CK(msgBuf->phoneNum,_gMsgSet.sendPhoneNum))
		{
			API_KillTimer(TIMER_TYPE_WCDMA_MSG);
			if(msgInputBuf->count < MAX_MSG_BUFFER_SIZE)
			{
				int index = 0;
				for(index = 0; index < msgInputBuf->count;index++)
				{
					 if(!IsHex(msgInputBuf->buf[index]))
					 	break;
				}
				msgInputBuf->buf[index] = 0x1a;

				AtCmdAnalysis_Msg_Send_Direct(_gMsgSet.reSendPhone,msgInputBuf->buf);
				//AtCmdAnalysis_Msg_Send(_gMsgSet.reSendPhone,msgBuf->buf);
			}
			_gMsgSet.recallStatus = false;	
		}
	}
	
	return -1;	
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Msg_Buffer_UTF8ToAsc
// 功能描述  : UTF8字符转换为ASC码
// 输入参数  : *MSG,10086,11#
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
typedef __packed struct
{
	char c0;
	char c1;
	char c2;
	char c3;
}tagHex,*tagHexPtr;
static char HexToAsc(char hex)
{
	if(hex >= 'a' &&  hex <= 'f')
		return hex - 'a' +10;
	if(hex >= '0' && hex <= '9')
		return hex - '0';
	if(hex >= 'A' && hex <= 'F')
		return hex - 'A' + 10;
	return 0;
}
static int AscToHex(char c,char* c0,char* c1)
{
	if(c0 == NULL || c1 == NULL)
		return -1;
	*c0 = (c >> 4);

	*c0 += (*c0 > 0x9?('A'-10):'0');
	*c1 = (c & 0xF);
	*c1 += (*c1 > 0x9?('A' -10):'0');

	return 0;
}

static BOOL IsHex(char c)
{
	if(c >= 'a' && c <= 'f')
		return true;
	if(c >= '0' && c <= '9')
		return true;
	if(c >= 'A' && c <= 'F')
		return true;
	return false;

}


int Msg_Buffer_PDUToUTF8(char* buf,size_t size,char* outbuf)
{	
	tagHexPtr p = NULL;
	int retSize = 0;
	uchar high;
	uchar low;
	unsigned short  utf =0;
	if(buf == NULL || outbuf == NULL)
		return -1;
	p = (tagHexPtr)buf;
	
	while(size >=  sizeof(tagHex))
	{
		if(IsHex(p->c0) && IsHex(p->c1) && IsHex(p->c2) && IsHex(p->c3))
		{

			high = 	(HexToAsc(p->c0)<<4) + HexToAsc(p->c1);

			low = (HexToAsc(p->c2)<<4) + HexToAsc(p->c3);
			
			utf = (((WORD)high)<<8) + low;
			if(utf <= 0x007F)
			{
				outbuf[retSize] = low;
				retSize++;	
			}
			else if(utf >0x007F && utf <= 0x07FF)
			{
				high = (utf>>6) | 0xC0;
				low = (low & 0x3F) | 0x80;
				outbuf[retSize] = high;
				retSize++;
				outbuf[retSize] = low;
				retSize++;					
  
			}
			else
			{
				high = (high >> 4) | 0xE0;
				outbuf[retSize] = high;
				retSize++;
				high = 	((utf<<4)>>10) | 0x80;
				low = (low & 0x3F) | 0x80;
				outbuf[retSize] = high;
				retSize++;
				outbuf[retSize] = low;
				retSize++;		 
			}

			size -= sizeof(tagHex);
			p++;
			//如果符合hex代码
		}
		else
		{
			break;	//遇到非hex代码直接退出 
		}
	}
	
	return retSize;	
}

int Msg_Buffer_UTF8ToPDU(char* buf,size_t size,char* outbuf,int MaxSize)
{	
	tagHex p;
	int retSize = 0;
	uchar high;
	uchar low;
	if(buf == NULL || outbuf == NULL)
		return -1;

	while(size > 0 && retSize < MaxSize)
	{
		if((*buf) <= 0x7F)
		{
			AscToHex((*buf),&p.c2,&p.c3);
			p.c0 = '0';
			p.c1 = '0';
			size--;
			buf++;
			//ASCII码
		}
		else if((((*buf) & 0xE0) == 0xC0) && (((*(buf+1)) & 0xC0) == 0x80))
		{
			high = (((*buf) & 0x1F)>>2);
			low =  (((*buf) & 0x3)<< 6) + ((*(buf+1)) & 0x3F);
			AscToHex(high,&p.c0,&p.c1);
			AscToHex(low,&p.c2,&p.c3);
 			buf += 2;
			size -= 2;
		}
		else if((((*buf) & 0xF0) == 0xE0) && (((*(buf+1)) & 0xC0) == 0x80) && (((*(buf+2)) & 0xC0) == 0x80))
		{
			high = (((*buf) & 0xF)<<4) + (((*(buf+1)) & 0x3F) >>2);
			low =  (((*(buf+1)) & 0x3) << 6) + ((*(buf+2)) & 0x3F);
			AscToHex(high,&p.c0,&p.c1);
			AscToHex(low,&p.c2,&p.c3);
			buf += 3;
		 	size-=3;
		}
		else
		{
			break;
		}
		memcpy(outbuf,&p,sizeof(tagHex));
		outbuf += sizeof(tagHex);
		retSize += sizeof(tagHex);
	}
		
	return retSize;	
}
//最大长度12个电话
#define MAX_PHONE_LIST_SIZE	12
#define MAX_PHONE_CELL_SIZE		19
static char _gPhoneList[MAX_PHONE_LIST_SIZE][MAX_PHONE_CELL_SIZE+1] = {0};
static char _gPhoneSize = 0;
static char _gMsgMutBuffer[260] = {0};
static int Msg_Phone_Splite(char* phones,int phoneSize)
{
	char* start = phones;
	char* p = start;
	char index = 0;
	if(phones == NULL)
		return -1;
	while((*p) != '\0' && phoneSize > 0 )
	{
		if(_gPhoneSize < MAX_PHONE_LIST_SIZE)
		{
			if((*p) == ',')
			{
				_gPhoneList[_gPhoneSize][index] = 0;
				_gPhoneSize++;
				
				index = 0;
			}
			else if((*p) >= '0' && (*p) <= '9' && index < MAX_PHONE_CELL_SIZE)
			{
				_gPhoneList[_gPhoneSize][index] = *p;
				index++;
			}
			
		}
		p++;
		phoneSize--;
	}
	if(index > 0)
	{
		_gPhoneList[_gPhoneSize][index] = 0;
		_gPhoneSize++;
	}

	return 0;
}
//static int Msg_Buffer_Hex(uchar* in,size_t size,uchar* out)
//{
//	while(size > 0)
//	{		
//		AscToHex(*in,(char*)out,(char*)(out+1));
//		out +=2; 
//		in++;
//		size--;
//	}
//	return 0;
//}
int Msg_Buffer_Analysis9A(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
{	
	uchar cmd = 0;
	uchar phoneSize = 0;
	char phones[260];
	uchar textSize = 0;
	
	cmd = DaPtr[0];
	if(cmd != 1)
		return -1;
	phoneSize = DaPtr[1];
	//电话最大长度255
	if(phoneSize > 255 )
		return -1;
	memcpy(phones,&DaPtr[2],phoneSize);
	phones[phoneSize] = 0x0;
	Msg_Phone_Splite(phones,phoneSize);
	textSize = DaPtr[2 + phoneSize];
	
	memcpy(_gMsgMutBuffer,&DaPtr[3 + phoneSize],textSize);
	_gMsgMutBuffer[textSize] = 0x0;
	
	Msg_SendStatus_Notice(1);
	return 0;
}
/***************************************************************************** 
 * 将一个字符的Unicode(UCS-2和UCS-4)编码转换成UTF-8编码. 
 * 
 * 参数: 
 *    unic     字符的Unicode编码值 
 *    pOutput  指向输出的用于存储UTF8编码值的缓冲区的指针 
 *    outsize  pOutput缓冲的大小 
 * 
 * 返回值: 
 *    返回转换后的字符的UTF8编码所占的字节数, 如果出错则返回 0 . 
 * 
 * 注意: 
 *     1. UTF8没有字节序问题, 但是Unicode有字节序要求; 
 *        字节序分为大端(Big Endian)和小端(Little Endian)两种; 
 *        在Intel处理器中采用小端法表示, 在此采用小端法表示. (低地址存低位) 
 *     2. 请保证 pOutput 缓冲区有最少有 6 字节的空间大小! 
 ****************************************************************************/  

int enc_unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput,int outSize)  
{  
    if(pOutput == NULL)
	{
		return 0;
	}		  
    if(outSize < 6)
	{
		return 0;
	}  
    if ( unic <= 0x0000007F )  
    {  
        // * U-00000000 - U-0000007F:  0xxxxxxx  
        *pOutput     = (unic & 0x7F);  
        return 1;  
    }  
    else if ( unic >= 0x00000080 && unic <= 0x000007FF )  
    {  
        // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx  
        *(pOutput+1) = (unic & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 6) & 0x1F) | 0xC0;  
        return 2;  
    }  
    else if ( unic >= 0x00000800 && unic <= 0x0000FFFF )  
    {  
        // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx  
        *(pOutput+2) = (unic & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >>  6) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 12) & 0x0F) | 0xE0;  
        return 3;  
    }  
    else if ( unic >= 0x00010000 && unic <= 0x001FFFFF )  
    {  
        // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx  
        *(pOutput+3) = (unic & 0x3F) | 0x80;  
        *(pOutput+2) = ((unic >>  6) & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >> 12) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 18) & 0x07) | 0xF0;  
        return 4;  
    }  
    else if ( unic >= 0x00200000 && unic <= 0x03FFFFFF )  
    {  
        // * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
        *(pOutput+4) = (unic & 0x3F) | 0x80;  
        *(pOutput+3) = ((unic >>  6) & 0x3F) | 0x80;  
        *(pOutput+2) = ((unic >> 12) & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >> 18) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 24) & 0x03) | 0xF8;  
        return 5;  
    }  
    else if ( unic >= 0x04000000 && unic <= 0x7FFFFFFF )  
    {  
        // * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx  
        *(pOutput+5) = (unic & 0x3F) | 0x80;  
        *(pOutput+4) = ((unic >>  6) & 0x3F) | 0x80;  
        *(pOutput+3) = ((unic >> 12) & 0x3F) | 0x80;  
        *(pOutput+2) = ((unic >> 18) & 0x3F) | 0x80;  
        *(pOutput+1) = ((unic >> 24) & 0x3F) | 0x80;  
        *pOutput     = ((unic >> 30) & 0x01) | 0xFC;  
        return 6;  
    }  
  
    return 0;  
}  
int Msg_Buffer_Analysis99(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
{	
	uchar cmd = 0;
	uchar phoneSize = 0;
	char phones[260];
	uchar text[520];
	uchar textSize = 0;
	ulong LongData;	
	int i = 0,textp = 0;	
	cmd = DaPtr[0];
	if(cmd != 1)
		return -1;
	phoneSize =0x0B;
	//电话最大长度255
	if(phoneSize > 255 )
		return -1;
	memcpy(phones,&DaPtr[1],phoneSize);
	phones[phoneSize] = 0x0;
	Msg_Phone_Splite(phones,phoneSize);		
	textSize = DaPtr[1 + phoneSize];
	while(textSize>0)
	{
		LongData = DaPtr[2 + phoneSize + i]<<8;
		i++;
		LongData += DaPtr[2 + phoneSize + i];
		i++;		
		textp += enc_unicode_to_utf8_one(LongData,&text[textp],520 - textp);
		textSize--;
	}
	memcpy(_gMsgMutBuffer,text,textp);
	_gMsgMutBuffer[textp] = 0x0;
	
	Msg_SendStatus_Notice(1);
	return 0;
}
//int Msg_Buffer_Analysis99(USART_FUNCTION uf,uchar *DaPtr,uint16 len)
//{	
//	uchar cmd = 0;
//	uchar phoneSize = 0;
//	char phones[260];
//	uchar textSize = 0;
//	uchar text[520];
//	cmd = DaPtr[0];
//	if(cmd != 1)
//		return -1;

//	memcpy(_gPhoneList[0],&DaPtr[1],11);
//	_gPhoneList[0][11] = 0x0;	

//	_gPhoneSize = 1;
//	
//	textSize = DaPtr[12];
//	Msg_Buffer_Hex(&DaPtr[13],textSize*2,text);
//	text[textSize*2] = '\0';
//	memset(_gMsgMutBuffer,NULL,260);	
//	Msg_Buffer_PDUToUTF8(text,textSize*4,_gMsgMutBuffer);

//	//memcpy(_gMsgMutBuffer,&DaPtr[13],textSize);
//	//_gMsgMutBuffer[textSize] = 0x0;
//	
//	Msg_SendStatus_Notice(1);
//	return 0;
//}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Receive_Warn(void)
// 功能描述  : 开关量状态查询
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
#if CONF_WARN_SEND_EN
void Receive_Warn()
{
	static tagAlarmPhoneStruct AlarmPhone = {0};
	char buf[512] = {0};
	const unsigned long minCount = 4;
	static unsigned long maxCount = 360*24;
	static unsigned long curCount = minCount;
	static unsigned long curMaxCount = minCount;
	AtCmdAnalysis_AlarmPhone_Get(&AlarmPhone);

	if(AlarmPhone.Warn_Function_Enable==1)
	{
		API_SetTimer(10000,-1,TIMER_TYPE_WARN,API_Receive_Warn,NULL);	
		if(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)==0)
		{
			if(curCount < curMaxCount)
			{
				curCount++; 
			}
			else
			{
				curCount = minCount;
				curMaxCount = curMaxCount > maxCount?maxCount:(curMaxCount<<2);			
				strncpy(buf,AlarmPhone.phonelNum1,MAX_PHONE_CELL_SIZE);
				strcat(buf,",");
				strncat(buf,AlarmPhone.phonelNum2,MAX_PHONE_CELL_SIZE);
				strcat(buf,",");
				strncat(buf,AlarmPhone.phonelNum3,MAX_PHONE_CELL_SIZE);
				strcat(buf,",");
				strncat(buf,AlarmPhone.phonelNum4,MAX_PHONE_CELL_SIZE);
				strcat(buf,",");
				strncat(buf,AlarmPhone.phonelNum5,MAX_PHONE_CELL_SIZE);
				strcat(buf,",");
				strncat(buf,AlarmPhone.phonelNum6,MAX_PHONE_CELL_SIZE);
				Msg_Phone_Splite(buf,strlen(buf));
				memcpy(_gMsgMutBuffer,AlarmPhone.Warning_type,strlen(AlarmPhone.Warning_type));								
				_gMsgMutBuffer[strlen(AlarmPhone.Warning_type)] = 0x0;
				Msg_SendStatus_Notice(1);						
			}
		}				
		else
		{
			curCount = minCount;
			curMaxCount = minCount;
		}		
	
	}
}
#endif
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : TIMER_FUN(MsgList_Send)
// 功能描述  : 群发短信
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

TIMER_FUN(MsgList_Send)
{
	if(_gPhoneSize > 0)
	{
		char phoneNum[90] = {0};
		int retSize = 0;
		_gPhoneSize--;			
		if(strlen(_gPhoneList[_gPhoneSize])>3)
		{
			phoneNum[0] = '\"';
			retSize = Msg_Buffer_UTF8ToPDU(_gPhoneList[_gPhoneSize],strlen(_gPhoneList[_gPhoneSize]),&phoneNum[1],80);
			if(retSize > 0)
			{
				LINE_LOG();
				phoneNum[retSize+1] = '\"';
				AtCmdAnalysis_Msg_Send(phoneNum,_gMsgMutBuffer);
			}
		}
		else
		{
			Msg_SendStatus_Notice(1);
		}
	}
}

int Msg_SendStatus_Notice(int status)
{
	if(status == 1)
	{
		API_SetTimer(20,1,TIMER_TYPE_WCDMA_MUT_MSG,NULL,TIMER_NAME(MsgList_Send));
	}
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AtCmdAnalysis_Receive_Warn_Delay
// 功能描述  : 延迟查询
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
#if CONF_WARN_SEND_EN
int AtCmdAnalysis_Receive_Warn_Delay(void)
{
	LINE_LOG();
	return 	API_SetTimer(13000,1,TIMER_TYPE_WARN,API_Receive_Warn,NULL);
}	
#endif		
//static void SystemReStart(void)
//{
//	NVIC_SystemReset();	
//}


/******************************************************************************/

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : MSGPROCESS_FUN(MODE)
// 功能描述  : 设置3G版本号
// 输入参数  : *MODE,R97#    
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
MSGPROCESS_FUN(MODE)
{
	char* phoneNum = NULL;
	char buf[100] = {0};
	int index = -1;

	if(argc < 2)
	{
		return -1;
	}
	phoneNum = argv[0];
	if(StrNcmp((uchar*)argv[1],"R97") == 0)
	{
		index = 0;
	}
	else if(StrNcmp((uchar*)argv[1],"R99") == 0)
	{
		index = 1;
	}
	if(index < 0)
	{
		sprintf(buf,"%s:%s\n",argv[1],STRING_FAILED);
	
	}
	else
	{
		AtCmdAnslysis_3GNetVersion_Set(index);
		
		sprintf(buf,"%s:%s\n",argv[1],STRING_SUCCESS);
		
	}
	AtCmdAnalysis_Msg_Send(phoneNum,buf);
	
	return 0;

}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : MSGPROCESS_FUN(APN)
// 功能描述  : 设置APN
// 输入参数  : *APN,0,UNINET#   *APN,<网络模式索引>,<APN名称>#    
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
MSGPROCESS_FUN(APN)
{
	char* phoneNum = NULL;
	char buf[100] = {0};
	tagCnsModSet* mod;
	int size = 0;
	int index = 0;
	if(argc < 3)
	{
		return -1;
	}
	phoneNum = argv[0];
	index = atoi(argv[1]);	
	AtCmdAnalysis_CnsMod_Get(&mod, &size);
	if(index >=0 && index < size)
	{
		memcpy(mod[index].apn,argv[2],ReadStrLen(argv[2])+1);
		AtCmdAnalysis_CnsMod_EERom_Story();
		sprintf(buf,"%d:%s"STRING_SUCCESS"\n",index,mod[index].apn);
	}
	else
	{
		sprintf(buf,"[%d]:"STRING_FAILED"\n",index);
	}
	AtCmdAnalysis_Msg_Send(phoneNum,buf);
	
	return 0;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : MSGPROCESS_FUN(SYS)
// 功能描述  : 开启后台程序
// 输入参数  : *SYS,0,192.168.1.1,8383,0,60000#   *SYS,<开启>,[IP地址],[端口号],[映射口],[超时]#    
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

MSGPROCESS_FUN(SYS)
{
	char* phoneNum = NULL;
	tagModuleSetValStruct moduleSetVal;
	char buf[100] = {0};
	char funIndex;
	tagSystemSetValStruct* sys = NULL;
	char CID[25] ="Error";

	if(argc < 1)
	{
		return -1;
	}
	phoneNum = argv[0];
	sys = AtCmdAnalysis_SystemSetValCtrl_Get();
	
	if(argc < 2)
		return -1;

	phoneNum = argv[0];	  		//取出电话号码

	if(AtCmdAnalysis_ModuleSetVal_Get(sys->sysSetValCtrl.mapDeviceID,&moduleSetVal) > -1)
	{
		sprintf(CID,"%s",moduleSetVal.CID);	
	}
		
	funIndex = *argv[1];

	if(funIndex == '0')	//关闭
	{
		//sys->sysMountMapStatus = SYSTEM_MOUNT_MAP_STATUS_OFF;
		//API_KillTimer(TIMER_TYPE_WCDMA_SYSTEM_MAP);
		API_SetTimer(1000,1,TIMER_TYPE_WCDMA_SYSTEM_MAP,TIMER_NAME(SystemMap),NULL);

		//API_SetTimer(sys->sysSetValCtrl.timeOut * 1000,1,TIMER_TYPE_WCDMA_SYSTEM_MAP,TIMER_NAME(SystemMap),NULL);
		sprintf(buf,"R:%s",STRING_STOP_SYSTEM_CTRL);
		AtCmdAnalysis_Msg_Send(phoneNum,buf);	
	}
	else if(funIndex == '1')	//开启
	{
		if(argc > 2)
		{
			memcpy(sys->sysSetValCtrl.IPAddress,argv[2],ReadStrLen(argv[2]) +1);
		}
		if(argc > 3)
		{
			sys->sysSetValCtrl.Port = atoi(argv[3]);
		}
		if(argc > 4)
		{
			sys->sysSetValCtrl.mapDeviceID = atoi(argv[4]);
		}
		if(argc > 5)
		{
			sys->sysSetValCtrl.timeOut = atoi(argv[5]);	//秒为单位
		}
		sys->sysMountMapStatus = SYSTEM_MOUNT_MAP_STATUS_ON;

		API_SetTimer(sys->sysSetValCtrl.timeOut * 1000,1,TIMER_TYPE_WCDMA_SYSTEM_MAP,TIMER_NAME(SystemMap),NULL);
		TIMER_RUN(DeviceBeat);		
		sprintf(buf,"%s\n"
					STRING_SERVER":%s:%d\n"
					STRING_MAP_PORT":%d:[%s]\n"
					STRING_TIME_OUT":%d"STRING_SECOND"\n"		
					,STRING_ENABLE
					,sys->sysSetValCtrl.IPAddress,sys->sysSetValCtrl.Port
					,sys->sysSetValCtrl.mapDeviceID,CID
					,sys->sysSetValCtrl.timeOut
				);

		AtCmdAnalysis_Msg_Send(phoneNum,buf);
	}
	else if(funIndex == 'v' || funIndex == 'V')
	{
		sprintf(buf,STRING_ISENABLE":%s %s\n"
					STRING_SERVER":%s:%d\n"
					STRING_MAP_PORT":%d:[%s]\n"
					STRING_TIME_OUT":%d"STRING_SECOND"\n"
					,(sys->sysMountMapStatus == SYSTEM_MOUNT_MAP_STATUS_ON?STRING_ENABLE:STRING_NOT_ENABLE)
					,(sys->NetIsOpen == MAX_NET_IS_OPEN_NUM?STRING_CONNECTTED:STRING_UNCONNECT)
					,sys->sysSetValCtrl.IPAddress,sys->sysSetValCtrl.Port
					,sys->sysSetValCtrl.mapDeviceID,CID
					,sys->sysSetValCtrl.timeOut
				);

		AtCmdAnalysis_Msg_Send(phoneNum,buf);
	}

	return 0;

}

/******************************************************************************/

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : MSGPROCESS_FUN(RESET)
// 功能描述  : 重启程序
// 输入参数  : *RESET
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

MSGPROCESS_FUN(RESET)
{
	char* phoneNum = NULL;

	char buf[100] = {0};

	if(argc < 1)
	{
		return -1;
	}
	phoneNum = argv[0];

	API_SetTimer(10000,1,TIMER_TYPE_SYSTEM_RESTART,NULL,TIMER_NAME(SystemReStart));
	sprintf(buf,"R:%s",STRING_SYSTEM_RESET);


	AtCmdAnalysis_Msg_Send(phoneNum,buf);


	return 0;

}


//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : MSGPROCESS_FUN(VIS)
// 功能描述  : 查看版本号
// 输入参数  : *VIS#
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

MSGPROCESS_FUN(VIS)
{
	char* phoneNum = NULL;
	char* wirelessModuleVision= NULL;
	char buf[100] = {0};
	if(argc < 1)
	{
		return -1;
	}
	phoneNum = argv[0];
	wirelessModuleVision = AtCmdAnalysis_WirelessModuleVision_Get();
	if(wirelessModuleVision == NULL)
		return -1;
	sprintf(buf,STRING_SOFTWARE_VISION":%g\n"
				STRING_HARDWARE_VISION":%g\n"
				STRING_WIRELESS_VISION":%s\n"
				,SOFTWARE_VER
				,HARDWARE_VER
				,wirelessModuleVision
				);
	AtCmdAnalysis_Msg_Send(phoneNum,buf);


	return 0;	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : MSGPROCESS_FUN(MSG)
// 功能描述  : 短信转发
// 输入参数  : *MSG,10086,11#
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

MSGPROCESS_FUN(MSG)
{
	char* phoneNum = NULL;
	char* sendPhone = NULL;
	char* sendContext = NULL;
	int lenSendPhone =0;
	if(argc < 3)
	{
		return -1;
	}
	phoneNum = argv[0];

	sendPhone = argv[1];
	lenSendPhone = strlen(sendPhone);
	if(lenSendPhone < 16)
	{
		char msgtBuf[140] ={0};
		if(Msg_Buffer_UTF8ToPDU(sendPhone,lenSendPhone,msgtBuf,sizeof(msgtBuf)-1) < 0)
			return -1;
		sprintf(_gMsgSet.sendPhoneNum,"\"%s\"",msgtBuf);
		sendContext = argv[2];

		strcpy(_gMsgSet.reSendPhone,phoneNum);
		_gMsgSet.recallStatus = true;

		AtCmdAnalysis_Msg_Send(_gMsgSet.sendPhoneNum,sendContext);
		API_SetTimer(60000,1,TIMER_TYPE_WCDMA_MSG,NULL,Msg_TimerOut);
	}

	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : MSGPROCESS_FUN(IP)
// 功能描述  : IP设置
// 输入参数  : *IP,0,192.168.1.1,8000#
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
MSGPROCESS_FUN(IP)
{
	WORD deviceID = 0;
	char* phoneNum = NULL;
	char buf[100] = "";
	int16 port = 0;	//端口号
	if(argc < 4)
	{
		return -1;
	}
	phoneNum = argv[0];
	deviceID = atoi(argv[1]);
	port = atoi(argv[3]);
	if(AtCmdAnalysis_IP_Set(deviceID,(uchar*)argv[2],ReadStrLen(argv[2]),port) > -1)
	{
		sprintf(buf,"%s\n",STRING_IP_SET_SUCCESS/*"Set Ip Success"*/);
	}
	else
	{
		sprintf(buf,"%s\n",STRING_IP_SET_ERROR/*"Error to set Ip!"*/);
	}
	AtCmdAnalysis_Msg_Send(phoneNum,buf);
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : MSGPROCESS_FUN(CID)
// 功能描述  : CID设置
// 输入参数  : *CID,0,10010#
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
MSGPROCESS_FUN(CID)
{
	WORD deviceID = 0;
	char* phoneNum = NULL;
	char buf[100] = "";

	if(argc < 3)
	{
		return -1;
	}
	phoneNum = argv[0];
	deviceID = atoi(argv[1]);

	if(AtCmdAnalysis_CID_Set(deviceID,(uchar*)argv[2],ReadStrLen(argv[2])) > -1)
	{
		sprintf(buf,"%s\n",STRING_CID_SET_SUCCESS/*"set Phone Num Success"*/);
	}
	else
	{
		sprintf(buf,"%s\n",STRING_CID_SET_ERROR/*"Error to set Phone Num!"*/);
	}
	AtCmdAnalysis_Msg_Send(phoneNum,buf);
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : MSGPROCESS_FUN(PHONE)
// 功能描述  : 电话号码设置
// 输入参数  : *PHONE,0,151000000#
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

MSGPROCESS_FUN(PHONE)
{
	WORD deviceID = 0;
	char* phoneNum = NULL;
	char buf[100] = "";

	if(argc < 3)
	{
		return -1;
	}
	phoneNum = argv[0];
	deviceID = atoi(argv[1]);

	if(AtCmdAnalysis_PhoneNum_Set(deviceID,(uchar*)argv[2],ReadStrLen(argv[2])) > -1)
	{
		sprintf(buf,"%s\n",STRING_PHONE_SET_SUCCESS/*"Set Phone Num Success"*/);
	}
	else
	{
		sprintf(buf,"%s\n",STRING_PHONE_SET_ERROR/*"Error to set Phone Num!"*/);
	}
	AtCmdAnalysis_Msg_Send(phoneNum,buf);
	return 0;	
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : MSGPROCESS_FUN(BEATTIM)
// 功能描述  : 心跳设置
// 输入参数  : *BEATTIM,1#
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

MSGPROCESS_FUN(BEATTIM)
{
	DWORD beatTime = 0;
	char* phoneNum = NULL;
	char buf[100] = "";

	if(argc < 2)
	{
		return -1;
	}
	phoneNum = argv[0];
	beatTime = atoi(argv[1]);
	
	if(AtCmdAnalysis_BeatTime_Set(beatTime,true) > -1)
	{
		sprintf(buf,"%s\n",STRING_BEATTIME_SET_SUCCESS/*"set Beat Time Success"*/); 	
	}
	else
	{
		sprintf(buf,"%s\n",STRING_BEATTIME_SET_ERROR/*"Error to set Beat Time!"*/);
	}
	AtCmdAnalysis_Msg_Send(phoneNum,buf);

	return 0;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : MSGPROCESS_FUN(DISPCOM)
// 功能描述  : 下发设置
// 输入参数  : *DISPCOM,1,1#
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

MSGPROCESS_FUN(DISPCOM)	//废弃
{
	DEBUG("%s==>OK\n",__FUNCTION__);
	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : MSGPROCESS_FUN(PORTON)
// 功能描述  : 采集口使能
// 输入参数  : *PORTON,0,1#
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

MSGPROCESS_FUN(PORTON)
{
	WORD deviceID = 0;
	uchar deviceFun = 0;
	char* phoneNum = NULL;
	char buf[100] = "";

	if(argc < 2)
	{
		return -1;
	}
	phoneNum = argv[0];
	deviceID = atoi(argv[1]);
	deviceFun = atoi(argv[2]);
	if(AtCmdAnalysis_Device_Fun(deviceID,deviceFun) > -1)
	{
		sprintf(buf,"%s\n",STRING_PORTON_SUCCESS/*"Enable Port Success"*/); 	
	}
	else
	{
		sprintf(buf,"%s\n",STRING_PORTON_ERROR/*"Error To Enable Port!"*/);
	}
	AtCmdAnalysis_Msg_Send(phoneNum,buf);

	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : MSGPROCESS_FUN(SETCNMP)
// 功能描述  : 设置网络优先级
// 输入参数  : *SETCNMP,0#
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

MSGPROCESS_FUN(SETCNMP)
{
	WORD cnmp = false;

	if(argc < 2)
	{
		return -1;
	}
	cnmp = atoi(argv[1]);

	return AtCmdAnalysis_CNMP_Set(cnmp);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : MSGPROCESS_FUN(STATUS)
// 功能描述  : 查询状态
// 输入参数  : *STATUS,0#
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

MSGPROCESS_FUN(STATUS)
{
	char* phoneNum = NULL;
	char buf[255] = "";
	WORD deviceID = 0;
	tagModeRunStateStruct runStatus;
	tagModuleSetValStruct moduleSetVal;
	
	if(argc < 2)
		return -1;

	phoneNum = argv[0];	  		//取出电话号码
 	deviceID = atoi(argv[1]);	//取出设备号

	if(AtCmdAnalysis_ModeRunState_Get(&runStatus) > -1)
	{
		if(AtCmdAnalysis_ModuleSetVal_Get(deviceID,&moduleSetVal) > -1)
		{
			char* CnsMod = NULL;
			char* isOpen = NULL;
			char* isEnable = NULL;
			char* apn	= "No Apn";
			tagCnsModSet* mod;
			int size = 0;
			
			CnsMod = STRING_NO_SERVICE;
			/*
				
	int index = 0;
	if(argc < 3)
	{
		return -1;
	}
	phoneNum = argv[0];
	index = atoi(argv[1]);	
	
			*/
			if(_gCnsMod != NULL)
			{
				int index = 0;
				for(index = 0; index < _gCnsModSize; index++)
				{
					if(_gCnsMod[index].index == runStatus.CnsMod)
					{
						CnsMod = _gCnsMod[index].CnsModStr;
						break;
					}
				}
			}
			AtCmdAnalysis_CnsMod_Get(&mod, &size);
			if(runStatus.CnsMod >= 0 && runStatus.CnsMod< size)
			{
				apn = mod[runStatus.CnsMod].apn;
			}
//			switch(runStatus.CnsMod)
//			{
//				case 0://no service 
//					//CnsMod = "no service";
//					CnsMod = STRING_NO_SERVICE;
//					break;
//				case 1://GSM
//					CnsMod = "GSM";
//					break;
//				case 2://GPRS
//					CnsMod = "GPRS";
//					break;
//				case 3://EGPRS (EDGE)
//				//	CnsMod = "EGPRS (EDGE)";
//					CnsMod = "EGPRS";
//					break;
//				case 4://WCDMA 
//					CnsMod = "WCDMA";
//					break;
//				case 5://HSDPA only
//				//	CnsMod = "HSDPA only";
//					CnsMod = STRING_ONLY"HSDPA";
//					break;
//				case 6://HSUPA only 
//					//CnsMod = "HSUPA only";
//					CnsMod = STRING_ONLY"HSUPA";
//					break;
//				case 7://HSPA (HSDPA and HSUPA) 
//					//CnsMod = "HSPA (HSDPA and HSUPA)";
//					CnsMod = "HSPA";
//					break;
//				default:
//					//CnsMod = "no service";
//					CnsMod = STRING_NO_SERVICE;
//			}
			if(runStatus.NetIsOpen[deviceID] == MAX_NET_IS_OPEN_NUM)
			{
				isOpen = STRING_CONNECTTED;//"Opened";
			}
			else
			{
				isOpen = STRING_UNCONNECT;//"Not Open";
			}
			if(moduleSetVal.deviceFun == 1)
			{
				isEnable = STRING_ENABLE;//"Enabled";
			}
			else
			{
				isEnable = STRING_NOT_ENABLE;//"Not Enable";
			}
		   	sprintf(buf,
			 	"%d)%s\n"			//设备号
				STRING_CSQ":%d,%s,%s\n"
				STRING_SERVER":%s:%d\n"
				STRING_ISENABLE":%s,%s,"STRING_BEATTIME"%d"STRING_MINUTE,
				deviceID,moduleSetVal.CID,
				runStatus.CSQ,CnsMod,apn,
				moduleSetVal.IPAddress,	moduleSetVal.Port,
				isEnable,isOpen,moduleSetVal.beatTime
				);		//

			AtCmdAnalysis_Msg_Send(phoneNum,buf);
		}	
	}

	return -1;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : MSGPROCESS_FUN(ERROR)
// 功能描述  : 读取错误记录
// 输入参数  : *ERROR,0#
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

MSGPROCESS_FUN(ERROR)
{
	char buf[256] = {0};
	char* phoneNum = NULL;

	tagErrorEERomContext menu;

	if(argc < 2)
		return -1;
	if(API_EEPROM_Read(_EEP_ADDR_ERROR_RECORD_CONTEXT, sizeof(tagErrorEERomContext), (uchar*)(&menu), true) < 0)
	{
		memset(&menu,NULL,sizeof(menu)); 
	}
	phoneNum = argv[0];	  		//取出电话号码
	if(*argv[1] == 'a')  //列出所有的错误信息
	{
		if(menu.size == 0)
		{
			sprintf(buf,STRING_NO_ERROR);
		}
		else
		{
			sprintf(buf,STRING_HAVE_ERROR":%d",menu.size);
		}
	}
	else if(*argv[1] == 'r')
	{
		menu.size = 0;
		menu.writePointer = 0;
		API_EEPROM_Write(_EEP_ADDR_ERROR_RECORD_CONTEXT, sizeof(tagErrorEERomContext), (uchar*)(&menu), true);
		sprintf(buf,STRING_REMOVE_ALL);
	}
	else
	{
		int index = atoi(argv[1]);
		if(index < menu.size)
		{
			index = index + menu.writePointer;
			if(index >= menu.size)
			{
				index = index - menu.size;
			}
			DEBUG("Read error Address:%d",_EEP_ADDR_ERROR_RECORD + index*256);
			if(API_EEPROM_Read(_EEP_ADDR_ERROR_RECORD + index*256,200,(uchar*)buf,true) < 0)
			{
				 sprintf(buf,STRING_READ_ADDRESS);
			}
		}
		else
		{
			sprintf(buf,STRING_WRONG_INDEX);
		}
	}
	AtCmdAnalysis_Msg_Send(phoneNum,buf);	

	return 0;
}

/******************************************************************************/

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Msg_BufferQueue_Process
// 功能描述  : 主线程内短信处理
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int Msg_BufferQueue_Process(void)
{
	QueuePtr qPtr = NULL;
	while(( qPtr= DeQueue(PResFullQueuePtr)) != NULL)
	{
		Msg_Buffer_Process(qPtr->data);
		EnQueue(PResFreeQueuePtr,qPtr);	//处理完毕后，加入到空闲的队列中去
	}	

	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Msg_Buffer_Receive
// 功能描述  : 中断内短信内容解析
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int Msg_Buffer_Receive(char* phoneNum,uchar c)
{
	if(_gMsgQueueBufferNodePtr == NULL)
	{
		if((_gMsgQueueBufferNodePtr = DeQueue(PResFreeQueuePtr)) == NULL)
			return -1;
		_gMsgQueueBufferPtr->count = 0;
	}
	
	_gMsgQueueBufferPtr->buf[_gMsgQueueBufferPtr->count] = c;
	_gMsgQueueBufferPtr->count++;

	if(_gMsgQueueBufferPtr->count >= 2)
	{
		if((_gMsgQueueBufferPtr->buf[_gMsgQueueBufferPtr->count-2] == 0x0d
			&& _gMsgQueueBufferPtr->buf[_gMsgQueueBufferPtr->count-1] == 0x0a)
			||_gMsgQueueBufferPtr->count >= MAX_MSG_BUFFER_SIZE )
		{
			strcpy(_gMsgQueueBufferPtr->phoneNum,phoneNum);
			//DEBUG("receive %s msg:%s\n",_gMsgQueueBufferPtr->phoneNum,_gMsgQueueBufferPtr->buf);
			
			//达到极限值或者收到结束符就将短信缓冲发到待解析队列中去
			if(EnQueue(PResFullQueuePtr,_gMsgQueueBufferNodePtr) < 0)
				return -1;
			_gMsgQueueBufferNodePtr = NULL;
			return -1; 
		}
	}

	return 0;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Msg_CnsModInit
// 功能描述  : 信号信息解析
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
int Msg_CnsModInit(const tagCnsModSet* mod, int size)
{
	_gCnsMod = mod;
	_gCnsModSize = size;
	
	return 0;
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Msg_Init
// 功能描述  : 短信解析模块初始化
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※

int Msg_Init(void)
{
//	char* testStr = (char*)TESTString;//"00430067006700688C376B4C"; 
//	char outbuf[100] = {0};
//	char* tStr = "Cggh谷歌";
//	int i = 0;
//
//	//gsmBytes2String(testStr,outbuf,sizeof(testStr));
//	//Msg_Buffer_PDUToUTF8(testStr,strlen(testStr),outbuf) ;
//	Msg_Buffer_UTF8ToPDU(testStr,strlen(testStr),outbuf);
//	DEBUG("\n==>%s\n",outbuf);
//	DEBUG("tStr:%s \n",tStr);
//	for(i=0;i<strlen(tStr);i++)
//	{
//		DEBUG("%x ",tStr[i]);
//	}
//	DEBUG("\n");


	return MsgBuffer_Queue_Init();
}

#endif
