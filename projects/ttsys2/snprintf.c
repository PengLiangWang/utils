#include <stdarg.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <math.h>
#include <limits.h> /*����ULONG_MAX*/
#include "ttdef.h"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#define NUL '\0'

typedef enum
{
  NO = 0, YES = 1
} boolean_e ;

#define NDIG 80

#define S_NULL "(null)"
#define S_NULL_LEN 6

#define FLOAT_DIGITS  6
#define EXPONENT_LENGTH  10

static char *apr_cvt( double arg , int ndigits , int *decpt , int *sign , int eflag , char *buf )
{
  register int r2 ;
  double fi, fj ;
  register char *p, *p1 ;

  if ( ndigits >= NDIG - 1 )
  {
    ndigits = NDIG - 2 ;
  }
  r2 = 0 ;
  *sign = 0 ;
  p = &buf[0] ;
  if ( arg < 0 )
  {
    *sign = 1 ;
    arg = -arg ;
  }
  arg = modf( arg , &fi ) ;
  p1 = &buf[NDIG] ;
  /*
   * Do integer part
   */
  if ( fi != 0 )
  {
    p1 = &buf[NDIG] ;
    while ( p1 > &buf[0] && fi != 0 )
    {
      fj = modf( fi / 10 , &fi ) ;
      *--p1 = ( int ) ((fj + .03) * 10) + '0' ;
      r2++ ;
    }
    while ( p1 < &buf[NDIG] )
    {
      *p++ = *p1++ ;
    }
  }
  else if ( arg > 0 )
  {
    while ( (fj = arg * 10) < 1 )
    {
      arg = fj ;
      r2-- ;
    }
  }
  p1 = &buf[ndigits] ;
  if ( eflag == 0 )
  {
    p1 += r2 ;
  }
  *decpt = r2 ;
  if ( p1 < &buf[0] )
  {
    buf[0] = '\0' ;
    return (buf) ;
  }
  while ( p <= p1 && p < &buf[NDIG] )
  {
    arg *= 10 ;
    arg = modf( arg , &fj ) ;
    *p++ = ( int ) fj + '0' ;
  }
  if ( p1 >= &buf[NDIG] )
  {
    buf[NDIG - 1] = '\0' ;
    return (buf) ;
  }
  p = p1 ;
  *p1 += 5 ;
  while ( *p1 > '9' )
  {
    *p1 = '0' ;
    if ( p1 > buf )
    {
      ++*--p1 ;
    }
    else
    {
      *p1 = '1' ;
      (*decpt)++ ;
      if ( eflag == 0 )
      {
        if ( p > buf )
        {
          *p = '0' ;
        }
        p++ ;
      }
    }
  }
  *p = '\0' ;
  return (buf) ;
}

static char *apr_ecvt( double arg , int ndigits , int *decpt , int *sign , char *buf )
{
  return (apr_cvt( arg , ndigits , decpt , sign , 1 , buf )) ;
}
static char *apr_fcvt(double arg, int ndigits, int *decpt, int *sign, char *buf)
{
    return (apr_cvt(arg, ndigits, decpt, sign, 0, buf));
}


static char *apr_gcvt( double number , int ndigit , char *buf , boolean_e altform )
{
  int sign, decpt ;
  register char *p1, *p2 ;
  register int i ;
  char buf1[NDIG] ;

  p1 = apr_ecvt( number , ndigit , &decpt , &sign , buf1 ) ;
  p2 = buf ;
  if ( sign )
  {
    *p2++ = '-' ;
  }
  for ( i = ndigit - 1 ; i > 0 && p1[i] == '0' ; i-- )
  {
    ndigit-- ;
  }
  if ( (decpt >= 0 && decpt - ndigit > 4) || (decpt < 0 && decpt < -3) )
  {
    /* use E-style */
    decpt-- ;
    *p2++ = *p1++ ;
    *p2++ = '.' ;
    for ( i = 1 ; i < ndigit ; i++ )
    {
      *p2++ = *p1++ ;
    }
    *p2++ = 'e' ;
    if ( decpt < 0 )
    {
      decpt = -decpt ;
      *p2++ = '-' ;
    }
    else
    {
      *p2++ = '+' ;
    }
    if ( decpt / 100 > 0 )
    {
      *p2++ = decpt / 100 + '0' ;
    }
    if ( decpt / 10 > 0 )
    {
      *p2++ = (decpt % 100) / 10 + '0' ;
    }
    *p2++ = decpt % 10 + '0' ;
  }
  else
  {
    if ( decpt <= 0 )
    {
      if ( *p1 != '0' )
      {
        *p2++ = '.' ;
      }
      while ( decpt < 0 )
      {
        decpt++ ;
        *p2++ = '0' ;
      }
    }
    for ( i = 1 ; i <= ndigit ; i++ )
    {
      *p2++ = *p1++ ;
      if ( i == decpt )
      {
        *p2++ = '.' ;
      }
    }
    if ( ndigit < decpt )
    {
      while ( ndigit++ < decpt )
      {
        *p2++ = '0' ;
      }
      *p2++ = '.' ;
    }
  }
  if ( p2[-1] == '.' && !altform )
  {
    p2-- ;
  }
  *p2 = '\0' ;
  return (buf) ;
}

