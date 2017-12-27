
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "dbactiontype.h"

#define HERE	printf("here [%s][%d]\n",__FILE__,__LINE__);
char	*keywordList[]
	={	"end","action","read_by","read_lock_by",
		"add","del_by","update_by",
		"open_select", "open_select_by",
		"fetch_select","close_select","free_lock",
		"double","long","str",
		"YMD","HMS","short","int",
		"index_by", "index_by_unique", "notnull",
                "update_by_select", "update_by_lock",  
		"open_select_for_update", "open_select_for_update_by",
                "_by_", "sum_for", "avg_for", "count", "max_for", "min_for",
                "avg_distinct_for", "count_distinct_for", "index_unique_by",
		"read", "update", "print_report" , "_order_by_",
		"read_lock","count_by","del_all","_or_",
		"open_select_distinct_for",
		"date",
	 };

int		lineCnt=1;
char 		_unGetBuf[100];
char		*tableName=NULL;
char		*tName=NULL;
FieldList	fList={NULL,NULL,0};
ArgList		aList={NULL,NULL,0};
int		aType;
char		*sName=NULL;
FILE		*i_file;
FILE		*c_file;
FILE		*ec_file;
FILE		*sql_file;
char		includeFileName[128];
char		SqlFileName[128];
char		cFileName[128];
char		actFileName[128];
char		systemDate[128];
char		hasDAC=0;
int		_indexFileNo = 0;
int		_scale;
int		_notNull = 0;

extern int processInformixAction(FILE *);
extern int processOracleAction(FILE *);
extern int outputCfileDACcode();
extern int outputECfileDACcode();
extern int outputOracleHead();
extern int outputInformixHead();

int getDate()
{
  time_t timer;
  struct tm *lt;

  timer = time(NULL);

  lt = localtime(&timer);
  strcpy(systemDate, asctime(lt));
}

int unGetstr(char *str)
{
  int j;

  for (j = 0; j<=(int)(strlen(str) - 1); j++){
    _unGetBuf[j] = str[j];
  } 
  _unGetBuf[j] = 0;
}

int getChar(FILE *in,char *ch)
{
	static int bufpos = 0;
	int	c;

	if (_unGetBuf[bufpos] != 0){
	  c = _unGetBuf[bufpos++];
 	}
	else{
	  if ( bufpos != 0 ){
	    bufpos = 0;
	    _unGetBuf[0] = 0;
	    c = 32;
	  }
	  else
	    c=fgetc(in);
	}
	if (c!=EOF)
	{
		if ( c == '#' ){
		  while (c != '\n'){
		    c = fgetc(in);
		  }
		}
		*ch=c;
		if (c=='\n'){
			++lineCnt;
		}
		return(0);
	};
	return(-1);
}

int getStr(FILE *in,char *str,int size)
{
	int	r;
	int	i;
	char	c;
	int	g;

	i=0; g=1;
	*str='\0';
	while(g)
	{
		r=getChar(in,&c);

		if (r!=0)
			return(r);

		if (isspace(c))
		{
			if (i!=0){	
				str[i]='\0';
				g=0;
			};
		}
		else if (i>=size)
				return(-1);
			else str[i++]=c;
	};
	return(r);
}

int checkNotNull(FILE *in)
{
  char str[64];
  int j, r;

  r=getStr(in,str,64);
  if (r!=0)
    return(r);
 
  if (strcmp(str, "notnull") == 0){
    _notNull = 1;
  }
  else{
/*
    ungetc(' ', in);
    for (j = strlen(str) - 1; j>=0; j--){
      ungetc(str[j], in);
    } 
    ungetc(' ', in);
*/
    unGetstr(str);
  }

  return(0);
}

int SearchOperate(char *name)
{
  int i = -1;

  while ( *name ){
    if (( *name == '=' ) || ( *name == '!' ) || ( *name == '<' ) ||
        ( *name == '>' )){
      return(i);
    }
    else{
      if ((!isalpha(*name)) && (!isdigit(*name)) && (*name != '_'))
        return(-1);
      name++;
      i = (i == -1) ? i+1 : i;
      i++;
    }
  }
  return(0);
}

