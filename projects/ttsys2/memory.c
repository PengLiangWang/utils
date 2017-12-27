#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "memory.h"

#define TTS_ALIGN(size, boundary) \
           (((size) + ((boundary) - 1)) & ~((boundary) - 1))

#define TTS_ALIGN_DEFAULT(size) TTS_ALIGN(size, 8)

#define MIN_ALLOC_SIZE      8192

#define BOUNDARY_INDEX      12
#define BOUNDARY_SIZE       (1 << BOUNDARY_INDEX)

typedef struct memory_node_t  TMemNode;

struct memory_node_t
{
  TMemNode     *next;
  char         *first_avail;
  char         *endp;
  int           size;
};

#define SIZEOF_MEMNODE  TTS_ALIGN_DEFAULT(sizeof(TMemNode))

static int MemNodeCreate(TMemNode **node,uint32 size)
{
  TMemNode  *mem;

  size = TTS_ALIGN ( size + SIZEOF_MEMNODE , BOUNDARY_SIZE ) ;
  if ( size < MIN_ALLOC_SIZE )
  {
    size = MIN_ALLOC_SIZE;
  }

  mem = ( TMemNode * )malloc( size );
  if ( mem == NULL )
  {
    return TTS_ENOMEM;
  }


  memset ( mem , 0 , size ) ;

  mem->next = NULL ;
  mem->first_avail = ( char * ) mem + SIZEOF_MEMNODE ;
  mem->endp = ( char * ) mem + size ;
  mem->size = size;
  *node = mem;

  return TTS_SUCCESS;

}

static void MemNodeDestory(TMemNode *node)
{
  TMemNode  *nd = NULL;

  while ( node != NULL )
  {
    nd = node ;
    node = node->next ;
    free( nd ) ;
  }

  return ;
}
typedef struct cleanup_t  cleanup_t;

struct TMemPool
{
  TMemPool      *parent;
  TMemPool      *child;
  TMemPool      *sibling;
  TMemPool     **ref;
  cleanup_t     *cleanups;
  TMemNode      *self;
  TMemNode      *active;
  TMemNode      *freelist;
  char          *self_first_avail;
};

#define SIZEOF_POOLS  TTS_ALIGN_DEFAULT(sizeof(TMemPool))

int  MemPoolCreate(TMemPool **newpool,TMemPool *parent,uint32 size)
{
  TMemPool    *pool;
  TMemNode    *node;
  int          rv;

  *newpool = NULL;

  if ( size < MIN_ALLOC_SIZE - SIZEOF_MEMNODE )
  {
    size = MIN_ALLOC_SIZE - SIZEOF_MEMNODE;
  }
  else
  {
    size += MIN_ALLOC_SIZE - SIZEOF_MEMNODE;
  }

  rv = MemNodeCreate( &node , size ) ;
  if ( rv != TTS_SUCCESS )
  {
    return rv;
  }

  pool = ( TMemPool * )node->first_avail;
  pool->self_first_avail = node->first_avail = ( char * )pool + SIZEOF_POOLS;
  pool->self = pool->active = node;
  pool->freelist = NULL;

  if ( (pool->parent = parent) != NULL )
  {
    if ((pool->sibling = parent->child) != NULL)
    {
      pool->sibling->ref = &pool->sibling;
    }
    parent->child = pool;
    pool->ref = &parent->child;
  }
  else
  {
    pool->sibling = NULL;
    pool->ref = NULL;
  }

  *newpool = pool;

  return TTS_SUCCESS;
}

int  MemPoolCreateEx(TMemPool **newpool,TMemPool *parent)
{
  TMemPool    *pool;
  TMemNode    *node;
  int         rv;

  *newpool = NULL;

  rv = MemNodeCreate( &node , MIN_ALLOC_SIZE - SIZEOF_MEMNODE ) ;
  if ( rv != TTS_SUCCESS )
  {
    return rv;
  }

  pool = ( TMemPool * )node->first_avail;
  pool->self_first_avail = node->first_avail = ( char * )pool + SIZEOF_POOLS;
  pool->self = pool->active = node;
  pool->freelist = NULL;

  if ( (pool->parent = parent) != NULL )
  {
    if ((pool->sibling = parent->child) != NULL)
    {
      pool->sibling->ref = &pool->sibling;
    }
    parent->child = pool;
    pool->ref = &parent->child;
  }
  else
  {
    pool->sibling = NULL;
    pool->ref = NULL;
  }

  *newpool = pool;

  return TTS_SUCCESS;
}


