CC = gcc
CFLAGS = -g -I.
DEPS = funct.h
OBJ = bmp.o main.o 
SRCS = main.c

# the target name of the executable
TARGET = blind

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $(TARGET) $(SRCS) $(CFLAGS)
	
clean:
	rm -f *.o blind