int getNum(FILE *in,int *num, int iDefault)
{
	char	str[64];
	int	r;
	int	i,j;

	r=getStr(in,str,64);
	if (r!=0)
	  return(r);

	for(i=0; str[i]!=0; ++i){
	  if (!isdigit(str[i])){
	    if (iDefault){
/*
              ungetc(' ', in);
	      for(j=strlen(str) - 1; j>=0; j--){
		ungetc(str[j], in);
	      }
              ungetc(' ', in);
*/
	      unGetstr(str);
	      return(0);
	    }
	    else{
		return(-1);
	    }
	  }
	}

	*num=atoi(str);

	return(0);
}

int getName(FILE *in,char *name,int size)
{
	int	r;
	int	i;

	r=getStr(in,name,size);
	if (r!=0)
		return(r);

	if ((!isalpha(name[0])) && (name[0] != '_'))
		return(-1);
	for(i=0; name[i]!=0; ++i)
		if ((!isalnum(name[i])) &&(name[i]!='_') &&
	            (name[i] != '=') &&
	       	    (name[i] != '<') &&
       		    (name[i] != '>') &&
       		    (name[i] != '!'))
				return(-1);
	return(0);
}

int checkKeyword(char *name,int *keyword)
{
	int	i;

	for(i=0; i<(sizeof(keywordList)/sizeof(char *)); ++i){
		if (strcmp(keywordList[i],name)==0)
		{
			*keyword=i;
			return(0);
		};
	}
	return(-1);
}

int getKeyword(FILE *in,int *keyword)
{
	char	name[64];
	int	r;

	r=getName(in,name,64);
	if (r!=0)
		return(-1);

	r=checkKeyword(name,keyword);
	return(r);
}

int saveName(char **newP,char *name)
{
	int	len;

	len=strlen(name)+1;
	*newP=malloc(len);
	if (*newP==NULL)
		return(-2);
	strcpy(*newP,name);
	return(0);
}

int getTableName(FILE *in)
{
	char	name[64];
	int	r;
	int	u;
	int	i;

	r=getName(in,name,64);
	if (r!=0)
		return(-1);
	r=saveName(&tableName,name);
	if (r!=0)
		return(r);
	r=saveName(&tName,name);
	if (r!=0)
		return(r);
	u=1;
	for(i=0; tName[i]!='\0'; ++i)
	{
		if (u)
		{
			if (islower(tName[i]))
				tName[i]=toupper(tName[i]);
			u=0;
		};
		if (tName[i]=='_')
			u=1;
	};
	return(0);
}

int addField(char *name,int type,int size)
{
	FieldDef	*p;
	int		r;

	p=(FieldDef *)malloc(sizeof(FieldDef));
	if (p==NULL)
		return(-2);
	r=saveName(&p->name,name);
	if (r!=0)
	{
		free(p);
		return(-2);
	};
	p->next=NULL;
	p->type=type;
	p->size=size;
	if (_scale){
	  p->scale = _scale;
	  _scale = 0;
	}
	else
	  p->scale = 0;

        if (_notNull){
          p->notnull = 1;
          _notNull = 0;
        }
        else
          p->notnull = 0;

	if (fList.head==NULL)
	{
		fList.head=p;
		fList.tail=p;
		fList.cnt=1;
	}
	else
	{
		fList.tail->next=p;
		fList.tail=p;
		++fList.cnt;
	};
	if ((strcmp("dac",name)==0)&&(type==K_str)&&(size==16))
		hasDAC=1;
	return(0);
}

