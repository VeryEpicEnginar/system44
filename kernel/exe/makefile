CC      = gcc
AS      = gcc
LD      = ld

CFLAGS  = -m32 -ffreestanding -fno-stack-protector -fno-pic \
          -nostdlib -nostdinc -Wall -Wextra -O2

ASFLAGS = -m32 -ffreestanding

SRCS = elf.c exec.c exec_enter_user.S
OBJS = elf.o exec.o exec_enter_user.o

all: $(OBJS)

elf.o: elf.c elf.h
	$(CC) $(CFLAGS) -c elf.c -o elf.o

exec.o: exec.c exec.h elf.h
	$(CC) $(CFLAGS) -c exec.c -o exec.o

exec_enter_user.o: exec_enter_user.S
	$(AS) $(ASFLAGS) -c exec_enter_user.S -o exec_enter_user.o

clean:
	rm -f $(OBJS)