#define INS_CHAR(c, sp, bep, cc)                    \
{                                                   \
  if (sp)                                           \
  {                                                 \
    if (sp >= bep)                                  \
    {                                               \
      vbuff->curpos = sp;                           \
      if (flush_func(vbuff))                        \
      {                                             \
        return -1;                                  \
      }                                             \
      sp = vbuff->curpos;                           \
      bep = vbuff->endpos;                          \
    }                                               \
    *sp++ = (c);                                    \
  }                                                 \
  cc++;                                             \
}

#define NUM(c)   (c - '0')

#define STR_TO_DEC(str, num)                        \
  num = NUM(*str++);                                \
  while (m_isdigit(*str))                         \
  {                                                 \
    num *= 10 ;                                     \
    num += NUM(*str++);                             \
  }

#define NUM_BUF_SIZE 512

#define FIX_PRECISION(adjust, precision, s, s_len)  \
  if (adjust)                                       \
  {                                                 \
    int p = precision < NUM_BUF_SIZE - 1 ?          \
                       precision : NUM_BUF_SIZE - 1;\
    while (s_len < p)                               \
    {                                               \
      *--s = '0';                                   \
      s_len++;                                      \
    }                                               \
  }


#define PAD(width, len, ch)                         \
do                                                  \
{                                                   \
  INS_CHAR(ch, sp, bep, cc);                        \
  width--;                                          \
} while (width > len);

#define PREFIX(str, length, ch)                     \
  *--str = ch;                                      \
  length++;                                         \
  has_prefix=YES;

typedef long wide_int ;
typedef long widest_int ;
typedef unsigned long u_wide_int ;
typedef unsigned long u_widest_int ;
typedef int bool_int ;

static char *conv_10( register wide_int num , register bool_int is_unsigned , register bool_int *is_negative ,
    char *buf_end , register int *len )
{
  register char *p = buf_end ;
  register u_wide_int magnitude ;

  if ( is_unsigned )
  {
    magnitude = ( u_wide_int ) num ;
    *is_negative = FALSE ;
  }
  else
  {
    *is_negative = (num < 0) ;

    if ( *is_negative )
    {
      wide_int t = num + 1 ;
      magnitude = ((u_wide_int) - t) + 1 ;
    }
    else
    {
      magnitude = ( u_wide_int ) num ;
    }
  }

  do
  {
    register u_wide_int new_magnitude = magnitude / 10 ;
    *--p = ( char ) (magnitude - new_magnitude * 10 + '0') ;
    magnitude = new_magnitude ;
  }
  while ( magnitude ) ;

  *len = buf_end - p ;
  return (p) ;
}

static char *conv_10_quad( widest_int num , register bool_int is_unsigned , register bool_int *is_negative ,
    char *buf_end , register int *len )
{
  register char *p = buf_end ;
  u_widest_int magnitude ;

  if ( (num <= ULONG_MAX && is_unsigned) || (num <= LONG_MAX && !is_unsigned) )
  {
    return (conv_10( ( wide_int ) num , is_unsigned , is_negative , buf_end , len )) ;
  }

  if ( is_unsigned )
  {
    magnitude = ( u_widest_int ) num ;
    *is_negative = FALSE ;
  }
  else
  {
    *is_negative = (num < 0) ;

    if ( *is_negative )
    {
      widest_int t = num + 1 ;
      magnitude = ((u_widest_int) - t) + 1 ;
    }
    else
    {
      magnitude = ( u_widest_int ) num ;
    }
  }

  do
  {
    u_widest_int new_magnitude = magnitude / 10 ;
    *--p = ( char ) (magnitude - new_magnitude * 10 + '0') ;
    magnitude = new_magnitude ;
  }
  while ( magnitude ) ;

  *len = buf_end - p ;
  return (p) ;
}

