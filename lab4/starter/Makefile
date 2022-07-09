# # Makefile, ECE252  
# # Yiqing Huang <yqhuang@uwaterloo.ca>

CC = gcc 
CFLAGS_XML2 = $(shell xml2-config --cflags)
CFLAGS_CURL = $(shell curl-config --cflags)
CFLAGS = -Wall $(CFLAGS_XML2) $(CFLAGS_CURL) -std=gnu99 -g -DDEBUG1_
LD = gcc
LDFLAGS = -std=gnu99 -g 
LDLIBS_XML2 = $(shell xml2-config --libs)
LDLIBS_CURL = $(shell curl-config --libs)
LDLIBS = $(LDLIBS_XML2) $(LDLIBS_CURL) 

SRCS   = findpng2.c
OBJS3  = findpng2.o
TARGETS= findpng2.out

all: ${TARGETS}

findpng2.out: $(OBJS3) 
	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

%.o: %.c 
	$(CC) $(CFLAGS) -c $< 

%.d: %.c
	gcc -MM -MF $@ $<

-include $(SRCS:.c=.d)

.PHONY: clean
clean:
	rm -f *~ *.d *.o $(TARGETS) *.png *.html



# CC = gcc       # compiler
# CFLAGS = -Wall -g # compilation flg 
# LD = gcc       # linker
# LDFLAGS = -g   # debugging symbols in build
# LDLIBS =       # link with libz

# # For students 
# LIB_UTIL = 
# SRCS   = stack.c stack_test.c
# OBJS1  = $(LIB_UTIL) 
# OBJS2  = stack_test.c stack.o $(LIB_UTIL) 

# TARGETS= stack_test.out

# all: ${TARGETS}

# stack_test.out: $(OBJS2) 
# 	$(LD) -o $@ $^ $(LDLIBS) $(LDFLAGS) 

# %.o: %.c 
# 	$(CC) $(CFLAGS) -c $< 

# %.d: %.c
# 	gcc -MM -MF $@ $<

# -include $(SRCS:.c=.d)

# .PHONY: clean
# clean:
# 	rm -f *.d *.o *~ $(TARGETS) 