#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ttsys.h"

struct _t_engine_flow_node
{
  struct _t_engine_flow_node  *next;
  struct _t_engine_flow_node  *sub;
  struct _t_engine_flow_node  *def;
  int                          atomic;
  int                          success;
  int                          errcode;
};

typedef struct _t_engine_flow_node TEngineFlowNode;

struct _t_engine_flow_chart
{
   int                      flowId;
   int                      checksum;
   char                     flowCode[7];
   TEngineFlowNode         *flowRoot;
};

typedef struct _t_engine_flow_chart TEngineFlowChart;

static int _BuildFlowChildNode(TMemPool *pool,int errcode,char *flow,TEngineFlowNode **node,TEngineFlowNode **last)
{
   char              *p;
   char               atomic[4];
   int                len = 0;
   TEngineFlowNode   *newNode,*lastNode;

   lastNode = newNode = NULL;
   memset(atomic,0,sizeof(atomic));
   for ( p = flow ; p != NULL && *p != 0 ; p++)
   {
      if ( m_isdigit ( *p ) && len < 4 )
      {
         atomic[ len++ ] = *p;
      }

      if ( len > 0 && ( *p == '-' || *(p + 1) == 0 ) )
      {
         atomic[len] = 0;
         newNode = (TEngineFlowNode *)MemPoolAllocEx(pool,sizeof(TEngineFlowNode));
         if ( newNode == NULL )
         {
            ELOG(ERROR, "创建流程节点(TEngineFlowNode)时内存池中没有足够的内存分配(TTS_ENOMEM)");
            return TTS_ENOMEM;
         }
         newNode->next = newNode->sub = newNode->def =NULL;
         newNode->atomic = atoi(atomic);
         newNode->success = 0;
         if ( lastNode == NULL )
         {
            newNode->errcode = errcode;
            *node = lastNode = newNode;
         }
         else
         {
            lastNode->next = newNode;
            lastNode = newNode;
         }
         len = 0;
         memset(atomic,0,sizeof(atomic));
      }
      
   }
   if ( last != NULL )
   {
      *last = lastNode;
   }
   return 0;
}

//{()}
static int _BuildErrorFlowNode(TMemPool *pool,char *errFlow,TEngineFlowNode *flowNode)
{
   char             *p,*last,szFlow[256];
   int               errcode,len,res;
   TEngineFlowNode  *subNode,*lastSub,*defSub;

   lastSub = NULL;

   p = errFlow;

   if( *p++ != '{' )
   {
      ELOG(ERROR, "数据(%s)格式非法(TTS_EINVAL)",errFlow);
      return TTS_EINVAL;
   }
   
   for( ; p != NULL && *p != 0 ; )
   {
      if ( strncmp(p,"(E=",3) == 0 )
      {
         p += 3;
         memset(szFlow,0,sizeof(szFlow));
         errcode = strtol(p,(char **)&last,10);
         if ( last == NULL || *last == 0 )
         {
            ELOG(ERROR, "数据(%s)格式非法(TTS_EINVAL)",errFlow);
            return TTS_EINVAL;
         }
         p = last;
      }
      else if ( strncmp(p,"(DEF",4) == 0 )
      {
         if ( flowNode->def )
         {
            ELOG(ERROR, "交易流程(%s)[(DEF:非法)]非法(TTS_EINVAL)",errFlow);
            return TTS_EINVAL;
         }
         p += 4;
         errcode = 0;
      }
      else
      {
         ELOG(ERROR, "数据(%s)格式非法(TTS_EINVAL)",errFlow);
         return TTS_EINVAL;
      }

      if ( *p++ != ',' )
      {
         ELOG(ERROR, "数据(%s)格式非法(TTS_EINVAL)",errFlow);
         return TTS_EINVAL;
      }

      len = 0;
      
      for( ; p != NULL && *p != ')' ; p++)
      {
         szFlow[len++] = *p;
      }
      szFlow[len] = 0;

      res = _BuildFlowChildNode(pool, errcode,szFlow,&subNode,NULL);
      if ( res )
      {
         ELOG(ERROR, "创建错误流程节点(TEngineFlowNode)失败(%d)",res);
         return res;
      }

      if ( errcode )
      {
         if ( lastSub == NULL )
         {
            lastSub = subNode;
            flowNode->sub = subNode;
         }
         else
         {
            lastSub->sub = subNode;
            lastSub = subNode;
         }
      }
      else
      {
         flowNode->def = subNode;
      }

      p++;

      if ( *p == '(' )
      {
         continue;
      }
      else if ( *p == '}' )
      {
         break;
      }
   }

   return 0;
}


