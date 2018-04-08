#ifndef _MCU_H
#define _MCU_H
#ifdef __cplusplus
extern "C" {
#endif

/****************** 系统头文件********************/

#include "stm32f10x.h"


//常用变量类型定义
typedef unsigned char   tByte;
typedef unsigned int    tWord;



typedef unsigned char   uchar;
typedef unsigned short  uint;//int有的CPU是32位   unsigned int
typedef unsigned long   ulong;

#ifndef   BOOL
    typedef unsigned char 	BOOL;
#endif

#define	int8            signed char
#define	int16           signed short
#define	int32           signed long
#define	uint8           unsigned char
#define	uint16          unsigned short
#define	uint32          unsigned long

#define true  1
#define false 0

typedef unsigned char   BOOLEAN;
typedef unsigned char   INT8U;      	/* Unsigned  8 bit quantity         */
typedef signed   char   INT8S;      	/* Signed    8 bit quantity         */
typedef unsigned short  INT16U;    	    /* Unsigned 16 bit quantity         */
typedef signed   short  INT16S;    	    /* Signed   16 bit quantity         */
typedef unsigned long   INT32U;         /* Unsigned 32 bit quantity       	*/
typedef signed   long   INT32S;         /* Signed   32 bit quantity       	*/
typedef float           FP32;           /* Single precision floating point  */
typedef double          FP64;      	    /* Double precision floating point  */

#define BYTE            INT8S         	/* Define data types for backward compatibility ...   */
#define UBYTE           INT8U       		/* ... to uC/OS V1.xx.  Not actually needed for ...   */
#define WORD            INT16S      		/* ... uC/OS-II.            		*/
#define UWORD           INT16U
#define LONG            INT32S
#define ULONG           INT32U
#ifndef DWORD
typedef unsigned long	DWORD;
#endif

typedef signed char     S8;
typedef unsigned char   U8;
typedef short           S16;
typedef unsigned short  U16;
typedef int             S32;
typedef unsigned int    U32;
typedef long long       S64;
typedef unsigned long long U64;
typedef unsigned char   BIT;

typedef void (*pFUN)(void);
typedef void (*pFUNpar)(uint8);
typedef void (*pFUNparU8)(uint8);
typedef void (*pFUNparU16)(uint16);

typedef uint8 (*pU8FUN)(void);

typedef unsigned char OS_STK;       /* Each stack entry is 8-bit wide      */
//typedef INT16U             OS_FLAGS;   /* Date type for event flag bits (8, 16 or 32 bits)             */



#define SetBit(Value,Bit)       ( Value |= (0x01<<Bit) )        //Bit(0~7)//一定要用()
#define ClrBit(Value,Bit)       ( Value &= (~(0x01<<Bit)))

#define SetBit32(Value,Bit)     ( Value |= (1UL<<Bit) )         //Bit(0~31)//一定要用()
#define ClrBit32(Value,Bit)     ( Value &= (~(1UL<<Bit)))


#define BIT(n)                  ((uint8)((uint8)1<<n))      //n(0~7)
#define BIT8(n)                 ((uint8)((uint8)1<<n))      //n(0~7)
#define BIT16(n)                ((uint16)((uint16)1<<n))    //n(0~31)
#define BIT32(n)                ((uint32)((uint32)1UL<<n))  //n(0~31) 


//测试某位是否为1，为1返回1，为0返回为0
#define	isBit(dat, n)           ((dat&BIT(n)) ? 1: 0)//0~7
#define isBit8(dat, n)          ((dat&BIT(n)) ? 1: 0)//0~7

//测试某位是否为1，为1返回1，为0返回为0
#define	isBit16(dat, n)         ((dat&BIT16(n)) ? 1: 0)//0~15

#define	isBit32(dat, n)         ((dat&BIT32(n)) ? 1: 0)//0~31

//从16位数据拷贝到8位数据，高字节在前
#define cpyU8fU16(U8dat, U16dat)     \
{                                    \
   *((uint8 *)(U8dat+0))=U16dat>>8;  \
   *((uint8 *)(U8dat+1))=U16dat;     \
}

//从32位数据拷贝到8位数据，高字节在前
#define cpyU8fU32(U8dat, U32dat)              \
{                                             \
   *((uint8 *)U8dat)=(uint8)(U32dat>>24);     \
   *((uint8 *)(U8dat+1))=(uint8)(U32dat>>16); \
   *((uint8 *)(U8dat+2))=(uint8)(U32dat>>8);  \
   *((uint8 *)(U8dat+3))=(uint8)U32dat;       \
}

//从8位数据拷贝到16位数据，高字节在前
#define cpyU16fU8(U16dat, U8dat)     \
{                                    \
   U16dat = *((uint8 *)U8dat);       \
   U16dat <<= 8;                     \
   U16dat += *((uint8 *)(U8dat+1));  \
}

//从8位数据拷贝到32位数据，高字节在前
#define cpyU32fU8(U32dat, U8dat)     \
{                                    \
   U32dat = *((uint8 *)U8dat);       \
   U32dat <<= 8;                     \
   U32dat += *((uint8 *)(U8dat+1));  \
   U32dat <<= 8;                     \
   U32dat += *((uint8 *)(U8dat+2));  \
   U32dat <<= 8;                     \
   U32dat += *((uint8 *)(U8dat+3));  \
}

#define __NOP1__  __asm { nop }
#define __NOP2__ __NOP1__ __NOP1__
#define __NOP4__ __NOP2__ __NOP2__
#define __NOP8__ __NOP4__ __NOP4__
#define __NOP16__ __NOP8__ __NOP8__
#define __NOP32__ __NOP16__ __NOP16__
#define __NOP64__ __NOP32__ __NOP32__
#define __NOP128__ __NOP64__ __NOP64__


#define DelayNop(a)             \
    if ((a)&(0x01))    {__NOP1__}                    \
    if ((a)&(0x02))    {__NOP2__}                    \
    if ((a)&(0x04))    {__NOP4__}                    \
    if ((a)&(0x08))    {__NOP8__}                    \
    if ((a)&(0x10))    {__NOP16__}                    \
    if ((a)&(0x20))    {__NOP32__}                    \
    if ((a)&(0x40))    {__NOP64__}                    \
    if ((a)&(0x80))    {__NOP128__}                    


//二进制常量定义
#define B0000_0000 0x00
#define B0000_0001 0x01
#define B0000_0010 0x02
#define B0000_0011 0x03
#define B0000_0100 0x04
#define B0000_0101 0x05
#define B0000_0110 0x06
#define B0000_0111 0x07
#define B0000_1000 0x08
#define B0000_1001 0x09
#define B0000_1010 0x0A
#define B0000_1011 0x0B
#define B0000_1100 0x0C
#define B0000_1101 0x0D
#define B0000_1110 0x0E
#define B0000_1111 0x0F

#define B0001_0000 0x10
#define B0001_0001 0x11
#define B0001_0010 0x12
#define B0001_0011 0x13
#define B0001_0100 0x14
#define B0001_0101 0x15
#define B0001_0110 0x16
#define B0001_0111 0x17
#define B0001_1000 0x18
#define B0001_1001 0x19
#define B0001_1010 0x1A
#define B0001_1011 0x1B
#define B0001_1100 0x1C
#define B0001_1101 0x1D
#define B0001_1110 0x1E
#define B0001_1111 0x1F

#define B0010_0000 0x20
#define B0010_0001 0x21
#define B0010_0010 0x22
#define B0010_0011 0x23
#define B0010_0100 0x24
#define B0010_0101 0x25
#define B0010_0110 0x26
#define B0010_0111 0x27
#define B0010_1000 0x28
#define B0010_1001 0x29
#define B0010_1010 0x2A
#define B0010_1011 0x2B
#define B0010_1100 0x2C
#define B0010_1101 0x2D
#define B0010_1110 0x2E
#define B0010_1111 0x2F

#define B0011_0000 0x30
#define B0011_0001 0x31
#define B0011_0010 0x32
#define B0011_0011 0x33
#define B0011_0100 0x34
#define B0011_0101 0x35
#define B0011_0110 0x36
#define B0011_0111 0x37
#define B0011_1000 0x38
#define B0011_1001 0x39
#define B0011_1010 0x3A
#define B0011_1011 0x3B
#define B0011_1100 0x3C
#define B0011_1101 0x3D
#define B0011_1110 0x3E
#define B0011_1111 0x3F

#define B0100_0000 0x40
#define B0100_0001 0x41
#define B0100_0010 0x42
#define B0100_0011 0x43
#define B0100_0100 0x44
#define B0100_0101 0x45
#define B0100_0110 0x46
#define B0100_0111 0x47
#define B0100_1000 0x48
#define B0100_1001 0x49
#define B0100_1010 0x4A
#define B0100_1011 0x4B
#define B0100_1100 0x4C
#define B0100_1101 0x4D
#define B0100_1110 0x4E
#define B0100_1111 0x4F

#define B0101_0000 0x50
#define B0101_0001 0x51
#define B0101_0010 0x52
#define B0101_0011 0x53
#define B0101_0100 0x54
#define B0101_0101 0x55
#define B0101_0110 0x56
#define B0101_0111 0x57
#define B0101_1000 0x58
#define B0101_1001 0x59
#define B0101_1010 0x5A
#define B0101_1011 0x5B
#define B0101_1100 0x5C
#define B0101_1101 0x5D
#define B0101_1110 0x5E
#define B0101_1111 0x5F

#define B0110_0000 0x60
#define B0110_0001 0x61
#define B0110_0010 0x62
#define B0110_0011 0x63
#define B0110_0100 0x64
#define B0110_0101 0x65
#define B0110_0110 0x66
#define B0110_0111 0x67
#define B0110_1000 0x68
#define B0110_1001 0x69
#define B0110_1010 0x6A
#define B0110_1011 0x6B
#define B0110_1100 0x6C
#define B0110_1101 0x6D
#define B0110_1110 0x6E
#define B0110_1111 0x6F

#define B0111_0000 0x70
#define B0111_0001 0x71
#define B0111_0010 0x72
#define B0111_0011 0x73
#define B0111_0100 0x74
#define B0111_0101 0x75
#define B0111_0110 0x76
#define B0111_0111 0x77
#define B0111_1000 0x78
#define B0111_1001 0x79
#define B0111_1010 0x7A
#define B0111_1011 0x7B
#define B0111_1100 0x7C
#define B0111_1101 0x7D
#define B0111_1110 0x7E
#define B0111_1111 0x7F

#define B1000_0000 0x80
#define B1000_0001 0x81
#define B1000_0010 0x82
#define B1000_0011 0x83
#define B1000_0100 0x84
#define B1000_0101 0x85
#define B1000_0110 0x86
#define B1000_0111 0x87
#define B1000_1000 0x88
#define B1000_1001 0x89
#define B1000_1010 0x8A
#define B1000_1011 0x8B
#define B1000_1100 0x8C
#define B1000_1101 0x8D
#define B1000_1110 0x8E
#define B1000_1111 0x8F

#define B1001_0000 0x90
#define B1001_0001 0x91
#define B1001_0010 0x92
#define B1001_0011 0x93
#define B1001_0100 0x94
#define B1001_0101 0x95
#define B1001_0110 0x96
#define B1001_0111 0x97
#define B1001_1000 0x98
#define B1001_1001 0x99
#define B1001_1010 0x9A
#define B1001_1011 0x9B
#define B1001_1100 0x9C
#define B1001_1101 0x9D
#define B1001_1110 0x9E
#define B1001_1111 0x9F

#define B1010_0000 0xA0
#define B1010_0001 0xA1
#define B1010_0010 0xA2
#define B1010_0011 0xA3
#define B1010_0100 0xA4
#define B1010_0101 0xA5
#define B1010_0110 0xA6
#define B1010_0111 0xA7
#define B1010_1000 0xA8
#define B1010_1001 0xA9
#define B1010_1010 0xAA
#define B1010_1011 0xAB
#define B1010_1100 0xAC
#define B1010_1101 0xAD
#define B1010_1110 0xAE
#define B1010_1111 0xAF

#define B1011_0000 0xB0
#define B1011_0001 0xB1
#define B1011_0010 0xB2
#define B1011_0011 0xB3
#define B1011_0100 0xB4
#define B1011_0101 0xB5
#define B1011_0110 0xB6
#define B1011_0111 0xB7
#define B1011_1000 0xB8
#define B1011_1001 0xB9
#define B1011_1010 0xBA
#define B1011_1011 0xBB
#define B1011_1100 0xBC
#define B1011_1101 0xBD
#define B1011_1110 0xBE
#define B1011_1111 0xBF

#define B1100_0000 0xC0
#define B1100_0001 0xC1
#define B1100_0010 0xC2
#define B1100_0011 0xC3
#define B1100_0100 0xC4
#define B1100_0101 0xC5
#define B1100_0110 0xC6
#define B1100_0111 0xC7
#define B1100_1000 0xC8
#define B1100_1001 0xC9
#define B1100_1010 0xCA
#define B1100_1011 0xCB
#define B1100_1100 0xCC
#define B1100_1101 0xCD
#define B1100_1110 0xCE
#define B1100_1111 0xCF

#define B1101_0000 0xD0
#define B1101_0001 0xD1
#define B1101_0010 0xD2
#define B1101_0011 0xD3
#define B1101_0100 0xD4
#define B1101_0101 0xD5
#define B1101_0110 0xD6
#define B1101_0111 0xD7
#define B1101_1000 0xD8
#define B1101_1001 0xD9
#define B1101_1010 0xDA
#define B1101_1011 0xDB
#define B1101_1100 0xDC
#define B1101_1101 0xDD
#define B1101_1110 0xDE
#define B1101_1111 0xDF

#define B1110_0000 0xE0
#define B1110_0001 0xE1
#define B1110_0010 0xE2
#define B1110_0011 0xE3
#define B1110_0100 0xE4
#define B1110_0101 0xE5
#define B1110_0110 0xE6
#define B1110_0111 0xE7
#define B1110_1000 0xE8
#define B1110_1001 0xE9
#define B1110_1010 0xEA
#define B1110_1011 0xEB
#define B1110_1100 0xEC
#define B1110_1101 0xED
#define B1110_1110 0xEE
#define B1110_1111 0xEF

#define B1111_0000 0xF0
#define B1111_0001 0xF1
#define B1111_0010 0xF2
#define B1111_0011 0xF3
#define B1111_0100 0xF4
#define B1111_0101 0xF5
#define B1111_0110 0xF6
#define B1111_0111 0xF7
#define B1111_1000 0xF8
#define B1111_1001 0xF9
#define B1111_1010 0xFA
#define B1111_1011 0xFB
#define B1111_1100 0xFC
#define B1111_1101 0xFD
#define B1111_1110 0xFE
#define B1111_1111 0xFF 

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
// 别名区    ADDRESS=0x4200 0000 + (0x0001 100C*0x20) + (bitx*4) ;bitx:第x位
// 把“位段地址＋位序号”转换别名地址宏
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
// 把该地址转换成一个指针
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))

