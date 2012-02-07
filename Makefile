SDIR=src
LDIR=lib
ODIR=obj
BDIR=.
MBDIR=mbin
TBDIR=tbin

BINS=$(BDIR)/quincy $(BDIR)/conman
TESTS=$(TBDIR)/conftest $(TBDIR)/mittest
MAPS=map/vmap map/simap map/ismap
MOUT=$(LDIR)/vmap.? $(LDIR)/simap.? $(LDIR)/ismap.?
MODULES=$(MBDIR)/wave $(MBDIR)/love $(MBDIR)/fish

COBJS=$(ODIR)/util.o $(ODIR)/conf.o
SOBJS=$(ODIR)/subprocess.o $(ODIR)/bufreader.o $(ODIR)/util.o
QOBJS=$(ODIR)/vmap.o $(ODIR)/simap.o $(ODIR)/ismap.o $(ODIR)/module.o \
    $(SOBJS) $(COBJS)
MOBJS=$(ODIR)/ircsock.o $(ODIR)/vmap.o $(SOBJS) $(COBJS)
FOBJS=$(ODIR)/status.o $(ODIR)/module_main.o

LDFLAGS=-pthread
CFLAGS=-std=c99 -D_POSIX_C_SOURCE=200112L -D_BSD_SOURCE -D_XOPEN_SOURCE=700
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

# make rules for quincy and conman
$(BDIR)/quincy: $(ODIR)/quincy.o $(QOBJS)
	$(CC) $(LDFLAGS) -o $@ $^ -lpcre
$(BDIR)/conman: $(ODIR)/conman.o $(MOBJS)
	$(CC) $(LDFLAGS) -o $@ $^

# make rules for the tests
$(TBDIR)/conftest: $(ODIR)/conftest.o $(ODIR)/vmap.o $(COBJS)
	$(CC) $(LDFLAGS) -o $@ $^
$(TBDIR)/mittest: $(ODIR)/mittest.o $(ODIR)/ismap.o $(ODIR)/util.o
	$(CC) $(LDFLAGS) -o $@ $^

# make rule for the bodule binaries
$(MBDIR)/%: $(ODIR)/%.o $(FOBJS)
	$(CC) $(LDFLAGS) -o $@ $^ -lpcre

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(ODIR)/%.o: $(LDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(ODIR)/%.o: test/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(ODIR)/%.o: modules/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

map/%: map/%.def map/%.dec map/%.def
	$(BDIR)/mstruct.sh $< map $(LDIR)

clean:
	rm -f $(ODIR)/*.o $(BINS) $(TESTS) $(MOUT)

