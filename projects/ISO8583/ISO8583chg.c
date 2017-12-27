#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <time.h>
#include "ISO8583.h"

#define BMAX        1024
#define PACK        1
#define UNPACK      2

#define SIZEOF(type,value)   ((int)(sizeof(((type *)0)->value)))
#define CHG(value)    POS_BIT_##value,offsetof(ISO8583,value),\
                      SIZEOF(ISO8583,value),SIZEOF(ISO8583,value) - 1

int chg_str_bcd(int chg,unsigned char **m_dst_8583,
                    int field_length,unsigned char *m_src_8583);
int chg_str_bcd_a(int chg,unsigned char **m_dst_8583,
                    int field_length,unsigned char *m_src_8583);
int chg_bit_bit(int chg,unsigned char **m_dst_8583,
                    int field_length,unsigned char *m_src_8583);
int chg_str_str(int chg,unsigned char **m_dst_8583,
                    int field_length,unsigned char *m_src_8583);
int chg_str_bv2(int chg,unsigned char **m_dst_8583,
                    int field_length,unsigned char *m_src_8583);
int chg_str_av2(int chg,unsigned char **m_dst_8583,
                    int field_length,unsigned char *m_src_8583);
int chg_str_bv3(int chg,unsigned char **m_dst_8583,
                    int field_length,unsigned char *m_src_8583);
int chg_str_av3(int chg,unsigned char **m_dst_8583,
                    int field_length,unsigned char *m_src_8583);

typedef struct
{
  int bitName;
  int offset;
  int valsize;
  int size;
  int (*chgF)(int chg,unsigned char **m_dst_8583,
              int field_length,unsigned char *m_src_8583);
}ISO8583Chg;

static ISO8583Chg chgList[] =
{
  {CHG(tpdu),                                    chg_str_bcd},
  {CHG(message_explain),                         chg_str_bcd},
  {CHG(message_type),                            chg_str_bcd},
  {CHG(primary_bitmap),                          chg_bit_bit},
  {CHG(primary_account_number),                  chg_str_bv2},
  {CHG(processing_code),                         chg_str_bcd},
  {CHG(amount_of_transaction),                   chg_str_bcd},
  {CHG(amount_of_settlement),                    chg_str_bcd},
  {CHG(amount_of_cardholder_billing),            chg_str_bcd},
  {CHG(transmission_date_and_time),              chg_str_bcd},
  {CHG(amount_of_cardholder_billing_fee),        chg_str_bcd},
  {CHG(conversion_rate_of_settlement),           chg_str_bcd},
  {CHG(conversion_rate_of_cardholder_billing),   chg_str_bcd},
  {CHG(system_trace_audit_number),               chg_str_bcd},
  {CHG(time_of_local_transaction),               chg_str_bcd},
  {CHG(date_of_local_transaction),               chg_str_bcd},
  {CHG(date_of_expiration),                      chg_str_bcd},
  {CHG(date_of_settlement),                      chg_str_bcd},
  {CHG(date_of_conversion),                      chg_str_bcd},
  {CHG(date_of_capture),                         chg_str_bcd},
  {CHG(merchant_type),                           chg_str_bcd},
  {CHG(acquiring_institution_country_code),      chg_str_bcd},
  {CHG(primary_account_number_extended_country_code),chg_str_bcd},
  {CHG(forwarding_institution_country_code),     chg_str_av2},
  {CHG(point_of_service_entry_mode),             chg_str_bcd},
  {CHG(card_sequence_number),                    chg_str_bcd},
  {CHG(network_international_id),                chg_str_bcd},
  {CHG(point_of_service_condition_code),         chg_str_bcd},
  {CHG(point_of_service_PIN_capture_code),       chg_str_bcd},
  {CHG(auth_id_response_length),                 chg_str_bcd},
  {CHG(amount_of_transaction_fee),               chg_str_str},
  {CHG(amount_of_settlement_fee),                chg_str_str},
  {CHG(amount_of_transaction_processing_fee),    chg_str_str},
  {CHG(amount_of_settlement_processing_fee),     chg_str_str},
  {CHG(acquiring_institution_id_code),           chg_str_bv2},
  {CHG(forwarding_institution_id_code),          chg_str_bv2},
  {CHG(extended_primary_account_number),         chg_str_bv2},
  {CHG(track_2_data),                            chg_str_bv2},
  {CHG(track_3_data),                            chg_str_bv3},
  {CHG(retrieval_reference_number),              chg_str_str},
  {CHG(auth_id_response),                        chg_str_str},
  {CHG(response_code),                           chg_str_str},
  {CHG(service_restriction_code),                chg_str_str},
  {CHG(card_acceptor_terminal_id),               chg_str_str},
  {CHG(card_acceptor_id_code),                   chg_str_str},
  {CHG(card_acceptor_name_location),             chg_str_str},
  {CHG(additional_response_data),                chg_str_av2},
  {CHG(track_1_data),                            chg_str_av2},
  {CHG(private_additional_data),                 chg_str_bv3},
  {CHG(currency_code_of_transaction),            chg_str_str},
  {CHG(currency_code_of_settlement),             chg_str_str},
  {CHG(currency_code_of_cardholder_billing),     chg_str_str},
  {CHG(PIN),                                     chg_bit_bit},
  {CHG(security_related_control_info),           chg_str_bcd},
  {CHG(balance_amounts),                         chg_str_av3},
  {CHG(pboc_ic_transaction_information),         chg_str_av3},
  {CHG(pboc_electronic_data),                    chg_str_av3},
  {CHG(reserved_private_data_60),                chg_str_bv3},
  {CHG(reserved_private_data_61),                chg_str_bv3},
  {CHG(reserved_private_data_62),                chg_str_av3},
  {CHG(reserved_private_data_63),                chg_str_av3},
  {CHG(MAC64),                                   chg_bit_bit},
  {0,0,0,0,NULL},
};

