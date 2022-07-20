CC = gcc 
CFLAGS = -Wall -std=c99 -g 
LD = gcc
LDFLAGS = -std=c99 -g 
LDLIBS = -lz -lcurl 

SRCS   = paster2.c 
OBJS1  = paster2.o
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