#define node_free_space(node_) ((uint32)(node_->endp - node_->first_avail))

static TMemNode *MemNodeAlloc(TMemPool *pool,uint32 size)
{
  TMemNode *node;
  TMemNode **last;

  node = pool->freelist;
  last = &pool->freelist;

  while ( node != NULL )
  {
    if ( size <= node_free_space(node) )
    {
      *last = node->next;
      node->next = NULL;
      return node;
    }
    else
    {
      last = &node->next;
      node = node->next;
    }
  }

  if ( ( MemNodeCreate(&node,size) ) != TTS_SUCCESS )
  {
    return NULL;
  }

  return node;
}

static void MemNodeFree(TMemNode **freelist)
{
  TMemNode *node;

  node = *freelist;

  if (node == NULL)
  {
    return ;
  }

  while ( node->next != NULL )
  {
    node->first_avail = ( char * ) node + SIZEOF_MEMNODE ;

    memset(node->first_avail,0,node->endp - node->first_avail);

    node = node->next;
  }

  node->first_avail = ( char * ) node + SIZEOF_MEMNODE ;

  memset(node->first_avail,0,node->endp - node->first_avail);


  *freelist = node;
  return ;
}

void *MemPoolAlloc(TMemPool *pool,uint32 size)
{
  TMemNode    *active,*node;
  void        *mem;

  size = TTS_ALIGN_DEFAULT(size);

  active = pool->active;

  if ( size <= node_free_space(active) )
  {
    mem = active->first_avail;
    active->first_avail += size;
    return mem;
  }

  if ( ( node = MemNodeAlloc(pool,size) ) == NULL )
  {
    return NULL;
  }

  active->next = node;

  mem = node->first_avail;

  node->first_avail += size;

  pool->active = node;

  return mem;
}

void *MemPoolAllocEx(TMemPool *pool,uint32 size)
{
  void *res = MemPoolAlloc(pool,size);
  memset(res,0,size);
  return res;
}

static void RunCleanups(cleanup_t **cref);

void MemPoolClearEx(TMemPool *pool)
{
  TMemNode  *freelist;

  while (pool->child)
  {
    MemPoolDestoryEx(pool->child);
  }

  RunCleanups(&pool->cleanups);

  pool->cleanups = NULL;

  pool->self->first_avail = pool->self_first_avail;

  memset(pool->self_first_avail,0,pool->self->endp - pool->self_first_avail);

  freelist = pool->self->next;

  if (freelist == NULL)
  {
    return ;
  }

  MemNodeFree(&freelist);

  freelist->next = pool->freelist;

  pool->freelist = pool->self->next;

  pool->active = pool->self;

  pool->active->next = NULL;

  return ;
}

void MemPoolDestoryEx(TMemPool *pool)
{

  while (pool->child)
  {
    MemPoolDestoryEx(pool->child);
  }

  RunCleanups(&pool->cleanups);

  if (pool->parent)
  {
    if ((*pool->ref = pool->sibling) != NULL)
    {
      pool->sibling->ref = pool->ref;
    }
  }

  if ( pool->freelist )
  {
    MemNodeDestory(pool->freelist);
  }
  MemNodeDestory(pool->self);

  return ;
}

struct cleanup_t
{
  struct cleanup_t *next;
  const void *data;
  int  (*plain_cleanup_fn) (void *);
};

