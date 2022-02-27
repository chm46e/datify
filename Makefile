include config.mk

SRC = src/datify.c src/sql.c src/util.c

OBJ = $(SRC:.c=.o)

all: datify

.c.o:
	$(CC) -o $@ -c $(CFLAGS) $<

$(OBJ): config.mk

datify: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f datify $(OBJ)

install: all
	mkdir -p $(PREFIX)/bin
	cp -f datify $(PREFIX)/bin
	chmod 755 $(PREFIX)/bin/datify
	
uninstall:
	rm -f $(PREFIX)/bin/datify

.PHONY: all clean install uninstall
