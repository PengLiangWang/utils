#include <stdio.h>
#include <time.h>
#include <string.h>
#include "dbaction.h"

extern FieldDef *findFieldDef(char *name);
static int outputEraseTailSpaceFunction() ;
static int outputOrderBy(FILE *out,ArgDef *p) ;
static int outputCfileActionPrintReport() ;

static char *UpToLow(char *str,int a,int b)
{
   int   i,u;

   for(i=0,u=1; str[i]!='\0'; ++i)
   {
      if (u&&(str[i]>='A'&&str[i]<='Z'))
      {
         str[i]=(char)(str[i]+('a'-'A'));
         u=0;
      }
      else if (str[i]=='_')
         u=1;
   };
   return(str);
}
static char *LowToUp(char *str,int a,int b)
{
   int   i,u;

   for(i=0,u=1; str[i]!='\0'; ++i)
   {
      if (u&&(str[i]>='a'&&str[i]<='z'))
      {
         str[i]=(char)(str[i]-('a'-'A'));
         u=0;
      }
      else if (str[i]=='_')
         u=1;
   };
   return(str);
}

static int outputSqlField(FieldDef *field)
{
  FieldDef *ft;

  switch(field->type){

    case K_long:
      fprintf(sql_file,"\t%s\t NUMBER(10)",field->name);
      break;

    case K_short:
      fprintf(sql_file,"\t%s\t NUMBER(5)",field->name);
      break;

    case K_int:
      fprintf(sql_file,"\t%s\t NUMBER(10)",field->name);
      break;

    case K_double:
      if (field->size == 8)
   fprintf(sql_file,"\t%s\t NUMBER",field->name);
      else
   if (field->scale == 0)
     fprintf(sql_file,"\t%s\t NUMBER(%d,2)",field->name, field->size-3);
   else
     fprintf(sql_file,"\t%s\t NUMBER(%d,%d)",field->name, field->size, field->scale);
      break;

    case K_str:
      fprintf(sql_file,"\t%s\t VARCHAR2(%d)",
            field->name,field->size);
      break;

    case K_YYYYMMDD:
      fprintf(sql_file,"\t%s\t VARCHAR2(10)",field->name);
      break;

    case K_HHMISS:
      fprintf(sql_file,"\t%s\t VARCHAR2(8)",field->name);
      break;

    default:
      return(-1);
  }

  if (field->notnull)
    fprintf(sql_file, " NOT NULL");

  ft = field->next;
  if ( ft != NULL )
    fprintf(sql_file, ",\n");
  else
    fprintf(sql_file, "\n");

  return(0);
}


static int outputIncludeStructField(FieldDef *field)
{
   switch(field->type)
   {
      case K_long:
         fprintf(i_file,"\tlong\t%s;\n",field->name);    break;
      case K_short:
         fprintf(i_file,"\tshort\t%s;\n",field->name);   break;
      case K_int:
         fprintf(i_file,"\tint\t%s;\n",field->name);     break;
      case K_double:
         fprintf(i_file,"\tdouble\t%s;\n",field->name);  break;
      case K_str:
         fprintf(i_file,"\tchar\t%s[%d];\n",
               field->name,field->size+1);   break;
      case K_YYYYMMDD:
         fprintf(i_file,"\tchar\t%s[11];\n",field->name);break;
      case K_HHMISS:
         fprintf(i_file,"\tchar\t%s[9];\n",field->name); break;

      default:
         return(-1);
   };
   return(0);
}

static int outputSqlHead()
{
   FieldDef   *p;

   fprintf(sql_file, "rem*  This File \"%s\"\n", SqlFileName);
   fprintf(sql_file, "rem*  Genenated by\n");
   fprintf(sql_file, "rem*  Application dbaction V3.0 for Oracle\n");
   fprintf(sql_file, "rem*  with the action file \"%s\".\n",actFileName);
   fprintf(sql_file, "rem*  Create: %s \n", systemDate);

   fprintf(sql_file,"CREATE TABLE %s \n  (\n", tableName);
   p=fList.head;
   while( p!= NULL){
     outputSqlField(p);
     p=p->next;
   }
   fprintf(sql_file,"  ); \n");

   return(0);
}

static int outputIncludeHead()
{
   FieldDef   *p;

   fprintf(i_file, "/*\n This Include File \"%s\" \n Genenated By\n",includeFileName);
   fprintf(i_file, " Application dbaction V3.0 for Oracle\n");
   fprintf(i_file, " with the action file \"%s\".\n", actFileName);
   fprintf(i_file, " Create: %s*/\n\n", systemDate);

   fprintf(i_file,"#ifndef _%s_%s_INCLUDE_H_\n",sName,tableName);
   fprintf(i_file,"#define _%s_%s_INCLUDE_H_\n",sName,tableName);
   fprintf(i_file,"\n#ifdef\t__cplusplus\nextern \"C\" {\n#endif\n\n");

   fprintf(i_file,"#include \"dbutil.h\"\n");
   if (hasDAC)
   {
      fprintf(i_file,"#include \"dac.h\"\n");
   };
   fprintf(i_file,"\n");
   fprintf(i_file,"typedef struct {\n");
   p=fList.head;
   while(p!=NULL)
   {
      outputIncludeStructField(p);
      p=p->next;
   };
   fprintf(i_file,"        } %s;\n",tName);
   fprintf(i_file,"\n");

   return(0);
}

static int outputCfileHead()
{

   fprintf(c_file,"/* \n This C File \"%s\" \n Genenated By\n", cFileName);
   fprintf(c_file, " Application dbaction V3.0 for Oracle\n");
   fprintf(c_file, " with the action file \"%s\".\n", actFileName);
   fprintf(c_file, " Create: %s*/\n\n", systemDate);

   fprintf(c_file,"#include <stdio.h>\n");
   fprintf(c_file,"#include <string.h>\n");
   fprintf(c_file,"#include <stdlib.h>\n");
   fprintf(c_file,"#include <unistd.h>\n");
   fprintf(c_file,"#include \"DBulog.h\"\n");
   fprintf(c_file,"#include \"%s\"\n",includeFileName);
   fprintf(c_file,"static %s\tR_%s;\n",tName,tableName);
   outputEraseTailSpaceFunction();
   return(0);
}

static int outputEraseTailSpaceFunction()
{
   FieldDef *f;

   fprintf(c_file, "\nstatic int %s_EraseTailSpace(%s *_erase_data)",tName, tName);
        fprintf(c_file, "\n{");

        f = fList.head;
        while (f != NULL ){
          if ( f->type == K_str )
            fprintf(c_file, "\n\tERASE_TAIL_SPACE(_erase_data->%s);", f->name); 
          f = f->next;
       }
       fprintf(c_file, "\n\treturn(0);");
       fprintf(c_file, "\n}\n");
       return(0);
}

int outputOracleHead()
{
   int   r;

   r = outputSqlHead();
   if ( r == 0){
     r=outputIncludeHead();
     if (r==0)
      r=outputCfileHead();
   }
   return(r);
}

static char *ValueName(ArgDef *p)
{
   static   char   ss[128];

   sprintf(ss,"%s_%s_%d",p->name,getOperateDef(p->operate),p->n);
   return(ss);
}