#define BIT_ADDR(addr, bitnum)   MEM_ADDR( BITBAND(addr, bitnum)  )

#define GPIOA_ODR_Addr    (GPIOA_BASE+12) 		//0x4001080C
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) 		//0x40010C0C
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) 		//0x4001100C
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) 		//0x4001140C
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) 		//0x4001180C

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) 		//0x40010808
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) 		//0x40010C08
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) 		//0x40011008
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) 		//0x40011408
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) 		//0x40011808

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
#define PA0   BIT_ADDR(GPIOA_ODR_Addr, 0)  		//输出
#define PA1   BIT_ADDR(GPIOA_ODR_Addr, 1)  		//输出
#define PA2   BIT_ADDR(GPIOA_ODR_Addr, 2)  		//输出
#define PA3   BIT_ADDR(GPIOA_ODR_Addr, 3)  		//输出
#define PA4   BIT_ADDR(GPIOA_ODR_Addr, 4)  		//输出
#define PA5   BIT_ADDR(GPIOA_ODR_Addr, 5)  		//输出
#define PA6   BIT_ADDR(GPIOA_ODR_Addr, 6)  		//输出
#define PA7   BIT_ADDR(GPIOA_ODR_Addr, 7)  		//输出
#define PA8   BIT_ADDR(GPIOA_ODR_Addr, 8)  		//输出
#define PA9   BIT_ADDR(GPIOA_ODR_Addr, 9)  		//输出
#define PA10  BIT_ADDR(GPIOA_ODR_Addr, 10) 		//输出
#define PA11  BIT_ADDR(GPIOA_ODR_Addr, 11) 		//输出
#define PA12  BIT_ADDR(GPIOA_ODR_Addr, 12) 		//输出
#define PA13  BIT_ADDR(GPIOA_ODR_Addr, 13) 		//输出
#define PA14  BIT_ADDR(GPIOA_ODR_Addr, 14) 		//输出
#define PA15  BIT_ADDR(GPIOA_ODR_Addr, 15) 		//输出

