LDFLAGS=-pthread
CFLAGS=-std=c99 -pedantic -Wall -Wextra

SDIR=src
ODIR=obj
BDIR=.

BIN=quincy

ifndef RELEASE
CFLAGS+=-g
else
CFLAGS+=-O3 -Os
endif

ifdef PROFILE
CFLAGS+=-pg
LDFLAGS+=-pg
endif

all: dirs $(BIN)
dirs:
	mkdir -p $(SDIR) $(ODIR) $(BDIR)

$(BIN): $(BDIR)/$(BIN)
$(BDIR)/$(BIN): $(ODIR)/$(BIN).o
	$(CC) $(LDFLAGS) -o $@ $^

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(ODIR)/*.o $(BDIR)/$(BIN)