static int outputActionDef(FILE *out)
{
   ArgDef      *p, *pt;
   FieldDef   *f;
   char      n;
        int             j;
   int   or_flag=0;

              /* write to SQL file */
   if ((aType == K_index_by) || 
            (aType == K_index_by_unique) ||
            (aType == K_index_unique_by)){
     if ((aType == K_index_by_unique) || (aType == K_index_unique_by))
       fprintf(out, "CREATE UNIQUE INDEX %s_I%d ON %s(", tName, _indexFileNo, tName);
     else
       fprintf(out, "CREATE INDEX %s_I%d ON %s(", tName, _indexFileNo, tName);

     p = aList.head;
     while(p!=NULL){
       f=findFieldDef(p->name);
       if (f==NULL)
         return(-1);

       pt = p->next;
       if (pt != NULL)
         fprintf(out,"%s,",p->name);
       else
         fprintf(out,"%s",p->name);
       p=p->next;
     }
     fprintf(out,");\n");

     _indexFileNo++;
     return(0);
   }
   else{    /* not index process */
   fprintf(out,"int %s_%s_%s",sName,tableName,keywordList[aType]);
        j = 0;
   p=aList.head;
   or_flag=0;
   while(p!=NULL)
   {
                if (strcmp(p->name, "_by_") == 0){
                  j = -1;
        fprintf(out,"_by");
                }
                else if (strcmp(p->name, "_or_") == 0){
        or_flag=1;
                }
                else if (strcmp(p->name, "_order_by_") == 0){
                  j = -1;
        fprintf(out,"_order_by");
                }
                else{
                  if ((j == 0)||(strcmp(p->name,"DESC")==0))
          fprintf(out,"_%s",p->name);
                  else if (or_flag==0)
          fprintf(out,"_and_%s",p->name);
        else
        {
          fprintf(out,"_or_%s",p->name);
          or_flag=0;
        };
            outputOperateDef(out,p->operate);
                }
                j++;
      p=p->next;
   };
   fprintf(out,"(");
   p=aList.head;
        if ((aType == K_sum_for) ||
            (aType == K_avg_for) ||
            (aType == K_avg_distinct_for) ||
            (aType == K_count_distinct_for) ||
            (aType == K_open_select_distinct_for) ||
            (aType == K_max_for) ||
            (aType == K_min_for)) {
          while ((p!=NULL) && (strcmp(p->name, "_by_") != 0) ){
             p=p->next;
          }
       if (p!=NULL) p=p->next;
        }

   n=' ';
   while(p!=NULL)
   {
      if (strcmp(p->name,"_order_by_")==0)
            break;

      if(strcmp(p->name,"_or_")!=0)
      {
          f=findFieldDef(p->name);
          if (f==NULL)
             return(-1);
          switch(f->type)
          {
         case K_long:
            fprintf(out,"%clong %s",n,ValueName(p));break;
         case K_short:
            fprintf(out,"%cshort %s",n,ValueName(p));break;
         case K_int:
            fprintf(out,"%cint %s",n,ValueName(p));break;
         case K_double:
            fprintf(out,"%cdouble %s",n,ValueName(p));break;
         case K_str:
         case K_YYYYMMDD:
         case K_HHMISS:
            fprintf(out,"%cchar *%s",n,ValueName(p));break;
         default:
            return(-1);
          };
      };
      p=p->next;
      n=',';
   };

   switch(aType)
   {
      case K_update_by_select:
         fprintf(out,"%c%s *_a_data,Select_Info *_a_sInfo)",n,tName);break;
      case K_update_by_lock:
         fprintf(out,"%c%s *_a_data,Lock_Info *_a_lock)",n,tName);break;
      case K_read_lock_by:
         fprintf(out,"%c%s *_a_data,Lock_Info *_a_lock)",n,tName);break;
      case K_read_lock:
         fprintf(out,"%c%s *_a_data,Lock_Info *_a_lock)",n,tName);break;
      case K_add:
      case K_read_by:
      case K_read:
      case K_sum_for:
      case K_avg_for:
      case K_avg_distinct_for:
      case K_count_distinct_for:
      case K_max_for:
      case K_min_for:
      case K_update:
      case K_update_by:
         fprintf(out,"%c%s *_a_data)",n,tName);  break;
      case K_count_by:
      case K_count:
         fprintf(out,"%clong *_a_cnt)",n); break;
      case K_open_select_by:
      case K_open_select_for_update_by:
      case K_open_select_for_update:
      case K_open_select:
      case K_open_select_distinct_for:
      case K_close_select:
         fprintf(out,"%cSelect_Info *_a_sInfo)",n);break;
      case K_fetch_select:
         fprintf(out,"%cSelect_Info *_a_sInfo,%s *_a_data)",n,tName);break;
      case K_free_lock:
         fprintf(out,"%cLock_Info *_a_lock)",n);break;
      case K_del_by:
      case K_del_all:
         fprintf(out,")");break;
      case K_print_report:
         fprintf(out,"FILE *out)");break;
      default:
         return(-1);
   };

   return(0);
   }
}

static int outputIncludeFileActionDef()
{
   int r;
   r=outputActionDef(i_file);
   if (r!=0)
      return(-1);
   fprintf(i_file,";\n");
   return(0);
}

static int outputSqlActionDef()
{
   int r;

   r=outputActionDef(sql_file);
   if (r!=0)
     return(-1);
   return(0);
}

static int outputCfileActionDef()
{
   int   r;
   r=outputActionDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\n");
   return(0);
}

static int outputFieldList(FILE *out)
{
   FieldDef   *f;
   int      n;

   f=fList.head;
        n=0;
   while(f!=NULL)
   {
      if (strcmp(f->name,"_order_by_")==0)
         return(0);

                if (strcmp(f->name, "_by_") == 0)
                  return(0); 

      switch(f->type)
      {
         case K_long:
         case K_short:
         case K_int:
         case K_double:
         case K_str:
         case K_YYYYMMDD:
         case K_HHMISS:
            fprintf(out," \\n\\\n\t\t\t%c%s",(n==0)?' ':',', f->name);
            break;
         default:
            return(-1);
      };
      f=f->next;
      n++;
   };
   return(0);
}
static int outputSqlWhere(FILE *out)
{
   FieldDef   *f;
   ArgDef      *p;
   int      n;
   int      or_flag=0;
   int      first=1;

   p=aList.head;
   n=0;
   or_flag=0;
   while(p!=NULL)
   {
      if (strcmp(p->name,"_order_by_")==0)
         break;

      if (strcmp(p->name,"_or_")==0)
         or_flag=1;
      else
      {
         if (first)
         {
            fprintf(out,"\\\n\t\tWHERE ");
            first=0;
         };

         f=findFieldDef(p->name);
         if (f==NULL)
            return(-1);
         switch(f->type)
         {
         case K_long:
         case K_short:
         case K_int:
         case K_double:
         case K_str:
         case K_YYYYMMDD:
         case K_HHMISS:
            fprintf(out,"%s%s %s:%s",
                                        (n==0)?"":
               (or_flag?" OR \\n\\\n\t\t\t":
                     " AND \\n\\\n\t\t\t"),
               p->name,p->operate,ValueName(p));
            break;
         default:
            return(-1);
         };
         or_flag=0;
      };
      n++;
      p=p->next;
   };
   return(outputOrderBy(out,p));
}

static int outputMathsSelectRecvDef(FILE *out)
{
        FieldDef *f;
   ArgDef   *p;

   p=aList.head;
   while(p!=NULL)
   {
                if (strcmp(p->name, "_by_") == 0){
                    break;
                }

      f=findFieldDef(p->name);
      if (f==NULL)
         return(-1);
      switch(f->type)
      {
         case K_long:
            fprintf(out,"\t\t&R_%s.%s,4,DT_ITG,\n",
                  tableName,f->name);
            break;
         case K_short:
            fprintf(out,"\t\t&R_%s.%s,2,DT_ITG,\n",
                  tableName,f->name);
            break;
         case K_int:
            fprintf(out,"\t\t&R_%s.%s,4,DT_ITG,\n",
                  tableName,f->name);
            break;
         case K_double:
            fprintf(out,"\t\t&R_%s.%s,8,DT_REA,\n",
                  tableName,f->name);
            break;
         case K_str:
         case K_YYYYMMDD:
         case K_HHMISS:
            break;  
         default:
            return(-1);
      };
      p=p->next;
   };
   return(0);
}

