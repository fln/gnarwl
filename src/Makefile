OBJ=config.o dbaccess.o util.o mailhandler.o
SOBJ=config.o dbaccess.o util.o

catch:
	$(MAKE) -C ..

all: clean compile

compile: $(OBJ) $(SOBJ)
	$(CC) $(CFLAGS) -o $(BIN) gnarwl.c $(OBJ) $(LFLAGS)
	$(CC) $(CFLAGS) -o $(SBIN) damnit.c $(SOBJ) $(LFLAGS)

clean:
	rm -f DEADJOE *.o *~ $(BIN) $(SBIN)

install:
	mkdir -m 755 -p $(DESTDIR)$(BINDIR) $(DESTDIR)$(SBINDIR)
	install -m 755 -s $(BIN) $(DESTDIR)$(BINDIR)
	install -m 755 -s $(SBIN) $(DESTDIR)$(SBINDIR)

