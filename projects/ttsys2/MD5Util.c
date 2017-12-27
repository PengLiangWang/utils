#include <string.h>
#include "MD5Util.h"

void MDString1(char *string, char *mdstring)
{
  MD5_CTX context;
  unsigned char digest[16];
  unsigned int len;

  len = (unsigned int) strlen( (const char *)string );

  MD5_Init (&context);
  MD5_Update (&context, string, len);
  MD5_Final(digest, &context);

  binToStr(digest,mdstring,16);
}
