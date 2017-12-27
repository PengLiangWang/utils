##########  This File is Your Makefile Demo

BIN_FILE= uit_test
ETC_FILE=uitCom
LIB_FILE=libuit.a
C_FILE=test.c
INCLUDE_FILE=
C_SRC_FILE=uitCom.c UitText.c UitMenu.c UitForm.c UitFormEdit.c \
	uit_test.c uit_read.c tt.c
DOC_SRC_FILE=uit.txt test.uit
PUB_INCLUDE_SRC_FILE=UIT.h uit_read.h UitFormEdit.h

CLEAN_FILE_SRC=
USR_CLEAN_FILE=
C_FLAG=

include $(PREFIX)/make/makeobj.inc

###### The Following is The Objects That Your Need Compile

# LIBS=-L. -L$(PREFIX)/lib

OBJS=UitText.o UitMenu.o UitForm.o UitFormEdit.o uit_read.o

###### The Following is The Normal Compile Rule

# EXEC_FILE=tt

libuit.a:$(OBJS)
	ar rv libuit.a $(OBJS)

uitCom: uitCom.c
	$(CC) $(C_FLAG) -o $@ $?
	cp uitCom $(PREFIX)/etc/.

UitMenu.o:UitMenu.c UIT.h
UitForm.o:UitForm.c UIT.h
UitText.o:UitText.c UIT.h
UitFormEdit.o:UitFormEdit.c UitFormEdit.h
uit_read.o:uit_read.c uit_read.h

test.c: test.uit uitCom
test.h: test.uit uitCom

test.o:test.c test.h
uit_test.o: uit_test.c test.h

uit_test: uit_test.o test.o libuit.a
	$(CC) $(C_FLAG) -o $@ uit_test.o test.o  -L. -L$(PREFIX)/lib \
	$(WINLIB) 

t: t.o readN.o
	$(CC) $(C_FLAG) -o $@ t.o readN.o -lHcurses $(DBLIB)
tt.o:tt.c 
tt:tt.o libuit.a
	$(CC) $(C_FLAG) -o tt tt.o uit_read.o -L. -L$(PREFIX)/lib \
			$(WINLIB)