int getFieldList(FILE *in)
{
	char	name[64];
	int	type;
	int	size;
	int	r;
	int	g;
	int	keyword;

	g=1;r=0;
	while(g&&(r==0))
	{
		r=getName(in,name,64);
		if (r!=0)
			break;
		r=checkKeyword(name,&keyword);
		if (r==0)
		{
			if (keyword==K_end)
			{
				g=0;r=0;
			}
			else
			{
				g=0;r=-1;
			};
		}
		else
		{
			r=getKeyword(in,&type);
			if (r!=0)
			{
				g=0;r=-1;
			}
			else
			{
				switch(type)
				{
					case K_str:
						r = getNum(in, &size, 0);
						if (r==0){
                                                  checkNotNull(in);  
						  r=addField(name,type,size);
                                                }
						break;
					case K_double:
						size = 0;
						r = getNum(in, &size, 1);
                                                if (r == 0){ 
						  if(size!=0){
						      _scale = 0;
						      r = getNum(in, &_scale,1);
						  }
                                                  checkNotNull(in);  
						  if (size != 0) {
						     r=addField(name,type,size);
						  }
						  else{
						    r=addField(name,type,8);
						  }
                                                }
						break;
					case K_long:
                                                checkNotNull(in);
						r=addField(name,type,4);
						break;
					case K_short:
                                                checkNotNull(in);
						r=addField(name,type,2);
						break;
					case K_int:
                                                checkNotNull(in);
						r=addField(name,type,4);
						break;
					case K_YYYYMMDD:
                                                checkNotNull(in);
						r=addField(name,type,10);
						break;
					case K_HHMISS:
                                                checkNotNull(in);
						r=addField(name,type,8);
						break;
					case K_date:
						checkNotNull(in);
						r=addField(name,type,4);
						break;
					default:
					  	g=0;r=-1;
				};
			};
		};
	};
	return(r);
}

char *getOperateDef(char *operate)
{
	char	*s;

	if (strcmp(operate,"")==0)
			s="";
	else if (strcmp(operate,"=")==0)
			s="";
        if (strcmp(operate, "!=") == 0)
			s="_EN";
        else if  (strcmp(operate, "<=") == 0)
			s="_LE";
        else if  (strcmp(operate, ">=") == 0)
			s="_GE";
        else if  (strcmp(operate, "<>") == 0)
			s="_EN";
        else if  (strcmp(operate, "<") == 0)
			s="_L";
        else if  (strcmp(operate, ">") == 0)
			s="_G";
        else s="";
	return(s);
}

int outputOperateDef(FILE *out,char *operate)
{
	char	*s;

	s=getOperateDef(operate);
	fputs(s,out);
	return(0);
}

static int argN=0;
int addArg(char *name)
{
	ArgDef	*p;
	int	r;

	p=(ArgDef *)malloc(sizeof(ArgDef));
	if (p==NULL)
		return(-2);
          
        if ((strcmp(name, "_by_") != 0)
           &&(strcmp(name, "_order_by_") != 0)
           &&(strcmp(name, "_or_") != 0)){

          r = SearchOperate(name); 
          if (r < 0)
            return(r);
          if (r == 0)
            strcpy(p->operate, "=");
          else{
            if ((int)(strlen(name) - r) > 2 )
              return(-1);

            strcpy(p->operate, name+r);
            if ((strcmp(p->operate, "!=") != 0) && 
                (strcmp(p->operate, "<=") != 0) &&
                (strcmp(p->operate, ">=") != 0) &&
                (strcmp(p->operate, "<>") != 0) &&
                (strcmp(p->operate, "=") != 0)  &&
                (strcmp(p->operate, "<") != 0)  &&
                (strcmp(p->operate, ">") != 0)) 
              return(-1);

            name[r] = 0;
          }
        }

	p->n=argN++;
	r=saveName(&p->name,name);
	if (r!=0)
	{
		free(p);
		return(r);
	};
	p->next=NULL;
	if (aList.head==NULL)
	{
		aList.head=p;
		aList.tail=p;
		aList.cnt=1;
	}
	else
	{
		aList.tail->next=p;
		aList.tail=p;
		++aList.cnt;
	};
	return(0);
}

