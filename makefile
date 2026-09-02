CC=gcc
CFLAGS=-I
LDIR=lib
SDIR=src

LIBS=-lm

ODIR=obj

_DEPS=stringing.h
_OBJ=main.c stringing.c

DEPS = $(patsubst %,$(LDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(SDIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

make: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)