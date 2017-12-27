#include <stdio.h>
#include <time.h>
#include <string.h>
#include "dbaction.h"

#define HERE	printf("here [%s][%d]\n",__FILE__,__LINE__);
extern FieldDef *findFieldDef(char *name);
static int outputEraseTailSpaceFunction() ;
static char *ValueName(ArgDef *p)
{
        static  char    ss[128];

        sprintf(ss,"%s_%s_%d",p->name,getOperateDef(p->operate),p->n);
        return(ss);
}

static int outputECfileArgToValue(FILE *out,ArgDef *p)
{
/** add by lhw 990531 **/
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
/** end **/

	while(p!=NULL){
	  if (strcmp(p->name,"_order_by_")==0)
			break;
	  if (strcmp(p->name,"_or_")!=0)
          	fprintf(out, "\n\t%s_%s = %s;", tName, ValueName(p), ValueName(p));
          p = p->next; 
        }
	return(0);
}

static int outputSqlField(FieldDef *field)
{
  FieldDef *ft;

  switch(field->type){

    case K_long:
      fprintf(sql_file,"\t%s\t INTEGER",field->name);
      break;

    case K_short:
      fprintf(sql_file,"\t%s\t SMALLINT",field->name);
      break;

    case K_int:
      fprintf(sql_file,"\t%s\t INTEGER",field->name);
      break;

    case K_double:
      fprintf(sql_file,"\t%s\t FLOAT",field->name);
/*
      if (field->size == 8)
	fprintf(sql_file,"\t%s\t FLOAT",field->name);
      else
	if (field->scale == 0)
	  fprintf(sql_file,"\t%s\t FLOAT(%d,2)",field->name, field->size-3);
	else
	  fprintf(sql_file,"\t%s\t FLOAT(%d,%d)",field->name, field->size, field->scale);
*/
      break;

    case K_str:
      fprintf(sql_file,"\t%s\t CHAR(%d)",
			   field->name,field->size);
      break;

    case K_YYYYMMDD:
      fprintf(sql_file,"\t%s\t CHAR(10)",field->name);
      break;

    case K_HHMISS:
      fprintf(sql_file,"\t%s\t CHAR(8)",field->name);
      break;
    case K_date:
      fprintf(sql_file,"\t%s\t DATE",field->name);
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
					field->name,field->size+1);	break;
		case K_YYYYMMDD:
			fprintf(i_file,"\tchar\t%s[11];\n",field->name);break;
		case K_HHMISS:
			fprintf(i_file,"\tchar\t%s[9];\n",field->name); break;
		case K_date:
			fprintf(i_file,"\tlong\t%s;\n",field->name);    break;

		default:
			return(-1);
	};
	return(0);
}

static int outputSqlHead()
{
	FieldDef	*p;

        fprintf(sql_file, "{\n");
	fprintf(sql_file, "  This File \"%s\"\n", SqlFileName);
	fprintf(sql_file, "  Genenated by\n");
	fprintf(sql_file, "  Application <dbaction> v2.0 for Informix\n");
	fprintf(sql_file, "  with the action file \"%s\".\n",actFileName);
	fprintf(sql_file, "  Create: %s ", systemDate);
        fprintf(sql_file, "}\n");

	fprintf(sql_file,"\nCREATE TABLE %s \n  (\n", tableName);
	p=fList.head;
	while( p!= NULL){
	  outputSqlField(p);
	  p=p->next;
	}
	fprintf(sql_file,"  ); \n");

	return(0);
}

static int outputIncludeHeadforCfile()
{

	FieldDef	*p;

        fprintf(i_file, "#else\n");
/******* Add by Wangly 三  3月 21 20:55:44 EST 2001 ******/
	fprintf(i_file,"\n#ifdef\t__cplusplus\nextern \"C\" {\n#endif\n\n");
/******* Add by Wangly 三  3月 21 20:55:44 EST 2001 End **/

	fprintf(i_file,"#include \"dbutil.h\"\n"); 
	if (hasDAC)
	{
		fprintf(i_file,"#include \"dac.h\"\n");
	};
	fprintf(i_file,"#include \"DBlog.h\"\n");
	fprintf(i_file,"\n");
	fprintf(i_file,"typedef struct {\n");
	p=fList.head;
	while(p!=NULL)
	{
		outputIncludeStructField(p);
		p=p->next;
	};
	fprintf(i_file,"        } %s;\n",tName);

/**** add By Wangly 三  3月 21 20:58:19 EST 2001	*****/
	fprintf(i_file,"\n#ifdef\t__cplusplus\n}\n#endif\n\n");
/**** add By Wangly End ****/
	fprintf(i_file,"\n#endif\n");
/******* Add by Wangly 三  3月 21 20:55:44 EST 2001 ******/
	fprintf(i_file,"\n#ifdef\t__cplusplus\nextern \"C\" {\n#endif\n\n");
/******* Add by Wangly 三  3月 21 20:55:44 EST 2001 End **/

}
static int outputIncludeHeadforECfile()
{

	FieldDef	*p;

        fprintf(i_file, "\n#ifdef INFORMIX_EC");
	fprintf(i_file,"\n$include dbutil.h;\n"); 
	fprintf(i_file,"\n");
	fprintf(i_file,"$typedef struct {\n");
	p=fList.head;
	while(p!=NULL)
	{
		outputIncludeStructField(p);
		p=p->next;
	};
	fprintf(i_file,"        } %s;\n",tName);
	fprintf(i_file,"\n");
}
static int outputIncludeHead()
{
	FieldDef	*p;

	fprintf(i_file, "/*\n This Include File \"%s\" \n Genenated By\n",includeFileName);
	fprintf(i_file, " Application <dbaction> v2.0 for Informix \n");
	fprintf(i_file, " with the action file \"%s\".\n", actFileName);
	fprintf(i_file, " Create: %s*/\n\n", systemDate);

	fprintf(i_file,"#ifndef _%s_%s_INCLUDE_H_\n",sName,tableName);
	fprintf(i_file,"#define _%s_%s_INCLUDE_H_\n",sName,tableName);

        outputIncludeHeadforECfile();
        outputIncludeHeadforCfile();
  	fprintf(i_file, "int %s_%s_debug_print(char *reason,%s *adata, char *filename, int line_no);\n", sName,tableName, tName);
	if (hasDAC)
	{
	fprintf(i_file,"int %s_%s_check_dac(%s *_a_data,char *dac);\n",
					sName,tableName,tName);
	fprintf(i_file,"int %s_%s_creat_dac(%s *_a_data,char *dac);\n",
					sName,tableName,tName);
	};
         
	return(0);
}