#define PA0in   BIT_ADDR(GPIOA_IDR_Addr, 0)  	//输入
#define PA1in   BIT_ADDR(GPIOA_IDR_Addr, 1)  	//输入
#define PA2in   BIT_ADDR(GPIOA_IDR_Addr, 2)  	//输入
#define PA3in   BIT_ADDR(GPIOA_IDR_Addr, 3)  	//输入
#define PA4in   BIT_ADDR(GPIOA_IDR_Addr, 4)  	//输入
#define PA5in   BIT_ADDR(GPIOA_IDR_Addr, 5)  	//输入
#define PA6in   BIT_ADDR(GPIOA_IDR_Addr, 6)  	//输入
#define PA7in   BIT_ADDR(GPIOA_IDR_Addr, 7)  	//输入
#define PA8in   BIT_ADDR(GPIOA_IDR_Addr, 8)  	//输入
#define PA9in   BIT_ADDR(GPIOA_IDR_Addr, 9)  	//输入
#define PA10in  BIT_ADDR(GPIOA_IDR_Addr, 10) 	//输入
#define PA11in  BIT_ADDR(GPIOA_IDR_Addr, 11) 	//输入
#define PA12in  BIT_ADDR(GPIOA_IDR_Addr, 12) 	//输入
#define PA13in  BIT_ADDR(GPIOA_IDR_Addr, 13) 	//输入
#define PA14in  BIT_ADDR(GPIOA_IDR_Addr, 14) 	//输入
#define PA15in  BIT_ADDR(GPIOA_IDR_Addr, 15) 	//输入

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
#define PB0   BIT_ADDR(GPIOB_ODR_Addr, 0)  		//输出
#define PB1   BIT_ADDR(GPIOB_ODR_Addr, 1)  		//输出
#define PB2   BIT_ADDR(GPIOB_ODR_Addr, 2)  		//输出
#define PB3   BIT_ADDR(GPIOB_ODR_Addr, 3)  		//输出
#define PB4   BIT_ADDR(GPIOB_ODR_Addr, 4)  		//输出
#define PB5   BIT_ADDR(GPIOB_ODR_Addr, 5)  		//输出
#define PB6   BIT_ADDR(GPIOB_ODR_Addr, 6)  		//输出
#define PB7   BIT_ADDR(GPIOB_ODR_Addr, 7)  		//输出
#define PB8   BIT_ADDR(GPIOB_ODR_Addr, 8)  		//输出
#define PB9   BIT_ADDR(GPIOB_ODR_Addr, 9)  		//输出
#define PB10  BIT_ADDR(GPIOB_ODR_Addr, 10) 		//输出
#define PB11  BIT_ADDR(GPIOB_ODR_Addr, 11) 		//输出
#define PB12  BIT_ADDR(GPIOB_ODR_Addr, 12) 		//输出
#define PB13  BIT_ADDR(GPIOB_ODR_Addr, 13) 		//输出
#define PB14  BIT_ADDR(GPIOB_ODR_Addr, 14) 		//输出
#define PB15  BIT_ADDR(GPIOB_ODR_Addr, 15) 		//输出