static int outputMaxMinSelectRecvDef(FILE *out)
{
        FieldDef *f;
   ArgDef   *p;

   p=aList.head;
   while(p!=NULL)
   {
                if (strcmp(p->name, "_by_") == 0)
                    break;
                if (strcmp(p->name, "_order_by_") == 0)
                    break;

      f=findFieldDef(p->name);
      if (f==NULL)
         return(-1);
      switch(f->type)
      {
         case K_long:
            fprintf(out,"\t\t&R_%s.%s,4,DT_ITG,\n",
                  tableName,f->name);
            break;
         case K_short:
            fprintf(out,"\t\t&R_%s.%s,2,DT_ITG,\n",
                  tableName,f->name);
            break;
         case K_int:
            fprintf(out,"\t\t&R_%s.%s,4,DT_ITG,\n",
                  tableName,f->name);
            break;
         case K_double:
            fprintf(out,"\t\t&R_%s.%s,8,DT_REA,\n",
                  tableName,f->name);
            break;

         case K_str:
            fprintf(out,"\t\tR_%s.%s,%d,DT_STR,\n",
                  tableName,f->name,f->size);
            break;
         case K_YYYYMMDD:
            fprintf(out,"\t\tR_%s.%s,10,DT_STR,\n",
                  tableName,f->name,f->size);
            break;
         case K_HHMISS:
            fprintf(out,"\t\tR_%s.%s,8,DT_STR,\n",
                  tableName,f->name,f->size);
            break;
         default:
            return(-1);
      };
      p=p->next;
   };
   return(0);
}
static int outputSelectRecvDef(FILE *out)
{
   FieldDef   *f;

   f=fList.head;
   while(f!=NULL)
   {
      switch(f->type)
      {
         case K_long:
            fprintf(out,"\t\t&R_%s.%s,4,DT_ITG,\n",
                  tableName,f->name);
            break;
         case K_short:
            fprintf(out,"\t\t&R_%s.%s,2,DT_ITG,\n",
                  tableName,f->name);
            break;
         case K_int:
            fprintf(out,"\t\t&R_%s.%s,4,DT_ITG,\n",
                  tableName,f->name);
            break;
         case K_double:
            fprintf(out,"\t\t&R_%s.%s,8,DT_REA,\n",
                  tableName,f->name);
            break;
         case K_str:
            fprintf(out,"\t\tR_%s.%s,%d,DT_STR,\n",
                  tableName,f->name,f->size);
            break;
         case K_YYYYMMDD:
            fprintf(out,"\t\tR_%s.%s,10,DT_STR,\n",
                  tableName,f->name,f->size);
            break;
         case K_HHMISS:
            fprintf(out,"\t\tR_%s.%s,8,DT_STR,\n",
                  tableName,f->name,f->size);
            break;
         default:
            return(-1);
      };
      f=f->next;
   };
   return(0);
}
static int outputMathsWhereValueDef(FILE *out)
{
   FieldDef   *f;
   ArgDef      *p;

   p=aList.head;
        while ((p!=NULL) && (strcmp(p->name, "_by_") != 0) ){
          p=p->next;
        }
   if (p!=NULL)
          p=p->next;

   while(p!=NULL)
   {
      f=findFieldDef(p->name);
      if (f==NULL)
         return(-1);
      switch(f->type)
      {
         case K_long:
            fprintf(out,"\t\t\":%s\",&%s,4,DT_ITG,\n",
                  ValueName(p),ValueName(p));
            break;
         case K_short:
            fprintf(out,"\t\t\":%s\",&%s,2,DT_ITG,\n",
                  ValueName(p),ValueName(p));
            break;
         case K_int:
            fprintf(out,"\t\t\":%s\",&%s,4,DT_ITG,\n",
                  ValueName(p),ValueName(p));
            break;
         case K_double:
            fprintf(out,"\t\t\":%s\",&%s,8,DT_REA,\n",
                  ValueName(p),ValueName(p));
            break;
         case K_str:
         case K_YYYYMMDD:
         case K_HHMISS:
            fprintf(out,"\t\t\":%s\",%s,-1,DT_STR,\n",
                  ValueName(p),ValueName(p));
            break;
         default:
            return(-1);
      };
      p=p->next;
   };
   return(0);
}

static int outputMaxMinWhereValueDef(FILE *out)
{
   FieldDef   *f;
   ArgDef      *p;

   p=aList.head;
        while ((p!=NULL) && (strcmp(p->name, "_by_") != 0) ){
          p=p->next;
        }
   if (p!=NULL)
          p=p->next;

   while(p!=NULL)
   {
      if(strcmp(p->name,"_order_by_")==0)
            break;

      f=findFieldDef(p->name);
      if (f==NULL)
         return(-1);
      switch(f->type)
      {
         case K_long:
            fprintf(out,"\t\t\":%s\",&%s,4,DT_ITG,\n",
                  ValueName(p),ValueName(p));
            break;
         case K_short:
            fprintf(out,"\t\t\":%s\",&%s,2,DT_ITG,\n",
                  ValueName(p),ValueName(p));
            break;
         case K_int:
            fprintf(out,"\t\t\":%s\",&%s,4,DT_ITG,\n",
                  ValueName(p),ValueName(p));
            break;
         case K_double:
            fprintf(out,"\t\t\":%s\",&%s,8,DT_REA,\n",
                  ValueName(p),ValueName(p));
            break;
         case K_str:
         case K_YYYYMMDD:
         case K_HHMISS:
            fprintf(out,"\t\t\":%s\",%s,-1,DT_STR,\n",
                  ValueName(p),ValueName(p));
            break;
         default:
            return(-1);
      };
      p=p->next;
   };
   return(0);
}
static int outputWhereValueDef(FILE *out)
{
   FieldDef   *f;
   ArgDef      *p;

   p=aList.head;
   while(p!=NULL)
   {
      if(strcmp(p->name,"_order_by_")==0)
            break;
      f=findFieldDef(p->name);
   if(strcmp(p->name,"_or_")!=0){
      if (f==NULL)
         return(-1);
      switch(f->type)
      {
         case K_long:
            fprintf(out,"\t\t\":%s\",&%s,4,DT_ITG,\n",
                  ValueName(p),ValueName(p));
            break;
         case K_short:
            fprintf(out,"\t\t\":%s\",&%s,2,DT_ITG,\n",
                  ValueName(p),ValueName(p));
            break;
         case K_int:
            fprintf(out,"\t\t\":%s\",&%s,4,DT_ITG,\n",
                  ValueName(p),ValueName(p));
            break;
         case K_double:
            fprintf(out,"\t\t\":%s\",&%s,8,DT_REA,\n",
                  ValueName(p),ValueName(p));
            break;
         case K_str:
         case K_YYYYMMDD:
         case K_HHMISS:
            fprintf(out,"\t\t\":%s\",%s,-1,DT_STR,\n",
                  ValueName(p),ValueName(p));
            break;
         default:
            return(-1);
      };
      }
      p=p->next;
   };
   return(0);
}


