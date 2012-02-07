SDIR=src
LDIR=lib
ODIR=obj
BDIR=.
MBDIR=mbin
TBDIR=tbin

BINS=$(BDIR)/quincy $(BDIR)/cm
TESTS=$(TBDIR)/conftest $(TBDIR)/mittest
MAPS=map/vmap map/simap map/ismap
MOUT=$(LDIR)/vmap.? $(LDIR)/simap.? $(LDIR)/ismap.?
MODULES=$(MBDIR)/wave $(MBDIR)/love

COBJS=$(ODIR)/util.o $(ODIR)/conf.o
QOBJS=$(ODIR)/vmap.o $(ODIR)/simap.o $(ODIR)/ismap.o $(ODIR)/module.o $(COBJS)
MOBJS=$(ODIR)/ircsock.o $(ODIR)/vmap.o $(ODIR)/subprocess.o $(COBJS)
FOBJS=$(ODIR)/status.o $(ODIR)/module_main.o

LDFLAGS=-pthread
CFLAGS=-std=c99 -D_POSIX_C_SOURCE=200112L -D_BSD_SOURCE
CFLAGS+=-pedantic -Wall -Wextra
CFLAGS+=-I$(LDIR)

ifndef RELEASE
CFLAGS+=-g
else
CFLAGS+=-O3 -Os
endif

ifdef PROFILE
CFLAGS+=-pg
LDFLAGS+=-pg
endif

all: dirs maps modules $(BINS)
maps:    $(MAPS)
modules: $(MODULES)
tests:   $(TESTS)
dirs:
	mkdir -p $(SDIR) $(LDIR) $(ODIR) $(BDIR) $(MBDIR) $(TBDIR)

$(BDIR)/quincy: $(ODIR)/quincy.o $(QOBJS)
	$(CC) $(LDFLAGS) -o $@ $^ -lpcre
$(BDIR)/cm: $(ODIR)/cm.o $(MOBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(TBDIR)/conftest: $(ODIR)/conftest.o $(ODIR)/vmap.o $(COBJS)
	$(CC) $(LDFLAGS) -o $@ $^
$(TBDIR)/mittest: $(ODIR)/mittest.o $(ODIR)/ismap.o $(ODIR)/util.o
	$(CC) $(LDFLAGS) -o $@ $^

$(MBDIR)/%: $(ODIR)/%.o $(FOBJS)
	$(CC) $(LDFLAGS) -o $@ $^ -lpcre

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(ODIR)/%.o: $(LDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(ODIR)/%.o: test/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(ODIR)/%.o: conman/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(ODIR)/%.o: modules/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

map/%: map/%.def map/%.dec map/%.def
	$(BDIR)/mstruct.sh $< map $(LDIR)

clean:
	rm -f $(ODIR)/*.o $(BINS) $(TESTS) $(MOUT)

