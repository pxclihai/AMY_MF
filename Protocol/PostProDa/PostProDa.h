#ifndef _POSTPRODA_H
#define _POSTPRODA_H

/********************************************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif
/********************************************************************************************************/
typedef __packed struct
{	uint lLenth:4;
	uint mLenth:4;
	uint hLenth:4;
	uint lChksum:4;
}Packed_data;
/********************************************************************************************************/
typedef __packed union
{	uint  word;
	Packed_data	div;
}Taglchksum;
/********************************************************************************************************/
typedef __packed union
{	uchar byte[4];
	uint16 word[2];
	long  zlong;
	ulong uzlong;
	float zfloat;
}uni_float;
/********************************************************************************************************/
extern uchar RTN;

void PostSendByte(USART_FUNCTION uf,uchar ChDa);
void PostSendBytes(USART_FUNCTION uf,uchar *pchar,uchar count);
void PostSendByteASC(USART_FUNCTION uf,uchar ChDa);
void PostSend_Word(USART_FUNCTION uf,uint TemInt);
void PostSend_HexStr(USART_FUNCTION uf,uchar *P,uchar Cnt);
void PostSend_Str(USART_FUNCTION uf,char *PS1);
void PostSendHead(USART_FUNCTION uf);
void PostSendHeadCmd(USART_FUNCTION uf,uchar Addr,uchar cmd);
void PostSendTail(USART_FUNCTION uf);
void PostSendPostChksum(USART_FUNCTION uf);
void PostSendDataLen(USART_FUNCTION uf); 
void PostSendDefault(USART_FUNCTION uf);
void DealPostProtocol(USART_FUNCTION uf,uchar *DaPtr,uint16 len);

/********************************************************************************************************/
#ifdef __cplusplus
} // extern "C"
#endif
/********************************************************************************************************/
#endif
/**************************************end of line ******************************************************/



