#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "bitmap.h"

int CheckBitmap(unsigned char *bitmap, int bit)
{
  bit--;
  return((bitmap[bit/8]>>(7-bit%8)) & 0x01);
}

int SetBitmap(unsigned char *bitmap, int bit)
{
  bit--;
  bitmap[bit/8] |= (0x80 >> (bit%8));
  return 0;
}

int UnsetBitmap(unsigned char *bitmap, int bit)
{
  bit--;
  bitmap[bit/8] &= ~(0x80>>(bit%8));
  return 0;
}

