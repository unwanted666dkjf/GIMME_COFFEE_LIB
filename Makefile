CC = gcc
CFLAGS = -Iheaders -std=gnu99 -Wall -O2 -g
LIBEXPORT_FLAGS = -DGIMME_COFFEE_EXPORTS
SRCDIR = src
LIBS =
SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:.c=.o)
TARGET = GimmeCoffee
STATIC_LIB = lib_GimmeCoffee.a
SHARED_LIB = lib_GimmeCoffee.so

.PHONY: all exe static shared clean

all: shared


exe: $(OBJS) main.o
	$(CC) $(CFLAGS) $(OBJS) main.o $(LIBS) -o $(TARGET)


static: $(OBJS)
	ar rcs $(STATIC_LIB) $(OBJS) $(LIBS)


shared: CFLAGS += -fPIC
shared: clean-objs
	$(CC) $(CFLAGS) $(LIBEXPORT_FLAGS) -fPIC -shared -o $(SHARED_LIB) $(SRCS)

%.o: %.c
	$(CC) $(CFLAGS) $(LIBEXPORT_FLAGS) -c $< -o $@


main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o


clean-objs:
	rm -f $(OBJS) main.o

clean:
	rm -f $(OBJS) main.o $(TARGET) $(STATIC_LIB) $(SHARED_LIB)