static int outputECfileHead()
{

	fprintf(ec_file,"/* \n This EC File \"%s\" \n Genenated By\n", cFileName);
	fprintf(ec_file, " Application <dbaction> v2.0 for Informix \n");
	fprintf(ec_file, " with the action file \"%s\".\n", actFileName);
	fprintf(ec_file, " Create: %s*/\n\n", systemDate);

	fprintf(ec_file,"#include <stdio.h>\n");
	fprintf(ec_file,"#include <string.h>\n");
	fprintf(ec_file,"$include \"%s\";\n",includeFileName);

        outputEraseTailSpaceFunction();
	return(0);
}
static int outputEraseTailSpaceFunction()
{
	FieldDef *f;

	fprintf(ec_file, "\nstatic int %s_EraseTailSpace(%s *_erase_data)",tName, tName);
        fprintf(ec_file, "\n{");

        f = fList.head;
        while (f != NULL ){
          if ( f->type == K_str )
            fprintf(ec_file, "\n\tERASE_TAIL_SPACE(_erase_data->%s);", f->name); 
          f = f->next;
       }
       fprintf(ec_file, "\n\treturn(0);");
       fprintf(ec_file, "\n}\n");
       return(0);
}

int outputInformixHead()
{
	int	r;

	r = outputSqlHead();
	if ( r == 0){
	  r=outputIncludeHead();
	  if (r==0)
		r=outputECfileHead();
	}
	return(r);
}

static int outputActionDef(FILE *out)
{
	ArgDef		*p, *pt;
	FieldDef	*f;
	char		n;
        int             j;
	int		or_flag=0;

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
		  if ((j == 0) ||(strcmp(p->name,"DESC")==0))
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
/*** add by lhw 990531  ***/
	(aType == K_open_select_distinct_for) ||
/*** end 		****/
            (aType == K_max_for) ||
            (aType == K_min_for) 
	    ) {
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
		if (strcmp(p->name,"_or_")!=0)
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
			case K_date:
				fprintf(out,"%clong %s",n,ValueName(p));break;
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
		case K_count:
		case K_count_by:
			fprintf(out,"%clong *_a_cnt)",n);	break;
		case K_open_select_by:
		case K_open_select_for_update_by:
		case K_open_select_for_update:
/** add by lhw 990531 ***/
		case K_open_select_distinct_for:
/** end	***/
		case K_open_select:
		case K_close_select:
			fprintf(out,"%cSelect_Info *_a_sInfo)",n);break;
		case K_fetch_select:
			fprintf(out,"%cSelect_Info *_a_sInfo,%s *_a_data)",n,tName);break;
		case K_free_lock:
			fprintf(out,"%cLock_Info *_a_lock)",n);break;
		case K_del_by:
		case K_del_all:
			fprintf(out,")");break;

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

static int outputECfileActionDef()
{
	int	r;
	r=outputActionDef(ec_file);
	if (r!=0)
		return(-1);
	fprintf(ec_file,"\n");
	return(0);
}

static int outputFieldList(FILE *out)
{
	FieldDef	*f;
	int		n;

	f=fList.head;
        n=0;
	while(f!=NULL)
	{
		switch(f->type)
		{
			case K_long:
			case K_short:
			case K_int:
			case K_double:
			case K_str:
			case K_date:
				fprintf(out," \\\n\t\t\t%c%s",(n==0)?' ':',', f->name);
				break;
/**********
			case K_YYYYMMDD:
				fprintf(out," \\\n\t\t\t%cTO_CHAR(%s,'YYYY-MM-DD')",
					       (n==0)?' ':',', f->name);
				break;
			case K_HHMISS:
				fprintf(out," \\\n\t\t\t%cTO_CHAR(%s,'HH24:MI:SS')",
						(n==0)?' ':',', f->name);
				break;
**********/
			case K_YYYYMMDD:
				fprintf(out," \\\n\t\t\t%c%s",
					       (n==0)?' ':',', f->name);
				break;
			case K_HHMISS:
				fprintf(out," \\\n\t\t\t%c%s",
						(n==0)?' ':',', f->name);
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
	FieldDef	*f;
	ArgDef		*p;
	int		n;
	int		or_flag;

	p=aList.head;
	n=0;
	or_flag=0;

/*** add by lhw 990531 **/
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
/**  end **/




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
				case K_date:
					fprintf(out,"%s%s %s $%s_%s",
					    (n==0) ? "" : 
						(or_flag?" OR\n\t\t\t":
							" AND\n\t\t\t"),
						p->name,p->operate,tName,
						ValueName(p));
					break;
				case K_str:
				case K_YYYYMMDD:
				case K_HHMISS:
					fprintf(out,"%s%s %s $%s_%s",
					    (n==0) ? "" : 
						(or_flag?" OR\n\t\t\t":
							" AND\n\t\t\t"),
						p->name,p->operate,tName,
						ValueName(p));
					break;
				default:
					return(-1);
			};
			n++;
			or_flag=0;
		};
		p=p->next;

	};
	if (p!=NULL)
	{
/***lhw 990531	fprintf(out," \\\n\t\tORDER BY ");
***/
		fprintf(out," \n\t\tORDER BY ");
		n=0;
		p=p->next;
		while(p!=NULL)
		{
			 if (strcmp(p->name,"DESC")==0)
                        {
                                fprintf(out," DESC");
                        }
			else{
			f=findFieldDef(p->name);
			if (f==NULL)
				return(-1);
			fprintf(out,"%s%s",
					(n==0)?"":",",
					p->name);
			}
			p=p->next;
			n++;
		};
	};
	return(0);
}

/*** add by lhw 990531 ***/
static int outputDistinctArg(FILE *out)
{
	FieldDef	*f;
	ArgDef		*p;
	int		n;
	int		or_flag;

	p=aList.head;
	n=0;
	    if ((aType == K_sum_for) ||
            (aType == K_avg_for) ||
            (aType == K_avg_distinct_for) ||
            (aType == K_count_distinct_for) ||
            (aType == K_open_select_distinct_for) ||
            (aType == K_max_for) ||
            (aType == K_min_for)) {
          while ((p!=NULL) && (strcmp(p->name, "_by_") != 0) )
	 {
	     fprintf(out,"\n\t\t%s%s",(n==0)?"":",", p->name);    
             p=p->next;
		n++;
          }
          if (p!=NULL) p=p->next;
        }


}
/** end **/



static int outputMathsSelectRecvDef(FILE *out)
{
        int n=0;
        FieldDef *f;
	ArgDef	*p;

	p=aList.head;
	while(p!=NULL)
	{
                if (strcmp(p->name, "_by_") == 0){
                    break;
                }

		f=findFieldDef(p->name);
		if (f==NULL)
			return(-1);
 		if (n != 0)
 		  fprintf(out, ",");
		switch(f->type)
		{
			case K_long:
			case K_short:
			case K_int:
			case K_double:
			case K_str:
			case K_YYYYMMDD:
			case K_HHMISS:
			case K_date:
				fprintf(out,"\n\t\t\t$p.%s", f->name);
      				n++;
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
        int n = 0;
        FieldDef *f;
	ArgDef	*p;

	p=aList.head;
	while(p!=NULL)
	{
                if (strcmp(p->name, "_by_") == 0){
                    break;
                }

                if ( n != 0 )
                  fprintf(ec_file, ",");
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
			case K_date:
				fprintf(out,"\n\t\t\t$p.%s", p->name);
                                n++;
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
	FieldDef	*f;

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
						tableName,f->name,f->size+1);
				break;
			case K_YYYYMMDD:
				fprintf(out,"\t\tR_%s.%s,11,DT_STR,\n",
						tableName,f->name,f->size+1);
				break;
			case K_HHMISS:
				fprintf(out,"\t\tR_%s.%s,9,DT_STR,\n",
						tableName,f->name,f->size+1);
				break;
			case K_date:
				fprintf(out,"\t\t&R_%s.%s,4,DT_ITG,\n",
						tableName,f->name);
				break;
			default:
				return(-1);
		};
		f=f->next;
	};
	return(0);
}

