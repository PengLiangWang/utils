include $(PREFIX)/make/makeobj.inc
CC=cc
BINDIR=$(HOME)/bin
LIBDIR=$(HOME)/lib
INCDIR=$(HOME)/include
INCL=  -I../../include -Iact#-DPOS_DEBUG
INCLUDE= -I$(HOME)/include  -Iact#-DPOS_DEBUG
LIBWEB= -L. -L$(LIBDIR)
LOCALLIB= -L. -L$(LIBDIR) -lcurses -lMd5 -lpthread -ldbutil -lttsys $(DBLIB)
PROOBJ=monwin.o  init.o  welmon.o refresh.o end.o scrcon.o act/DB_PosJnls.o
PROEXE=monwin
RM = rm -f

$(PROEXE):  ACT  $(PROOBJ)
	@echo " Making $@ ..."
	@$(CC) -o $@ $(PROOBJ) $(LOCALLIB)

ACT : 
	cd act && $(MAKE) && $(MAKE) DB_PosJnls.o  
#.c.o:   *.h $(INCDIR)/*.h
#	@echo " Compiling $< ..."
#	@$(CC) -c -O2 $(INCLUDE) $<

all:    $(PROEXE)

#install:
##	make
#	@echo "Install ..."
#	cp $(PROEXE) $(BINDIR)

#lean:
#@rm -f core
#@rm -f *.o
#@rm -f $(PROEXE)
INSTALL:
	make
	@echo "Install ..."
	cp $(PROEXE) $(BINDIR)
CLEAN:
	cd act && $(MAKE) clean
	-$(RM) $(PROEXE) 
	-$(RM) *.o
	-$(RM) core.*
