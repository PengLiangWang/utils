#include <string.h>
#include "exml.h"
#include "expat.h"
#include "xlate.h"
#include "file.h"


#define XML_NEED_GBK

struct EXmlParser
{
  EXmlDoc        *doc;
  TMemPool       *p;
  EXmlElement    *cur_elem;
  int             error;
#define EXML_ERROR_EXPAT             1
#define EXML_ERROR_PARSE_DONE        2
  XML_Parser      xp;

#ifdef XML_NEED_GBK
  TXlate          *convset;
#endif

  enum XML_Error  xp_err;
};


/**
 * EXML文档结构定义
 */
struct EXmlDoc 
{
  const  char  *version;         /**< EXML文档的版本 */     
  const  char  *encoding;        /**< EXML文档编码方式(只支持UTF-8) */
  EXmlElement  *root;            /**< EXML文档的根节点 */
};


#define EXML_NS_ERROR_UNKNOWN_PREFIX   (-1000)
#define EXML_NS_ERROR_INVALID_DECL     (-1001)




EXmlDoc *EXmlGetDocument(EXmlParser *parser)
{
  return parser->doc;
}

static void start_handler(void *userdata, const char *name, const char **attrs)
{
  EXmlParser     *parser = userdata;
  EXmlElement    *elem;
  EXmlAttr       *attr;
  EXmlAttr       *prev;
  char           *colon;
  const char     *quoted;
  char           *elem_name;

  if (parser->error)
  {
    return ;
  }

  elem = MemPoolAllocEx(parser->p, sizeof(*elem));

  elem->name = elem_name = (char *)m_strdup(parser->p, name);

  while (*attrs)
  {
    attr = MemPoolAllocEx(parser->p, sizeof(*attr));
    attr->name = (char *)m_strdup(parser->p, *attrs++);
    attr->value = (char *)m_strdup(parser->p, *attrs++);
    attr->next = elem->attr;
    elem->attr = attr;
  }

  if (parser->cur_elem == NULL)
  {
    parser->cur_elem = parser->doc->root = elem;
  }
  else
  {
    elem->parent = parser->cur_elem;

    if (elem->parent->last_child == NULL)
    {
      elem->parent->first_child = elem->parent->last_child = elem;
    }
    else
    {
      elem->parent->last_child->next = elem;
      elem->parent->last_child = elem;
    }
    parser->cur_elem = elem;
  }
}

static void end_handler(void *userdata, const char *name)
{
  EXmlParser *parser = (EXmlParser *)userdata;

  if (parser->error)
  {
    return;
  }

  parser->cur_elem = parser->cur_elem->parent;
}

static int isspacedata(const char *data,int len)
{
  int  i;

  for (i = 0;i < len ;i++ )
  {
    if ( !isspace(data[i]) )
    {
      return 0;
    }
  }

  return 1;
}

static void cdata_handler(void *userdata, const char *data, int len)
{
  EXmlParser     *parser = (EXmlParser *)userdata;
  EXmlElement    *elem;
  EXmlTextHeader *hdr;
  char           *s;
  uint32        dlen;

  if (parser->error)
  {
    return;
  }

  if ( isspacedata(data,len) )
  {
    return ;
  }

  elem = parser->cur_elem;

#ifdef XML_NEED_GBK
  if (XlateConvBuffer(parser->convset,data,(uint32 *)&len,(char **)&s,&dlen) != TTS_SUCCESS)
  {
    parser->error =  EXML_ERROR_EXPAT;
    return ;
  }
#else
  s = m_strndup(parser->p, data, len);
#endif

  if ( elem->text )
  {
    elem->text = (char *)m_strcat(parser->p,elem->text,s,NULL);
  }
  else
  {
    elem->text = s;
  }
}


static int cleanup_parser(void *ctx)
{
  EXmlParser *parser = (EXmlParser *)ctx;

  XML_ParserFree(parser->xp);

  parser->xp = NULL;

  return TTS_SUCCESS;
}