int freeArgList()
{
	ArgDef	*p;

	while(aList.head!=NULL)
	{
		p=aList.head->next;
		free(aList.head->name);
		free(aList.head);
		aList.head=p;
	};
	aList.tail=NULL;
	aList.cnt=0;
	return(0);
}

int getActionArg(FILE *in)
{
	char	name[64], save[64];
	int	r;
	int	keyword;
	int	g;

	argN=0;
	g=1;r=0;
	while(g&&(r==0))
	{
		r=getName(in,name,64);
		if(r==0)
		{
           		if ((strcmp(name, "_by_") != 0)
           		    &&(strcmp(name, "_order_by_") != 0)
           		    &&(strcmp(name, "_or_") != 0)){
        	 		strcpy(save,name);
        			r = SearchOperate(name); 
        			if (r < 0)
           	     		  return(r);
       		       		if (r > 0)
       		  		  name[r] = 0;
                        }

			r=checkKeyword(name,&keyword);
			if (r==0)
			{
           			if ((strcmp(name, "_by_") == 0)
           			   ||(strcmp(name, "_order_by_") == 0)
           			   ||(strcmp(name, "_or_") == 0)){
                                  r = addArg(name);
 				}
 				else
				  if (keyword==K_end)
					g=0;
				   else
				   {
					g=0; r=-1;
				   }
			}
			else{
                          strcpy(name, save);
                          r=addArg(name);
                        }
		};
	};
	return(r);
}

int getAction(FILE *in)
{
	int	r;
	int	keyword;

	freeArgList();
	r=getKeyword(in,&aType);
/*       printf("\n*******=[%d][%d]", r,aType);  */
	if(r!=0)
		return(-1);
	switch(aType)
	{
		case K_read_by:
		case K_read_lock_by:
		case K_update_by:
		case K_open_select_by:
		case K_open_select_for_update_by:
		case K_open_select:
		case K_open_select_for_update:
		case K_open_select_distinct_for:
		case K_del_by:
		case K_index_by:
		case K_index_by_unique:
		case K_index_unique_by:
		case K_sum_for:
		case K_avg_for:
		case K_avg_distinct_for:
		case K_count_distinct_for:
		case K_max_for:
		case K_min_for:
		case K_count_by:
		case K_count:
			r=getActionArg(in);
			if (r!=0)
				return(-1);
			break;
		case K_read:
		case K_read_lock:
		case K_update:
		case K_update_by_select:
		case K_update_by_lock:
		case K_add:
		case K_fetch_select:
		case K_close_select:
		case K_free_lock:
		case K_print_report:
		case K_del_all:
			r=getKeyword(in,&keyword);
			if ((r!=0)||(keyword!=K_end))
					return(-1);
			break;

		default:
			return(-1);
	};
	return(0);
}

FieldDef *findFieldDef(char *name)
{
	FieldDef	*p;

	p=fList.head;
	while(p!=NULL){
		if (strcmp(p->name,name)==0){
			return(p);
			}
		else p=p->next;
		}
	return(NULL);
}

int putDebugFunction(FILE *out)
{
  int n=0;
  FieldDef *f;

  fprintf(out, "\nint %s_%s_debug_print(char *reason,%s *adata, char *filename, int line_no)", sName,tableName, tName);
  fprintf(out, "\n{");
  fprintf(out,"\n\tDBlog_print(filename,line_no,\"TABLE [%s] REASON[%%s] LOG\",reason);\n",tName);
  f = fList.head;
  while (f != NULL){
  	fprintf(out, "\n\tDBlog_print(filename,line_no, \"");
	switch(f->type){
    	  case K_long:
		fprintf(out,"%s: %%ld\", adata->%s ",f->name,f->name); 
  		fprintf(out, ");");
		break;
	  case K_short:
		fprintf(out,"%s: %%d\", adata->%s ",f->name,f->name); 
  		fprintf(out, ");");
		break;
	case K_int:
		fprintf(out,"%s: %%d\", adata->%s ",f->name,f->name); 
  		fprintf(out, ");");
		break;
	case K_double:
		fprintf(out,"%s: %%lf\", adata->%s ",f->name,f->name); 
  		fprintf(out, ");");
		break;
	case K_str:
	case K_YYYYMMDD:
	case K_HHMISS:
		fprintf(out,"%s: %%s\", adata->%s ",f->name,f->name); 
  		fprintf(out, ");");
		break;
    	case K_date:
		fprintf(out,"%s: %%ld\", adata->%s ",f->name,f->name); 
  		fprintf(out, ");");
		break;

	default:
		fprintf(out,"\");");
		return(-1);
	}
        f = f->next;
  }
  fprintf(out, "\n\n\treturn(0);\n}\n");
  return(0);
}

