#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "jtprintf.h"

int jtprintf(const char* fmt, ...)
{
	char *buffer=NULL;
	va_list argptr;
	int cnt;
	va_start(argptr, fmt);
	//cnt = vprintf( fmt, argptr ) + 1;
	//buffer = (char*)malloc( cnt * sizeof(char) );
	//buffer[cnt-1] = 0;
	cnt = vfprintf(stderr, fmt, argptr);
	//cnt = vsprintf(buffer, fmt, argptr);
	va_end(argptr);

	//fprintf(stderr, "%s", buffer);
	return(cnt);
}