#define PB0in   BIT_ADDR(GPIOB_IDR_Addr, 0)  	//输入
#define PB1in   BIT_ADDR(GPIOB_IDR_Addr, 1)  	//输入
#define PB2in   BIT_ADDR(GPIOB_IDR_Addr, 2)  	//输入
#define PB3in   BIT_ADDR(GPIOB_IDR_Addr, 3)  	//输入
#define PB4in   BIT_ADDR(GPIOB_IDR_Addr, 4)  	//输入
#define PB5in   BIT_ADDR(GPIOB_IDR_Addr, 5)  	//输入
#define PB6in   BIT_ADDR(GPIOB_IDR_Addr, 6)  	//输入
#define PB7in   BIT_ADDR(GPIOB_IDR_Addr, 7)  	//输入
#define PB8in   BIT_ADDR(GPIOB_IDR_Addr, 8)  	//输入
#define PB9in   BIT_ADDR(GPIOB_IDR_Addr, 9)  	//输入
#define PB10in  BIT_ADDR(GPIOB_IDR_Addr, 10) 	//输入
#define PB11in  BIT_ADDR(GPIOB_IDR_Addr, 11) 	//输入
#define PB12in  BIT_ADDR(GPIOB_IDR_Addr, 12) 	//输入
#define PB13in  BIT_ADDR(GPIOB_IDR_Addr, 13) 	//输入
#define PB14in  BIT_ADDR(GPIOB_IDR_Addr, 14) 	//输入
#define PB15in  BIT_ADDR(GPIOB_IDR_Addr, 15) 	//输入
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
#define PC0   BIT_ADDR(GPIOC_ODR_Addr, 0)  		//输出
#define PC1   BIT_ADDR(GPIOC_ODR_Addr, 1)  		//输出
#define PC2   BIT_ADDR(GPIOC_ODR_Addr, 2)  		//输出
#define PC3   BIT_ADDR(GPIOC_ODR_Addr, 3)  		//输出
#define PC4   BIT_ADDR(GPIOC_ODR_Addr, 4)  		//输出
#define PC5   BIT_ADDR(GPIOC_ODR_Addr, 5)  		//输出
#define PC6   BIT_ADDR(GPIOC_ODR_Addr, 6)  		//输出
#define PC7   BIT_ADDR(GPIOC_ODR_Addr, 7)  		//输出
#define PC8   BIT_ADDR(GPIOC_ODR_Addr, 8)  		//输出
#define PC9   BIT_ADDR(GPIOC_ODR_Addr, 9)  		//输出
#define PC10  BIT_ADDR(GPIOC_ODR_Addr, 10) 		//输出
#define PC11  BIT_ADDR(GPIOC_ODR_Addr, 11) 		//输出
#define PC12  BIT_ADDR(GPIOC_ODR_Addr, 12) 		//输出
#define PC13  BIT_ADDR(GPIOC_ODR_Addr, 13) 		//输出
#define PC14  BIT_ADDR(GPIOC_ODR_Addr, 14) 		//输出
#define PC15  BIT_ADDR(GPIOC_ODR_Addr, 15) 		//输出