int chg_str_bcd(int chg,unsigned char **m_dst_8583,int field_length,unsigned char *m_src_8583)
{
  int 	i,len = 0;
  unsigned char	ch;
  char  *s = 0;

  switch(chg)
  {
    case PACK:
      s = (char *)m_src_8583;
      len = strlen(s);
      if(len < field_length)
        field_length = len;
      /*if(field_length % 2 != 0)
      {
        *(*m_dst_8583)++ = 0x0f & *m_src_8583++;
        field_length--;
      }
      for(i=0; i<field_length; i+=2)
      {
        **m_dst_8583 = 0x0f & *m_src_8583++;
        **m_dst_8583 <<= 4;
        *(*m_dst_8583)++ |= 0x0f & *m_src_8583++;
      }*/
      for(i=0; i<field_length; i+=2)
      {
        **m_dst_8583 = 0x0f & *m_src_8583++;
        **m_dst_8583 <<= 4;
        *(*m_dst_8583)++ |= 0x0f & *m_src_8583++;
      }
      if(field_length % 2 != 0)
      {
        *(*m_dst_8583-1) &= 0xf0 ;
      }
      break;
    case UNPACK:
      /*
      if(field_length % 2 != 0)
      {
        *m_src_8583++ = 0x30 | ( 0x0f & *(*m_dst_8583)++ );
        field_length--;
      }
      for(i=0; i<field_length/2; i++)
      {
        ch = **m_dst_8583;
        ch >>= 4;
        *m_src_8583++ = 0x30 | ( 0x0f & ch );
        *m_src_8583++ = 0x30 | ( 0x0f & *(*m_dst_8583)++ );
      }
      */
      for(i=0; i<field_length/2; i++)
      {
        ch = **m_dst_8583;
        ch >>= 4;
        *m_src_8583++ = 0x30 | ( 0x0f & ch );
        *m_src_8583++ = 0x30 | ( 0x0f & *(*m_dst_8583)++ );
      }
      if(field_length % 2 != 0)
      {
        ch = 0xf0 & *(*m_dst_8583)++;
        *m_src_8583++ = 0x30 | ( ch >> 4 );
      }
      break;
    default:
      return -1;
  }

  return 0;
}

