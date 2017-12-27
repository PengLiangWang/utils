PUB_INCLUDE_SRC_FILE=expat.h
INCLUDE_SRC_FILE=expat.h
C_SRC_FILE= 
PC_SRC_FILE=
DOC_SRC_FILE= 
SQL_SRC_FILE=
SH_FILE=
OBJ_FILE= 
LIB_FILE=libxml2.a
SH_FILE= 
BIN_FILE=
EXEC_LOCAL= 
OTHER_SRC_FILE= 
C_FLAG=-g
include $(PREFIX)/make/makeobj.inc

LIBOBJ=xmlparse.o xmlrole.o  xmltok.o 

LIBS= -L. -L$(LIBDIR) -lxml2 $(SOCKETLIB)

$(LIB_FILE):$(LIBOBJ)
	ar rv $@ $?

#xml:elements.o
#	$(CC) -o xml elements.o $(LIBS)
