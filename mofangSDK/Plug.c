
#include <stdio.h>
//#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#include "Plug.h"

//#include "Protool.h"
#include "PlugSdk.h"




int mofanSDK()
{
	//time_t curtime;
	//time(&curtime);

	if(0!=InitPlugSdk())
	{
		qwl_sendstring("gNetDump init error\n");
		DestroyPlugSdk();
		return -1;
	}
	
	return 0;
	
}