EXmlParser * EXmlParserCreate(TMemPool *pool)
{
  EXmlParser *parser = MemPoolAllocEx(pool, sizeof(EXmlParser));

  parser->p = pool;
  parser->doc = MemPoolAllocEx(pool, sizeof(*parser->doc));
 
#ifdef XML_NEED_GBK
  {
    int  rv = XlateOpen(&parser->convset,"GBK","UTF-8",pool);
    if (rv != TTS_SUCCESS)
    {
      return NULL;
    }
  }
#endif
  
  parser->xp = XML_ParserCreate(NULL);
  if (parser->xp == NULL)
  {
    return NULL;
  }
  
  MemPoolRegisterCleanup(pool, parser, cleanup_parser);

  XML_SetUserData(parser->xp, parser);
  XML_SetElementHandler(parser->xp, start_handler, end_handler);
  XML_SetCharacterDataHandler(parser->xp, cdata_handler);

  return parser;
}


static int do_parse(EXmlParser *parser,const char *data, uint32 len,int is_final)
{
  char     *udata;
  uint32    ulen;

  if (parser->xp == NULL)
  {
    parser->error = EXML_ERROR_PARSE_DONE;
  }
  else
  {
    int   rv;

#ifdef XML_NEED_GBK
    if (len <= 0)
    {
      udata = (char *)data;
      ulen = len;
    }
    else
    {
      TXlate   *convset;
      
      rv = XlateOpen(&convset,"UTF-8","GBK",parser->p);
      if (rv != TTS_SUCCESS)
      {
        return rv;
      }

      rv = XlateConvBuffer(convset,data,&len,(char **)&udata,&ulen);
      if (rv != TTS_SUCCESS)
      {
        return rv;
      }
    }
#else
    {
      udata = data;
      ulen = len;
    }
#endif

    rv = XML_Parse(parser->xp, (const char *)udata, ulen, is_final);
    if (rv == 0)
    {
      parser->error = EXML_ERROR_EXPAT;
      parser->xp_err = XML_GetErrorCode(parser->xp);
    }
  }

  return parser->error ? TTS_EGENERAL : TTS_SUCCESS;
}

int EXmlParserFeed(EXmlParser *parser,const char *data,uint32 len)
{
  return do_parse(parser, data, len, 0 );
}

int EXmlParserDone(EXmlParser *parser, EXmlDoc **pdoc)
{
  char end;

  int status = do_parse(parser, &end, 0, 1 /* is_final */);

  MemPoolRunCleanup(parser->p, parser, cleanup_parser);
  
#ifdef XML_NEED_GBK
  XlateClose(parser->convset);
#endif

  if (status)
  {
    return status;
  }

  if (pdoc != NULL)
  {
    *pdoc = parser->doc;
  }

  return TTS_SUCCESS;
}

int EXmlDocParserFile(const char *filename,EXmlDoc **doc,TMemPool *pool,uint32 blen)
{
  int          rv;
  char        *buffer;
  uint32       len;
  TFile       *file;
  EXmlParser  *parser = EXmlParserCreate(pool);

  if (parser == NULL)
  {
    return TTS_ENOMEM;
  }

  
  rv = FileOpen(&file,filename,O_RDONLY,pool);
  if ( rv != TTS_SUCCESS )
  {
    return rv;
  }

  buffer = MemPoolAllocEx(pool,blen);

  len = blen;

  rv = FileRead(file,buffer,&len);

  while ( rv == TTS_SUCCESS )
  {
    rv = EXmlParserFeed(parser, (const char *)buffer, len);
    if ( rv != TTS_SUCCESS )
    {
      FileClose(file);
      return rv;
    }
    
    len = blen;
    memset(buffer,0,blen);
    rv = FileRead(file,buffer,&len);
  }
  
  FileClose(file);

  if ( rv != TTS_EOF )
  {
    return rv;
  }

  return EXmlParserDone(parser, doc);
}


static uint32 text_size(const char *t)
{
  if (t)
  {
    return strlen(t);
  }
  
  return 0;
  
}

