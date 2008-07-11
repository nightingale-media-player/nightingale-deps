
#ifndef _JPEG_DEBUG_H_
#define _JPEG_DEBUG_H_

#ifdef _MSC_VER
#define JPEG_DEBUG(n, ...)
#else
#define JPEG_DEBUG(n, format...)	do{ \
	if((n)<=JPEG_DEBUG_LEVEL)jpeg_debug((n),format); \
}while(0)
#define JPEG_DEBUG_LEVEL 4
#endif

void jpeg_debug(int n, const char *format, ... );

#endif

