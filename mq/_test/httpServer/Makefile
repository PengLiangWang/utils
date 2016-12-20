cc=gcc -g

obj=testmain.o httpserver.o socket.o utils.o

all:test

test:$(obj)
	$(cc) -o $@ $(obj)

clean:
	rm -f $(obj) test
