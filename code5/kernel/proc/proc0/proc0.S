#define NR_fork 0

.macro syscall0 A7
	ori $a7, $r0, \A7
	syscall 0
.endm

	.globl start
start:
	syscall0 NR_fork
	bnez $a0, father
child:
	b child
father:
	b father
