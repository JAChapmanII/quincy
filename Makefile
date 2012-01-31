SDIR=src
LDIR=lib
ODIR=obj
BDIR=.

BIN=quincy
MAPS=$(LDIR)/vmap $(LDIR)/simap

LDFLAGS=-pthread
CFLAGS=-std=c99 -pedantic -Wall -Wextra -I$(LDIR)

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
$(BDIR)/$(BIN): $(ODIR)/$(BIN).o $(ODIR)/vmap.o $(ODIR)/util.o
	$(CC) $(LDFLAGS) -o $@ $^

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<
$(ODIR)/%.o: $(LDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(LDIR)/%: $(LDIR)/%.def $(LDIR)/%.dec $(LDIR)/%.def
	$(LDIR)/mstruct.sh $< $(LDIR)

clean:
	rm -f $(ODIR)/*.o $(BDIR)/$(BIN)