uint32 elem_size(const EXmlElement *elem)
{
  uint32 size;

  const EXmlAttr *attr;

  size = 0;

  size += 1 + strlen(elem->name) + 1;

  if (EXML_ELEM_IS_EMPTY(elem))
  {
    size += 1;
  }
  else
  {
    size = 2 * size + 1;
  }

  for (attr = elem->attr; attr; attr = attr->next)
  {
    size += 1 + strlen(attr->name) + 2 + strlen(attr->value) + 1;
  }

  size += text_size(elem->text);

  for (elem = elem->first_child; elem; elem = elem->next)
  {
    size += elem_size(elem);
  }

  return size;
}


static char *write_text(char *s, const char *text)
{
  if ( text  )
  {
    uint32 len = strlen(text);
    memcpy(s, text, len);
    s += len;
  }
  return s;
}

static char *write_elem(char *s, const EXmlElement *elem)
{
  const EXmlElement  *child;
  uint32            len;
  int                 empty;
  const EXmlAttr     *attr;

  empty = EXML_ELEM_IS_EMPTY(elem);

  len = sprintf(s, "<%s", elem->name);

  s += len;

  for (attr = elem->attr; attr; attr = attr->next)
  {
    len = sprintf(s, " %s=\"%s\"", attr->name, attr->value);
    s += len;
  }

  if (empty)
  {
    *s++ = '/';
    *s++ = '>';
    return s;
  }

  *s++ = '>';

  s = write_text(s, elem->text);

  for ( child = elem->first_child; child; child = child->next )
  {
    s = write_elem(s, child );
  }

  len = sprintf(s, "</%s>", elem->name);

  s += len;

  return s;
}


int EXmlDocToText(TMemPool * pool, EXmlDoc *doc,char **buffer,uint32 *size)
{
  uint32  esize=0,esize2 = 0;
  char   *str;
  static  char  xmlheader[64];
  
  if ( doc->version && doc->encoding )
  {
    memset(xmlheader, 0, sizeof(xmlheader));
    snprintf(xmlheader, 64, "<?xml version=\"%s\" encoding=\"%s\" ?>", doc->version, doc->encoding);
    esize2 = strlen(xmlheader);
    esize = esize2 + elem_size(doc->root) + 1;
    str = MemPoolAllocEx(pool, esize);
    if ( str == NULL )
    {
      return TTS_ENOMEM;
    }
    strncpy(str, xmlheader, esize2);
  }
  else
  {
    esize = elem_size(doc->root) + 1;
    str = MemPoolAllocEx(pool, esize);
    if ( str == NULL )
    {
      return TTS_ENOMEM;
    }
  }

  write_elem(str + esize2, (doc->root));
  str[esize - 1] = '\0';

  if ( size )
  {
    *size = esize - 1;
  }

  *buffer = str;

  return TTS_SUCCESS;
}

int EXmlDocParseText(TMemPool *pool,EXmlDoc **doc,const char *buffer,uint32 size)
{
  int          rv;
  uint32       len;
  EXmlParser  *parser = EXmlParserCreate(pool);

  if (parser == NULL)
  {
    return TTS_EGENERAL;
  }

  rv = EXmlParserFeed(parser,buffer, size);
  if ( rv != TTS_SUCCESS )
  {
    return rv;
  }

  return EXmlParserDone(parser, doc);
}


int EXmlDocCreate(TMemPool *pool,const char *name,EXmlDoc **doc)
{
  EXmlDoc  *new = MemPoolAllocEx(pool,sizeof(EXmlDoc));
  if ( new == NULL )
  {
    return TTS_ENOMEM;
  }

  new->root = MemPoolAllocEx(pool,sizeof(EXmlElement));

  if ( new->root == NULL )
  {
    return TTS_ENOMEM;
  }
  
  new->root->name = (char *)m_strdup(pool, name);

  *doc = new;

  return TTS_SUCCESS ;
}

int EXmlDocCreateEx(TMemPool *pool,const char *name,const char *version, const char *encoding, EXmlDoc **doc)
{
  EXmlDoc  *new = MemPoolAllocEx(pool,sizeof(EXmlDoc));
  if ( new == NULL )
  {
    return TTS_ENOMEM;
  }

  new->root = MemPoolAllocEx(pool,sizeof(EXmlElement));

  if ( new->root == NULL )
  {
    return TTS_ENOMEM;
  }
  
  new->root->name = (char *)m_strdup(pool, name);

  new->version = (char *)m_strndup(pool,version,strlen(version));

  new->encoding = (char *)m_strndup(pool,encoding,strlen(encoding));

  *doc = new;

  return TTS_SUCCESS ;
}