static int outputMathsFieldList(FILE *out)
{
        ArgDef        *p;
   FieldDef   *f;
   int      n;

   p=aList.head;
        n=0;
   while(p!=NULL)
   {
      if (strcmp(p->name,"_order_by_")==0)
         return(-1);

                if (strcmp(p->name, "_by_") == 0)
                  return(0); 

      f=findFieldDef(p->name);
      if (f==NULL)
         return(-1);
      switch(f->type)
      {
         case K_long:
         case K_short:
         case K_int:
         case K_double:
         case K_str:
           if (aType == K_sum_for)
             fprintf(out," \\\n\t\t\t%cSUM(%s)",
                                                (n==0)?' ':',', f->name);
           if (aType == K_avg_for)
             fprintf(out," \\\n\t\t\t%cAVG(%s)",
                                                (n==0)?' ':',', f->name);
           if (aType == K_avg_distinct_for)
             fprintf(out," \\\n\t\t\t%cAVG(DISTINCT(%s))",
                                                (n==0)?' ':',', f->name);
           if (aType == K_count_distinct_for)
                fprintf(out," \\\n\t\t\t%cCOUNT(DISTINCT(%s))",
                                                (n==0)?' ':',', f->name);
                      n++; 
            break;
                       
/*
         case K_str:
         case K_YYYYMMDD:
         case K_HHMISS:
                      break;
*/
                        default:
            return(-1);
      };
      p=p->next;
   };
   return(0);
}


static int outputMaxMinFieldList(FILE *out)
{
        ArgDef        *p;
   FieldDef   *f;
   int      n;

   p=aList.head;
        n=0;
   while(p!=NULL)
   {
      if (strcmp(p->name,"_order_by_")==0)
         return(0);

                if (strcmp(p->name, "_by_") == 0)
                  return(0); 

      f=findFieldDef(p->name);
      if (f==NULL)
         return(-1);
      switch(f->type)
      {
         case K_long:
         case K_short:
         case K_int:
         case K_double:
         case K_str:
         case K_YYYYMMDD:
         case K_HHMISS:
            if (aType == K_max_for)
               fprintf(out," \\\n\t\t\t%cMAX(%s)",
                                                (n==0)?' ':',', f->name);
            if (aType == K_min_for)
               fprintf(out," \\\n\t\t\t%cMIN(%s)",
                                                (n==0)?' ':',', f->name);
            if (aType == K_open_select_distinct_for)
               fprintf(out," \\\n\t\t\t%c%s",
                                                (n==0)?' ':',', f->name);
            break;
                        default:
            return(-1);
      };
                n++; 
      p=p->next;
   };
   return(0);
}
static int outputMathsSqlWhere(FILE *out)
{
   FieldDef   *f;
   ArgDef      *p;
   int      n;
   int      or_flag=0;

   p=aList.head;
        while ((p!=NULL) && (strcmp(p->name, "_by_") != 0) ){
          p=p->next;
        }

        if ( p!=NULL){
          p=p->next;
          if (p!=NULL)
            fprintf(out, "\\\n\t\tWHERE ");
        }
   n=0;
   or_flag=0;
   while(p!=NULL)
   {
      if (strcmp(p->name,"_order_by_")==0)
         break;

   if (strcmp(p->name,"_or_")==0)
      or_flag=1;
   else
   {
      f=findFieldDef(p->name);
      if (f==NULL)
         return(-1);
      switch(f->type)
         {
         case K_long:
         case K_short:
         case K_int:
         case K_double:
         case K_str:
         case K_YYYYMMDD:
         case K_HHMISS:
            fprintf(out,"%s%s %s:%s",
                                        (n==0)?"":
               (or_flag?" OR \\\n\t\t\t":
                   " AND \\\n\t\t\t"),
               p->name,p->operate,ValueName(p));
            break;
         default:
            return(-1);
         };
      or_flag=0;
      };
      n++;
      p=p->next;
   };
   return(0);
}
static int outputOrderBy(FILE *out,ArgDef *p)
{
   FieldDef   *f;
   int      n;

   if (p!=NULL)
   {
      fprintf(out," \\\n\t\tORDER BY ");
      n=0;
      p=p->next;
   
      while(p!=NULL)
      {
         if (strcmp(p->name,"DESC")==0)
         {
            fprintf(out," DESC");
         }
         else
         {
            f=findFieldDef(p->name);
            if (f==NULL)
               return(-1);
            fprintf(out,"%s%s",
                  (n==0)?"":",",
                  p->name);
         };
         p=p->next;
         n++;
      };
   };
   return(0);
}