#define PC0in   BIT_ADDR(GPIOC_IDR_Addr, 0)  	//输入
#define PC1in   BIT_ADDR(GPIOC_IDR_Addr, 1)  	//输入
#define PC2in   BIT_ADDR(GPIOC_IDR_Addr, 2)  	//输入
#define PC3in   BIT_ADDR(GPIOC_IDR_Addr, 3)  	//输入
#define PC4in   BIT_ADDR(GPIOC_IDR_Addr, 4)  	//输入
#define PC5in   BIT_ADDR(GPIOC_IDR_Addr, 5)  	//输入
#define PC6in   BIT_ADDR(GPIOC_IDR_Addr, 6)  	//输入
#define PC7in   BIT_ADDR(GPIOC_IDR_Addr, 7)  	//输入
#define PC8in   BIT_ADDR(GPIOC_IDR_Addr, 8)  	//输入
#define PC9in   BIT_ADDR(GPIOC_IDR_Addr, 9)  	//输入
#define PC10in  BIT_ADDR(GPIOC_IDR_Addr, 10) 	//输入
#define PC11in  BIT_ADDR(GPIOC_IDR_Addr, 11) 	//输入
#define PC12in  BIT_ADDR(GPIOC_IDR_Addr, 12) 	//输入
#define PC13in  BIT_ADDR(GPIOC_IDR_Addr, 13) 	//输入
#define PC14in  BIT_ADDR(GPIOC_IDR_Addr, 14) 	//输入
#define PC15in  BIT_ADDR(GPIOC_IDR_Addr, 15) 	//输入

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
#define PD0   BIT_ADDR(GPIOD_ODR_Addr, 0)  		//输出
#define PD1   BIT_ADDR(GPIOD_ODR_Addr, 1)  		//输出
#define PD2   BIT_ADDR(GPIOD_ODR_Addr, 2)  		//输出
#define PD3   BIT_ADDR(GPIOD_ODR_Addr, 3)  		//输出
#define PD4   BIT_ADDR(GPIOD_ODR_Addr, 4)  		//输出
#define PD5   BIT_ADDR(GPIOD_ODR_Addr, 5)  		//输出
#define PD6   BIT_ADDR(GPIOD_ODR_Addr, 6)  		//输出
#define PD7   BIT_ADDR(GPIOD_ODR_Addr, 7)  		//输出
#define PD8   BIT_ADDR(GPIOD_ODR_Addr, 8)  		//输出
#define PD9   BIT_ADDR(GPIOD_ODR_Addr, 9)  		//输出
#define PD10  BIT_ADDR(GPIOD_ODR_Addr, 10) 		//输出
#define PD11  BIT_ADDR(GPIOD_ODR_Addr, 11) 		//输出
#define PD12  BIT_ADDR(GPIOD_ODR_Addr, 12) 		//输出
#define PD13  BIT_ADDR(GPIOD_ODR_Addr, 13) 		//输出
#define PD14  BIT_ADDR(GPIOD_ODR_Addr, 14) 		//输出
#define PD15  BIT_ADDR(GPIOD_ODR_Addr, 15) 		//输出