int chg_str_bcd_a(int chg,unsigned char **m_dst_8583,int field_length,unsigned char *m_src_8583)
{
  int 	i,len = 0;
  unsigned char	ch;
  char  *s = 0;
  switch(chg)
  {
    case PACK :
      s = (char *)m_src_8583;
      len = strlen(s);
      if(len < field_length)
        field_length = len;
      for(i=0; i<field_length; i+=2)
      {
        **m_dst_8583 = 0x0f & *m_src_8583++;
        **m_dst_8583 <<= 4;
        *(*m_dst_8583)++ |= 0x0f & *m_src_8583++;
      }
      if(field_length % 2 != 0)
      {
        *(*m_dst_8583-1) &= 0xf0 ;
      }
      break;
    case UNPACK :
      for(i=0; i<field_length/2; i++)
      {
        ch = **m_dst_8583;
        ch >>= 4;
        *m_src_8583++ = 0x30 | ( 0x0f & ch );
        *m_src_8583++ = 0x30 | ( 0x0f & *(*m_dst_8583)++ );
      }
      if(field_length % 2 != 0)
      {
        ch = 0xf0 & *(*m_dst_8583)++;
        *m_src_8583++ = 0x30 | ( ch >> 4 );
      }
      break;
    default:
      return -1;
  }
  return 0;
}

int chg_bit_bit(int chg,unsigned char **m_dst_8583,int field_length,unsigned char *m_src_8583)
{
  int 	i = 0;
  switch(chg)
  {
    case PACK :
      for(i=0; i<field_length; i++)
        *(*m_dst_8583)++ = *m_src_8583++;
      break;
    case UNPACK :
      for(i=0; i<field_length; i++)
        *m_src_8583++ = *(*m_dst_8583)++;
      break;
    default:
      return -1;
  }
  return 0;
}



int chg_str_str(int chg,unsigned char **m_dst_8583,int field_length,unsigned char *m_src_8583)
{
  int 	i,len = 0;
  char  *s = 0;
  switch(chg)
  {
    case PACK :
      s = (char *)m_src_8583;
      len = strlen(s);
      if(len < field_length)
        field_length = len;
      for(i=0; i<field_length; i++)
        *(*m_dst_8583)++ = *m_src_8583++;
      break;
    case UNPACK :
      for(i=0; i<field_length; i++)
        *m_src_8583++ = *(*m_dst_8583)++;
      break;
    default:
      return -1;
  }
  return 0;
}

int chg_str_bv2(int chg,unsigned char **m_dst_8583,int field_length,unsigned char *m_src_8583)
{
  int 	i,len = 0;
  unsigned char ch;
  char  *s = 0;

  switch(chg)
  {
    case PACK :
      s = (char *)m_src_8583;
      len = strlen(s);
      if(len < field_length)
        field_length = len;
      ch = field_length / 10;
      ch <<= 4;
      ch |= field_length % 10;
      *(*m_dst_8583)++ = ch;
      return(chg_str_bcd_a(chg,m_dst_8583,field_length,m_src_8583));
      break;
    case UNPACK :
      i = (((char)**m_dst_8583&0xf0)>>4)*10 + (**m_dst_8583&0x0f);
      if(i<field_length) field_length = i;
      (*m_dst_8583)++;
      return(chg_str_bcd_a(chg,m_dst_8583,field_length,m_src_8583));
      break;
    default:
      break;
  }
  return 0;
}

int chg_str_av2(int chg,unsigned char **m_dst_8583,int field_length,unsigned char *m_src_8583)
{
  int 	i,len = 0;
  unsigned char ch;
  char  *s = 0;

  switch(chg)
  {
    case PACK :
      s = (char *)m_src_8583;
      len = strlen(s);
      if(len < field_length)
        field_length = len;
      field_length = strlen(s);
      ch = field_length / 10;
      ch <<= 4;
      ch |= field_length % 10;
      *(*m_dst_8583)++ = ch;
      return(chg_str_str(chg,m_dst_8583,field_length,m_src_8583));
      break;
    case UNPACK :
      i = (((char)**m_dst_8583&0xf0)>>4)*10 + (**m_dst_8583&0x0f);
      if(i<field_length) field_length = i;
      (*m_dst_8583)++;
      return(chg_str_str(chg,m_dst_8583,field_length,m_src_8583));
      break;
    default:
      break;
  }
  return 0;
}

