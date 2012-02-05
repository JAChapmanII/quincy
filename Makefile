SDIR=src
LDIR=lib
ODIR=obj
BDIR=.

BINS=$(BDIR)/quincy $(BDIR)/cm $(BDIR)/conftest
MAPS=map/vmap map/simap map/ismap
MODULES=modules/wave

QOBJS=$(ODIR)/vmap.o $(ODIR)/simap.o $(ODIR)/ismap.o $(ODIR)/util.o
MOBJS=$(ODIR)/ircsock.o
FOBJS=$(ODIR)/status.o $(ODIR)/module.o

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

all: dirs $(MAPS) $(MODULES) $(BINS)
dirs:
	mkdir -p $(SDIR) $(ODIR) $(BDIR)

$(BDIR)/quincy: $(ODIR)/quincy.o $(QOBJS)
	$(CC) $(LDFLAGS) -o $@ $^
$(BDIR)/cm: $(ODIR)/cm.o $(MOBJS)
	$(CC) $(LDFLAGS) -o $@ $^
$(BDIR)/conftest: $(ODIR)/conftest.o $(MOBJS)
	$(CC) $(LDFLAGS) -o $@ $^
modules/%: $(ODIR)/%.o $(FOBJS)
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
	rm -f $(ODIR)/*.o $(BINS)