#define PD0in   BIT_ADDR(GPIOD_IDR_Addr, 0)  	//输入
#define PD1in   BIT_ADDR(GPIOD_IDR_Addr, 1)  	//输入
#define PD2in   BIT_ADDR(GPIOD_IDR_Addr, 2)  	//输入
#define PD3in   BIT_ADDR(GPIOD_IDR_Addr, 3)  	//输入
#define PD4in   BIT_ADDR(GPIOD_IDR_Addr, 4)  	//输入
#define PD5in   BIT_ADDR(GPIOD_IDR_Addr, 5)  	//输入
#define PD6in   BIT_ADDR(GPIOD_IDR_Addr, 6)  	//输入
#define PD7in   BIT_ADDR(GPIOD_IDR_Addr, 7)  	//输入
#define PD8in   BIT_ADDR(GPIOD_IDR_Addr, 8)  	//输入
#define PD9in   BIT_ADDR(GPIOD_IDR_Addr, 9)  	//输入
#define PD10in  BIT_ADDR(GPIOD_IDR_Addr, 10) 	//输入
#define PD11in  BIT_ADDR(GPIOD_IDR_Addr, 11) 	//输入
#define PD12in  BIT_ADDR(GPIOD_IDR_Addr, 12) 	//输入
#define PD13in  BIT_ADDR(GPIOD_IDR_Addr, 13) 	//输入
#define PD14in  BIT_ADDR(GPIOD_IDR_Addr, 14) 	//输入
#define PD15in  BIT_ADDR(GPIOD_IDR_Addr, 15) 	//输入