static int outputECMaxMinArgList(FILE *out)
{

	FieldDef	*f;
	ArgDef		*p;

	p=aList.head;
        while ((p!=NULL) && (strcmp(p->name, "_by_") != 0) ){
          p=p->next;
        }
        if (p != NULL)
          p = p->next;
	while(p!=NULL)
	{

		f=findFieldDef(p->name);
		if (f==NULL)
			return(-1);
		switch(f->type)
		{
			case K_long:
				fprintf(out,"\t$long %s_%s;\n", tName,p->name);
				break;
			case K_short:
				fprintf(out,"\t$short %s_%s;\n",tName,p->name);
				break;
			case K_int:
				fprintf(out,"\t$int %s_%s;\n", tName, p->name);
				break;
			case K_double:
				fprintf(out,"\t$double %s_%s\n",tName,p->name);
				break;
			case K_str:
			case K_YYYYMMDD:
			case K_HHMISS:
				fprintf(out,"\t$char *%s_%s;\n", tName,p->name);
				break;
			case K_date:
				fprintf(out,"\t$long %s_%s;\n", tName,p->name);
				break;
			default:
				return(-1);
		};
		p=p->next;
	};

	return(0);
}

static int outputECArgList(FILE *out)
{

	FieldDef	*f;
	ArgDef		*p;

	p=aList.head;
/*** add by lhw 990531***/
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
/*** end **/




	while(p!=NULL)
	{
		if (strcmp(p->name,"_order_by_")==0)
			break;

		if (strcmp(p->name,"_or_")!=0)
		{
		    f=findFieldDef(p->name);
		    if (f==NULL)
			return(-1);
		    switch(f->type)
		    {
			case K_long:
				fprintf(out,"\t$long %s_%s;\n", tName,ValueName(p));
				break;
			case K_short:
				fprintf(out,"\t$short %s_%s;\n",tName,ValueName(p));
				break;
			case K_int:
				fprintf(out,"\t$int %s_%s;\n", tName, ValueName(p));
				break;
			case K_double:
				fprintf(out,"\t$double %s_%s;\n",tName,ValueName(p));
				break;
			case K_str:
			case K_YYYYMMDD:
			case K_HHMISS:
				fprintf(out,"\t$char *%s_%s;\n", tName,ValueName(p));
				break;
			case K_date:
				fprintf(out,"\t$long %s_%s;\n", tName,ValueName(p));
				break;
			default:
				return(-1);
		    };
		};
		p=p->next;
	};

	return(0);
}
static int outputMathsFieldList(FILE *out)
{
        ArgDef     	*p;
	FieldDef	*f;
	int		n;

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
			case K_date:
			  if (aType == K_sum_for)
			    fprintf(out," \n\t\t\t%cSUM(%s)",
                                                (n==0)?' ':',', f->name);
			  if (aType == K_avg_for)
			    fprintf(out," \n\t\t\t%cAVG(%s)",
                                                (n==0)?' ':',', f->name);
			  if (aType == K_avg_distinct_for)
			    fprintf(out," \n\t\t\t%cAVG(DISTINCT(%s))",
                                                (n==0)?' ':',', f->name);
			  if (aType == K_count_distinct_for)
		   	    fprintf(out," \n\t\t\t%cCOUNT(DISTINCT(%s))",
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
        ArgDef     	*p;
	FieldDef	*f;
	int		n;

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
			case K_date:
				if (aType == K_max_for)
					fprintf(out," \n\t\t\t%cMAX(%s)",
                                                (n==0)?' ':',', f->name);
				if (aType == K_min_for)
					fprintf(out," \n\t\t\t%cMIN(%s)",
                                                (n==0)?' ':',', f->name);
				break;
/**************
			case K_YYYYMMDD:
				if (aType == K_max_for)
				  fprintf(out,
                                  " \n\t\t\t%cMAX(TO_CHAR(%s, 'YYYY-MM-DD'))",
					       (n==0)?' ':',', f->name);
				if (aType == K_min_for)
				  fprintf(out,
                                  " \n\t\t\t%cMIN(TO_CHAR(%s, 'YYYY-MM-DD'))",
					       (n==0)?' ':',', f->name);
				break;
			case K_HHMISS:
				if (aType == K_max_for)
				  fprintf(out,
                                   " \n\t\t\t%cMAX(TO_CHAR(%s,'HH24:MI:SS'))",
						(n==0)?' ':',', f->name);
				if (aType == K_min_for)
				  fprintf(out,
                                   " \n\t\t\t%cMIN(TO_CHAR(%s,'HH24:MI:SS'))",
						(n==0)?' ':',', f->name);
				break;
*************/
			case K_YYYYMMDD:
				if (aType == K_max_for)
				  fprintf(out,
                                  " \n\t\t\t%cMAX(%s)",
					       (n==0)?' ':',', f->name);
				if (aType == K_min_for)
				  fprintf(out,
                                  " \n\t\t\t%cMIN(%s)",
					       (n==0)?' ':',', f->name);
				break;
			case K_HHMISS:
				if (aType == K_max_for)
				  fprintf(out,
                                   " \n\t\t\t%cMAX(%s)",
						(n==0)?' ':',', f->name);
				if (aType == K_min_for)
				  fprintf(out,
                                   " \n\t\t\t%cMIN(%s)",
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
	FieldDef	*f;
	ArgDef		*p;
	int		n;
	int		or_flag;

	p=aList.head;
        while ((p!=NULL) && (strcmp(p->name, "_by_") != 0) ){
          p=p->next;
        }

        if ( p!=NULL){
          p=p->next;
          if (p!=NULL)
            fprintf(out, "\n\t\tWHERE ");
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
			case K_date:
				fprintf(out,"%s %s %s $%s_%s",
                                        (n==0)?"":
					(or_flag?" OR\n\t\t\t":
						 " AND\n\t\t\t"),
					p->name,p->operate,tName,p->name);
				break;
			default:
				return(-1);
		    };
		    n++;
		    or_flag=0;
		};
		p=p->next;
	};
	return(0);
}

