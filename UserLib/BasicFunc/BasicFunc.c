#include "config.h"

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : asc2hex
// 功能描述  : 把ascii 的'0'~'f' 转换成 0~f 一个byte
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
uchar asc2hex(uchar c1)
{	
    if (c1<0x61 && c1>0x40)
		{c1=c1-0x37;}
	else if (c1<0x41)
		{c1=c1-0x30;}
	else if (c1>0x60)
		{c1=c1-0x57;}
	return(c1);			
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AscTOHex
// 功能描述  : 把两个ASC码合成一个HEX字符
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
uchar AscTOHex(uchar TPA,uchar TPB)   
{	
    uchar c1,c2;
	c1=asc2hex(TPA);
	c2=asc2hex(TPB);
	return((c1<<4)|c2);
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AscToHexFun
// 功能描述  : 把ASC码缓冲转换成HEX缓冲区
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void  AscToHexFun(uchar *PTemp,uint Len)  
{	
    uchar  Ta,*PsouA;
	uint   i;
	PsouA = PTemp;
	Len >>=1;
	for(i=0; i<Len; i++)
	{	Ta = *PTemp++;
		*PsouA++ = AscTOHex(Ta,*PTemp++);
	}
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : TwocharToInt
// 功能描述  : 将两个char型合成一个int
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
unsigned int TwocharToInt(unsigned char c1,unsigned char c2)
{	
    unsigned int tempcmd;
	tempcmd=c1;
	tempcmd<<=8;
	tempcmd|=c2;
	return tempcmd;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : twoascii2byte
// 功能描述  : 
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
uchar twoascii2byte(uchar c1,uchar c2)
{  	
    uchar ReturnData;
	c1=asc2hex(c1);
	c2=asc2hex(c2);
	ReturnData=(c1<<4)|c2;
	return ReturnData;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : AscWord2Byte
// 功能描述  : 把指向2个字节的ASCII码数，转换为HEX数 , 在与PC机通信的程序中使用, 如'1','3'=="13"  ==>0x13
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
uchar AscWord2Byte(uint *p)
{	
    uchar c1,c2;
	c1=asc2hex(*p>>8);
	c2=asc2hex(*p);
	return((c2<<4)|c1);	//此组合和C51相反
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : 
// 功能描述  : "9"的ASCII码与"A"的ASCII码值相差==7  注：字母都统一为大写字母的ASCII码值 
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
uchar hi_hex2asc(uchar c1)
{	
    c1=c1>>4;
	return(c1>=0xa? (c1+0x37):(c1+0x30));
}	
uchar low_hex2asc(uchar c1)
{	
    c1=c1&0xf;
	return(c1>=0xa? (c1+0x37):(c1+0x30));
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Delay_us
// 功能描述  : 延时
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void Delay_1us(void)
{	
    u8 i;
	for(i=0;i<9;i++)
	{}
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Delay_2us
// 功能描述  : 延时
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void Delay_2us(void)
{	
    u8 i;
	for(i=0;i<18;i++)
	{}
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Delay_10_uS
// 功能描述  : 延时
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void  CS_Delay_10_uS(void)
{
	Delay_2us();
	Delay_2us();
	Delay_2us();
	Delay_2us();
	Delay_2us();
}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Delay_100_uS
// 功能描述  : 延时
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void  Delay_uS(u16 delay_max)
{
	while(delay_max--)
    {

		  	Delay_1us();
    }

}
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : Delay_N_mS
// 功能描述  : 延时ms
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void Delay_MS( uint n_milisecond) 
{
	uchar i;
	while(n_milisecond--)
    {
		i=100;
		API_WatchDog_Feed();  	// 喂看门狗
        while(i--)
		{
		  	CS_Delay_10_uS();
		}
    }
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : CopyMemory
// 功能描述  : 数据拷贝
// 输入参数  : None.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void CopyMemory(uchar *pObject,uchar *pSource,uchar cnt)
{   
    uchar i;
	for(i=0;i<cnt;i++)
		*pObject++=*pSource++;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : chk_post_chksum
// 功能描述  : 邮电部协议接收数据的校验和算法
// 输入参数  : p1 要校验的数据 ReDaLen 数据长度.
// 输出参数  : None
// 返回参数  : 0成功 1失败
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
uchar chk_post_chksum(uchar  *p1,uint  ReDaLen)  
{	
    uint i,csA,csB,Sum=0;
	uchar TA,TB,TeC;

	for(i=0;i<ReDaLen;i++)
	{	Sum += *p1++;
	}
	Sum = ~Sum+1;
	TA = *p1++;
	TB = *p1++;
	TeC=AscTOHex(TA,TB);
	csA = (uint)TeC;
	csA = csA<<8;
	TA = *p1++;
	TB = *p1++;
	TeC=AscTOHex(TA,TB);
	csB =(uint)TeC;
	if((INT16U)Sum==(INT16U)(csA+csB))
	{	
	    return 0;
	}
	else
	{	
	    return 1;
	}
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : chk_LcdMode_chksum
// 功能描述  : 升级程序数据的校验和算法
// 输入参数  : p1 要校验的数据 ReDaLen 数据长度.
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
uchar chk_LcdMode_chksum(uchar *p1,uint ReDaLen)	
{	
    uint i;
	uint16 cs;
	uint16 sum = 0;
	uchar TeA,TeB;
	for(i=0;i<ReDaLen;i++)
	{	sum += *p1++;
	}
	TeA = *p1++;
	TeB = *p1++;
	cs = (uint16)(TeA<<8) + TeB;
	if((uint16)(sum)==cs)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : InvertedOrder
// 功能描述  : 字节倒叙排列
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void InvertedOrder(uchar *p,uint count)
{
    uchar tmp; 
	uint i;
	for(i=0;i<count/2;++i)
	{
		tmp = p[i];
		p[i] = p[count-i-1];
		p[count-i-1] = tmp;
	}
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : InitBufDaFun
// 功能描述  : 初始化缓冲区函数
// 输入参数  : TpSA：数据指针  TLenA：数据长度 	Ta：初始值
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
void  InitBufDaFun(uchar *TpSA,uint TLenA,uchar Ta)    
{	
    uint i;
	for(i=0;i<TLenA;i++)
	{	
	    *TpSA++ = Ta;		
	}
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 函数名称  : ReadStrLen
// 功能描述  : 获取字符串长度
// 输入参数  : None
// 输出参数  : None
// 返回参数  : None
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
uint16 ReadStrLen(char *TempChar)
{
	uint16 n=0;
	while(*TempChar!='\0')
	{	
		TempChar++;
		n++;
	}
	return n;
}

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
//                          程序结束(END)
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※






