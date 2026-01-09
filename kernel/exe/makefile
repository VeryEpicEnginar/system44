CC = gcc
LD = ld

CFLAGS = -m32 \
         -ffreestanding \
         -fno-pie \
         -fno-stack-protector \
         -Wall -Wextra -O2 \
         -I. -I../ -I../mm -I../lib

LDFLAGS = -m elf_i386

SRCS = elf.c exec.c uex.c
OBJS = $(SRCS:.c=.o)

TARGET = exe.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(LD) -r $(LDFLAGS) -o $@ $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