static char *getFileName(char *s)
{
	static	char	name[128];
	int		i;

	strncpy(name,s,128);
	name[127]='\0';
	i=strlen(name);
	while(i>=0)
		if (name[i]=='.')
		{
			name[i]='\0';
			return(name);
		}
		else --i;
	return(name);
}

int OracleAction()
{
        FILE *in;
  	int r;
	char	*name;

	name=getFileName(actFileName);
	in=fopen(actFileName,"r");
	if (in==NULL)
	{
		perror("open act file error");
		exit(1);
	}

	r=getTableName(in);
	if (r!=0)
	{
		fprintf(stderr,"get table name error at line[%d]\n",lineCnt);
		exit(1);
	};
	r=getFieldList(in);
	if (r!=0)
	{
		fprintf(stderr,"get field list error at line[%d]\n",lineCnt);
		exit(1);
	};
	sprintf(includeFileName,"%s.h",name);
	sprintf(SqlFileName, "%s.sql",name);
	sprintf(cFileName,"%s.c",name);

	i_file=fopen(includeFileName,"w");
	if (i_file==NULL)
	{
		perror("open output include file error");
		exit(1);
	};

	sql_file=fopen(SqlFileName,"w");
	if (sql_file==NULL)
	{
		perror("open output SQL file error");
		exit(1);
	};


	c_file=fopen(cFileName,"w");
	if (c_file==NULL)
	{
		perror("open output include file error");
		exit(1);
	};

	getDate();
	r=outputOracleHead();
	if (r!=0)
	{
		fprintf(stderr,"output error\n");
		goto E;
	};
	if (hasDAC)
	{
		r=outputCfileDACcode();
		if (r!=0)
		{
			fprintf(stderr,"create dac code err\n");
			goto E;
		};
	};

	r=processOracleAction(in);
	if (r!=0)
	{
		fprintf(stderr,"process action at line[%d]\n",lineCnt);
		goto E;
	};
/**** add By Wangly 三  3月 21 20:58:19 EST 2001	*****/
	fprintf(i_file,"\n#ifdef\t__cplusplus\n}\n#endif\n\n");
/**** add By Wangly End ****/
	fprintf(i_file,"\n#endif\n");

   	r = putDebugFunction(c_file);
	if (r!=0)
	{
		fprintf(stderr,"output error\n");
		goto E;
	};
        
	return(0);
E:
        fclose(in);
	fclose(i_file);
	fclose(sql_file);
	fclose(c_file);
	return(-1);
}

