# Makefile  

CC = gcc       # compiler
CFLAGS = -Wall -g -std=c99 # compilation flags
LD = gcc       # linker
LDFLAGS = -g   # debugging symbols in build
LDLIBS = -lz   # link with libz

# For students 
LIB_UTIL = png_util/crc.o png_util/lab_png.h png_util/zutil.o
SRCS   = pnginfo.c pngfind.c #catpng.c
OBJS1   = pnginfo.o  $(LIB_UTIL)
OBJS2   = pngfind.o  $(LIB_UTIL)
#OBJS3   = catpng.o  $(LIB_UTIL)

TARGETS = pnginfo.out pngfind.out #catpng.out

all: ${TARGETS}

# main.out: $(OBJS) 
# 	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

# %.o: %.c 
# 	$(CC) $(CFLAGS) -c $< 

# %.d: %.c
# 	gcc -MM -MF $@ $<

# -include $(SRCS:.c=.d)

pnginfo.out: $(OBJS1) 
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

%.o: %.c 
	$(CC) $(CFLAGS) -c $< 

%.d: %.c
	gcc -MM -MF $@ $<

-include $(SRCS:.c=.d)

pngfind.out: $(OBJS2) 
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

%.o: %.c 
	$(CC) $(CFLAGS) -c $< 

%.d: %.c
	gcc -MM -MF $@ $<

-include $(SRCS:.c=.d)

# catpng.out: $(OBJS3) 
# 	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

# %.o: %.c 
# 	$(CC) $(CFLAGS) -c $< 

# %.d: %.c
# 	gcc -MM -MF $@ $<

# -include $(SRCS:.c=.d)

.PHONY: clean
clean:
	rm -f *.d *.o $(TARGETS) 
