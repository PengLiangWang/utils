APP_DEF_USER = note
PUB_INCLUDE_SRC_FILE= note.h IBnote.h
C_SRC_FILE=note.c note_queue.c note_server.c 
ETC_FILE=IBnote IBreload
LIB_FILE=libIBnote.a
C_FLAG=

include $(PRELOAD)/make/makeobj.inc

LINKLIB=  -L. -L$(PREFIX)/lib -lIBnote -lIButil

LIBOBJ=note.o note_queue.o IBtranscode.o IBGroup.o IBaddr.o

libIBnote.a:$(PARALLELMAKE) $(LIBOBJ)
	ar rv $@ $(LIBOBJ)

IBnote:note_server.o libIBnote.a
	$(CC) -o $@ note_server.o $(LINKLIB)

IBreload:note_reload.o libIBnote.a
	$(CC) -o $@ note_reload.o $(LINKLIB) $(WINLIB)
