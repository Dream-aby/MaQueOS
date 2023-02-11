#include "asm.h"

	.globl start
start:
	addi.d $t0, $r0, -1
create:
	syscall1_a NR_create, file
	bne $t0, $a0, ok
	syscall1_a NR_output, str
	b exit
ok:
	syscall1_a NR_output, str1
exit:
	syscall0 NR_exit

str:
	.string "file exist.\n"
str1:
	.string "create successfully.\n"
file:
	.string "hello_xt"