static char *conv_in_addr( struct in_addr *ia , char *buf_end , int *len )
{
  unsigned addr = ntohl( ia->s_addr ) ;
  char *p = buf_end ;
  bool_int is_negative ;
  int sub_len ;

  p = conv_10( (addr & 0x000000FF) , TRUE , &is_negative , p , &sub_len ) ;
  *--p = '.' ;
  p = conv_10( (addr & 0x0000FF00) >> 8 , TRUE , &is_negative , p , &sub_len ) ;
  *--p = '.' ;
  p = conv_10( (addr & 0x00FF0000) >> 16 , TRUE , &is_negative , p , &sub_len ) ;
  *--p = '.' ;
  p = conv_10( (addr & 0xFF000000) >> 24 , TRUE , &is_negative , p , &sub_len ) ;

  *len = buf_end - p ;
  return (p) ;
}

static char *conv_fp(register char format, register double num,
    boolean_e add_dp, int precision, bool_int *is_negative,
    char *buf, int *len)
{
    register char *s = buf;
    register char *p;
    int decimal_point;
    char buf1[NDIG];

    if (format == 'f')
	     p = (char *)apr_fcvt(num, precision, &decimal_point, is_negative, buf1);
    else			/* either e or E format */
       p = (char *)apr_ecvt(num, precision + 1, &decimal_point, is_negative, buf1);

    /*
     * Check for Infinity and NaN
     */
    if (m_isalpha(*p)) {
	*len = strlen(strcpy(buf, p));
	*is_negative = FALSE;
	return (buf);
    }

    if (format == 'f') {
	if (decimal_point <= 0) {
	    *s++ = '0';
	    if (precision > 0) {
		*s++ = '.';
		while (decimal_point++ < 0)
		    *s++ = '0';
	    }
	    else if (add_dp)
		*s++ = '.';
	}
	else {
	    while (decimal_point-- > 0)
		*s++ = *p++;
	    if (precision > 0 || add_dp)
		*s++ = '.';
	}
    }
    else {
	*s++ = *p++;
	if (precision > 0 || add_dp)
	    *s++ = '.';
    }

    /*
     * copy the rest of p, the NUL is NOT copied
     */
    while (*p)
	*s++ = *p++;

    if (format != 'f') {
	char temp[EXPONENT_LENGTH];	/* for exponent conversion */
	int t_len;
	bool_int exponent_is_negative;

	*s++ = format;		/* either e or E */
	decimal_point--;
	if (decimal_point != 0) {
	    p = conv_10((wide_int) decimal_point, FALSE, &exponent_is_negative,
			&temp[EXPONENT_LENGTH], &t_len);
	    *s++ = exponent_is_negative ? '-' : '+';

	    /*
	     * Make sure the exponent has at least 2 digits
	     */
	    if (t_len == 1)
		*s++ = '0';
	    while (t_len--)
		*s++ = *p++;
	}
	else {
	    *s++ = '+';
	    *s++ = '0';
	    *s++ = '0';
	}
    }

    *len = s - buf;
    return (buf);
}


/*
 * Convert num to a base X number where X is a power of 2. nbits determines X.
 * For example, if nbits is 3, we do base 8 conversion
 * Return value:
 *      a pointer to a string containing the number
 *
 * The caller provides a buffer for the string: that is the buf_end argument
 * which is a pointer to the END of the buffer + 1 (i.e. if the buffer
 * is declared as buf[ 100 ], buf_end should be &buf[ 100 ])
 *
 * As with conv_10, we have a faster version which is used when
 * the number isn't quad size.
 */
static char *conv_p2( register u_wide_int num , register int nbits , char format , char *buf_end , register int *len )
{
  register int mask = (1 << nbits) - 1 ;
  register char *p = buf_end ;
  static const char low_digits[] = "0123456789abcdef" ;
  static const char upper_digits[] = "0123456789ABCDEF" ;
  register const char *digits = (format == 'X') ? upper_digits : low_digits ;

  do
  {
    *--p = digits[num & mask] ;
    num >>= nbits ;
  }
  while ( num ) ;

  *len = buf_end - p ;
  return (p) ;
}