int EXmlDocAddElement(TMemPool *pool,EXmlDoc *doc,const char *name,const char *data,EXmlElement **new)
{
  EXmlElement *root ;
  EXmlElement *elem ;

  
  if ( doc == NULL )
  {
    return TTS_ENULL;
  }

  root = doc->root;

  elem = MemPoolAllocEx(pool,sizeof(*elem));
  if ( elem == NULL )
  {
    return TTS_ENOMEM;
  }

  elem->name = (char *)m_strdup(pool, name);

  if ( data != NULL )
  {
    elem->text = (char *)m_strndup(pool,data,strlen(data));
  }

  if ( root == NULL || elem == NULL )
  {
    return TTS_ENULL;
  }

  if (root->last_child == NULL)
  {
     root->first_child = root->last_child = elem;
  }
  else
  {
    root->last_child->next = elem;
    root->last_child = elem;
  }

  if ( new != NULL )
  {
    *new = elem;
  }

  return TTS_SUCCESS;
}


EXmlElement *EXmlDocGetRootElement(EXmlDoc *doc)
{
  if ( doc != NULL )
  {
    return doc->root;
  }

  return NULL;
}

EXmlElement *EXmlDocGetNodeLists(const char *name,EXmlDoc *doc)
{
  return EXmlElementGetChildNode(name,doc->root);
}

EXmlElement *EXmlElementGetChildNode(const char *name,EXmlElement *elem)
{
  EXmlElement *el,*step = elem;
  
  if ( m_strcmp((char*)(step->name),(char*)name) == 0 )
  {
    return step;
  }
  for (step = step->first_child; step != NULL; step = step->next)
  {
    el = EXmlElementGetChildNode(name,step);
    if (el != NULL)
    {
      return el;
    }
  }

  return NULL;
}

char *EXmlElementGetNodeData(TMemPool *pool,EXmlElement *elem)
{
  char  *s;
  int    len;

  len = text_size(elem->text);

  if (len <= 0)
  {
    return NULL;
  }
  
  s = (char *)m_strndup(pool,elem->text,len);

  return s;
}

char *EXmlElementGetChildNodeData(TMemPool *pool,EXmlElement *elem,const char *name)
{
  EXmlElement  *step = elem;
  char         *s,*p;
  int           len;

  for ( step = step->first_child; step != NULL; step = step->next)
  {
    if (m_strcmp((char*)(step->name),(char*)name) == 0)
    {
      break;
    }
  }

  if ( step == NULL )
  {
    return NULL;
  }

  len = text_size(step->text) ;

  if ( len <= 0 )
  {
    return NULL;
  }

  
  s = (char *)m_strndup(pool,step->text,len);

  return s;
}

const char *EXmlElementGetAttr(const char *name,EXmlElement *elem)
{
  EXmlAttr  *attr = elem->attr;

  for (attr = elem->attr; attr != NULL; attr = attr->next)
  {
    if ( m_strcmp((char*)(attr->name),(char*)name) == 0 )
    {
      return attr->value;
    }
  }

  return NULL;
}

EXmlElement * EXmlElementCreate(TMemPool *pool,const char *name,const char *data)
{
  EXmlElement *elem = MemPoolAllocEx(pool,sizeof(*elem));

  if ( elem == NULL )
  {
    return NULL;
  }

  elem->name = (char *)m_strdup(pool, name);

  if (data != NULL)
  {
    elem->text = (char *)m_strndup(pool,data,strlen(data));
  }

  return elem;
}

int EXmlElementSetAttr(TMemPool *pool,EXmlElement *elem,const char *name,const char *data)
{
  EXmlAttr *attr;

  attr = MemPoolAllocEx(pool, sizeof(*attr));
  if (attr == NULL)
  {
    return TTS_ENOMEM;
  }
  attr->name = (char *)m_strdup(pool, name);
  attr->value =(char *)m_strdup(pool, data);
  attr->next = elem->attr;
  elem->attr = attr;
  return TTS_SUCCESS;
}

