catch:
	$(MAKE) -C .. all

all: clean manpage

clean: 
	rm -f *~ $(BIN).$(MAN_SEC)* $(SBIN).$(MAN_SEC)* DEADJOE

manpage:
	@echo Generating manpage...
	@sed "s\\_CONFDIR_\\$(CONFDIR)\\g ; s\\_HOMEDIR_\\$(HOMEDIR)\\g ; s\\_MANSEC_\\$(MAN_SEC)\\g" < $(BIN).man | groff -man -Tascii > $(BIN).$(MAN_SEC)
	@sed "s\\_CONFDIR_\\$(CONFDIR)\\g ; s\\_HOMEDIR_\\$(HOMEDIR)\\g ; s\\_MANSEC_\\$(MAN_SEC)\\g" < $(SBIN).man | groff -man -Tascii > $(SBIN).$(MAN_SEC)


install:
	mkdir -m 755 -p $(DOCDIR) $(MANDIR)/man$(MAN_SEC)
	install -m 644 $(BIN).$(MAN_SEC) $(MANDIR)/man$(MAN_SEC)
	install -m 644 $(SBIN).$(MAN_SEC) $(MANDIR)/man$(MAN_SEC)
	install -m 644 FAQ $(DOCDIR)
	install -m 644 LICENSE $(DOCDIR)
	install -m 644 AUTHORS $(DOCDIR)
	install -m 644 INSTALL $(DOCDIR)
	install -m 644 ISPEnv.schema $(DOCDIR)
	install -m 644 ISPEnv2.schema $(DOCDIR)
	install -m 644 example.ldif $(DOCDIR)
	install -m 644 HISTORY $(DOCDIR)
	install -m 644 README $(DOCDIR)
	gzip -f -9 $(DOCDIR)/FAQ
	gzip -f -9 $(DOCDIR)/HISTORY
	gzip -f -9 $(MANDIR)/man$(MAN_SEC)/$(BIN).$(MAN_SEC)
	gzip -f -9 $(MANDIR)/man$(MAN_SEC)/$(SBIN).$(MAN_SEC)
