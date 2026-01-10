CC      = gcc
LD      = ld
AS      = gcc
OBJCOPY = objcopy

CFLAGS = -m32 -ffreestanding -fno-stack-protector -fno-pic \
         -fno-builtin -nostdlib -nostdinc -Wall -Wextra -O2

ASFLAGS = -m32 -ffreestanding -c
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

KERNEL_DIR = kernel

C_SOURCES := \
$(KERNEL_DIR)/core/init.c \
$(KERNEL_DIR)/core/panic.c \
$(KERNEL_DIR)/core/log.c \
$(KERNEL_DIR)/drivers/*.c \
$(KERNEL_DIR)/fs/*.c \
$(KERNEL_DIR)/int/*.c \
$(KERNEL_DIR)/lib/*.c \
$(KERNEL_DIR)/mm/*.c \
$(KERNEL_DIR)/sched/*.c \
$(KERNEL_DIR)/exe/elf.c \
$(KERNEL_DIR)/exe/exec.c

ASM_SOURCES := \
$(KERNEL_DIR)/core/entry.s \
$(KERNEL_DIR)/core/switch.S \
$(KERNEL_DIR)/int/isr.asm \
$(KERNEL_DIR)/exe/exec_enter_user.S


C_OBJS   := $(C_SOURCES:.c=.o)
ASM_OBJS := $(ASM_SOURCES:.s=.o)
ASM_OBJS := $(ASM_OBJS:.S=.o)
ASM_OBJS := $(ASM_OBJS:.asm=.o)

OBJS := $(C_OBJS) $(ASM_OBJS)

KERNEL_ELF = system44.elf
KERNEL_BIN = system44.bin

all: $(KERNEL_BIN)

$(KERNEL_ELF): $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

$(KERNEL_BIN): $(KERNEL_ELF)
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.S
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.asm
	nasm -f elf32 $< -o $@

clean:
	rm -f $(OBJS) $(KERNEL_ELF) $(KERNEL_BIN)

.PHONY: all clean
