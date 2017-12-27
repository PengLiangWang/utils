#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "triex.h"

enum
{
  TRIEX_NODE_MAX = 10    /* Êý×Ö 0-9 */
};

typedef struct stru_trie_ex_node  TTrieExNode;
typedef struct stru_trie_ex_child TTrieExChildPtr;

static int  flag = 0;


struct stru_trie_ex_child
{
  int       c;
  int       child;
};

struct stru_trie_ex_node
{
  int       children[TRIEX_NODE_MAX];
  int       val;
  int       vlen;
};



struct stru_trie_ex
{
  TMemPool     *pool;
  TShmemPool   *spool;
  int           count;
  TTrieExNode  *root;
};

int TrieExMake(TMemPool *pool,TShmemPool *spool,TTrieEx **trieex,int *addr)
{
  TTrieEx          *trie;
  TTrieExChildPtr  *childPtr;
  int               i;

  trie = (TTrieEx *)MemPoolAllocEx(pool,sizeof(TTrieEx));
  if ( trie == NULL )
  {
    return TTS_ENOMEM;
  }

  trie->pool = pool;
  trie->spool = spool;
  trie->count = 0;

  trie->root = (TTrieExNode *)ShmemPoolAlloc(spool,sizeof(TTrieExNode));
  if ( trie->root == NULL )
  {
    return TTS_ENOMEM;
  }
  
  childPtr = ( TTrieExChildPtr *)ShmemPoolAlloc(spool,sizeof(TTrieExChildPtr) * TRIEX_NODE_MAX );
  for ( i = 0; i < TRIEX_NODE_MAX; i++)
  {
    trie->root->children[i] = ShmemGetOffset(spool,(char *)&childPtr[i]);
  }

  if ( addr )
  {
    *addr = ShmemGetOffset(spool,(char*)trie->root);
  }

  *trieex = trie;

  return TTS_SUCCESS;
}

int TrieExOpen(TMemPool *pool,TShmemPool *spool,int addr,TTrieEx **trieex)
{
  TTrieEx  *trie;

  trie = (TTrieEx *)MemPoolAllocEx(pool,sizeof(TTrieEx));
  if ( trie == NULL )
  {
    return TTS_ENOMEM;
  }

  trie->pool = pool;
  trie->spool = spool;
  trie->count = 0;
  
  trie->root = ( TTrieExNode *)ShmemPoolGetAddr(spool,addr);
  if ( trie->root == NULL )
  {
    return TTS_EINVAL;
  }
  
  *trieex = trie;

  return TTS_SUCCESS;
}

static void TrieExLink(TShmemPool *spool,TTrieExNode *parent,TTrieExNode *child,char c)
{
  TTrieExChildPtr  *childPtr;

  childPtr = ( TTrieExChildPtr *)ShmemPoolGetAddr(spool,parent->children[c-'0']);
  if ( childPtr->c == c )
  {
    return ;
  }

  childPtr->c = c;
  childPtr->child = ShmemGetOffset(spool,(char *)child);
  
  return ;
}

static TTrieExNode *TrieExNodeAlloc(TShmemPool *spool,TTrieExNode *parent,char c)
{
  TTrieExNode      *node;
  TTrieExChildPtr  *childPtr;
  int               i;

  if ( parent )
  {
    childPtr = ( TTrieExChildPtr *)ShmemPoolGetAddr(spool,parent->children[ c -'0']);
    if ( childPtr->c == c )
    {
      node = ( TTrieExNode *)ShmemPoolGetAddr(spool,childPtr->child);
      return node;
    }
    
    node = (TTrieExNode *)ShmemPoolAlloc(spool,sizeof(TTrieExNode));
    if ( node == NULL )
    {
      return NULL;
    }
    TrieExLink(spool,parent,node,c);
  }
  else
  {
    node = (TTrieExNode *)ShmemPoolAlloc(spool,sizeof(TTrieExNode));
    if ( node == NULL )
    {
      return NULL;
    }
  }
  childPtr = ( TTrieExChildPtr *)ShmemPoolAlloc(spool,sizeof(TTrieExChildPtr) * TRIEX_NODE_MAX );
  for ( i = 0; i < TRIEX_NODE_MAX; i++)
  {
    node->children[i] = ShmemGetOffset(spool,(char *)&childPtr[i]);
  }

  return node;
}


