PREFIX = /usr/local

INCS = 
LIBS = -lsqlite3

CFLAGS= -O2 -march=znver2 -pipe -std=gnu18 -Wall -Wextra -Wno-deprecated-declarations $(INCS)
LDFLAGS = $(LIBS)

CC = cc

