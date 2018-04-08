#include "config.h"

//������������������������������������������������������������������������������
// ��������  : asc2hex
// ��������  : ��ascii ��'0'~'f' ת���� 0~f һ��byte
// �������  : None.
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
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

//������������������������������������������������������������������������������
// ��������  : AscTOHex
// ��������  : ������ASC��ϳ�һ��HEX�ַ�
// �������  : None.
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
uchar AscTOHex(uchar TPA,uchar TPB)   
{	
    uchar c1,c2;
	c1=asc2hex(TPA);
	c2=asc2hex(TPB);
	return((c1<<4)|c2);
}

//������������������������������������������������������������������������������
// ��������  : AscToHexFun
// ��������  : ��ASC�뻺��ת����HEX������
// �������  : None.
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
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

//������������������������������������������������������������������������������
// ��������  : TwocharToInt
// ��������  : ������char�ͺϳ�һ��int
// �������  : None.
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
unsigned int TwocharToInt(unsigned char c1,unsigned char c2)
{	
    unsigned int tempcmd;
	tempcmd=c1;
	tempcmd<<=8;
	tempcmd|=c2;
	return tempcmd;
}

//������������������������������������������������������������������������������
// ��������  : twoascii2byte
// ��������  : 
// �������  : None.
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
uchar twoascii2byte(uchar c1,uchar c2)
{  	
    uchar ReturnData;
	c1=asc2hex(c1);
	c2=asc2hex(c2);
	ReturnData=(c1<<4)|c2;
	return ReturnData;
}

//������������������������������������������������������������������������������
// ��������  : AscWord2Byte
// ��������  : ��ָ��2���ֽڵ�ASCII������ת��ΪHEX�� , ����PC��ͨ�ŵĳ�����ʹ��, ��'1','3'=="13"  ==>0x13
// �������  : None.
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
uchar AscWord2Byte(uint *p)
{	
    uchar c1,c2;
	c1=asc2hex(*p>>8);
	c2=asc2hex(*p);
	return((c2<<4)|c1);	//����Ϻ�C51�෴
}

//������������������������������������������������������������������������������
// ��������  : 
// ��������  : "9"��ASCII����"A"��ASCII��ֵ���==7  ע����ĸ��ͳһΪ��д��ĸ��ASCII��ֵ 
// �������  : None.
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
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

//������������������������������������������������������������������������������
// ��������  : Delay_us
// ��������  : ��ʱ
// �������  : None.
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
void Delay_1us(void)
{	
    u8 i;
	for(i=0;i<9;i++)
	{}
}
//������������������������������������������������������������������������������
// ��������  : Delay_2us
// ��������  : ��ʱ
// �������  : None.
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
void Delay_2us(void)
{	
    u8 i;
	for(i=0;i<18;i++)
	{}
}

//������������������������������������������������������������������������������
// ��������  : Delay_10_uS
// ��������  : ��ʱ
// �������  : None.
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
void  CS_Delay_10_uS(void)
{
	Delay_2us();
	Delay_2us();
	Delay_2us();
	Delay_2us();
	Delay_2us();
}
//������������������������������������������������������������������������������
// ��������  : Delay_100_uS
// ��������  : ��ʱ
// �������  : None.
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
void  Delay_uS(u16 delay_max)
{
	while(delay_max--)
    {

		  	Delay_1us();
    }

}
//������������������������������������������������������������������������������
// ��������  : Delay_N_mS
// ��������  : ��ʱms
// �������  : None.
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
void Delay_MS( uint n_milisecond) 
{
	uchar i;
	while(n_milisecond--)
    {
		i=100;
		API_WatchDog_Feed();  	// ι���Ź�
        while(i--)
		{
		  	CS_Delay_10_uS();
		}
    }
}

//������������������������������������������������������������������������������
// ��������  : CopyMemory
// ��������  : ���ݿ���
// �������  : None.
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
void CopyMemory(uchar *pObject,uchar *pSource,uchar cnt)
{   
    uchar i;
	for(i=0;i<cnt;i++)
		*pObject++=*pSource++;
}

//������������������������������������������������������������������������������
// ��������  : chk_post_chksum
// ��������  : �ʵ粿Э��������ݵ�У����㷨
// �������  : p1 ҪУ������� ReDaLen ���ݳ���.
// �������  : None
// ���ز���  : 0�ɹ� 1ʧ��
//������������������������������������������������������������������������������
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

//������������������������������������������������������������������������������
// ��������  : chk_LcdMode_chksum
// ��������  : �����������ݵ�У����㷨
// �������  : p1 ҪУ������� ReDaLen ���ݳ���.
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
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

//������������������������������������������������������������������������������
// ��������  : InvertedOrder
// ��������  : �ֽڵ�������
// �������  : None
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
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

//������������������������������������������������������������������������������
// ��������  : InitBufDaFun
// ��������  : ��ʼ������������
// �������  : TpSA������ָ��  TLenA�����ݳ��� 	Ta����ʼֵ
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
void  InitBufDaFun(uchar *TpSA,uint TLenA,uchar Ta)    
{	
    uint i;
	for(i=0;i<TLenA;i++)
	{	
	    *TpSA++ = Ta;		
	}
}

//������������������������������������������������������������������������������
// ��������  : ReadStrLen
// ��������  : ��ȡ�ַ�������
// �������  : None
// �������  : None
// ���ز���  : None
//������������������������������������������������������������������������������
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

//������������������������������������������������������������������������������
//                          �������(END)
//������������������������������������������������������������������������������





