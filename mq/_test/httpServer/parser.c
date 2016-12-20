#include <stdio.h>
#include <string.h>

int parse_header(char *header, HashTable *htable)
{
	int		 offset = 0, i=0;
	int		 bsize;
	char	*pstr;
	char	temp[256] = {'\0'};
	
	bsize = strlen(header);
	pstr = header;

	while( *(pstr+offset) != 0x20 )
		temp[i++] = *(pstr+offset);

	HSetValue(htable, "request_method", temp);

	HSetValue(htable, "http_version", temp);

		
}
