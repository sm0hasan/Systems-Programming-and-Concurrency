CC = gcc 
CFLAGS = -Wall -std=c99 -g 
LD = gcc
LDFLAGS = -std=c99 -g 
LDLIBS = -lz -lcurl 

LIB_UTIL = png_util/crc.o png_util/lab_png.h png_util/zutil.o
SRCS   = test_lab3.c #main_write_callback.c zutil.c crc.c catpng.c shm_stack.c
OBJS1  = test_lab3.o $(LIB_UTIL)
TARGETS= paster2

all: ${TARGETS}

paster2: $(OBJS1) 
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) -pthread

%.o: %.c 
	$(CC) $(CFLAGS) -c $< 

%.d: %.c
	gcc -MM -MF $@ $<

-include $(SRCS:.c=.d)

.PHONY: clean
clean:
	rm -rf *~ *.d *.o $(TARGETS) *.png images