int chg_str_bv3(int chg,unsigned char **m_dst_8583,int field_length,unsigned char *m_src_8583)
{
  int 	i,len = 0;
  unsigned char ch;
  char  *s = 0;


  switch(chg)
  {
    case PACK :
      s = (char *)m_src_8583;
      len = strlen(s);
      if(len < field_length)
        field_length = len;
      ch = field_length / 100;
      *(*m_dst_8583)++ = ch;
      i = field_length - ch * 100;
      ch = i / 10;
      ch <<= 4;
      ch |= i % 10;
      *(*m_dst_8583)++ = ch;
      return(chg_str_bcd_a(chg,m_dst_8583,field_length,m_src_8583));
      break;
    case UNPACK :
      i = (**m_dst_8583&0x0f)*100;
      (*m_dst_8583)++;
      i += (((char)**m_dst_8583&0xf0)>>4)*10+(**m_dst_8583&0x0f);
      if(i < field_length) field_length = i;
      (*m_dst_8583)++;
      return(chg_str_bcd_a(chg,m_dst_8583,field_length,m_src_8583));
      break;
    default:
      break;
  }
  return 0;
}


int chg_str_av3(int chg,unsigned char **m_dst_8583,int field_length,unsigned char *m_src_8583)
{
  int 	i,len = 0;
  unsigned char ch;
  char  *s = 0;


  switch(chg)
  {
    case PACK :
      s = (char *)m_src_8583;
      len = strlen(s);
      if(len < field_length)
        field_length = len;
      ch = field_length / 100;
      *(*m_dst_8583)++ = ch;
      i = field_length - ch * 100;
      ch = i / 10;
      ch <<= 4;
      ch |= i % 10;
      *(*m_dst_8583)++ = ch;
      return(chg_str_str(chg,m_dst_8583,field_length,m_src_8583));
      break;
    case UNPACK :
      i = (**m_dst_8583&0x0f)*100;
      (*m_dst_8583)++;
      i += (((char)**m_dst_8583&0xf0)>>4)*10+(**m_dst_8583&0x0f);
      if(i < field_length) field_length = i;
      (*m_dst_8583)++;
      return(chg_str_str(chg,m_dst_8583,field_length,m_src_8583));
      break;
    default:
      break;
  }
  return 0;
}

int chg_key_bit(int chg,unsigned char **m_dst_8583,int field_length,unsigned char *m_src_8583)
{
  int 	i,len = 0;
  unsigned char ch;

  field_length = 24;

  switch(chg)
  {
    case PACK :
      ch = field_length / 100;
      *(*m_dst_8583)++ = ch;
      i = field_length - ch * 100;
      ch = i / 10;
      ch <<= 4;
      ch |= i % 10;
      *(*m_dst_8583)++ = ch;
      for(i=0; i<field_length; i++)
        *(*m_dst_8583)++ = *m_src_8583++;
      break;
    case UNPACK :
      for(i=0; i<field_length; i++)
        *m_src_8583++ = *(*m_dst_8583)++;
      break;
    default:
      return -1;
  }
  return 0;
}

int ISO8583_set_field(int flag)
{
  int  i;

  for(i = 0;chgList[i].bitName!=0;i++)
  {
    if(chgList[i].bitName == POS_BIT_reserved_private_data_62)
    {
      if(flag)
      {
        chgList[i].chgF = chg_key_bit;
      }
      else
        chgList[i].chgF = chg_str_av3;
    }
  }
  return 0;
}


int chg_ISO8583_n(int chg,unsigned char **m_dst_8583,void *m_src_8583,int *lastField)
{
  int  r = 0,i;
  int  addr = 0,addr1 = 0;
  *lastField = 0;
  addr = (int)*m_dst_8583;
  r = chg_str_bcd(chg,m_dst_8583,10,(unsigned char *)(((ISO8583*)m_src_8583)->tpdu));
  if(r)
    goto E;
  r = chg_str_bcd(chg,m_dst_8583,12,(unsigned char *)(((ISO8583*)m_src_8583)->message_explain));
  if(r)
    goto E;
  r = chg_str_bcd(chg,m_dst_8583,4,(unsigned char *)(((ISO8583*)m_src_8583)->message_type));
  if(r)
    goto E;
  r = chg_bit_bit(chg,m_dst_8583,8,(unsigned char *)(((ISO8583*)m_src_8583)->primary_bitmap));
  if(r)
    goto E;
  for(i=4, r=0; (r==0)&&(chgList[i].bitName!=0); i++)
  {
    if(check_bitmap((ISO8583*)m_src_8583,chgList[i].bitName))
    {
      *lastField=chgList[i].bitName;
      r = (*chgList[i].chgF)(chg,m_dst_8583,chgList[i].size,((unsigned char *)m_src_8583)+chgList[i].offset);
    }
  }
  addr1 = (int)*m_dst_8583;
  *lastField = addr1 - addr;
E:
  return r;
}

