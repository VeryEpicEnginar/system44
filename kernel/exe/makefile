CC = gcc
LD = ld

CFLAGS = -m32 \
         -ffreestanding \
         -fno-pie \
         -fno-stack-protector \
         -Wall -Wextra -O2 \
         -I. -I../ -I../mm -I../lib

LDFLAGS = -m elf_i386

SRCS = elf.c
OBJS = elf.o

TARGET = exe.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJS)

elf.o: elf.c elf.h
	$(CC) $(CFLAGS) -c elf.c -o elf.o

clean:
	rm -f $(OBJS) $(TARGET)
