#include "asm.h"

	.globl start
start:
	ori $t0, $r0, 3
output:
	syscall1_a NR_output, str
timer:	
	syscall1_i NR_timer,1
	addi.d $t0,$t0,-1
	bnez $t0, output
exit:
	syscall0 NR_exit

str:
	.string "hello xtos!\n"

