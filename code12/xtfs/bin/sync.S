#include "asm.h"

	.globl start
start:
	syscall0 NR_sync
exit:
	syscall0 NR_exit

str:
	.string "\n"