//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
#define PE0   BIT_ADDR(GPIOE_ODR_Addr, 0)  		//输出
#define PE1   BIT_ADDR(GPIOE_ODR_Addr, 1)  		//输出
#define PE2   BIT_ADDR(GPIOE_ODR_Addr, 2)  		//输出
#define PE3   BIT_ADDR(GPIOE_ODR_Addr, 3)  		//输出
#define PE4   BIT_ADDR(GPIOE_ODR_Addr, 4)  		//输出
#define PE5   BIT_ADDR(GPIOE_ODR_Addr, 5)  		//输出
#define PE6   BIT_ADDR(GPIOE_ODR_Addr, 6)  		//输出
#define PE7   BIT_ADDR(GPIOE_ODR_Addr, 7)  		//输出
#define PE8   BIT_ADDR(GPIOE_ODR_Addr, 8)  		//输出
#define PE9   BIT_ADDR(GPIOE_ODR_Addr, 9)  		//输出
#define PE10  BIT_ADDR(GPIOE_ODR_Addr, 10) 		//输出
#define PE11  BIT_ADDR(GPIOE_ODR_Addr, 11) 		//输出
#define PE12  BIT_ADDR(GPIOE_ODR_Addr, 12) 		//输出
#define PE13  BIT_ADDR(GPIOE_ODR_Addr, 13) 		//输出
#define PE14  BIT_ADDR(GPIOE_ODR_Addr, 14) 		//输出
#define PE15  BIT_ADDR(GPIOE_ODR_Addr, 15) 		//输出