static int outputMaxMinSqlWhere(FILE *out)
{
	FieldDef	*f;
	ArgDef		*p;
	int		n;
	int		or_flag;

	p=aList.head;
        while ((p!=NULL) && (strcmp(p->name, "_by_") != 0) ){
          p=p->next;
        }

        if ( p!=NULL){
          p=p->next;
          if (p!=NULL)
            fprintf(out, "\n\t\tWHERE ");
        }
	n=0;
	or_flag=0;
	while(p!=NULL)
	{
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
			case K_date:
				fprintf(out,"%s %s %s $%s_%s",
					    (n==0) ? "" : 
						(or_flag?" OR\n\t\t\t":
							" AND\n\t\t\t"),
				              p->name,p->operate,tName,p->name);
				break;

			default:
				return(-1);
		    };
		    n++;
		    or_flag=0;
		};
		p=p->next;
	};
	return(0);
}
static int outputECfileActionMathsFor()
{
	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0;\n");
        r = outputECMaxMinArgList(ec_file);
	if (r!=0)
		return(-1);
	fprintf(ec_file,"\t$%s p;", tName);
        fprintf(ec_file, "\n");

	p=aList.head;
        while ((p!=NULL) && (strcmp(p->name, "_by_") != 0) ){
          p=p->next;
        }
	if (p!=NULL)
          p=p->next;
	outputECfileArgToValue(ec_file,p);

	fprintf(ec_file,"\n\tmemset(&p, 0, sizeof(p));\n");

	fprintf(ec_file,"\tEXEC SQL SELECT "); 
	r=outputMathsFieldList(ec_file);
	if (r!=0)
		return(-1);
        fprintf(ec_file, "\n\t         INTO "); 
	r=outputMathsSelectRecvDef(ec_file);
	if (r!=0)
		return(-1);
        fprintf(ec_file, "\n\t         FROM\t%s", tableName);
	r = outputMathsSqlWhere(ec_file);
	if (r!=0)
		return(-1);

        fprintf(ec_file, ";");
	fprintf(ec_file,"\n\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error maths : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");
        fprintf(ec_file, "\n\n\t%s_EraseTailSpace(&p);", tName);
/*
	if (hasDAC)
	{
		fprintf(ec_file,"\n        r=%s_%s_check_dac(&p,p.dac);\n",
				sName,tableName);
	};
*/
	fprintf(ec_file,"\n\tmemcpy(_a_data, &p, sizeof(p));");

        fprintf(ec_file, "\n\n\treturn(r);\n}");
	fprintf(ec_file,"\n");
	return(0);
}

static int outputECfileActionCount()
{
	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0;\n");
	fprintf(ec_file,"\t$long _aa_cnt;\n");
        r = outputECArgList(ec_file);
	if (r!=0)
		return(-1);

	p=aList.head;
	outputECfileArgToValue(ec_file,p);

	fprintf(ec_file,"\n\tEXEC SQL SELECT COUNT(*)"); 
        fprintf(ec_file, "\n\t         INTO "); 
	fprintf(ec_file, "\n\t           $_aa_cnt");
        fprintf(ec_file, "\n\t         FROM\t%s;", tableName);
	fprintf(ec_file,"\n\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error maths : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");
	fprintf(ec_file, "\n\t*_a_cnt=_aa_cnt;");
        fprintf(ec_file, "\n\n\treturn(0);\n}");
	fprintf(ec_file,"\n");
	return(0);
}
static int outputECfileActionCountBy()
{
	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0;\n");
	fprintf(ec_file,"\t$long _aa_cnt;\n");
        r = outputECArgList(ec_file);
	if (r!=0)
		return(-1);

	p=aList.head;
	outputECfileArgToValue(ec_file,p);

	fprintf(ec_file,"\n\tEXEC SQL SELECT COUNT(*)"); 
        fprintf(ec_file, "\n\t         INTO "); 
	fprintf(ec_file, "\n\t           $_aa_cnt");
        fprintf(ec_file, "\n\t         FROM\t%s", tableName);
	fprintf(ec_file, "\n\t         WHERE");
	fprintf(ec_file, "\n\t           ");
	r = outputSqlWhere(ec_file);
	if (r!=0)
		return(-1);

        fprintf(ec_file, ";");
	fprintf(ec_file,"\n\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error maths : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");
	fprintf(ec_file, "\n\t*_a_cnt=_aa_cnt;");
        fprintf(ec_file, "\n\n\treturn(0);\n}");
	fprintf(ec_file,"\n");
	return(0);
}
static int outputECfileActionMaxMinFor()
{

	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0;\n");
        r = outputECMaxMinArgList(ec_file);
	if (r!=0)
		return(-1);
	fprintf(ec_file,"\t$%s p;", tName);
        fprintf(ec_file, "\n");

	p=aList.head;
        while ((p!=NULL) && (strcmp(p->name, "_by_") != 0) ){
          p=p->next;
        }
	if (p!=NULL)
          p=p->next;
	outputECfileArgToValue(ec_file,p);

	fprintf(ec_file,"\n\tmemset(&p, 0, sizeof(p));\n");

	fprintf(ec_file,"\tEXEC SQL SELECT "); 
	r=outputMaxMinFieldList(ec_file);
	if (r!=0)
		return(-1);
        fprintf(ec_file, "\n\t         INTO "); 
	r=outputMaxMinSelectRecvDef(ec_file);
	if (r!=0)
		return(-1);
        fprintf(ec_file, "\n\t         FROM\t%s", tableName);
	r = outputMaxMinSqlWhere(ec_file);
	if (r!=0)
		return(-1);

        fprintf(ec_file, ";");
	fprintf(ec_file,"\n\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error max, min : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");
        fprintf(ec_file, "\n\n\t%s_EraseTailSpace(&p);", tName);
/*
	if (hasDAC)
	{
		fprintf(ec_file,"\n        r=%s_%s_check_dac(&p,p.dac);\n",
				sName,tableName);
	};
*/
	fprintf(ec_file,"\n\tmemcpy(_a_data, &p, sizeof(p));");

        fprintf(ec_file, "\n\n\treturn(r);\n}");
	fprintf(ec_file,"\n");
	return(0);
}

static int outputECfileActionReadBy()
{
	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0;\n");
        r = outputECArgList(ec_file);
	if (r!=0)
		return(-1);
	fprintf(ec_file,"\t$%s p;", tName);
        fprintf(ec_file, "\n");
	p=aList.head;
	outputECfileArgToValue(ec_file,p);

	fprintf(ec_file,"\n\tmemset(&p, 0, sizeof(p));\n");

	fprintf(ec_file,"\tEXEC SQL SELECT * INTO $p FROM %s\n", tableName);
        fprintf(ec_file,"\t         WHERE  ");
	r=outputSqlWhere(ec_file);
	if (r!=0)
		return(-1);

        fprintf(ec_file, ";");
	fprintf(ec_file,"\n\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\n\t\tif(sqlca.sqlcode!=SQLNOTFOUND)");
	fprintf(ec_file,"\n\t\t\tDBLOG(\"error read_by : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");
        fprintf(ec_file, "\n\n\t%s_EraseTailSpace(&p);", tName);

	if (hasDAC)
	{
		fprintf(ec_file,"\n        r=%s_%s_check_dac(&p,p.dac);\n",
				sName,tableName);
	};

	fprintf(ec_file,"\n\tmemcpy(_a_data, &p, sizeof(p));");

        fprintf(ec_file, "\n\n\treturn(r);\n}");
	fprintf(ec_file,"\n");
	return(0);
}

