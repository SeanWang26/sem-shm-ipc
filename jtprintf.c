#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "jtprintf.h"

int jtprintf(const char* fmt, ...)
{
	char buffer[4096];
	va_list argptr;
	int cnt;
	va_start(argptr, fmt);
	cnt = vsprintf(buffer, fmt, argptr);
	va_end(argptr);

	fprintf(stderr, "%s", buffer);
	return(cnt);
}