int EXmlElemenAddChildNode(EXmlElement *parent,EXmlElement *elem)
{
  if (parent == NULL || elem == NULL)
  {
    return TTS_EGENERAL;
  }

  if (parent->last_child == NULL)
  {
     parent->first_child = parent->last_child = elem;
  }
  else
  {
    parent->last_child->next = elem;
    parent->last_child = elem;
  }
  return TTS_SUCCESS;
}

const char * EXmlQuoteString(TMemPool *p, const char *s,int quotes)
{
  const char   *scan;
  uint32      len = 0;
  uint32      extra = 0;
  char         *qstr;
  char         *qscan;
  char          c;

  for (scan = s; (c = *scan) != '\0'; ++scan, ++len)
  {
    if (c == '<' || c == '>')
    {
      extra += 3;    /* &lt; or &gt; */
    }
    else if (c == '&')
    {
      extra += 4;    /* &amp; */
    }
    else if (quotes && c == '"')
    {
      extra += 5;    /* &quot; */
    }
  }

  /* nothing to do? */
  if (extra == 0)
  {
    return s;
  }

  qstr = MemPoolAllocEx(p, len + extra + 1);
  for (scan = s, qscan = qstr; (c = *scan) != '\0'; ++scan)
  {
    if (c == '<')
    {
      *qscan++ = '&';
      *qscan++ = 'l';
      *qscan++ = 't';
      *qscan++ = ';';
    }
    else if (c == '>')
    {
      *qscan++ = '&';
      *qscan++ = 'g';
      *qscan++ = 't';
      *qscan++ = ';';
    }
    else if (c == '&')
    {
      *qscan++ = '&';
      *qscan++ = 'a';
      *qscan++ = 'm';
      *qscan++ = 'p';
      *qscan++ = ';';
    }
    else if (quotes && c == '"')
    {
      *qscan++ = '&';
      *qscan++ = 'q';
      *qscan++ = 'u';
      *qscan++ = 'o';
      *qscan++ = 't';
      *qscan++ = ';';
    }
    else
    {
      *qscan++ = c;
    }
  }

  *qscan = '\0';
  return qstr;
}


void EXmlQuoteElemment(TMemPool *p, EXmlElement *elem)
{
  const char  *scan_txt;
  EXmlAttr    *scan_attr;
  EXmlElement *scan_elem;

  /* convert the element's text */
  /*for (scan_txt = elem->first_cdata.first;scan_txt != NULL;scan_txt = scan_txt->next)
  {
    scan_txt->text = EXmlQuoteString(p, scan_txt->text, 0);
  }
  
  for (scan_txt = elem->following_cdata.first;scan_txt != NULL;scan_txt = scan_txt->next)
  {
    scan_txt->text = EXmlQuoteString(p, scan_txt->text, 0);
  }*/

  scan_txt = EXmlQuoteString(p, elem->text, 0);

  /* convert the attribute values */
  for (scan_attr = elem->attr;scan_attr != NULL;scan_attr = scan_attr->next)
  {
    scan_attr->value = EXmlQuoteString(p, scan_attr->value, 1);
  }

  /* convert the child elements */
  for (scan_elem = elem->first_child;scan_elem != NULL;scan_elem = scan_elem->next)
  {
    EXmlQuoteElemment(p, scan_elem);
  }
}


char * EXmlParserGeterror(EXmlParser *parser,char *errbuf,uint32 errbufsize)
{
  int error = parser->error;
  const char *msg;

  /* clear our record of an error */
  parser->error = 0;

  switch (error) 
  {
    case 0:
      msg = "No error.";
      break;

    case EXML_ERROR_EXPAT:
      (void) snprintf(errbuf, errbufsize,
                          "XML parser error code: %s (%d)",
                          XML_ErrorString(parser->xp_err), parser->xp_err);
      return errbuf;

    case EXML_ERROR_PARSE_DONE:
      msg = "The parser is not active.";
      break;

    default:
      msg = "There was an unknown error within the XML body.";
      break;
  }

  (void) m_cpystrn(errbuf, msg, errbufsize);
  return errbuf;
}