static int outputMaxMinSqlWhere(FILE *out)
{
   FieldDef   *f;
   ArgDef      *p;
   int      n;
   int      or_flag;

   p=aList.head;
        while (p!=NULL)
   {
      if (strcmp(p->name, "_by_") == 0) 
         break;
      if (strcmp(p->name, "_order_by_") == 0) 
         goto ORDER;
             p=p->next;
        }

        if ( p!=NULL){
          p=p->next;
          if (p!=NULL)
            fprintf(out, "\\\n\t\tWHERE ");
        }
   n=0;
   or_flag=0;
   while(p!=NULL)
   {
      if (strcmp(p->name,"_order_by_")==0)
         break;

      if(strcmp(p->name,"_or_")==0) or_flag=1;
      else {
         f=findFieldDef(p->name);
         if (f==NULL)
            return(-1);
         switch(f->type)
         {
            case K_long:
            case K_short:
            case K_int:
            case K_double:
            case K_str:
            case K_YYYYMMDD:
            case K_HHMISS:
            fprintf(out,"%s%s %s:%s",
                                        (n==0)?"":
               (or_flag?" OR \\\n\t\t\t":
                   " AND \\\n\t\t\t"),
                    p->name,p->operate,ValueName(p));
            break;
            default:
            return(-1);
         };
         or_flag=0;
      }
      n++;
      p=p->next;
   };
ORDER:
   return(outputOrderBy(out,p));
}
static int outputCfileActionMathsFor()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"        DBcurs  _a_curs;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"   memset(&R_%s,0,sizeof(%s));\n",tableName,tName);
   fprintf(c_file,"        r=dbCursOpen(&_a_curs);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"                return(r);\n");
   fprintf(c_file,"        r=dbCursDefineSelect_va(&_a_curs,\n");
   fprintf(c_file,"                \"SELECT ");
   r=outputMathsFieldList(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file," \\\n\t\tFROM %s ", tableName);
   r=outputMathsSqlWhere(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\",\n");
   r=outputMathsSelectRecvDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL,\n");
   r=outputMathsWhereValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");

   fprintf(c_file,"        r=dbCursExec(&_a_curs);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
   fprintf(c_file,"        r=dbCursFetch(&_a_curs);\n");
   fprintf(c_file,"        if (r==SQLNOTFOUND) goto E;\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
        fprintf(c_file, "\n\n\t%s_EraseTailSpace(&R_%s);\n", tName,tableName);
   fprintf(c_file,"        memcpy(_a_data,&R_%s,sizeof(%s));\n",
                     tableName,tName);
   if (hasDAC)
   {
      fprintf(c_file,"        r=%s_%s_check_dac(_a_data,_a_data->dac);\n",
            sName,tableName);
   };
   fprintf(c_file,"\n");
   fprintf(c_file,"  E:\n");
   fprintf(c_file,"        dbCursClose(&_a_curs);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}

static int outputCfileActionMaxMinFor()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"        DBcurs  _a_curs;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"   memset(&R_%s,0,sizeof(%s));\n",tableName,tName);
   fprintf(c_file,"        r=dbCursOpen(&_a_curs);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"                return(r);\n");
   fprintf(c_file,"        r=dbCursDefineSelect_va(&_a_curs,\n");
   fprintf(c_file,"                \"SELECT ");
   r=outputMaxMinFieldList(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file," \\\n\t\tFROM %s ", tableName);
   r=outputMaxMinSqlWhere(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\",\n");
   r=outputMaxMinSelectRecvDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL,\n");
   r=outputMaxMinWhereValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");

   fprintf(c_file,"        r=dbCursExec(&_a_curs);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
   fprintf(c_file,"        r=dbCursFetch(&_a_curs);\n");
   fprintf(c_file,"        if (r==SQLNOTFOUND) goto E;\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
        fprintf(c_file, "\n\n\t%s_EraseTailSpace(&R_%s);\n", tName,tableName);
   fprintf(c_file,"        memcpy(_a_data,&R_%s,sizeof(%s));\n",
                     tableName,tName);
   if (hasDAC)
   {
      fprintf(c_file,"        r=%s_%s_check_dac(_a_data,_a_data->dac);\n",
            sName,tableName);
   };
   fprintf(c_file,"\n");
   fprintf(c_file,"  E:\n");
   fprintf(c_file,"        dbCursClose(&_a_curs);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}
static int outputCfileActionOpenSelectDistinctFor()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"        r=dbCursOpen(_a_sInfo);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"                return(r);\n");
   fprintf(c_file,"        r=dbCursDefineSelect_va(_a_sInfo,\n");
   fprintf(c_file,"                \"SELECT DISTINCT ");
   r=outputMaxMinFieldList(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file," \\\n\t\tFROM %s ", tableName);
   r=outputMaxMinSqlWhere(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\",\n");
   r=outputMaxMinSelectRecvDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL,\n");
   r=outputMaxMinWhereValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");

   fprintf(c_file,"        r=dbCursExec(_a_sInfo);\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"  E:\n");
   fprintf(c_file,"        if (r!=0) dbCursClose(_a_sInfo);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}

static int outputCfileActionReadBy()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"        DBcurs  _a_curs;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"   memset(&R_%s,0,sizeof(%s));\n",tableName,tName);
   fprintf(c_file,"        r=dbCursOpen(&_a_curs);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"                return(r);\n");
   fprintf(c_file,"        r=dbCursDefineSelect_va(&_a_curs,\n");
   fprintf(c_file,"                \"SELECT ");
   r=outputFieldList(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file," \\\n\t\tFROM %s ",tableName);
   r=outputSqlWhere(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\",\n");
   r=outputSelectRecvDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL,\n");
   r=outputWhereValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");

   fprintf(c_file,"        r=dbCursExec(&_a_curs);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
   fprintf(c_file,"        r=dbCursFetch(&_a_curs);\n");
   fprintf(c_file,"        if (r==SQLNOTFOUND) goto E;\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
        fprintf(c_file, "\n\n\t%s_EraseTailSpace(&R_%s);\n", tName,tableName);
   fprintf(c_file,"        memcpy(_a_data,&R_%s,sizeof(%s));\n",
                     tableName,tName);
   if (hasDAC)
   {
      fprintf(c_file,"        r=%s_%s_check_dac(_a_data,_a_data->dac);\n",
            sName,tableName);
   };
   fprintf(c_file,"\n");
   fprintf(c_file,"  E:\n");
   fprintf(c_file,"        dbCursClose(&_a_curs);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}

static int outputCfileActionRead()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"        DBcurs  _a_curs;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"   memset(&R_%s,0,sizeof(%s));\n",tableName,tName);
   fprintf(c_file,"        r=dbCursOpen(&_a_curs);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"                return(r);\n");
   fprintf(c_file,"        r=dbCursDefineSelect_va(&_a_curs,\n");
   fprintf(c_file,"                \"SELECT ");
   r=outputFieldList(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file," \\\n\t\tFROM %s \",\n",tableName);
   r=outputSelectRecvDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL,\n");
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");

   fprintf(c_file,"        r=dbCursExec(&_a_curs);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
   fprintf(c_file,"        r=dbCursFetch(&_a_curs);\n");
   fprintf(c_file,"        if (r==SQLNOTFOUND) goto E;\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
        fprintf(c_file, "\n\n\t%s_EraseTailSpace(&R_%s);\n", tName,tableName);
   fprintf(c_file,"        memcpy(_a_data,&R_%s,sizeof(%s));\n",
                     tableName,tName);
   if (hasDAC)
   {
      fprintf(c_file,"        r=%s_%s_check_dac(_a_data,_a_data->dac);\n",
            sName,tableName);
   };
   fprintf(c_file,"\n");
   fprintf(c_file,"  E:\n");
   fprintf(c_file,"        dbCursClose(&_a_curs);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}
static int outputCfileActionReadLock()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"   memset(&R_%s,0,sizeof(%s));\n",tableName,tName);
   fprintf(c_file,"        r=dbCursOpen(_a_lock);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"                return(r);\n");
   fprintf(c_file,"        r=dbCursDefineSelect_va(_a_lock,\n");
   fprintf(c_file,"                \"SELECT ");
   r=outputFieldList(c_file);
   if (r!=0)
      return(-1);

   fprintf(c_file," \\\n\t\tFROM %s ",tableName);
   fprintf(c_file," \\\n\t\tFOR UPDATE WAIT 20\",\n");
   r=outputSelectRecvDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL,\n");
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");

   fprintf(c_file,"        r=dbCursExec(_a_lock);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
   fprintf(c_file,"        r=dbCursFetch(_a_lock);\n");
   fprintf(c_file,"        if (r==SQLNOTFOUND) goto E;\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
        fprintf(c_file, "\n\n\t%s_EraseTailSpace(&R_%s);\n", tName,tableName);
   fprintf(c_file,"        memcpy(_a_data,&R_%s,sizeof(%s));\n",
                     tableName,tName);
   if (hasDAC)
   {
      fprintf(c_file,"        r=%s_%s_check_dac(_a_data,_a_data->dac);\n",
            sName,tableName);
   };
   fprintf(c_file,"\n");
   fprintf(c_file,"  E:\n");
   fprintf(c_file,"        if (r!=0) dbCursClose(_a_lock);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}
static int outputCfileActionReadLockBy()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"   memset(&R_%s,0,sizeof(%s));\n",tableName,tName);
   fprintf(c_file,"        r=dbCursOpen(_a_lock);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"                return(r);\n");
   fprintf(c_file,"        r=dbCursDefineSelect_va(_a_lock,\n");
   fprintf(c_file,"                \"SELECT ");
   r=outputFieldList(c_file);
   if (r!=0)
      return(-1);

   fprintf(c_file," \\\n\t\tFROM %s ",tableName);
   r=outputSqlWhere(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file," \\\n\t\tFOR UPDATE WAIT 25\",\n");
   r=outputSelectRecvDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL,\n");
   r=outputWhereValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");

   fprintf(c_file,"        r=dbCursExec(_a_lock);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
   fprintf(c_file,"        r=dbCursFetch(_a_lock);\n");
   fprintf(c_file,"        if (r==SQLNOTFOUND) goto E;\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
        fprintf(c_file, "\n\n\t%s_EraseTailSpace(&R_%s);\n", tName,tableName);
   fprintf(c_file,"        memcpy(_a_data,&R_%s,sizeof(%s));\n",
                     tableName,tName);
   if (hasDAC)
   {
      fprintf(c_file,"        r=%s_%s_check_dac(_a_data,_a_data->dac);\n",
            sName,tableName);
   };
   fprintf(c_file,"\n");
   fprintf(c_file,"  E:\n");
   fprintf(c_file,"        if (r!=0) dbCursClose(_a_lock);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}
static int outputCfileActionOpenSelectBy()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"        r=dbCursOpen(_a_sInfo);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"                return(r);\n");
   fprintf(c_file,"        r=dbCursDefineSelect_va(_a_sInfo,\n");
   fprintf(c_file,"                \"SELECT ");
   r=outputFieldList(c_file);
   if (r!=0)
      return(-1);

   fprintf(c_file," \\\n\t\tFROM %s ",tableName);
   r=outputSqlWhere(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\",\n");
   r=outputSelectRecvDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL,\n");
   r=outputWhereValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");

   fprintf(c_file,"        r=dbCursExec(_a_sInfo);\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"  E:\n");
   fprintf(c_file,"        if (r!=0) dbCursClose(_a_sInfo);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}

static int outputCfileActionOpenSelectForUpdateBy()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"        r=dbCursOpen(_a_sInfo);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"                return(r);\n");
   fprintf(c_file,"        r=dbCursDefineSelect_va(_a_sInfo,\n");
   fprintf(c_file,"                \"SELECT ");
   r=outputFieldList(c_file);
   if (r!=0)
      return(-1);

   fprintf(c_file," \\\n\t\tFROM %s ",tableName);
   r=outputSqlWhere(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file," \\\n\t\tFOR UPDATE WAIT 25 \",\n");
   r=outputSelectRecvDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL,\n");
   r=outputWhereValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");

   fprintf(c_file,"        r=dbCursExec(_a_sInfo);\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"  E:\n");
   fprintf(c_file,"        if (r!=0) dbCursClose(_a_sInfo);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}
static int outputCfileActionOpenSelect()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"        r=dbCursOpen(_a_sInfo);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"                return(r);\n");
   fprintf(c_file,"        r=dbCursDefineSelect_va(_a_sInfo,\n");
   fprintf(c_file,"                \"SELECT ");
   r=outputFieldList(c_file);
   if (r!=0)
      return(-1);

   fprintf(c_file," \\\n\t\tFROM %s ",tableName);
   r=outputSqlWhere(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\",\n",tableName);
   r=outputSelectRecvDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL,\n");
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");

   fprintf(c_file,"        r=dbCursExec(_a_sInfo);\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"  E:\n");
   fprintf(c_file,"        if (r!=0) dbCursClose(_a_sInfo);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}

static int outputCfileActionOpenSelectForUpdate()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"        r=dbCursOpen(_a_sInfo);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"                return(r);\n");
   fprintf(c_file,"        r=dbCursDefineSelect_va(_a_sInfo,\n");
   fprintf(c_file,"                \"SELECT ");
   r=outputFieldList(c_file);
   if (r!=0)
      return(-1);

   fprintf(c_file," \\\n\t\tFROM %s FOR UPDATE WAIT 25\",\n",tableName);
   r=outputSelectRecvDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL,\n");
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");

   fprintf(c_file,"        r=dbCursExec(_a_sInfo);\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"  E:\n");
   fprintf(c_file,"        if (r!=0) dbCursClose(_a_sInfo);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}
static int outputCfileActionFetchSelect()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"   memset(&R_%s,0,sizeof(%s));\n",tableName,tName);
   fprintf(c_file,"        r=dbCursFetch(_a_sInfo);\n");
   fprintf(c_file,"        if (r==SQLNOTFOUND) goto E;\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
        fprintf(c_file, "\n\n\t%s_EraseTailSpace(&R_%s);\n", tName,tableName);
   fprintf(c_file,"        memcpy(_a_data,&R_%s,sizeof(%s));\n",
                     tableName,tName);
   if (hasDAC)
   {
      fprintf(c_file,"        r=%s_%s_check_dac(_a_data,_a_data->dac);\n",
            sName,tableName);
   };
   fprintf(c_file,"\n");
   fprintf(c_file,"  E:\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}
static int outputInsertFieldDef(FILE *out)
{
   FieldDef   *f;
   char      n;

   f=fList.head;
   n=' ';
   while(f!=NULL)
   {
      fprintf(out,"%c%s \\n\\\n\t\t",n,f->name);
      n=',';
      f=f->next;
   };
   return(0);
}
static int outputInsertValueDef(FILE *out)
{
   FieldDef   *f;
   char      n;

   f=fList.head;
   n=' ';

   while(f!=NULL)
   {
      switch(f->type)
      {
         case K_long:
         case K_short:
         case K_int:
         case K_str:
         case K_double:
         case K_YYYYMMDD:
         case K_HHMISS:
            fprintf(out," \\n\\\n\t\t\t%c:U_%s",n,f->name);
            break;
         default:
            return(-1);
      };
      n=',';
      f=f->next;
   };
   return(0);
}

static int outputInsertUpdateHostValueDef(FILE *out)
{
   FieldDef   *f;

   f=fList.head;
   while(f!=NULL)
   {
      switch(f->type)
      {
         case K_long:
            fprintf(out,"\t\t\":U_%s\",&R_%s.%s,4,DT_ITG,\n",
                  f->name,tableName,f->name);
            break;
         case K_short:
            fprintf(out,"\t\t\":U_%s\",&R_%s.%s,2,DT_ITG,\n",
                  f->name,tableName,f->name);
            break;
         case K_int:
            fprintf(out,"\t\t\":U_%s\",&R_%s.%s,4,DT_ITG,\n",
                  f->name,tableName,f->name);
            break;
         case K_double:
            fprintf(out,"\t\t\":U_%s\",&R_%s.%s,8,DT_REA,\n",
                  f->name,tableName,f->name);
            break;
         case K_str:
         case K_YYYYMMDD:
         case K_HHMISS:
            fprintf(out,"\t\t\":U_%s\",R_%s.%s,-1,DT_STR,\n",
                  f->name,tableName,f->name);
            break;
         default:
            return(-1);
      };
      f=f->next;
   };
   return(0);
}

static int outputCfileActionAdd()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   if (hasDAC)
   {
      fprintf(c_file,"        %s_%s_creat_dac(_a_data,_a_data->dac);\n",
            sName,tableName);
   };
   fprintf(c_file,"        memcpy(&R_%s,_a_data,sizeof(%s));\n",
               tableName,tName);
   fprintf(c_file,"        r=dbExecSql_va(\"INSERT INTO %s \\\n\t\t(",tableName);
   r=outputInsertFieldDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,") \\\n\t\tVALUES (");
   r=outputInsertValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,")\",\n");
   r=outputInsertUpdateHostValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}
static int outputCfileActionDel()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"        r=dbExecSql_va(\"DELETE FROM %s ",tableName);
   r=outputSqlWhere(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\",\n");
   r=outputWhereValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"\tif (r==0&&db_row_process_count==0)\n");
   fprintf(c_file,"\t\tr=SQLNOTFOUND;\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}
static int outputCfileActionCount()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"        long   _a_count;\n");
   fprintf(c_file,"        DBcurs  _a_curs;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"        r=dbCursOpen(&_a_curs);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"                return(r);\n");
   fprintf(c_file,"        r=dbCursDefineSelect_va(&_a_curs,\n");
   fprintf(c_file,"           \"SELECT COUNT(*) ");
/***
   fprintf(c_file,"\\\n\t\tINTO :_a_count ");
***/
   fprintf(c_file," \\\n\t\tFROM %s",tableName);
   fprintf(c_file,"\",\n");
   fprintf(c_file,"\t\t&_a_count,4,DT_ITG,\n");
   fprintf(c_file,"\t\tNULL,\n");
   r=outputWhereValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                return(r);\n");
   fprintf(c_file,"        };\n");

   fprintf(c_file,"        r=dbCursExec(&_a_curs);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
   fprintf(c_file,"        r=dbCursFetch(&_a_curs);\n");
   fprintf(c_file,"        if (r==SQLNOTFOUND) goto E;\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
        fprintf(c_file, "\n\n\t%s_EraseTailSpace(&R_%s);\n", tName,tableName);

   fprintf(c_file,"        *_a_cnt=_a_count;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"  E:\n");
   fprintf(c_file,"        dbCursClose(&_a_curs);\n");
   fprintf(c_file,"   return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}
static int outputCfileActionCountBy()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"        long   _a_count;\n");
   fprintf(c_file,"        DBcurs  _a_curs;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"        r=dbCursOpen(&_a_curs);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"                return(r);\n");
   fprintf(c_file,"        r=dbCursDefineSelect_va(&_a_curs,\n");
   fprintf(c_file,"           \"SELECT COUNT(*) ");
/***
   fprintf(c_file,"\\\n\t\tINTO :_a_count ");
***/
   fprintf(c_file," \\\n\t\tFROM %s ",tableName);
   r=outputSqlWhere(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\",\n");
   fprintf(c_file,"\t\t&_a_count,4,DT_ITG,\n");
   fprintf(c_file,"\t\tNULL,\n");
   r=outputWhereValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                return(r);\n");
   fprintf(c_file,"        };\n");

   fprintf(c_file,"        r=dbCursExec(&_a_curs);\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
   fprintf(c_file,"        r=dbCursFetch(&_a_curs);\n");
   fprintf(c_file,"        if (r==SQLNOTFOUND) goto E;\n");
   fprintf(c_file,"        if (r!=0)\n");
   fprintf(c_file,"        {\n");
   fprintf(c_file,"                goto E;\n");
   fprintf(c_file,"        };\n");
        fprintf(c_file, "\n\n\t%s_EraseTailSpace(&R_%s);\n", tName,tableName);

   fprintf(c_file,"        *_a_cnt=_a_count;\n");
   fprintf(c_file,"\n");
   fprintf(c_file,"  E:\n");
   fprintf(c_file,"        dbCursClose(&_a_curs);\n");
   fprintf(c_file,"   return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}

static int outputCfileActionDelAll()
{

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"        r=dbExecSql_va(\"DELETE FROM %s\",\n",tableName);
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}
static int outputUpdateValueDef(FILE *out)
{
   FieldDef   *f;
   char      n;

   f=fList.head;
   n=' ';
   while(f!=NULL)
   {
      switch(f->type)
      {
         case K_long:
         case K_short:
         case K_int:
         case K_double:
         case K_str:
         case K_YYYYMMDD:
         case K_HHMISS:
            fprintf(out," \\\n\t\t\t%c%s=:U_%s",n,f->name,f->name);
            break;
         default:
            return(-1);
      };
      n=',';
      f=f->next;
   };
   return(0);
}


static int outputCfileActionUpdate()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   if (hasDAC)
   {
      fprintf(c_file,"        %s_%s_creat_dac(_a_data,_a_data->dac);\n",
            sName,tableName);
   };
   fprintf(c_file,"        memcpy(&R_%s,_a_data,sizeof(%s));\n",
               tableName,tName);
   fprintf(c_file,"        r=dbExecSql_va(\"UPDATE %s \\\n\t\t\tSET ",tableName);
   r=outputUpdateValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\",\n");
   r=outputInsertUpdateHostValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"\tif (r==0&&db_row_process_count==0)\n");
   fprintf(c_file,"\t\tr=SQLNOTFOUND;\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}

static int outputCfileActionUpdateBy()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   if (hasDAC)
   {
      fprintf(c_file,"        %s_%s_creat_dac(_a_data,_a_data->dac);\n",
            sName,tableName);
   };
   fprintf(c_file,"        memcpy(&R_%s,_a_data,sizeof(%s));\n",
               tableName,tName);
   fprintf(c_file,"        r=dbExecSql_va(\"UPDATE %s \\\n\t\t\tSET ",tableName);
   r=outputUpdateValueDef(c_file);
   if (r!=0)
      return(-1);
   r=outputSqlWhere(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\",\n");
   r=outputInsertUpdateHostValueDef(c_file);
   if (r!=0)
      return(-1);
   r=outputWhereValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"\tif (r==0&&db_row_process_count==0)\n");
   fprintf(c_file,"\t\tr=SQLNOTFOUND;\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}

static int outputCfileActionUpdateBySelect()
{
   int   r;

   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"        char %s_rowid[20];\n",tableName);
   if (hasDAC)
   {
      fprintf(c_file,"        %s_%s_creat_dac(_a_data,_a_data->dac);\n",
            sName,tableName);
   };
        if (aType == K_update_by_select)
          fprintf(c_file, "\n        dbCursGetRowID(_a_sInfo, %s_rowid, 20);\n", tableName);
        else
          fprintf(c_file, "\n        dbCursGetRowID(_a_lock, %s_rowid, 20);\n", tableName);

   fprintf(c_file,"        memcpy(&R_%s,_a_data,sizeof(%s));\n",
               tableName,tName);
   fprintf(c_file,"        r=dbExecSql_va(\"UPDATE %s \\\n\t\t\tSET ",tableName);
   r=outputUpdateValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file," \\\n\t\t\tWHERE ");
        fprintf(c_file, " rowid = :%s_rowid", tableName); 
   fprintf(c_file,"\",\n");
   r=outputInsertUpdateHostValueDef(c_file);
   fprintf(c_file,"\t\t\":%s_rowid\",%s_rowid,-1,DT_STR,\n", tableName, tableName);
   if (r!=0)
      return(-1);
   r=outputWhereValueDef(c_file);
   if (r!=0)
      return(-1);
   fprintf(c_file,"\t\tNULL);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}

static int outputCfileActionCloseSelect()
{
   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"        r=dbCursClose(_a_sInfo);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}
static int outputCfileActionFreeLock()
{
   fprintf(c_file,"{\n");
   fprintf(c_file,"        int     r;\n");
   fprintf(c_file,"        r=dbCursClose(_a_lock);\n");
   fprintf(c_file,"        return(r);\n");
   fprintf(c_file,"}\n\n");
   return(0);
}

static int outputCfileActionFunction()
{
   int   r;

   switch(aType)
   {
      case K_print_report:
         r=outputCfileActionPrintReport();   break;
      case K_read_by:
         r=outputCfileActionReadBy();      break;
      case K_read:
         r=outputCfileActionRead();      break;
      case K_sum_for:
      case K_avg_for:
      case K_avg_distinct_for:
      case K_count_distinct_for:
         r=outputCfileActionMathsFor();      break;
      case K_max_for:
      case K_min_for:
         r=outputCfileActionMaxMinFor();      break;
      case K_read_lock_by:
         r=outputCfileActionReadLockBy();   break;
      case K_read_lock:
         r=outputCfileActionReadLock();   break;
      case K_open_select_by:
         r=outputCfileActionOpenSelectBy();   break;
      case K_open_select_for_update_by:
         r=outputCfileActionOpenSelectForUpdateBy();
                    break;
      case K_open_select:
         r=outputCfileActionOpenSelect();   break;
      case K_open_select_for_update:
         r=outputCfileActionOpenSelectForUpdate();
                    break;
      case K_open_select_distinct_for:
         r=outputCfileActionOpenSelectDistinctFor();   break;
      case K_fetch_select:
         r=outputCfileActionFetchSelect();   break;
      case K_add:
         r=outputCfileActionAdd();      break;
      case K_del_by:
         r=outputCfileActionDel();      break;
      case K_del_all:
         r=outputCfileActionDelAll();      break;
      case K_update:
         r=outputCfileActionUpdate();      break;
      case K_update_by:
         r=outputCfileActionUpdateBy();      break;
      case K_update_by_select:
      case K_update_by_lock:
         r=outputCfileActionUpdateBySelect();   break;
      case K_close_select:
         r=outputCfileActionCloseSelect();   break;
      case K_free_lock:
         r=outputCfileActionFreeLock();      break;
      case K_count_by:
         r=outputCfileActionCountBy();      break;
      case K_count:
         r=outputCfileActionCount();      break;


      default:
         return(-1);
   };
   return(r);
}

static int outputCfileActionPrintReport()
{
  int n, len, tlen = 0;
  FieldDef *f;

  fprintf(c_file, "{");
  fprintf(c_file, "\n");
  fprintf(c_file, "\n\tint r,i;\n"); 
  fprintf(c_file, "\tSelect_Info aInfo;\n");
  fprintf(c_file, "\t%s P_%s;\n", tName, tableName);
  fprintf(c_file, "\n\tr = %s_%s_open_select(&aInfo);", sName, tableName);
  fprintf(c_file, "\n\tif (r!=0) \n\t\treturn(r);");
  fprintf(c_file, "\n\tfprintf(out, \"\\n");

  f = fList.head;
  n = 0;
  while (f!=NULL){
/*
    len = (f->size > strlen(f->name)) ? (f->size) : (strlen(f->name));
    tlen += len;
    fprintf(c_file, "%%-%ds ", len);
*/
    if (n!=0)
      fprintf(c_file, "\\t%%s");
    else
      fprintf(c_file, "%%s");
    n++;
    f = f->next; 
  }
  fprintf(c_file, "\\n\",");

  f = fList.head;
  n = 0;
  while (f!=NULL){
    if (n!=0)
      fprintf(c_file, ",");
    fprintf(c_file, "\"%s\" ", f->name);
    n++;
    f = f->next; 
  }
  fprintf(c_file, ");");
  fprintf(c_file, "\n\tfor (i=0; i<80; i++)");
  fprintf(c_file, "\n\t\tfprintf(out, \"-\");\n");   

  fprintf(c_file, "\n\twhile ((r = %s_%s_fetch_select(&aInfo, "
            ,sName, tableName); 
  fprintf(c_file, "&P_%s) == 0)){",tableName);

  fprintf(c_file, "\n\tfprintf(out, \"\\n");
  f = fList.head;
  n=0;
  while (f!=NULL){
    len = (f->size > strlen(f->name)) ? f->size : strlen(f->name);
    if (n!=0)
      len = len + n;
    switch(f->type){
   case K_long:
/*           fprintf(c_file,"%%-%dld ",len);break; */
      if (n!=0)
             fprintf(c_file,"\\t%%ld");
      else
             fprintf(c_file,"%%ld");
      break;
   case K_short:
/*      fprintf(c_file,"%%-%dd",len);break; */ 
      if (n!=0)
        fprintf(c_file,"\\t%%d");
      else
        fprintf(c_file,"%%d");
      break;
   case K_int:
/*      fprintf(c_file,"%%-%dd",len);break; */
      if (n!=0)
        fprintf(c_file,"\\t%%d");
      else
        fprintf(c_file,"%%d");
      break;
   case K_double:
/*      fprintf(c_file,"%%-%df",len);break; */
      if (n!=0)
        fprintf(c_file,"\\t%%f");
      else
        fprintf(c_file,"%%f");
      break;
   case K_str:
   case K_YYYYMMDD:
   case K_HHMISS:
/*      fprintf(c_file,"%%-%ds",len);break; */
      if (n!=0)
        fprintf(c_file,"\\t%%s");
      else
        fprintf(c_file,"%%s");
      break;
   default:
      return(-1);
    }
    n++;
    f = f->next; 
  }
  fprintf(c_file, "\",");

  f = fList.head;
  n = 0;
  while (f!=NULL){
    if (n!=0)
      fprintf(c_file, ",");
    fprintf(c_file, "P_%s.%s ", tableName,f->name);
    n++;
    f = f->next; 
  }
  fprintf(c_file, ");");
  fprintf(c_file, "\n\t}");

  fprintf(c_file, "\n\tif (r!=SQLNOTFOUND)");
  fprintf(c_file, "\n\t\treturn(r);");
  fprintf(c_file, "\n\tr = %s_%s_close_select(&aInfo);", sName, tableName);
  fprintf(c_file, "\n\tif (r!=0) \n\t\treturn(r);");
  fprintf(c_file, "\n\treturn(0);");
  fprintf(c_file, "\n}\n");
  return(0);
}

static int outputAction()
{
   int   r;

   if (( aType == K_index_by ) || 
            (aType == K_index_by_unique) ||
            (aType == K_index_unique_by)){
     r=outputSqlActionDef();
     return(r);
   }
   else{
     r=outputIncludeFileActionDef();
     if (r!=0)
       return(r);

     r=outputCfileActionDef();
     if (r!=0)
       return(r);
     r=outputCfileActionFunction();
     return(r);
       }
}

int processOracleAction(FILE *in)
{
   int   r;
   int   keyword;
   int   g;

   g=1;r=0;
   while(g&&(r==0))
   {
      r=getKeyword(in,&keyword);
      if (r!=0)
         return(-1);

      if (keyword==K_end){
         g=0;
                }  
      else
        if (keyword==K_action){
         r=getAction(in);
         if (r==0){
           r=outputAction();
         }
        }

   };
   return(r);
}

int outputCfileDACcode()
{
   int      r;
   FieldDef   *f;

   fprintf(c_file,"#include \"dac.h\"\n");
   fprintf(c_file,"static int %s_%s_put_dac_buf(%s *_a_data)\n",
            sName,tableName,tName);
   fprintf(c_file,"{\n");
   fprintf(c_file,"        DAC_buf_clear();\n");
   f=fList.head;
   while(f!=NULL)
   {
      if (strcmp(f->name,"dac")!=0)
         switch(f->type)
         {
         case K_long:
            fprintf(c_file,"\tDAC_buf_put_long(_a_data->%s);\n",f->name);
            break;
         case K_short:
            fprintf(c_file,"\tDAC_buf_put_short(_a_data->%s);\n",f->name);
            break;
         case K_int:
            fprintf(c_file,"\tDAC_buf_put_int(_a_data->%s);\n",f->name);
            break;
         case K_double:
            fprintf(c_file,"\tDAC_buf_put_double(&_a_data->%s);\n",f->name);
            break;
         case K_str:
         case K_YYYYMMDD:
         case K_HHMISS:
            fprintf(c_file,"\tKILL_SPACE(_a_data->%s);\n",
                        f->name);
            fprintf(c_file,"\tDAC_buf_put_str(_a_data->%s);\n",f->name);
            break;
         default:
            return(-1);
         };
      f=f->next;
   };
   fprintf(c_file,"        return(0);\n");
   fprintf(c_file,"}\n");
   fprintf(c_file,"static int %s_%s_check_dac(%s *_a_data,char *dac)\n",
               sName,tableName,tName);
   fprintf(c_file,"{\n");
   fprintf(c_file,"        %s_%s_put_dac_buf(_a_data);\n",
               sName,tableName);
   fprintf(c_file,"        return(DAC_buf_check_DAC(dac));\n");
   fprintf(c_file,"}\n");
   fprintf(c_file,"static int %s_%s_creat_dac(%s *_a_data,char *dac)\n",
               sName,tableName,tName);
   fprintf(c_file,"{\n");
   fprintf(c_file,"        %s_%s_put_dac_buf(_a_data);\n",
               sName,tableName);
   fprintf(c_file,"        return(DAC_buf_creat_DAC(dac));\n");
   fprintf(c_file,"}\n\n");

   return(0);
}