static int outputECfileActionRead()
{
	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0;\n");
        r = outputECArgList(ec_file);
	if (r!=0)
		return(-1);
	fprintf(ec_file,"\t$%s p;", tName);
        fprintf(ec_file, "\n");
	p=aList.head;
	outputECfileArgToValue(ec_file,p);

	fprintf(ec_file,"\n\tmemset(&p, 0, sizeof(p));\n");

	fprintf(ec_file,"\tEXEC SQL SELECT * INTO $p FROM %s;", tableName);

	fprintf(ec_file,"\n\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error read : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");
        fprintf(ec_file, "\n\n\t%s_EraseTailSpace(&p);", tName);

	if (hasDAC)
	{
		fprintf(ec_file,"\n        r=%s_%s_check_dac(&p,p.dac);\n",
				sName,tableName);
	};

	fprintf(ec_file,"\n\tmemcpy(_a_data, &p, sizeof(p));");

        fprintf(ec_file, "\n\n\treturn(r);\n}");
	fprintf(ec_file,"\n");
	return(0);
}
static int outputECfileActionReadLock()
{
	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0, err;\n");
        r = outputECArgList(ec_file);
	if (r!=0)
		return(-1);
        fprintf(ec_file, "\n\t$char *curs_name;\n");
	fprintf(ec_file,"\t$%s p;", tName);
	p=aList.head;
	outputECfileArgToValue(ec_file,p);

        fprintf(ec_file, "\n\tdbGetNewCursorName(_a_lock->cursor_name);");
        fprintf(ec_file, "\n\tcurs_name=_a_lock->cursor_name;");
	fprintf(ec_file,"\n\tmemset(&p, 0, sizeof(p));\n");

	fprintf(ec_file,"\n\tEXEC SQL DECLARE $curs_name CURSOR FOR ");
        fprintf(ec_file,"\n\t         SELECT * INTO $p FROM %s", tableName);
        fprintf(ec_file,"\n\t         FOR UPDATE");
        fprintf(ec_file, ";");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error declare : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\tdbFreeCursName(curs_name);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file,"\n\n\tEXEC SQL OPEN $curs_name;");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error declare : %%d\",sqlca.sqlcode);");
        fprintf(ec_file, "\n\t\terr = sqlca.sqlcode;");
        fprintf(ec_file, "\n\t\tdbFreeLock(_a_lock);");
	fprintf(ec_file,"\n\t\treturn (err);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file,"\n\n\tEXEC SQL FETCH $curs_name;");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tif (sqlca.sqlcode!=SQLNOTFOUND)");
	fprintf(ec_file,"\n\t\t\tDBLOG(\"error fetch : %%d\",sqlca.sqlcode);");
        fprintf(ec_file, "\n\t\terr = sqlca.sqlcode;");
        fprintf(ec_file, "\n\t\tdbFreeLock(_a_lock);");
	fprintf(ec_file,"\n\t\treturn (err);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file, "\n\n\t%s_EraseTailSpace(&p);", tName);

	if (hasDAC)
	{
		fprintf(ec_file,"\n\tr=%s_%s_check_dac(&p,p.dac);\n",
				sName,tableName);
        	fprintf(ec_file, "\n\tif(r!=0)\n\t\tdbFreeLock(_a_lock);");
	};

	fprintf(ec_file,"\n\tmemcpy(_a_data, &p, sizeof(p));");
        fprintf(ec_file, "\n\n\treturn(r);\n}");
	fprintf(ec_file,"\n");
	return(0);
}
static int outputECfileActionReadLockBy()
{
	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0, err;\n");
        r = outputECArgList(ec_file);
	if (r!=0)
		return(-1);
        fprintf(ec_file, "\n\t$char *curs_name;\n");
	fprintf(ec_file,"\t$%s p;", tName);
	p=aList.head;
	outputECfileArgToValue(ec_file,p);

        fprintf(ec_file, "\n\tdbGetNewCursorName(_a_lock->cursor_name);");
        fprintf(ec_file, "\n\tcurs_name=_a_lock->cursor_name;");
	fprintf(ec_file,"\n\tmemset(&p, 0, sizeof(p));\n");

	fprintf(ec_file,"\n\tEXEC SQL DECLARE $curs_name CURSOR FOR ");
        fprintf(ec_file,"\n\t         SELECT * INTO $p FROM %s\n", tableName);
        fprintf(ec_file,"\t         WHERE  ");
	r=outputSqlWhere(ec_file);
	if (r!=0)
		return(-1);

        fprintf(ec_file,"\n\t         FOR UPDATE");
        fprintf(ec_file, ";");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error declare : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\tdbFreeCursName(curs_name);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file,"\n\n\tEXEC SQL OPEN $curs_name;");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error declare : %%d\",sqlca.sqlcode);");
        fprintf(ec_file, "\n\t\terr = sqlca.sqlcode;");
        fprintf(ec_file, "\n\t\tdbFreeLock(_a_lock);");
	fprintf(ec_file,"\n\t\treturn (err);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file,"\n\n\tEXEC SQL FETCH $curs_name;");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tif (sqlca.sqlcode!=SQLNOTFOUND)");
	fprintf(ec_file,"\n\t\t\tDBLOG(\"error fetch : %%d\",sqlca.sqlcode);");
        fprintf(ec_file, "\n\t\terr = sqlca.sqlcode;");
        fprintf(ec_file, "\n\t\tdbFreeLock(_a_lock);");
	fprintf(ec_file,"\n\t\treturn (err);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file, "\n\n\t%s_EraseTailSpace(&p);", tName);

	if (hasDAC)
	{
		fprintf(ec_file,"\n        r=%s_%s_check_dac(&p,p.dac);\n",
				sName,tableName);
        	fprintf(ec_file, "\n\tif(r!=0)\n\t\tdbFreeLock(_a_lock);");
	};

	fprintf(ec_file,"\n\tmemcpy(_a_data, &p, sizeof(p));");
        fprintf(ec_file, "\n\n\treturn(r);\n}");
	fprintf(ec_file,"\n");
	return(0);
}

static int outputECfileActionOpenSelectDistinctFor()
{

	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0, err;\n");
	
        r = outputECArgList(ec_file);
	if (r!=0)
		return(-1);
        fprintf(ec_file, "\n\t$char *curs_name;\n");
	p=aList.head;
	outputECfileArgToValue(ec_file,p);

        fprintf(ec_file, "\n\tdbGetNewCursorName(_a_sInfo->cursor_name);");
        fprintf(ec_file, "\n\tcurs_name=_a_sInfo->cursor_name;");

	fprintf(ec_file,"\n\tEXEC SQL DECLARE $curs_name CURSOR");
	fprintf(ec_file,"\n\t\tWITH HOLD");
        fprintf(ec_file,"\n\t\tFOR SELECT DISTINCT ");

	r=outputDistinctArg(ec_file);
	if (r!=0)
		return(-1);


	fprintf(ec_file,"\n\t\t FROM %s", tableName);
        fprintf(ec_file,"\n\t\tWHERE  ");
	r=outputSqlWhere(ec_file);
	if (r!=0)
		return(-1);

        fprintf(ec_file, "\n;");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error declare : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\tdbFreeCursName(curs_name);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file,"\n\n\tEXEC SQL OPEN $curs_name;");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error declare : %%d\",sqlca.sqlcode);");
        fprintf(ec_file, "\n\t\terr = sqlca.sqlcode;");
        fprintf(ec_file, "\n\t\tdbCloseSelect(_a_sInfo);");
	fprintf(ec_file,"\n\t\treturn (err);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file, "\n\n\treturn(r);\n}");
	fprintf(ec_file,"\n");
	return(0);
}