/*static void print_text( const EXmlText *t)
{
  printf("    ");
  for (; t; t = t->next)
  {
    printf("%s",t->text);
  }
  printf("\n");
}*/

static char  XmlLevel[128];

void EXmlElementDebug(int level, const EXmlElement *elem,FILE *fp)
{
  const EXmlElement   *child;
  uint32               len;
  int                  empty;
  const EXmlAttr      *attr;
  
  if ( elem == NULL )
  {
    return ;
  }

  empty = EXML_ELEM_IS_EMPTY(elem);

  memset(XmlLevel, 0x20, sizeof(XmlLevel));
  XmlLevel[level*2] = 0;

  fprintf(fp,"%s<%s",XmlLevel, elem->name);

  for (attr = elem->attr; attr; attr = attr->next)
  {
    fprintf(fp," %s=\"%s\"",attr->name, attr->value);
  }

  if (empty)
  {
    fprintf(fp,"/>\n");
    return ;
  }
  if ( elem->text )
  {
     fprintf(fp,">%s" ,elem->text );
  }
  else
  {
    fprintf(fp,">\n");
  }
  

  for (child = elem->first_child; child; child = child->next)
  {
    EXmlElementDebug(level + 1, child ,fp);
  }

  if ( elem->text )
  {
    memset(XmlLevel, 0x00, sizeof(XmlLevel));
  }
  else
  {
    memset(XmlLevel, 0x20, sizeof(XmlLevel));
    XmlLevel[level*2] = 0;
  }

  fprintf(fp,"%s</%s>\n",XmlLevel,elem->name);

  return ;
}




/*************/

struct TEXml
{
   TMemPool     *pool;
   EXmlDoc      *doc;
   EXmlElement  *root;
   EXmlElement  *current;
   EXmlElement  *temp;
   char          path[128];
   int           res;
};

int EXmlMake(const char *RootName,TEXml **exml, TMemPool *MemPool)
{
  TEXml *xml;

  xml = (TEXml *)MemPoolAllocEx(MemPool,sizeof(TEXml));
  if ( xml == NULL )
  {
     return TTS_ENOMEM;
  }

  xml->res = EXmlDocCreateEx(MemPool, RootName, "1.0", "UTF-8",&(xml->doc));
  if ( xml->res != TTS_SUCCESS )
  {
    return xml->res;
  }

  xml->pool = MemPool;

  xml->root = EXmlDocGetRootElement(xml->doc);
  
  xml->current = xml->root;

  xml->temp = NULL;

  memset(xml->path, 0, sizeof(xml->path));

  *exml = xml;
  
  return TTS_SUCCESS;

}

int EXmlToText(TEXml *exml, char **buffer,uint32 *size, TMemPool *pool)
{
  return EXmlDocToText(pool, exml->doc, buffer , size);
}

int TextToEXml(char *buffer, uint32 size, TEXml **exml, TMemPool *pool)
{
  TEXml *xml;

  xml = (TEXml *)MemPoolAllocEx(pool,sizeof(TEXml));
  if ( xml == NULL )
  {
     return TTS_ENOMEM;
  }

  xml->res = EXmlDocParseText(pool, &(xml->doc), buffer, size);
  if ( xml->res )
  {
     return xml->res;
  }

  xml->root = xml->current = EXmlDocGetRootElement(xml->doc);
  
  xml->pool = pool;

  *exml = xml;

  return TTS_SUCCESS;

}

