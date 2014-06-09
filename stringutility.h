#ifndef STRING_UTILITY_H
#define STRING_UTILITY_H
#include <string.h>

inline char *strsep_s(char **stringp, const char *delim)
{
	char *tok;
	while((tok = strsep(stringp, delim)))
	{
		if(strlen(tok))
			return tok;
	}

	return NULL;
}

#endif