int ISO8583_hton(ISO8583 *data,unsigned char *buf,int *bufSize)
{
  int r = 0;
  unsigned char tbuf[BMAX];
  unsigned char *ptr = 0;
  char src_tpdu[5];
  char dst_tpdu[5];

  *bufSize = 0;
  memset(tbuf,0,sizeof(tbuf));
  ptr = (unsigned char *)&tbuf[2];

  memset(src_tpdu,0,sizeof(src_tpdu));
  memset(dst_tpdu,0,sizeof(dst_tpdu));

  memcpy(src_tpdu,data->tpdu + 2,4);
  memcpy(dst_tpdu,data->tpdu + 6,4);

  memcpy(data->tpdu + 2,dst_tpdu,4);
  memcpy(data->tpdu + 6,src_tpdu,4);

  r = chg_ISO8583_n(PACK,&ptr,(void*)data,bufSize);
  if (r)
  {
    return r;
  }

  tbuf[0] = (unsigned char)(*bufSize / 0xFF);
  tbuf[1] = (unsigned char)(*bufSize % 0xFF);

  memcpy(buf,tbuf,*bufSize + 2);
  *bufSize += 2;

  return r;
}

int ISO8583_ntoh(ISO8583 *data,unsigned char *buf)
{
  int  bufSize = 0;


  buf += 2;

  return chg_ISO8583_n(UNPACK,&buf,(void*)data,&bufSize);
}

static char BtoA(unsigned char b)
{
  if (b < 10)
    return( '0' + b );
  if ( b < 16 )
    return((char)('A'+b-10));
  return('*');
}

static char * bintostr(unsigned char *bin,int len)
{
  static char  buf[256];
  char  *str;

  memset(buf,0,sizeof(buf));
  str = buf;
  while(len>0)
  {
    *str=BtoA((unsigned char)(((unsigned char)*bin)/16)); ++str;
    *str=BtoA((unsigned char)(((unsigned char)*bin)&15)); ++str;
    ++bin;
    --len;
  };
  *str='\0';

  return (char*)buf;
}

int ISOBUFF_dump(char *title,unsigned char *buffer,int size)
{
  time_t    timer;
  struct    tm  *lt;
  char      filename[128];
  FILE      *fp;
  int       i,j;

  time(&timer);

  lt = (struct	tm *)localtime(&timer);

  memset(filename,0,sizeof(filename));

  sprintf(filename,"%s/log/ISO%02d%02d%02d.txt",getenv("HOME"),lt->tm_year - 100,lt->tm_mon+1,lt->tm_mday);

  fp = fopen(filename,"a");

  fprintf(fp,"<dump name=\"%s\" length=\"%d\" pid=\"%d\"/>\n",title,size,getpid());

  for (i = 0;i < (size / 16);i++)
  {
    fprintf(fp,"  %02X ",buffer[i * 16] & 0xff);

    for(j = 1;j < 15;j++)
      fprintf(fp,"%02X ",buffer[i*16 + j] & 0xff);

    fprintf(fp,"%02X\n",buffer[i*16 + 15] & 0xff);
  }

  if (size % 16)
  {
    fprintf(fp,"  %02X ",buffer[i * 16 ] & 0xff);

    for (j = 1; j < (size % 16) - 1;j++)
      fprintf(fp,"%02X ",buffer[i * 16 + j] & 0xff);

    fprintf(fp,"%02X\n",buffer[i * 16 + (size % 16) - 1] & 0xff);
  }
  fprintf(fp,"</dump>\n");
  fclose(fp);
  return 0;
}

