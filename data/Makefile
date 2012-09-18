catch:
	$(MAKE) -C .. all

all: clean spec config

clean: 
	rm -f $(BIN)-$(VER).spec gnarwl.cfg

config:
	@sed "s\\_HOMEDIR_\\$(HOMEDIR)\\g ; s\\_VER_\\$(VER)\\g ; s\\_MAN_SEC_\\$(MAN_SEC)\\g" < config.tmpl > gnarwl.cfg

spec:
	@sed "s\\_BIN_\\$(BIN)\\g ; s\\_VER_\\$(VER)\\g; s\\_SBIN_\\$(SBIN)\\g; s\\_SEC_\\$(MAN_SEC)\\g" < spec.tmpl > $(BIN)-$(VER).spec 

install:
	mkdir -m 755 -p  $(HOMEDIR)/block $(HOMEDIR)/bin $(CONFDIR)
	echo \|$(BINDIR)/$(BIN) > $(HOMEDIR)/.forward
	install -m 644 header.txt $(HOMEDIR)
	install -m 644 footer.txt $(HOMEDIR)
	install -m 600 gnarwl.cfg $(CONFDIR)
	cat badheaders.txt | $(SBINDIR)/$(SBIN) -a $(HOMEDIR)/badheaders.db
	cat blacklist.txt | $(SBINDIR)/$(SBIN) -a $(HOMEDIR)/blacklist.db