static int outputECfileActionOpenSelectBy()
{

	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0, err;\n");
        r = outputECArgList(ec_file);
	if (r!=0)
		return(-1);
        fprintf(ec_file, "\n\t$char *curs_name;\n");
	p=aList.head;
	outputECfileArgToValue(ec_file,p);

        fprintf(ec_file, "\n\tdbGetNewCursorName(_a_sInfo->cursor_name);");
        fprintf(ec_file, "\n\tcurs_name=_a_sInfo->cursor_name;");

	fprintf(ec_file,"\n\tEXEC SQL DECLARE $curs_name CURSOR");
	fprintf(ec_file,"\n\t\tWITH HOLD");
        fprintf(ec_file,"\n\t\tFOR SELECT * FROM %s", tableName);
        fprintf(ec_file,"\n\t\tWHERE  ");
	r=outputSqlWhere(ec_file);
	if (r!=0)
		return(-1);

        fprintf(ec_file, "\n;");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error declare : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\tdbFreeCursName(curs_name);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file,"\n\n\tEXEC SQL OPEN $curs_name;");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error declare : %%d\",sqlca.sqlcode);");
        fprintf(ec_file, "\n\t\terr = sqlca.sqlcode;");
        fprintf(ec_file, "\n\t\tdbCloseSelect(_a_sInfo);");
	fprintf(ec_file,"\n\t\treturn (err);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file, "\n\n\treturn(r);\n}");
	fprintf(ec_file,"\n");
	return(0);
}

static int outputECfileActionOpenSelectForUpdateBy()
{

	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0, err;\n");
        r = outputECArgList(ec_file);
	if (r!=0)
		return(-1);
        fprintf(ec_file, "\n\t$char *curs_name;\n");
	p=aList.head;
	outputECfileArgToValue(ec_file,p);

        fprintf(ec_file, "\n\tdbGetNewCursorName(_a_sInfo->cursor_name);");
        fprintf(ec_file, "\n\tcurs_name=_a_sInfo->cursor_name;");

	fprintf(ec_file,"\n\tEXEC SQL DECLARE $curs_name CURSOR ");
        fprintf(ec_file,"\n\t\tFOR SELECT * FROM %s\n", tableName);
        fprintf(ec_file,"\t\t\tWHERE  ");
	r=outputSqlWhere(ec_file);
	if (r!=0)
		return(-1);

        fprintf(ec_file,"\n\t         FOR UPDATE");
        fprintf(ec_file, ";");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error declare : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\tdbFreeCursName(curs_name);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file,"\n\n\tEXEC SQL OPEN $curs_name;");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error declare : %%d\",sqlca.sqlcode);");
        fprintf(ec_file, "\n\t\terr = sqlca.sqlcode;");
        fprintf(ec_file, "\n\t\tdbCloseSelect(_a_sInfo);");
	fprintf(ec_file,"\n\t\treturn (err);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file, "\n\n\treturn(r);\n}");
	fprintf(ec_file,"\n");
	return(0);
}
static int outputECfileActionOpenSelect()
{

	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0, err;\n");
        fprintf(ec_file, "\n\t$char *curs_name;\n");

        fprintf(ec_file, "\n\tdbGetNewCursorName(_a_sInfo->cursor_name);");
        fprintf(ec_file, "\n\tcurs_name=_a_sInfo->cursor_name;");

	fprintf(ec_file,"\n\tEXEC SQL DECLARE $curs_name CURSOR ");
	fprintf(ec_file,"\n\t\tWITH HOLD");
        fprintf(ec_file,"\n\t\tFOR SELECT * FROM %s", tableName);
	r=outputSqlWhere(ec_file);
	if (r!=0)
		return(-1);

        fprintf(ec_file, ";");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error declare : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\tdbFreeCursName(curs_name);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file,"\n\n\tEXEC SQL OPEN $curs_name;");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error declare : %%d\",sqlca.sqlcode);");
        fprintf(ec_file, "\n\t\terr = sqlca.sqlcode;");
        fprintf(ec_file, "\n\t\tdbCloseSelect(_a_sInfo);");
	fprintf(ec_file,"\n\t\treturn (err);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file, "\n\n\treturn(r);\n}");
	fprintf(ec_file,"\n");
	return(0);
}

static int outputECfileActionOpenSelectForUpdate()
{

	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0, err;");
        fprintf(ec_file, "\n\t$char *curs_name;\n");

        fprintf(ec_file, "\n\tdbGetNewCursorName(_a_sInfo->cursor_name);");
        fprintf(ec_file, "\n\tcurs_name=_a_sInfo->cursor_name;");

	fprintf(ec_file,"\n\tEXEC SQL DECLARE $curs_name CURSOR FOR ");
        fprintf(ec_file,"\n\t         SELECT * FROM %s", tableName);
	r=outputSqlWhere(ec_file);
	if (r!=0)
		return(-1);

        fprintf(ec_file,"\n\t         FOR UPDATE");
        fprintf(ec_file, ";");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error declare : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\tdbFreeCursName(curs_name);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file,"\n\n\tEXEC SQL OPEN $curs_name;");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error declare : %%d\",sqlca.sqlcode);");
        fprintf(ec_file, "\n\t\terr = sqlca.sqlcode;");
        fprintf(ec_file, "\n\t\tdbCloseSelect(_a_sInfo);");
	fprintf(ec_file,"\n\t\treturn (err);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file, "\n\n\treturn(r);\n}");
	fprintf(ec_file,"\n");
	return(0);
}
static int outputECfileActionFetchSelect()
{
	int	r;

	fprintf(ec_file,"{\n");
	fprintf(ec_file,"\tint r = 0, err;\n");
        fprintf(ec_file, "\t$char *curs_name;\n");
	fprintf(ec_file,"\t$%s p;", tName);

        fprintf(ec_file, "\n\n\tcurs_name=_a_sInfo->cursor_name;");
	fprintf(ec_file,"\n\tmemset(&p, 0, sizeof(p));\n");

        fprintf(ec_file,"\tEXEC SQL FETCH $curs_name INTO $p;");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tif (sqlca.sqlcode!=SQLNOTFOUND)");
	fprintf(ec_file,"\n\t\t\tDBLOG(\"error fetch : %%d\",sqlca.sqlcode);");
        fprintf(ec_file, "\n\t\terr = sqlca.sqlcode;");
	fprintf(ec_file,"\n\t\treturn (err);");
        fprintf(ec_file, "\n\t}");

        fprintf(ec_file, "\n\n\t%s_EraseTailSpace(&p);", tName);
	if (hasDAC)
	{
		fprintf(ec_file,"        r=%s_%s_check_dac(&p,p.dac);\n",
				sName,tableName);
	};
	fprintf(ec_file,"\n");
	fprintf(ec_file,"\n\tmemcpy(_a_data, &p, sizeof(p));");
        fprintf(ec_file, "\n\n\treturn(r);\n}");
	fprintf(ec_file,"\n");
	return(0);
}
static int outputInsertFieldDef(FILE *out)
{
	FieldDef	*f;
	char		n;

	f=fList.head;
	n=' ';
	while(f!=NULL)
	{
		fprintf(out,"%c%s \\\n\t\t",n,f->name);
		n=',';
		f=f->next;
	};
	return(0);
}

