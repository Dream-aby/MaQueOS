#include "asm.h"

	.globl start
start:
	addi.d $t0, $r0, -1
destroy:
	syscall1_a NR_destroy,file
	bne $t0, $a0, ok
	syscall1_a NR_output, str
	b exit
ok:
	syscall1_a NR_output, str1
exit:
	syscall0 NR_exit

str:
	.string "file does not exist.\n"
str1:
	.string "destroy successfully.\n"
file:
	.string "hello_xt"