int EXmlGetValue(TEXml *exml, char *fieldPath, char *fieldName, const char **fieldValue)
{
  EXmlElement   *elem;
  static char    attr[65];
  
  memset(attr, 0, sizeof(attr));

  exml->res = TTS_SUCCESS;

  if ( fieldName == NULL )
  {
    return TTS_EINVAL;
  }

  if ( fieldPath != NULL )
  {
    
    char        *path;
    char        *elemName = NULL;
    char        *last;
    int          used = 0;

    if ( exml->path != NULL )
    {
      if ( m_strcmp(exml->path,fieldPath) == 0 )
      {
        used = 1;
        elem = exml->current;
      }
    }
    
    if ( used == 0 )
    {
      memset(exml->path, 0, sizeof(exml->path));
      strncpy(exml->path, fieldPath, 128);
      elem = exml->root;
      path = (char *)m_strdup(exml->pool, fieldPath);

      do
      {
        elemName = (char *)m_strtok(path,(const char *)"/",&last);
        if ( elemName == NULL )
        {
          break;
        }

        elem = EXmlElementGetChildNode(elemName,elem);
        if ( elem == NULL )
        {
          exml->res = TTS_NOTFOUND;
          memset(exml->path, 0, sizeof(exml->path));
          return exml->res;
        }

        path = NULL;

      }while ( elemName != NULL );

      exml->current = elem;
    }
  }

  if ( fieldName[0] == '[' && fieldName[strlen(fieldName) -1] == ']' )
  {
     strncpy(attr, fieldName + 1, strlen(fieldName) - 2);
     *fieldValue = (char *)EXmlElementGetAttr(attr, exml->current);
  }
  else
  {
    elem = EXmlElementGetChildNode(fieldName, exml->current);
    if ( elem == NULL )
    {
      exml->res = TTS_NOTFOUND;
      memset(exml->path, 0, sizeof(exml->path));
      return  exml->res;
    }
    *fieldValue = (const char *)elem->text;
  }

  return exml->res;
}


int EXmlSetValue(TEXml *exml,char *fieldPath,char *fieldName,char *fieldValue)
{
  static char  attr[65];

  memset(attr, 0, sizeof(attr));

  if ( fieldName == NULL )
  {
    return TTS_EINVAL;
  }

  if ( fieldPath != NULL )
  {
    EXmlElement *elem,*parent;
    char        *path;
    char        *elemName = NULL;
    char        *last;
    int          used = 0;

    if ( exml->path != NULL )
    {
      if ( m_strcmp(exml->path,fieldPath) == 0 )
      {
        used = 1;
      }
    }
    
    if ( used == 0 )
    {
      memset(exml->path, 0, sizeof(exml->path));
      strncpy(exml->path, fieldPath, 128);
      path = (char *)m_strdup(exml->pool, fieldPath);
      parent = elem = exml->root;

      do
      {
        elemName = (char *)m_strtok(path,(const char *)"/",&last);
        if ( elemName == NULL )
        {
          break;
        }

        elem = EXmlElementGetChildNode(elemName,elem);
        if ( elem == NULL )
        {
          elem = EXmlElementCreate(exml->pool, 
                                             (const char *)elemName,
                                             NULL);
          if ( elem == NULL )
          {
            memset(exml->path, 0, sizeof(exml->path));
            return TTS_ENULL;
          }
          
          exml->res = EXmlElemenAddChildNode(parent,elem);
          if ( exml->res != TTS_SUCCESS )
          {
            memset(exml->path, 0, sizeof(exml->path));
            return exml->res;
          }
        }

        parent = elem;

        path = NULL;

      }while ( elemName != NULL );

      exml->current = elem;
    }
  }

  if ( fieldName[0] == '[' && fieldName[strlen(fieldName) -1] == ']' )
  {
     strncpy(attr, fieldName + 1, strlen(fieldName) - 2);
     EXmlElementSetAttr(exml->pool, exml->current, attr, fieldValue);
     return TTS_SUCCESS;
  }
  else
  {
    exml->temp = EXmlElementGetChildNode(fieldName,exml->current);
    if ( exml->temp != NULL )
    {
      exml->temp->text = (char *)m_strdup(exml->pool, fieldValue);
      return TTS_SUCCESS;
    }

    exml->temp = EXmlElementCreate(exml->pool,(const char *)fieldName,(const char *)fieldValue);
    if ( exml->temp == NULL )
    {
      return TTS_ENULL;
    }

    exml->res = EXmlElemenAddChildNode(exml->current,exml->temp);

    return exml->res;
  }
}

int EXmlPrint(TEXml *exml, FILE *fp)
{
   EXmlElementDebug(0, exml->root, fp);
   return 0;
}

