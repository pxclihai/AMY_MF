#ifndef _DOWNLOAD_H
#define _DOWNLOAD_H

void DownLoad_SendByte(USART_FUNCTION uf,uchar ChDa);
void DownLoad_SendHead(USART_FUNCTION uf,uchar Addr,uchar cmd);
void DownLoad_SendChksum(USART_FUNCTION uf);
void DownLoad_SendTail(USART_FUNCTION uf);
void DownLoad_DealProtocol(USART_FUNCTION uf,uchar *DaPtr,uint16 len);

#endif


