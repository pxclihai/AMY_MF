#ifndef __PlugSdk_h__
#define __PlugSdk_h__
/********************************
MF-->Ä§·½
**********************************/
//#include "Protool.h"
#include "config.h"

extern int            mRegist;
extern int            mLogin;
extern char           mLoginServer[16];
extern char           mLoginTocken[32];
extern char           mLoginPluginId[32];
extern int            mLoginPort;
extern int            mHeartInterval;
//int          mMacConfigNum;
extern long 		       mHeartTime;
extern int            mQuitProtool;


extern int mofanSDK();


extern	int InitPlugSdk(void);
extern 	int DestroyPlugSdk(void);

u16 MF_get_dev_mac(u8 mac[6]);
u16 MF_get_dev_sn(char * sn);

#endif