static char *conv_p2_quad( u_widest_int num , register int nbits , char format , char *buf_end , register int *len )
{
  register int mask = (1 << nbits) - 1 ;
  register char *p = buf_end ;
  static const char low_digits[] = "0123456789abcdef" ;
  static const char upper_digits[] = "0123456789ABCDEF" ;
  register const char *digits = (format == 'X') ? upper_digits : low_digits ;

  if ( num <= ULONG_MAX )
  {
    return (conv_p2( ( u_wide_int ) num , nbits , format , buf_end , len )) ;
  }

  do
  {
    *--p = digits[num & mask] ;
    num >>= nbits ;
  }
  while ( num ) ;

  *len = buf_end - p ;
  return (p) ;
}

/*
 * Do format conversion placing the output in buffer
 */
int m_vformatter( int(*flush_func)( mvformatter_buff_t * ) , mvformatter_buff_t *vbuff , const char *fmt ,
    va_list ap )
{
  register char *sp ;
  register char *bep ;
  register int cc = 0 ;
  register int i ;

  register char *s = NULL ;
  char *q ;
  int s_len ;

  register int min_width = 0 ;
  int precision = 0 ;

  enum
  {
    LEFT, RIGHT
  } adjust ;
  char pad_char ;
  char prefix_char ;

  double fp_num ;
  widest_int i_quad = ( widest_int ) 0 ;
  u_widest_int ui_quad ;
  wide_int i_num = ( wide_int ) 0 ;
  u_wide_int ui_num ;

  char num_buf [NUM_BUF_SIZE] ;
  char char_buf [2] ; /* for printing %% and %<unknown> */

  enum var_type_enum
  {
    IS_QUAD, IS_LONG, IS_SHORT, IS_INT
  } ;
  enum var_type_enum var_type = IS_INT ;


  boolean_e alternate_form ;
  boolean_e print_sign ;
  boolean_e print_blank ;
  boolean_e adjust_precision ;
  boolean_e adjust_width ;
  bool_int is_negative ;

  sp = vbuff->curpos ;
  bep = vbuff->endpos ;

  while ( *fmt )
  {
    if ( *fmt != '%' )
    {
      INS_CHAR( *fmt , sp , bep , cc ) ;
    }
    else
    {
      boolean_e print_something = YES ;
      adjust = RIGHT ;
      alternate_form = print_sign = print_blank = NO ;
      pad_char = ' ' ;
      prefix_char = NUL ;

      fmt++ ;
      
      if ( !m_islower( *fmt ) )
      {
        for ( ; ; fmt++ )
        {
          if ( *fmt == '-' )
            adjust = LEFT ;
          else if ( *fmt == '+' )
            print_sign = YES ;
          else if ( *fmt == '#' )
            alternate_form = YES ;
          else if ( *fmt == ' ' )
            print_blank = YES ;
          else if ( *fmt == '0' )
            pad_char = '0' ;
          else
            break ;
        }
        
        if ( m_isdigit( *fmt ) )
        {
          STR_TO_DEC( fmt , min_width ) ;
          adjust_width = YES ;
        }
        else if ( *fmt == '*' )
        {
          min_width = va_arg(ap, int) ;
          fmt++ ;
          adjust_width = YES ;
          if ( min_width < 0 )
          {
            adjust = LEFT ;
            min_width = -min_width ;
          }
        }
        else
          adjust_width = NO ;

        /*
         * Check if a precision was specified
         */
        if ( *fmt == '.' )
        {
          adjust_precision = YES ;
          fmt++ ;
          if ( m_isdigit( *fmt ) )
          {
            STR_TO_DEC( fmt , precision ) ;
          }
          else if ( *fmt == '*' )
          {
            precision = va_arg(ap, int) ;
            fmt++ ;
            if ( precision < 0 )
              precision = 0 ;
          }
          else
            precision = 0 ;
        }
        else
          adjust_precision = NO ;
      }
      else
      {
        adjust_precision = adjust_width = NO ;
      }

      /*
       * Modifier check
       */
      if ( *fmt == 'q' )
      {
        var_type = IS_QUAD ;
        fmt++ ;
      }
      else if ( *fmt == 'l' )
      {
        var_type = IS_LONG ;
        fmt++ ;
      }
      else if ( *fmt == 'h' )
      {
        var_type = IS_SHORT ;
        fmt++ ;
      }
      else
      {
        var_type = IS_INT ;
      }

      /*
       * Argument extraction and printing.
       * First we determine the argument type.
       * Then, we convert the argument to a string.
       * On exit from the switch, s points to the string that
       * must be printed, s_len has the length of the string
       * The precision requirements, if any, are reflected in s_len.
       *
       * NOTE: pad_char may be set to '0' because of the 0 flag.
       *   It is reset to ' ' by non-numeric formats
       */
      switch ( *fmt )
      {
        case 'u' :
          if ( var_type == IS_QUAD )
          {
            i_quad = va_arg( ap , u_widest_int ) ;
            s = conv_10_quad( i_quad , 1 , &is_negative , &num_buf [NUM_BUF_SIZE] , &s_len ) ;
          }
          else
          {
            if (var_type == IS_LONG)
              i_num = (wide_int) va_arg(ap, u_wide_int) ;
            else if (var_type == IS_SHORT)
              i_num = (wide_int) (unsigned short) va_arg(ap, unsigned int) ;
            else
              i_num = (wide_int) va_arg(ap, unsigned int) ;
            s = conv_10( i_num , 1 , &is_negative , &num_buf [NUM_BUF_SIZE] , &s_len ) ;
          }
          FIX_PRECISION( adjust_precision , precision , s , s_len ) ;
          break ;

        case 'd' :
        case 'i' :
          if ( var_type == IS_QUAD )
          {
            i_quad = va_arg( ap , widest_int ) ;
            s = conv_10_quad( i_quad , 0 , &is_negative , &num_buf [NUM_BUF_SIZE] , &s_len ) ;
          }
          else
          {
            if (var_type == IS_LONG)
              i_num = (wide_int) va_arg(ap, wide_int) ;
            else if (var_type == IS_SHORT)
              i_num = (wide_int) (short) va_arg(ap, int) ;
            else
              i_num = (wide_int) va_arg(ap, int) ;
            s = conv_10( i_num , 0 , &is_negative , &num_buf [NUM_BUF_SIZE] , &s_len ) ;
          }
          FIX_PRECISION( adjust_precision , precision , s , s_len ) ;

          if ( is_negative )
            prefix_char = '-' ;
          else if ( print_sign )
            prefix_char = '+' ;
          else if ( print_blank )
            prefix_char = ' ' ;

          break ;

        case 'o' :
          if ( var_type == IS_QUAD )
          {
            ui_quad = va_arg( ap , u_widest_int ) ;
            s = conv_p2_quad( ui_quad , 3 , *fmt , &num_buf [NUM_BUF_SIZE] , &s_len ) ;
          }
          else
          {
            if (var_type == IS_LONG)
              ui_num = (u_wide_int) va_arg(ap, u_wide_int) ;
            else if (var_type == IS_SHORT)
              ui_num = (u_wide_int) (unsigned short) va_arg(ap, unsigned int) ;
            else
              ui_num = (u_wide_int) va_arg(ap, unsigned int) ;

            s = conv_p2( ui_num , 3 , *fmt , &num_buf [NUM_BUF_SIZE] , &s_len ) ;
          }
          FIX_PRECISION( adjust_precision , precision , s , s_len ) ;

          if ( alternate_form && *s != '0' )
          {
            *--s = '0' ;
            s_len++ ;
          }
          break ;

        case 'x' :
        case 'X' :
          if ( var_type == IS_QUAD )
          {
            ui_quad = va_arg( ap , u_widest_int ) ;
            s = conv_p2_quad( ui_quad , 4 , *fmt , &num_buf [NUM_BUF_SIZE] , &s_len ) ;
          }
          else
          {
            if (var_type == IS_LONG)
              ui_num = (u_wide_int) va_arg(ap, u_wide_int) ;
            else if (var_type == IS_SHORT)
              ui_num = (u_wide_int) (unsigned short) va_arg(ap, unsigned int) ;
            else
              ui_num = (u_wide_int) va_arg(ap, unsigned int) ;
            s = conv_p2( ui_num , 4 , *fmt , &num_buf [NUM_BUF_SIZE] , &s_len ) ;
          }

          FIX_PRECISION( adjust_precision , precision , s , s_len ) ;
          if ( alternate_form && i_num != 0 )
          {
            *--s = *fmt ; /* 'x' or 'X' */
            *--s = '0' ;
            s_len += 2 ;
          }
          break ;

        case 's' :
          s = va_arg(ap, char *) ;
          if ( s != NULL )
          {
            if ( !adjust_precision )
            {
              s_len = strlen( s ) ;
            }
            else
            {
              const char *walk ;

              for ( walk = s, s_len = 0 ; ( s_len < precision ) && ( *walk != '\0' ) ; ++walk, ++s_len );
            }
          }
          else
          {
            s = S_NULL ;
            s_len = S_NULL_LEN ;
          }
          //pad_char = ' ' ;
          break ;
        case 'f' :
        case 'e':
        case 'E':
          fp_num = va_arg(ap, double);
          {
            s = conv_fp(*fmt, fp_num, alternate_form,(adjust_precision == NO) ? FLOAT_DIGITS : precision,&is_negative, &num_buf[1], &s_len);
		        if (is_negative)
			        prefix_char = '-';
		        else if (print_sign)
			        prefix_char = '+';
		        else if (print_blank)
			        prefix_char = ' ';
		       }
           break;
        case 'g' :
        case 'G' :
          if ( adjust_precision == NO )
            precision = FLOAT_DIGITS ;
          else if ( precision == 0 )
            precision = 1 ;

          /*
           * * We use &num_buf[ 1 ], so that we have room for the sign
           */

          s = apr_gcvt(va_arg(ap, double), precision, &num_buf[1],alternate_form) ;
          if ( *s == '-' )
            prefix_char = *s++ ;
          else if ( print_sign )
            prefix_char = '+' ;
          else if ( print_blank )
            prefix_char = ' ' ;

          s_len = strlen( s ) ;

          if ( alternate_form && ( q = ( char* ) strchr( s , '.' ) ) == NULL )
          {
            s [s_len++] = '.' ;
            s [s_len] = '\0' ; /* delimit for following strchr() */
          }
          if ( *fmt == 'G' && ( q = ( char* ) strchr( s , 'e' ) ) != NULL )
            *q = 'E' ;
          break ;

        case 'c' :
          char_buf[0] = (char) (va_arg(ap, int)) ;
          s = &char_buf [0] ;
          s_len = 1 ;
          pad_char = ' ' ;
          break ;

        case '%' :
          char_buf [0] = '%' ;
          s = &char_buf [0] ;
          s_len = 1 ;
          pad_char = ' ' ;
          break ;

        case 'n' :
          if ( var_type == IS_QUAD )
            *( va_arg( ap , widest_int * )) = cc ;
          else if ( var_type == IS_LONG )
            *( va_arg( ap , long * )) = cc ;
          else if ( var_type == IS_SHORT )
            *( va_arg( ap , short * )) = cc ;
          else
            *( va_arg( ap , int * )) = cc ;
          print_something = NO ;
          break ;

        case NUL :
          /*
           * The last character of the format string was %.
           * We ignore it.
           */
          continue ;

          /*
           * The default case is for unrecognized %'s.
           * We print %<char> to help the user identify what
           * option is not understood.
           * This is also useful in case the user wants to pass
           * the output of format_converter to another function
           * that understands some other %<char> (like syslog).
           * Note that we can't point s inside fmt because the
           * unknown <char> could be preceded by width etc.
           */
        default :
          char_buf [0] = '%' ;
          char_buf [1] = *fmt ;
          s = char_buf ;
          s_len = 2 ;
          pad_char = ' ' ;

          break ;
      }

      if ( prefix_char != NUL && s != S_NULL && s != char_buf )
      {
        *--s = prefix_char ;
        s_len++ ;
      }

      if ( adjust_width && adjust == RIGHT && min_width > s_len )
      {
        if ( pad_char == '0' && prefix_char != NUL )
        {
          INS_CHAR( *s , sp , bep , cc ) ;
          s++ ;
          s_len-- ;
          min_width-- ;
        }
        PAD( min_width , s_len , pad_char ) ;
      }

    
      if ( print_something == YES )
      {
        for ( i = s_len ; i != 0 ; i-- )
        {
          INS_CHAR( *s , sp , bep , cc ) ;
          s++ ;
        }
      }

      if ( adjust_width && adjust == LEFT && min_width > s_len )
      {
        PAD( min_width , s_len , pad_char ) ;
      }
    }
    fmt++ ;
  }

  vbuff->curpos = sp ;

  return cc ;
  
}