static int outputECfileActionAdd()
{

	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0, err;\n");

	fprintf(ec_file,"\t$%s p;\n", tName);
	if (hasDAC)
	{
		fprintf(ec_file,"\tif (%s_%s_creat_dac(_a_data,_a_data->dac)!=0)\n",
				sName,tableName);
		fprintf(ec_file,"\t{\tDBLOG(\"creat dac err!\");\n");
		fprintf(ec_file,"\t\treturn(ERROR_DAC);\t}\n");
	};

        fprintf(ec_file, "\tmemcpy(&p, _a_data, sizeof(p));\n");
	fprintf(ec_file,"\n\tEXEC SQL INSERT INTO %s VALUES ($p)", tableName);
        fprintf(ec_file, ";");

	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error add : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");

	fprintf(ec_file,"\n\treturn(r);\n");
	fprintf(ec_file,"}\n\n");
	return(0);
}

static int outputECfileActionDel()
{

	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0, err;\n");

        r = outputECArgList(ec_file);
	if (r!=0)
		return(-1);
        fprintf(ec_file, "\n");
	p=aList.head;
	outputECfileArgToValue(ec_file,p);

	fprintf(ec_file,"\n\tEXEC SQL DELETE FROM %s ", tableName);
        fprintf(ec_file,"\n\t         WHERE  ");
	r=outputSqlWhere(ec_file);
	if (r!=0)
		return(-1);

        fprintf(ec_file, ";");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error delete : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");
	fprintf(ec_file,"\n\tif(sqlca.sqlerrd[2]==0){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error delete : SQLNOTFOUND\");");
	fprintf(ec_file,"\n\t\treturn (SQLNOTFOUND);");
        fprintf(ec_file, "\n\t}");

	fprintf(ec_file,"\n\treturn(r);\n");
	fprintf(ec_file,"}\n\n");
	return(0);
}
static int outputECfileActionDelAll()
{

	int	r;
        ArgDef  *p;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0, err;\n");

	fprintf(ec_file,"\n\tEXEC SQL DELETE FROM %s ", tableName);
        fprintf(ec_file, ";");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error delete : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");
	fprintf(ec_file,"\n\tif(sqlca.sqlerrd[2]==0){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error delete : SQLNOTFOUND\");");
	fprintf(ec_file,"\n\t\treturn (SQLNOTFOUND);");
        fprintf(ec_file, "\n\t}");

	fprintf(ec_file,"\n\treturn(r);\n");
	fprintf(ec_file,"}\n\n");
	return(0);
}

static int outputECfileActionUpdateBy()
{
	int	r, n = 0;
        ArgDef  *p;
        FieldDef *f;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0, err;");
	fprintf(ec_file, "\n\t$%s p;\n", tName);

        r = outputECArgList(ec_file);
	if (r!=0)
		return(-1);
        fprintf(ec_file, "\n");
	if (hasDAC)
	{
		fprintf(ec_file,"\tif (%s_%s_creat_dac(_a_data,_a_data->dac)!=0)\n",
				sName,tableName);
		fprintf(ec_file,"\t{\tDBLOG(\"creat dac err!\");\n");
		fprintf(ec_file,"\t\treturn(ERROR_DAC);\t}\n");
	};

	p=aList.head;
	outputECfileArgToValue(ec_file,p);
	fprintf(ec_file,"\n\tmemcpy(&p, _a_data, sizeof(p));\n");

	fprintf(ec_file,"\n\tEXEC SQL UPDATE %s SET ", tableName);
	f=fList.head;
	while(f!=NULL){
          if (n!=0)
            fprintf(ec_file,",");
          fprintf(ec_file, "\n\t\t\t%s = $p.%s", f->name, f->name);
          n++;
          f = f->next; 
        }

        fprintf(ec_file,"\n\t         WHERE  ");
	r=outputSqlWhere(ec_file);
	if (r!=0)
		return(-1);

        fprintf(ec_file, ";");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error update : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");
	fprintf(ec_file,"\n\tif(sqlca.sqlerrd[2]==0){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error update : SQLNOTFOUND\");");
	fprintf(ec_file,"\n\t\treturn (SQLNOTFOUND);");
        fprintf(ec_file, "\n\t}");

	fprintf(ec_file,"\n\treturn(r);\n");
	fprintf(ec_file,"}\n\n");
	return(0);
}

static int outputECfileActionUpdate()
{
	int	r, n = 0;
        ArgDef  *p;
        FieldDef *f;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0, err;");
	fprintf(ec_file, "\n\t$%s p;\n", tName);

        r = outputECArgList(ec_file);
	if (r!=0)
		return(-1);
        fprintf(ec_file, "\n");
	if (hasDAC)
	{
		fprintf(ec_file,"\tif (%s_%s_creat_dac(_a_data,_a_data->dac)!=0)\n",
				sName,tableName);
		fprintf(ec_file,"\t{\tDBLOG(\"creat dac err!\");\n");
		fprintf(ec_file,"\t\treturn(ERROR_DAC);\t}\n");
	};

	p=aList.head;
	outputECfileArgToValue(ec_file,p);
	fprintf(ec_file,"\n\tmemcpy(&p, _a_data, sizeof(p));\n");

	fprintf(ec_file,"\n\tEXEC SQL UPDATE %s SET ", tableName);
	f=fList.head;
	while(f!=NULL){
          if (n!=0)
            fprintf(ec_file,",");
          fprintf(ec_file, "\n\t\t\t%s = $p.%s", f->name, f->name);
          n++;
          f = f->next; 
        }

        fprintf(ec_file, ";");
	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error update : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");
	fprintf(ec_file,"\n\tif(sqlca.sqlerrd[2]==0){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error update : SQLNOTFOUND\");");
	fprintf(ec_file,"\n\t\treturn (SQLNOTFOUND);");
        fprintf(ec_file, "\n\t}");

	fprintf(ec_file,"\n\treturn(r);\n");
	fprintf(ec_file,"}\n\n");
	return(0);
}
static int outputECfileActionUpdateForSelect()
{

	int	r, n = 0;
        ArgDef  *p;
        FieldDef  *f;

	fprintf(ec_file,"{\n");
        fprintf(ec_file, "\tint r = 0, err;");
        fprintf(ec_file, "\n\t$char *curs_name;");
	fprintf(ec_file, "\n\t$%s p;\n", tName);

	if (hasDAC)
	{
		fprintf(ec_file,"\tif (%s_%s_creat_dac(_a_data,_a_data->dac)!=0)\n",
				sName,tableName);
		fprintf(ec_file,"\t{\tDBLOG(\"creat dac err!\");\n");
		fprintf(ec_file,"\t\treturn(ERROR_DAC);\t}\n");
	};

        if (aType == K_update_by_lock){
          fprintf(ec_file, "\n\tcurs_name=_a_lock->cursor_name;");
        }
        else{
          fprintf(ec_file, "\n\tcurs_name=_a_sInfo->cursor_name;");
        }

	fprintf(ec_file,"\n\tmemcpy(&p, _a_data, sizeof(p));\n");

	fprintf(ec_file,"\n\tEXEC SQL UPDATE %s SET ", tableName);
	f=fList.head;
	while(f!=NULL){
          if (n!=0)
            fprintf(ec_file,",");
          fprintf(ec_file, "\n\t\t\t%s = $p.%s", f->name, f->name);
          n++;
          f = f->next; 
        }
        fprintf(ec_file,"\n\t         WHERE CURRENT OF $curs_name");
        fprintf(ec_file, ";");

	fprintf(ec_file,"\n\tif(sqlca.sqlcode){");
	fprintf(ec_file,"\n\t\tDBLOG(\"error update : %%d\",sqlca.sqlcode);");
	fprintf(ec_file,"\n\t\treturn (sqlca.sqlcode);");
        fprintf(ec_file, "\n\t}");

	fprintf(ec_file,"\n\treturn(r);\n");
	fprintf(ec_file,"}\n\n");
	return(0);
}
static int outputECfileActionCloseSelect()
{
	fprintf(ec_file,"{\n");
	fprintf(ec_file,"        int     r;\n");
	fprintf(ec_file,"        r=dbCloseSelect(_a_sInfo);\n");
	fprintf(ec_file,"        return(r);\n");
	fprintf(ec_file,"}\n\n");
	return(0);
}
static int outputECfileActionFreeLock()
{
	fprintf(ec_file,"{\n");
	fprintf(ec_file,"        int     r;\n");
	fprintf(ec_file,"        r=dbFreeLock(_a_lock);\n");
	fprintf(ec_file,"        return(r);\n");
	fprintf(ec_file,"}\n\n");
	return(0);
}