#define PE0in   BIT_ADDR(GPIOE_IDR_Addr, 0)  	//输入
#define PE1in   BIT_ADDR(GPIOE_IDR_Addr, 1)  	//输入
#define PE2in   BIT_ADDR(GPIOE_IDR_Addr, 2)  	//输入
#define PE3in   BIT_ADDR(GPIOE_IDR_Addr, 3)  	//输入
#define PE4in   BIT_ADDR(GPIOE_IDR_Addr, 4)  	//输入
#define PE5in   BIT_ADDR(GPIOE_IDR_Addr, 5)  	//输入
#define PE6in   BIT_ADDR(GPIOE_IDR_Addr, 6)  	//输入
#define PE7in   BIT_ADDR(GPIOE_IDR_Addr, 7)  	//输入
#define PE8in   BIT_ADDR(GPIOE_IDR_Addr, 8)  	//输入
#define PE9in   BIT_ADDR(GPIOE_IDR_Addr, 9)  	//输入
#define PE10in  BIT_ADDR(GPIOE_IDR_Addr, 10) 	//输入
#define PE11in  BIT_ADDR(GPIOE_IDR_Addr, 11) 	//输入
#define PE12in  BIT_ADDR(GPIOE_IDR_Addr, 12) 	//输入
#define PE13in  BIT_ADDR(GPIOE_IDR_Addr, 13) 	//输入
#define PE14in  BIT_ADDR(GPIOE_IDR_Addr, 14) 	//输入
#define PE15in  BIT_ADDR(GPIOE_IDR_Addr, 15) 	//输入
//※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※※
#ifdef __cplusplus
} // extern "C"
#endif

#endif

