#include "asm.h"

	.globl start
start:
	addi.d $t0, $r0, -1
open:
	syscall1_a NR_open, file
	bne $t0, $a0, write
	syscall1_a NR_output, str
	b exit
write:
	or $t1, $a0, $r0
	syscall2_ra NR_write,$t1,buf_write
close:
	syscall1_r NR_close,$t1
exit:
	syscall0 NR_exit

str:
	.string "file does not exist.\n"
file:
	.string "hello_xt"
buf_write:
	.ascii "hello, xt!\n\0"
	.fill 500,1,0