static int outputECfileActionFunction()
{
	int	r;

	switch(aType)
	{
		case K_read:
			r=outputECfileActionRead();		break;
		case K_read_by:
			r=outputECfileActionReadBy();		break;
		case K_sum_for:
		case K_avg_for:
		case K_avg_distinct_for:
		case K_count_distinct_for:
			r=outputECfileActionMathsFor();		break;
		case K_max_for:
		case K_min_for:
			r=outputECfileActionMaxMinFor();		break;
		case K_read_lock_by:
			r=outputECfileActionReadLockBy();	break;
		case K_read_lock:
			r=outputECfileActionReadLock();		break;
		case K_open_select_by:
			r=outputECfileActionOpenSelectBy();	break;
		case K_open_select_distinct_for:
			r=outputECfileActionOpenSelectDistinctFor();	break;
		case K_open_select_for_update_by:
			r=outputECfileActionOpenSelectForUpdateBy();
                 	break;
		case K_open_select:
			r=outputECfileActionOpenSelect();	break;
		case K_open_select_for_update:
			r=outputECfileActionOpenSelectForUpdate();
                 	break;
		case K_fetch_select:
			r=outputECfileActionFetchSelect();	break;
		case K_add:
			r=outputECfileActionAdd();		break;
		case K_del_by:
			r=outputECfileActionDel();		break;
		case K_del_all:
			r=outputECfileActionDelAll();		break;
		case K_update:
			r=outputECfileActionUpdate();		break;
		case K_update_by:
			r=outputECfileActionUpdateBy();		break;
		case K_update_by_select:
		case K_update_by_lock:
			r=outputECfileActionUpdateForSelect();	break;
		case K_close_select:
			r=outputECfileActionCloseSelect();	break;
		case K_free_lock:
			r=outputECfileActionFreeLock();		break;
		case K_count:
			r=outputECfileActionCount();		break;
		case K_count_by:
			r=outputECfileActionCountBy();		break;

		default:
			return(-1);
	};
	return(r);
}

static int outputAction()
{
	int	r;

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

	  r=outputECfileActionDef();
	  if (r!=0)
	    return(r);
	  r=outputECfileActionFunction();
	  return(r);
       }
}

int processInformixAction(FILE *in)
{
	int	r;
	int	keyword;
	int	g;

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
else
	printf("line %d r is %d\n",__LINE__,r);
		  }

	};
	return(r);
}

int outputECfileDACcode()
{
	int		r;
	FieldDef	*f;

	fprintf(ec_file,"#include \"dac.h\"\n");
	fprintf(ec_file,"static int %s_%s_put_dac_buf(%s *_a_data)\n",
				sName,tableName,tName);
	fprintf(ec_file,"{\n");
	fprintf(ec_file,"        DAC_buf_clear();\n");
	f=fList.head;
	while(f!=NULL)
	{
		if (strcmp(f->name,"dac")!=0)
		   switch(f->type)
		   {
			case K_long:
				fprintf(ec_file,"\tDAC_buf_put_long(_a_data->%s);\n",f->name);
				break;
			case K_short:
				fprintf(ec_file,"\tDAC_buf_put_short(_a_data->%s);\n",f->name);
				break;
			case K_int:
				fprintf(ec_file,"\tDAC_buf_put_int(_a_data->%s);\n",f->name);
				break;
			case K_double:
				fprintf(ec_file,"\tDAC_buf_put_double(&_a_data->%s);\n",f->name);
				break;
			case K_str:
			case K_YYYYMMDD:
			case K_HHMISS:
				fprintf(ec_file,"\tKILL_SPACE(_a_data->%s);\n",
								f->name);
				fprintf(ec_file,"\tDAC_buf_put_str(_a_data->%s);\n",f->name);
				break;
			case K_date:
				fprintf(ec_file,"\tDAC_buf_put_long(_a_data->%s);\n",f->name);
				break;
			default:
				return(-1);
		   };
		f=f->next;
	};
	fprintf(ec_file,"        return(0);\n");
	fprintf(ec_file,"}\n");
	fprintf(ec_file,"int %s_%s_check_dac(%s *_a_data,char *dac)\n",
					sName,tableName,tName);
	fprintf(ec_file,"{\n");
	fprintf(ec_file,"	int	r;\n");
	fprintf(ec_file,"        %s_%s_put_dac_buf(_a_data);\n",
					sName,tableName);
	fprintf(ec_file,"        r=DAC_buf_check_DAC(dac);\n");
	fprintf(ec_file,"        if (r!=0)\n");
  	fprintf(ec_file,"               %s_%s_debug_print(\"DACERROR\",_a_data,__FILE__,__LINE__);\n",sName,tableName);
	fprintf(ec_file,"        return(r);\n");
	fprintf(ec_file,"}\n");
	fprintf(ec_file,"int %s_%s_creat_dac(%s *_a_data,char *dac)\n",
					sName,tableName,tName);
	fprintf(ec_file,"{\n");
	fprintf(ec_file,"        %s_%s_put_dac_buf(_a_data);\n",
					sName,tableName);
	fprintf(ec_file,"        return(DAC_buf_creat_DAC(dac));\n");
	fprintf(ec_file,"}\n\n");

	return(0);
}



