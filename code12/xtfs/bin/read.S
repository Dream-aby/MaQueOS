#include "asm.h"

	.globl start
start:
	addi.d $t0, $r0, -1
open:
	syscall1_a NR_open, file
	bne $t0, $a0, read
	syscall1_a NR_output, str
	b exit
read:
	or $t1, $a0, $r0
	syscall2_ra NR_read,$t1,buf_read
close:
	syscall1_r NR_close,$t1
print:
	syscall1_a NR_output, buf_read
exit:
	syscall0 NR_exit

str:
	.string "file does not exist.\n"
file:
	.string "hello_xt"
buf_read:
	.fill 512,1,0
