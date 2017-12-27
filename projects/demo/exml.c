#include "ttsys.h"


static TMemPool  *MemPool = NULL;

void ExitClear()
{
   if ( MemPool )
   {
      MemPoolDestoryEx(MemPool);
   }
}

/*
int main()
{
   int           res;
   EXmlDoc      *XmlDoc;
   EXmlElement  *mobilePay, *element;
   char         *XmlBuf;
   int           XmlSize;

   atexit(ExitClear);

   res = MemPoolCreateEx(&MemPool, NULL );
   if ( res )
   {
      printf("����ȫ���ڴ��ʧ��,������Ϣ(%d,%s)\n",res,GetStrError(res));
      exit(0);
   }

   res = EXmlDocCreateEx(MemPool, "mobilePay", (const char *)"1.0" , (const char *)"GB2312-2-1", &XmlDoc);  
   if ( res )
   {
      printf("�����ֻ�Ӧ��������ʧ��,������Ϣ(%d:%s)", res, GetStrError(res) );
      return res;
   }

   mobilePay = EXmlDocGetRootElement( XmlDoc );

   element = EXmlElementCreate(MemPool, "RespCode",  "0091");
   if ( element == NULL )
   {
      printf("�����ֻ�Ӧ������Ԫ�� RespCode ʧ��,������Ϣ(%d:%s)", res, GetStrError(res) );
      return res;
   }
   res = EXmlElemenAddChildNode(mobilePay, element);
   if ( res )
   {
      printf("�����ֻ�Ӧ������Ԫ�� RespCode ʧ��,������Ϣ(%d:%s)", res, GetStrError(res) );
      return res;
   }

   element = EXmlElementCreate(MemPool, "RespDesc", "ERROR" );
   if ( element == NULL )
   {
      printf("�����ֻ�Ӧ������Ԫ�� RespDesc ʧ��,������Ϣ(%d:%s)", res, GetStrError(res) );
      return res;
   }

   res = EXmlElemenAddChildNode(mobilePay, element);
   if ( res )
   {
      printf("�����ֻ�Ӧ������Ԫ�� RespDesc ʧ��,������Ϣ(%d:%s)", res, GetStrError(res) );
      return res;
   }

   res = EXmlDocToText(MemPool, XmlDoc, &XmlBuf, &XmlSize);
   if ( res )
   {
      printf("�����ֻ�Ӧ��������ʧ��,������Ϣ(%d:%s)", res, GetStrError(res) );
      return res;
   }

   printf("%s\n",XmlBuf);


   EXmlElementDebug(0, mobilePay, stdout);

   return res;
}
*/


int main()
{
   int           res;
   TEXml        *EXml;
   EXmlElement  *mobilePay, *element;
   char         *XmlBuf;
   int           XmlSize;

   atexit(ExitClear);

   res = MemPoolCreateEx(&MemPool, NULL );
   if ( res )
   {
      printf("����ȫ���ڴ��ʧ��,������Ϣ(%d,%s)\n",res,GetStrError(res));
      exit(0);
   }
   //int EXmlMake(const char *RootName,TEXml **exml, TMemPool *MemPool);
   res = EXmlMake("mobilePay", &EXml, MemPool);  
   if ( res )
   {
      printf("�����ֻ�Ӧ��������ʧ��,������Ϣ(%d:%s) %d \n", res, GetStrError(res) ,__LINE__);
      return res;
   }

   res = EXmlSetValue(EXml, NULL, "[application]", "registeruser.req");
   if ( res )
   {
      printf("�����ֻ�Ӧ��������ʧ��,������Ϣ(%d:%s) %d \n", res, GetStrError(res) ,__LINE__);
      return res;
   }

   res = EXmlSetValue(EXml, NULL, "[version]", "1000000");
   if ( res )
   {
      printf("�����ֻ�Ӧ��������ʧ��,������Ϣ(%d:%s) %d \n", res, GetStrError(res) ,__LINE__);
      return res;
   }

   res = EXmlSetValue(EXml, NULL, "RespCode", "0000");
   if ( res )
   {
      printf("�����ֻ�Ӧ��������ʧ��,������Ϣ(%d:%s) %d \n", res, GetStrError(res) ,__LINE__);
      return res;
   }

   res = EXmlSetValue(EXml, NULL, "RespText", "���׳ɹ�");
   if ( res )
   {
      printf("�����ֻ�Ӧ��������ʧ��,������Ϣ(%d:%s) %d \n", res, GetStrError(res) ,__LINE__);
      return res;
   }

   res = EXmlSetValue(EXml, "mobilePay/response", "RespText", "���׳ɹ�");
   if ( res )
   {
      printf("�����ֻ�Ӧ��������ʧ��,������Ϣ(%d:%s) %d \n", res, GetStrError(res) ,__LINE__);
      return res;
   }

   res = EXmlSetValue(EXml, "mobilePay/response/jiangsu", "RespText", "���׳ɹ�");
   if ( res )
   {
      printf("�����ֻ�Ӧ��������ʧ��,������Ϣ(%d:%s) %d \n", res, GetStrError(res) ,__LINE__);
      return res;
   }

   res = EXmlSetValue(EXml, "mobilePay/response/jiangsu/nanjng", "RespText", "���׳ɹ�");
   if ( res )
   {
      printf("�����ֻ�Ӧ��������ʧ��,������Ϣ(%d:%s) %d \n", res, GetStrError(res) ,__LINE__);
      return res;
   }


   
//(TEXml *exml, char **buffer,uint32 *size, TMemPool *pool)
   res = EXmlToText(EXml, &XmlBuf, &XmlSize, MemPool);
   if ( res )
   {
      printf("�����ֻ�Ӧ��������ʧ��,������Ϣ(%d:%s) %d \n", res, GetStrError(res) ,__LINE__);
      return res;
   }

   printf("%s\n",XmlBuf);
   printf("%d\n",XmlSize);

   EXmlPrint(EXml, stdout);

   return res;
}