int ISO8583_dump(char *title,ISO8583 *data)
{
  time_t   timer;
  struct tm  *lt;
  char     filename[128];
  FILE    *fp;


  fp = stdout;
  time(&timer);
  lt = (struct	tm *)localtime(&timer);
  memset(filename,0,sizeof(filename));
  sprintf(filename,"%s/log/ISO%02d%02d%02d.txt",getenv("HOME"),lt->tm_year - 100,lt->tm_mon+1,lt->tm_mday);

  //fp = fopen(filename,"a");

  fprintf(stdout,"%02d:%02d:%02d [title:%s][pid:%d]\n",lt->tm_hour,lt->tm_min,lt->tm_sec,title,getpid());
  fprintf(stdout,"*******************************************************************************\n");
  fprintf(stdout,"[%02d] [%03d] : %s\n",-5,sizeof(data->tpdu)-1,data->tpdu);
  fprintf(stdout,"[%02d] [%03d] : %s\n",-4,sizeof(data->message_explain)-1,data->message_explain);
  fprintf(stdout,"[%02d] [%03d] : %s\n",-3,sizeof(data->message_type)-1,data->message_type);
  fprintf(stdout,"[%02d] [%03d] : %s\n",-2,sizeof(data)-1,bintostr(data->primary_bitmap,8));
  if(check_bitmap(data,2))
    fprintf(stdout,"[%02d] [%03d] : %s\n",2,sizeof(data->primary_account_number)-1,data->primary_account_number);

  if(check_bitmap(data,3))
    fprintf(stdout,"[%02d] [%03d] : %s\n",3,sizeof(data->processing_code)-1,data->processing_code);

  if(check_bitmap(data,4))
    fprintf(stdout,"[%02d] [%03d] : %s\n",4,sizeof(data->amount_of_transaction)-1,data->amount_of_transaction);

  if(check_bitmap(data,5))
    fprintf(stdout,"[%02d] [%03d] : %s\n",5,sizeof(data->amount_of_settlement)-1,data->amount_of_settlement);

  if(check_bitmap(data,6))
    fprintf(stdout,"[%02d] [%03d] : %s\n",6,sizeof(data->amount_of_cardholder_billing)-1,data->amount_of_cardholder_billing);

  if(check_bitmap(data,7))
    fprintf(stdout,"[%02d] [%03d] : %s\n",7,sizeof(data->transmission_date_and_time)-1,data->transmission_date_and_time);

  if(check_bitmap(data,8))
    fprintf(stdout,"[%02d] [%03d] : %s\n",8,sizeof(data->amount_of_cardholder_billing_fee)-1,data->amount_of_cardholder_billing_fee);

  if(check_bitmap(data,9))
    fprintf(stdout,"[%02d] [%03d] : %s\n",9,sizeof(data->conversion_rate_of_settlement)-1,data->conversion_rate_of_settlement);

  if(check_bitmap(data,10))
    fprintf(stdout,"[%02d] [%03d] : %s\n",10,sizeof(data->conversion_rate_of_cardholder_billing)-1,data->conversion_rate_of_cardholder_billing);

  if(check_bitmap(data,11))
    fprintf(stdout,"[%02d] [%03d] : %s\n",11,sizeof(data->system_trace_audit_number)-1,data->system_trace_audit_number);

  if(check_bitmap(data,12))
    fprintf(stdout,"[%02d] [%03d] : %s\n",12,sizeof(data->time_of_local_transaction)-1,data->time_of_local_transaction);

  if(check_bitmap(data,13))
    fprintf(stdout,"[%02d] [%03d] : %s\n",13,sizeof(data->date_of_local_transaction)-1,data->date_of_local_transaction);

  if(check_bitmap(data,14))
    fprintf(stdout,"[%02d] [%03d] : %s\n",14,sizeof(data->date_of_expiration)-1,data->date_of_expiration);

  if(check_bitmap(data,15))
    fprintf(stdout,"[%02d] [%03d] : %s\n",15,sizeof(data->date_of_settlement)-1,data->date_of_settlement);

  if(check_bitmap(data,16))
    fprintf(stdout,"[%02d] [%03d] : %s\n",16,sizeof(data->date_of_conversion)-1,data->date_of_conversion);

  if(check_bitmap(data,17))
    fprintf(stdout,"[%02d] [%03d] : %s\n",17,sizeof(data->date_of_capture)-1,data->date_of_capture);

  if(check_bitmap(data,18))
    fprintf(stdout,"[%02d] [%03d] : %s\n",18,sizeof(data->merchant_type)-1,data->merchant_type);

  if(check_bitmap(data,19))
    fprintf(stdout,"[%02d] [%03d] : %s\n",19,sizeof(data->acquiring_institution_country_code)-1,data->acquiring_institution_country_code);

  if(check_bitmap(data,20))
    fprintf(stdout,"[%02d] [%03d] : %s\n",20,sizeof(data->primary_account_number_extended_country_code)-1,data->primary_account_number_extended_country_code);

  if(check_bitmap(data,21))
    fprintf(stdout,"[%02d] [%03d] : %s\n",21,sizeof(data->forwarding_institution_country_code)-1,data->forwarding_institution_country_code);

  if(check_bitmap(data,22))
    fprintf(stdout,"[%02d] [%03d] : %s\n",22,sizeof(data->point_of_service_entry_mode)-1,data->point_of_service_entry_mode);

  if(check_bitmap(data,23))
    fprintf(stdout,"[%02d] [%03d] : %s\n",23,sizeof(data->card_sequence_number)-1,data->card_sequence_number);

  if(check_bitmap(data,24))
    fprintf(stdout,"[%02d] [%03d] : %s\n",24,sizeof(data->network_international_id)-1,data->network_international_id);

  if(check_bitmap(data,25))
    fprintf(stdout,"[%02d] [%03d] : %s\n",25,sizeof(data->point_of_service_condition_code)-1,data->point_of_service_condition_code);

  if(check_bitmap(data,26))
    fprintf(stdout,"[%02d] [%03d] : %s\n",26,sizeof(data->point_of_service_PIN_capture_code)-1,data->point_of_service_PIN_capture_code);

  if(check_bitmap(data,27))
    fprintf(stdout,"[%02d] [%03d] : %s\n",27,sizeof(data->auth_id_response_length)-1,data->auth_id_response_length);

  if(check_bitmap(data,28))
    fprintf(stdout,"[%02d] [%03d] : %s\n",28,sizeof(data->amount_of_transaction_fee)-1,data->amount_of_transaction_fee);

  if(check_bitmap(data,29))
    fprintf(stdout,"[%02d] [%03d] : %s\n",29,sizeof(data->amount_of_settlement_fee)-1,data->amount_of_settlement_fee);

  if(check_bitmap(data,30))
    fprintf(stdout,"[%02d] [%03d] : %s\n",30,sizeof(data->amount_of_transaction_processing_fee)-1,data->amount_of_transaction_processing_fee);

  if(check_bitmap(data,31))
    fprintf(stdout,"[%02d] [%03d] : %s\n",31,sizeof(data->amount_of_settlement_processing_fee)-1,data->amount_of_settlement_processing_fee);

  if(check_bitmap(data,32))
    fprintf(stdout,"[%02d] [%03d] : %s\n",32,sizeof(data->acquiring_institution_id_code)-1,data->acquiring_institution_id_code);

  if(check_bitmap(data,33))
    fprintf(stdout,"[%02d] [%03d] : %s\n",33,sizeof(data->forwarding_institution_id_code)-1,data->forwarding_institution_id_code);

  if(check_bitmap(data,34))
    fprintf(stdout,"[%02d] [%03d] : %s\n",34,sizeof(data->extended_primary_account_number)-1,data->extended_primary_account_number);

  if(check_bitmap(data,35))
    fprintf(stdout,"[%02d] [%03d] : %s\n",35,sizeof(data->track_2_data)-1,data->track_2_data);
    //fprintf(stdout,"[%02d] [%03d] : %s\n",35,sizeof(data->track_2_data)-1,bintostr(data->track_2_data,37));

  if(check_bitmap(data,36))
    fprintf(stdout,"[%02d] [%03d] : %s\n",36,sizeof(data->track_3_data)-1,data->track_3_data);

  if(check_bitmap(data,37))
    fprintf(stdout,"[%02d] [%03d] : %s\n",37,sizeof(data->retrieval_reference_number)-1,data->retrieval_reference_number);

  if(check_bitmap(data,38))
    fprintf(stdout,"[%02d] [%03d] : %s\n",38,sizeof(data->auth_id_response)-1,data->auth_id_response);

  if(check_bitmap(data,39))
    fprintf(stdout,"[%02d] [%03d] : %s\n",39,sizeof(data->response_code)-1,data->response_code);

  if(check_bitmap(data,40))
    fprintf(stdout,"[%02d] [%03d] : %s\n",40,sizeof(data->service_restriction_code)-1,data->service_restriction_code);

  if(check_bitmap(data,41))
    fprintf(stdout,"[%02d] [%03d] : %s\n",41,sizeof(data->card_acceptor_terminal_id)-1,data->card_acceptor_terminal_id);

  if(check_bitmap(data,42))
    fprintf(stdout,"[%02d] [%03d] : %s\n",42,sizeof(data->card_acceptor_id_code)-1,data->card_acceptor_id_code);

  if(check_bitmap(data,43))
    fprintf(stdout,"[%02d] [%03d] : %s\n",43,sizeof(data->card_acceptor_name_location)-1,data->card_acceptor_name_location);

  if(check_bitmap(data,44))
    fprintf(stdout,"[%02d] [%03d] : %s\n",44,sizeof(data->additional_response_data)-1,data->additional_response_data);

  if(check_bitmap(data,45))
    fprintf(stdout,"[%02d] [%03d] : %s\n",45,sizeof(data->track_1_data)-1,data->track_1_data);

  if(check_bitmap(data,48))
    fprintf(stdout,"[%02d] [%03d] : %s\n",48,sizeof(data->private_additional_data)-1,data->private_additional_data);

  if(check_bitmap(data,49))
    fprintf(stdout,"[%02d] [%03d] : %s\n",49,sizeof(data->currency_code_of_transaction)-1,data->currency_code_of_transaction);

  if(check_bitmap(data,50))
    fprintf(stdout,"[%02d] [%03d] : %s\n",50,sizeof(data->currency_code_of_settlement)-1,data->currency_code_of_settlement);

  if(check_bitmap(data,51))
    fprintf(stdout,"[%02d] [%03d] : %s\n",51,sizeof(data->currency_code_of_cardholder_billing)-1,data->currency_code_of_cardholder_billing);

  if(check_bitmap(data,52))
    fprintf(stdout,"[%02d] [%03d] : %s\n",52,sizeof(data->PIN)-1,bintostr(data->PIN,8));

  if(check_bitmap(data,53))
    fprintf(stdout,"[%02d] [%03d] : %s\n",53,sizeof(data->security_related_control_info)-1,data->security_related_control_info);

  if(check_bitmap(data,54))
    fprintf(stdout,"[%02d] [%03d] : %s\n",54,sizeof(data->balance_amounts)-1,data->balance_amounts);

  if(check_bitmap(data,55))
    fprintf(stdout,"[%02d] [%03d] : %s\n",55,sizeof(data->pboc_ic_transaction_information)-1,data->pboc_ic_transaction_information);

  if(check_bitmap(data,58))
    fprintf(stdout,"[%02d] [%03d] : %s\n",58,sizeof(data->pboc_electronic_data)-1,data->pboc_electronic_data);

  if(check_bitmap(data,60))
    fprintf(stdout,"[%02d] [%03d] : %s\n",60,sizeof(data->reserved_private_data_60)-1,data->reserved_private_data_60);

  if(check_bitmap(data,61))
    fprintf(stdout,"[%02d] [%03d] : %s\n",61,sizeof(data->reserved_private_data_61)-1,data->reserved_private_data_61);

  if(check_bitmap(data,62))
    fprintf(stdout,"[%02d] [%03d] : %s\n",62,sizeof(data->reserved_private_data_62)-1,bintostr(data->reserved_private_data_62,24));

  if(check_bitmap(data,63))
    fprintf(stdout,"[%02d] [%03d] : %s\n",63,sizeof(data->reserved_private_data_63)-1,data->reserved_private_data_63);

  if(check_bitmap(data,64))
    fprintf(stdout,"[%02d] [%03d] : %s\n",64,sizeof(data->MAC64)-1,data->MAC64);

  fprintf(stdout,"*******************************************************************************\n\n");
  //fclose(fp);
  return 0;
}

