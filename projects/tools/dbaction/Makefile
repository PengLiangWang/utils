COMMIT_DIR=
PUB_INCLUDE_SRC_FILE=
INCLUDE_SRC_FILE=dbaction.h dbactiontype.h OracleOci.h
C_SRC_FILE= dbactionMain.c dbactionInf.c dbactionOra.c 
INCLUDE_FILE=
EC_FILE=
OTHER_SRC_FILE=
SH_FILE=
SQL_FILE=
BIN_FILE=dbaction
C_FLAG=

include $(PREFIX)/make/makeobj.inc

DBACTOBJ=dbactionMain.o dbactionInf.o dbactionOra.o #sub.o

dbaction: $(DBACTOBJ)
	$(CC)  -o dbaction $(DBACTOBJ) 

