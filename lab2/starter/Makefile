# Makefile  

CC = gcc       # compiler
CFLAGS = -Wall -g -std=c99 # compilation flags
LD = gcc       # linker
LDFLAGS = -std=c99 -g  # debugging symbols in build
LDLIBS = -lcurl -lz    # link with libz and lcurl

SRCS = paster.c #main_write_header_cb.c #main.c crc.c zutil.c util.c
OBJS = paster.o #main_write_header_cb.o #main.o zutil.o crc.o util.c
TARGETS = paster

all: ${TARGETS}

paster: $(OBJS)
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

%.d: %.c
	gcc -MM -MF $@ $<

-include $(SRCS:.c=.d)

.PHONY: clean
clean:
	rm -rf *~ *.d *.o $(TARGETS) *.png *.dSYM