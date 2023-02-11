#include "asm.h"

	.globl start
start:
	syscall1_r NR_output, $sp
	syscall1_a NR_output, str
exit:
	syscall0 NR_exit
str:
	.string "\n"
