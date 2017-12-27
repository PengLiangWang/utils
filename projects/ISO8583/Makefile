INCLUDE_SRC_FILE= 
PUB_INCLUDE_SRC_FILE=ISO8583.h
INCLUDE_FILE=
C_SRC_FILE=ISO8583.c ISO8583chg.c  transrecords.c
EC_SRC_FILE= 
EC_FILE=
DOC_SRC_FILE=
SQL_SRC_FILE=
SH_FILE = 
SQL_FILE=
OTHER_SRC_FILE= 
BIN_FILE=ipkg posme posmc
LIB_FILE=libisomsg.a

C_FLAG=-g
APP_DEF_USER=ttsys
include $(PREFIX)/make/makeobj.inc

OBJS=ISO8583.o ISO8583chg.o transrecords.o

libisomsg.a:$(OBJS)
	ar rv $@ $(OBJS)

LIBS= -L. -L$(LIBDIR) -lisomsg -lttsys -lxml2 -ldl -lcurses $(SOCKETLIB)

ipkg:isopkg.o
	$(CC) $(C_FLAG) -g -o ipkg isopkg.o $(LIBS)

posme:PosEnM.o
	$(CC) $(C_FLAG) -g -o posme PosEnM.o $(LIBS)

posmc:PosCnM.o
	$(CC) $(C_FLAG) -g -o posmc PosCnM.o $(LIBS)
