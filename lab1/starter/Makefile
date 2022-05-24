

# Makefile  

CC = gcc       # compiler
CFLAGS = -Wall -g -std=c99 # compilation flags
LD = gcc       # linker
LDFLAGS = -g   # debugging symbols in build
LDLIBS = -lz   # link with libz

# For students 
LIB_UTIL = png_util/zutil.o png_util/lab_png.h
SRCS   = catpng.c
OBJS   = catpng.o $(LIB_UTIL)

TARGETS = catpng.out

all: ${TARGETS}

main.out: $(OBJS) 
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

%.o: %.c 
	$(CC) $(CFLAGS) -c $< 

%.d: %.c
	gcc -MM -MF $@ $<

-include $(SRCS:.c=.d)

pnginfo.out: $(OBJS) 
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

%.o: %.c 
	$(CC) $(CFLAGS) -c $< 

%.d: %.c
	gcc -MM -MF $@ $<

-include $(SRCS:.c=.d)

pngfind.out: $(OBJS) 
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

%.o: %.c 
	$(CC) $(CFLAGS) -c $< 

%.d: %.c
	gcc -MM -MF $@ $<

-include $(SRCS:.c=.d)

catpng.out: $(OBJS) 
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

%.o: %.c 
	$(CC) $(CFLAGS) -c $< 

%.d: %.c
	gcc -MM -MF $@ $<

-include $(SRCS:.c=.d)

.PHONY: clean
clean:
	rm -f *.d *.o $(TARGETS) 


# CC=gcc
# CFLAGS=-Wall -g
# LD=gcc
# LDFLAGS=-g
# LDLIBS = -lz
# LIB_UTIL = zutil.o crc.o
# OBJS1 = pnginfo.o crc.o
# OBJS2 = pngfind.o crc.o

# all: pnginfo pngfind

# pnginfo: $(OBJS1)
# 	$(LD) $(LDFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

# pngfind: $(OBJS2)
# 	$(LD) $(LDFLAGS) -o $@ $^ $(LDFLAGS) $(LDLIBS)

# .c.o:
# 	$(CC) $(CFLAGS) -c $<

# %.d: %.c
# 	gcc -MM -FF $@ $<

# .PHONY: clean
# clean:
# 	rm -f .f.o .out.d all.png pnginfo pngfind 
