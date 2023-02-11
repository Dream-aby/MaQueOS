OBJS = \
	init/head.o \
	init/main.o \
	drv/console.o \
	drv/font.o \
	excp/exception_handler.o \
	excp/exception.o \
	mm/memory.o \
	proc/process.o \
	proc/swtch.o \
	proc/ipc.o \
	drv/disk.o \
	fs/xtfs.o

GNU=../../cross-tool/bin/loongarch64-unknown-linux-gnu-
CC = $(GNU)gcc
LD = $(GNU)ld

CFLAGS = -Wall -Werror -O -fno-omit-frame-pointer -ggdb -MD -march=loongarch64 -mabi=lp64 -ffreestanding \
-fno-common -nostdlib -Iinclude -fno-stack-protector -fno-pie -no-pie
LDFLAGS = -z max-page-size=4096 -Ttext 0x9000000000200000

.c.o:
	@$(CC) $(CFLAGS) -c -o $*.o $<
.S.o:
	@$(CC) $(CFLAGS) -c -o $*.o $<

all: kernel

kernel: $(OBJS)
	@$(LD) $(LDFLAGS) $(OBJS) -o kernel

clean: 
	@find . -name '*.o' -type f -exec rm -rf {} \;
	@find . -name '*.d' -type f -exec rm -rf {} \;
	@rm -f kernel
