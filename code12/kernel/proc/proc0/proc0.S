#define NR_fork 0
#define NR_output 2
#define NR_exit 3
#define NR_pause 4
#define NR_mount 5
#define NR_exe 6

.macro syscall0 A7
	ori $a7, $r0, \A7
	syscall 0
.endm
.macro syscall1_a A7, A0
	la $a0, \A0
	ori $a7, $r0, \A7
	syscall 0
.endm
.macro syscall2_aa A7, A0, A1
	la $a0, \A0
	la $a1, \A1
	ori $a7, $r0, \A7
	syscall 0
.endm

	.globl start
start:
	syscall0 NR_fork
	bnez $a0, father
child:
	syscall0 NR_mount
	syscall2_aa NR_exe, cmd, arg
	syscall1_a NR_output, str
	syscall0 NR_exit
father:
	syscall0 NR_pause
	b father

str:
	.string "xtsh does not exist!\n"
cmd:
	.string "xtsh"
arg:
	.byte 0
