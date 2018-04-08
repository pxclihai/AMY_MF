#ifndef	_BASICFUNC_H
#define _BASICFUNC_H
 
#ifdef __cplusplus
extern "C" {
#endif

uchar asc2hex(uchar c1);
uchar AscTOHex(uchar TPA,uchar TPB);
void  AscToHexFun(uchar *PTemp,uint Len); 
uchar twoascii2byte(uchar c1,uchar c2);
uchar AscWord2Byte(uint *p);
uchar hi_hex2asc(uchar c1);
uchar low_hex2asc(uchar c1);
void Delay_2us(void);
void CS_Delay_10_uS(void);
void Delay_MS( uint n_milisecond);
	
void  Delay_uS(u16 delay_max);
	
void CopyMemory(uchar *pObject,uchar *pSource,uchar cnt);
uchar chk_post_chksum(uchar  *p1,uint  ReDaLen);
uchar chk_LcdMode_chksum(uchar *p1,uint ReDaLen);
unsigned int TwocharToInt(unsigned char c1,unsigned char c2);
void InvertedOrder(uchar *p,uint count);
void InitBufDaFun(uchar *TpSA,uint TLenA,uchar Ta);
uint16 ReadStrLen(char *TempChar);


#ifdef __cplusplus
}
#endif

#endif
