SDIR=src
ODIR=obj
BDIR=.

BINS=$(BDIR)/quincy $(BDIR)/cm
MAPS=map/vmap map/simap map/ismap
QOBJS=$(ODIR)/vmap.o $(ODIR)/simap.o $(ODIR)/ismap.o $(ODIR)/util.o
MOBJS=

LDFLAGS=-pthread
CFLAGS=-std=c99 -pedantic -Wall -Wextra

ifndef RELEASE
CFLAGS+=-g
else
CFLAGS+=-O3 -Os
endif

ifdef PROFILE
CFLAGS+=-pg
LDFLAGS+=-pg
endif

all: dirs $(MAPS) $(BINS)
dirs:
	mkdir -p $(SDIR) $(ODIR) $(BDIR)

$(BDIR)/quincy: $(ODIR)/quincy.o $(QOBJS)
	$(CC) $(LDFLAGS) -o $@ $^
$(BDIR)/cm: $(ODIR)/cm.o $(MOBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(ODIR)/%.o: conman/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

map/%: map/%.def map/%.dec map/%.def
	$(BDIR)/mstruct.sh $< map $(SDIR)

clean:
	rm -f $(ODIR)/*.o $(BDIR)/$(BIN)

