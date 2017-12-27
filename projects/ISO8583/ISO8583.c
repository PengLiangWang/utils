#include "ISO8583.h"

int check_bitmap(ISO8583 *data, int bit)
{
  bit--;
  return((data->primary_bitmap[bit/8]>>(7-bit%8)) & 0x01);
}

int set_bitmap(ISO8583 *data, int bit)
{
  bit--;
  data->primary_bitmap[bit/8] |= (0x80 >> (bit%8));
  return 0;
}

int unset_bitmap(ISO8583 *data, int bit)
{
  bit--;
  data->primary_bitmap[bit/8] &= (0xFF - (0x80>>(bit%8))); 
  return 0;
}

int clear_bitmap(ISO8583 *data)
{
  int i;
  for( i=0; i<8; i++)
    data->primary_bitmap[i] = 0;
  return 0;
}

