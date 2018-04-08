#ifndef __PlugSdk_h__
#define __PlugSdk_h__

//#include "Protool.h"
#include "config.h"

//structs PlugSdk
//{
//public:
	//PlugSdk();
	//~PlugSdk();

extern	int InitPlugSdk(void);
extern 	int DestroyPlugSdk(void);
	//Protool*     mProtool;
//private:
    u16 get_dev_mac(u8 mac[6]);
    u16 get_dev_sn(char * sn);

//};
#endif
