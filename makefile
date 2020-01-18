TARGET = generate
LIBS = 
CC = gcc
CFLAGS = -std=c99 -g -Wall -Werror -Wno-unused-variable 

.PHONY: default all clean

default: mkdirs $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@

clean:
	-rm -f *.o
	-rm -f *.tmp
	-rm -f $(TARGET)
	-rm -rf output/

mkdirs:
	mkdir -p {output/posts/direct,output/opinions}
