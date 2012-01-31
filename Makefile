SDIR=src
ODIR=obj
BDIR=.

BIN=quincy
MAPS=map/vmap map/simap map/ismap
OBJS=$(ODIR)/vmap.o $(ODIR)/simap.o $(ODIR)/ismap.o $(ODIR)/util.o

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

all: dirs $(MAPS) $(BIN)
dirs:
	mkdir -p $(SDIR) $(ODIR) $(BDIR)

$(BIN): $(BDIR)/$(BIN)
$(BDIR)/$(BIN): $(ODIR)/$(BIN).o $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

map/%: map/%.def map/%.dec map/%.def
	$(BDIR)/mstruct.sh $< map $(SDIR)

clean:
	rm -f $(ODIR)/*.o $(BDIR)/$(BIN)