int TrieExSet(TTrieEx *trie,char  *name,void *val,int vlen)
{
  TTrieExNode  *node;
  const  char  *n;
  void         *pval;

  node = trie->root;

  for ( n = name; *n; n++)
  {
    if ( !m_isdigit(*n ))
    {
      return TTS_EINVAL;
    }
    TTrieExNode  *child = (TTrieExNode*)TrieExNodeAlloc(trie->spool,node,*n);
    if ( child == NULL )
    {
      return TTS_ENOMEM;
    }
    TrieExLink(trie->spool,node,child,*n);
    node = child;
  }

  if ( node->val )
  {
    if ( flag )
    {
      return TTS_EXIST;
    }
    pval = ( TTrieExNode *)ShmemPoolGetAddr(trie->spool,node->val);
    memset(pval, 0, node->vlen);
    memcpy(pval, val, node->vlen);
  }
  else
  {
    pval = (void *)ShmemPoolAlloc(trie->spool,vlen);
    if ( pval == NULL )
    {
      return TTS_ENOMEM;
    }
    memcpy(pval,val,vlen);
    node->val = ShmemGetOffset(trie->spool,(char *)pval);
    node->vlen = vlen;
  }

  return TTS_SUCCESS;
}

int TrieExGet(TTrieEx *trie,char  *name,void *val,int *vlen)
{
  if ( trie->root )
  {
    const char *n;
    const TTrieExNode  *node;
    const TTrieExChildPtr *childPtr;

    node = trie->root;
    for ( n = name ; *n ;n++ )
    {
      if ( !m_isdigit(*n ))
      {
        return TTS_EINVAL;
      }

      childPtr = ( TTrieExChildPtr *)ShmemPoolGetAddr(trie->spool,node->children[*n - '0']);
      if ( *n == childPtr->c )
      {
        node = ( TTrieExNode *)ShmemPoolGetAddr(trie->spool,childPtr->child);
        if ( node->val )
        {
           break;
        }
      }
      else
      {
        node = NULL;
        break;
      }
    }
    if ( node && node->val )
    {
      void *pval = ( TTrieExNode *)ShmemPoolGetAddr(trie->spool,node->val);
      if ( vlen == NULL )
      {
        memset((char*)val,0,node->vlen);
        memcpy(val,pval,node->vlen);
        return TTS_SUCCESS;
      }
      if ( *vlen <= 0 || *vlen > node->vlen)
      {
        memset((char*)val,0,node->vlen);
        memcpy(val,pval,node->vlen);
        *vlen = node->vlen;
        return TTS_SUCCESS;
      }
      else
      {
        memset((char*)val,0,*vlen);
        memcpy(val,pval,*vlen);
        return TTS_SUCCESS;
      }
    }
  }
  return TTS_NOTFOUND;
}


static void TrieExNodePrint(TShmemPool *spool,TTrieExNode *node,char *name,int level)
{
  int              n,i;
  TTrieExChildPtr *childPtr;


  for ( n = 0; n < TRIEX_NODE_MAX ; n++ )
  {
    childPtr = ( TTrieExChildPtr *)ShmemPoolGetAddr(spool,node->children[n]);
    if ( childPtr != NULL )
    {
      if ( childPtr->c == '0' + n )
      {
        name[level] = childPtr->c;
        name[level + 1] = 0;
        node = ( TTrieExNode *)ShmemPoolGetAddr(spool,childPtr->child);
        if ( node->val )
        {
          printf("%s vlen = %d\n",name,node->vlen);
          return ;
        }
        TrieExNodePrint(spool,node,name,level + 1);
        name[level] = 0;
      }
    }
  }

}

int TrieExPrint(TTrieEx *trie,int maxlen)
{
  int    level  = 0;
  char  *name = (char *)MemPoolAllocEx(trie->pool,maxlen + 1);
  if ( name == NULL )
  {
    return TTS_ENOMEM;
  }

  if ( trie->root )
  {
    TrieExNodePrint(trie->spool,trie->root,name,level);
  }

  return TTS_SUCCESS;
}