void MemPoolRegisterCleanup(TMemPool * p, void *data,int (*cleanup_fn)(void *data))
{
  cleanup_t *c;
 
  if (p != NULL)
  {
    c = (cleanup_t *)MemPoolAlloc(p, sizeof(cleanup_t));
    c->data = data;
    c->plain_cleanup_fn = cleanup_fn;
    c->next = p->cleanups;
    p->cleanups = c;
  }
}

void MemPoolUnRegisterCleanup(TMemPool * p, void *data,int (*cleanup_fn)(void *))
{
  cleanup_t *c, **lastp;

  if (p == NULL)
  {
    return;
  }

  c = p->cleanups;
  lastp = &p->cleanups;
  while (c)
  {
    if (c->data == data && c->plain_cleanup_fn == cleanup_fn)
    {
      *lastp = c->next;
      break;
    }
    lastp = &c->next;
    c = c->next;
  }
}

int MemPoolRunCleanup(TMemPool * p, void *data,int (*cleanup_fn)(void *))
{
  MemPoolUnRegisterCleanup(p, data, cleanup_fn);
  return (*cleanup_fn)(data);
}

static void RunCleanups(cleanup_t **cref)
{
  int  i = 0;
  cleanup_t *c = *cref;

  while (c)
  {
    *cref = c->next;
    (*c->plain_cleanup_fn)((void *)c->data);
    c = *cref;
    i++;
  }

}

struct psprintf_data
{
  mvformatter_buff_t    vbuff;
  TMemPool             *pool;
  TMemNode             *node;
  uchar              got_a_new_node;
};

#define TTS_PSPRINTF_MIN_STRINGSIZE   32

static int psprintf_flush(mvformatter_buff_t *vbuff)
{
  struct psprintf_data *ps = (struct psprintf_data *)vbuff;
  TMemNode             *active,*node;
  int                   cur_len, size;
  char                 *strp;
  TMemPool             *pool;

  pool     = ps->pool;
  active   = ps->node;
  strp     = ps->vbuff.curpos;
  cur_len  = strp - active->first_avail;
  size     = cur_len << 1;

  if (size < TTS_PSPRINTF_MIN_STRINGSIZE)
  {
    size = TTS_PSPRINTF_MIN_STRINGSIZE;
  }

  if ( ( node = MemNodeAlloc(pool,size) ) == NULL )
  {
    return -1;
  }

  memcpy(node->first_avail, active->first_avail, cur_len);
  ps->vbuff.curpos = node->first_avail + cur_len;
  ps->vbuff.endpos = node->endp - 1;

  if ( ps->got_a_new_node )
  {
    memset(node->first_avail,0,node->endp - node->first_avail);
    node->next = pool->freelist;
    pool->freelist = node;
  }

  ps->node = node;
  ps->got_a_new_node = 1;

  return 0;
}


static char * mvsprintf(TMemPool *pool, const char *fmt, va_list ap)
{
  struct psprintf_data    ps;
  char                   *strp;
  uint32                size;
  TMemNode               *active, *node;

  ps.node = pool->active;
  ps.pool = pool;
  ps.vbuff.curpos  = ps.node->first_avail;

  ps.vbuff.endpos = ps.node->endp - 1;
  ps.got_a_new_node = 0;

  if ( ps.node->first_avail == ps.node->endp )
  {
    if ( psprintf_flush(&ps.vbuff) == -1 )
    {
      return NULL;
    }
  }

  if (m_vformatter(psprintf_flush, &ps.vbuff, fmt, ap) == -1)
  {
    return NULL;
  }

  strp = ps.vbuff.curpos;
  *strp++ = '\0';

  size = strp - ps.node->first_avail;
  size = TTS_ALIGN_DEFAULT(size);
  strp = ps.node->first_avail;
  ps.node->first_avail += size;

  if ( ps.got_a_new_node )
  {
    pool->active->next = ps.node;
    pool->active = ps.node;
  }

  return strp;
}

char * m_sprintf(TMemPool * p, const char *fmt, ...)
{
  va_list  ap;
  char    *res;

  va_start(ap, fmt);
  res = mvsprintf(p, fmt, ap);
  va_end(ap);
  return res;
}

