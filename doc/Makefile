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
	mkdir -m 755 -p $(DESTDIR)$(DOCDIR) $(DESTDIR)$(MANDIR)/man$(MAN_SEC)
	install -m 644 $(BIN).$(MAN_SEC) $(DESTDIR)$(MANDIR)/man$(MAN_SEC)
	install -m 644 $(SBIN).$(MAN_SEC) $(DESTDIR)$(MANDIR)/man$(MAN_SEC)
	install -m 644 FAQ $(DESTDIR)$(DOCDIR)
	install -m 644 LICENSE $(DESTDIR)$(DOCDIR)
	install -m 644 AUTHORS $(DESTDIR)$(DOCDIR)
	install -m 644 INSTALL $(DESTDIR)$(DOCDIR)
	install -m 644 ISPEnv.schema $(DESTDIR)$(DOCDIR)
	install -m 644 ISPEnv2.schema $(DESTDIR)$(DOCDIR)
	install -m 644 example.ldif $(DESTDIR)$(DOCDIR)
	install -m 644 HISTORY $(DESTDIR)$(DOCDIR)
	install -m 644 README $(DESTDIR)$(DOCDIR)
	gzip -f -9 $(DESTDIR)$(DOCDIR)/FAQ
	gzip -f -9 $(DESTDIR)$(DOCDIR)/HISTORY
	gzip -f -9 $(DESTDIR)$(MANDIR)/man$(MAN_SEC)/$(BIN).$(MAN_SEC)
	gzip -f -9 $(DESTDIR)$(MANDIR)/man$(MAN_SEC)/$(SBIN).$(MAN_SEC)