static int _BuildFlowNode(TMemPool *pool,char *flow,TEngineFlowChart *flowChart)
{
   char              szFlow[512],szErrFlow[256],szFlow2[256];
   char             *p,*perr;
   int               len,tag = 0,res;
   TEngineFlowNode  *lastNode,*newNode,*lastNode2;

   lastNode2 = newNode = NULL;

   memset(szFlow, 0, sizeof(szFlow));
   memset(szErrFlow, 0, sizeof(szErrFlow));
   memset(szFlow2, 0, sizeof(szFlow2));

   //校验程序&&除去空格
   p = szFlow;
   for ( len = 0 ; flow != NULL && *flow != 0 ; )
   {
      if ( *flow == ' ' )
      {
         flow++;
      }
      if ( strncmp(flow, "DEF", 3) == 0 )
      {
         strncpy(szFlow + len, flow, 3);
         flow += 3;
         len += 3;
      }
      else if ( isdigit(*flow) || *flow == 'E' || *flow == '-' ||  *flow == '{'   || *flow == '}' || 
                *flow == '(' || *flow == ')'   || *flow == ',' || *flow == '=' )
      {
         szFlow[len++]  = *flow++;
      }
      else
      {
         ELOG(ERROR, "流程中含有非法字符(%s)",flow);
         return TTS_EINVAL;
      }
   }

   szFlow[len] = 0;

   len = 0;
   
   for (; p != NULL && *p != 0; p++)
   {
      if ( tag == 0 )
      {
         //处理正常流程
         if ( *p != '{' )
         {
            szFlow2[len++] = *p;
         }
         else 
         {
            szFlow2[len] = 0;
           
            printf("_BuildFlowChildNode , %d\n", __LINE__);
            res = _BuildFlowChildNode(pool,0,szFlow2,&newNode,&lastNode2);
            if ( res )
            {
               return res;
            }
            if ( flowChart->flowRoot == NULL )
            {
               flowChart->flowRoot = newNode;
               lastNode = lastNode2;
            }
            else
            {
               lastNode->next = newNode;
               lastNode = lastNode2;

               printf("lastNode->atomic = %d,%08X\n",lastNode->atomic,lastNode);
               printf("lastNode2->atomic = %d,%08X\n",lastNode2->atomic,lastNode2);
            }
            tag = 1;
            memset(szFlow2, 0, sizeof(szFlow2));
            len = 0;
            szErrFlow[len++] = *p;
         }
      }
      else if ( tag == 1 )
      {
         if ( *p != '}' )
         {
            szErrFlow[len++] = *p;
         }
         else
         {
            szErrFlow[len++] = *p;
            szErrFlow[len] = 0;
            printf("_BuildErrorFlowNode , %d\n", __LINE__);
            res = _BuildErrorFlowNode(pool, szErrFlow, lastNode);
            if ( res )
            {
               return -1;
            }
            printf("ERROR : lastNode->atomic = %d,%08X\n",lastNode->atomic,lastNode);
            memset(szErrFlow, 0, sizeof(szErrFlow));
            tag = len = 0;
            //p++; //过滤 '-'// 由BuildChildNode过滤
         }
      }

      if ( *(p + 1 ) == 0 )
      {
         if ( tag == 0 )
         {
            if ( len == 0 )
            {
               break;
            }
            szFlow2[len] = 0;
            printf("END : _BuildErrorFlowNode ,%s, (len) %d\n", szFlow2, __LINE__);
            res = _BuildFlowChildNode(pool,0,szFlow2,&newNode,&lastNode2);
            if ( res )
            {
               return -1;
            }

            if ( newNode == NULL )
            {
            }
            if ( flowChart->flowRoot == NULL )
            {
               flowChart->flowRoot = newNode;
               lastNode = lastNode2;
            }
            else
            {
               lastNode->next = newNode;
               lastNode = lastNode2;
            }
         }
         else if ( tag == 1 )
         {
            break;
         }
      }
   }
   return 0;
}

#define FLOW_CHART  "01-02-03{(E=1024,05-23-12)(DEF,02-13-12)}-04-11{(DEF,02-03)}\0"

static TMemPool  *MemPool = NULL;

void ExitClear()
{
   if ( MemPool )
   {
      MemPoolDestoryEx(MemPool);
   }
}

int main()
{
   int                 res;
   TEngineFlowChart   *flowChart;
   TEngineFlowNode    *node;
   atexit(ExitClear);

   res = MemPoolCreateEx(&MemPool, NULL );
   if ( res )
   {
      printf("创建全局内存池失败,错误信息(%d,%s)\n",res,GetStrError(res));
      exit(0);
   }

   flowChart = (TEngineFlowChart *)MemPoolAllocEx(MemPool,sizeof(TEngineFlowChart));
   if ( flowChart == NULL )
   {
      ELOG(ERROR, "创建流程时内存池中没有足够的内存分配(TTS_ENOMEM)");
      return TTS_ENOMEM;
   }

   res = _BuildFlowNode(MemPool, FLOW_CHART, flowChart);
   if ( res )
   {
      printf("创建流程(FID:%d)节点时失败(%d)", res);
      return res;
   }

   

   for ( node = flowChart->flowRoot; node != NULL ; node = node->next)
   {
      printf("atomic = %d\n",node->atomic);
      getchar();
   }



   return 0;


}