int InformixAction()
{
	int r;
        FILE *in;
	char	*name;

	name=getFileName(actFileName);

	in=fopen(actFileName,"r");
	if (in==NULL)
	{
		perror("open act file error");
		exit(1);
	}

	r=getTableName(in);
	if (r!=0)
	{
		fprintf(stderr,"get table name error at line[%d]\n",lineCnt);
		exit(1);
	};
	r=getFieldList(in);
	if (r!=0)
	{
		fprintf(stderr,"get field list error at line[%d]\n",lineCnt);
		exit(1);
	};
	sprintf(includeFileName,"%s.h",name);
	sprintf(SqlFileName, "%s.sql",name);
	sprintf(cFileName,"%s.ec",name);

	i_file=fopen(includeFileName,"w");
	if (i_file==NULL)
	{
		perror("open output include file error");
		exit(1);
	};

	sql_file=fopen(SqlFileName,"w");
	if (sql_file==NULL)
	{
		perror("open output SQL file error");
		exit(1);
	};


	ec_file=fopen(cFileName,"w");
	if (ec_file==NULL)
	{
		perror("open output include file error");
		exit(1);
	};

	getDate();
	r=outputInformixHead();
	if (r!=0)
	{
		fprintf(stderr,"output error\n");
		goto E;
	};
	if (hasDAC)
	{
		r=outputECfileDACcode();
		if (r!=0)
		{
			fprintf(stderr,"create dac code err\n");
			goto E;
		};
	};

	r=processInformixAction(in);
	if (r!=0)
	{
		fprintf(stderr,"process action at line[%d]\n",lineCnt);
		goto E;
	};
/**** add By Wangly 三  3月 21 20:58:19 EST 2001	*****/
	fprintf(i_file,"\n#ifdef\t__cplusplus\n}\n#endif\n\n");
/**** add By Wangly End ****/
	fprintf(i_file,"\n#endif\n");

   	r = putDebugFunction(ec_file);
	if (r!=0)
	{
		fprintf(stderr,"output error\n");
		goto E;
	};
	return(0);
E:
        fclose(in);
	fclose(i_file);
	fclose(sql_file);
	fclose(ec_file);
	return(-1);
}

int SearchArg(char *argv[],char *str)
{
	int	i;

	for(i=0; argv[i]!=NULL; ++i)
		if (strcmp(argv[i],str)==0)
				return(1);
	return(0);
}
main(int argc,char *argv[])
{
	int r = 0;
 	char ss[30], *envbuf;

/*
  	strcpy(ss, "TEST");
	  sName = ss;
	  strcpy(actFileName, "test.act");
          r = OracleAction();
          if (r != 0)
              goto E;
*/
	if (argc < 3){
	  printf("Usage: dbaction V3.2 <SystemName> <ActionFileName> [Option]\n");
          printf("       Option:\n");
	  printf("               -o  create Oracle Sql and C file\n");
	  printf("               -i  create Informix Sql and EC file\n");
	  goto E;
	};

	envbuf = getenv("DB_FOR_DBACTION");
        if ((SearchArg(argv, "-o") == 0) && 
            (SearchArg(argv, "-i") == 0)){

	  if (( envbuf == NULL ) && 
	      ( strcmp(envbuf, "informix") != 0) &&
	      ( strcmp(envbuf, "oracle") != 0)){ 

	    printf("Usage: dbaction <SystemName> <ActionFileName> [Option]\n");
            printf("       Option:\n");
	    printf("               -o  create Oracle Sql and C file\n");
	    printf("               -i  create Informix Sql and EC file\n");
            goto E;
	  }
	  else{
	    if ( strcmp(envbuf, "oracle") == 0){
	      sName=argv[1];
	      strcpy(actFileName, argv[2]);
              r = OracleAction();
              if (r != 0)
                goto E;
            }
	    if ( strcmp(envbuf, "informix") == 0){
	      sName=argv[1];
	      strcpy(actFileName, argv[2]);
              r = InformixAction();
              if (r != 0)
                goto E;
            }
	  }
        }
        else{
	  sName=argv[1];
	  strcpy(actFileName, argv[2]);
          if (SearchArg(argv, "-o") != 0){
            r = OracleAction();
            if (r != 0)
              goto E;
          }
          else
            if (SearchArg(argv, "-i") != 0){
              r = InformixAction();
            if (r != 0)
              goto E;
            }
	    else{
	      printf("Usage: dbaction <SystemName> <ActionFileName> [Option]\n");
              printf("       Option:\n");
	      printf("               -o  create Oracle Sql and C file\n");
	      printf("               -i  create Informix Sql and EC file\n");

	    }
        }

E:
	return(r);
}
