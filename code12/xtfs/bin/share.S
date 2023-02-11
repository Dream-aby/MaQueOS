#include "asm.h"

	.globl start
start:
	addi.d $sp, $sp, -1
	ori $t0, $r0, 97
	st.b $t0, $sp, 0
	syscall1_r NR_output, $sp	# a
	syscall0 NR_fork
	bnez $a0, father
child:
	syscall1_r NR_output, $sp	# a
	ori $t0, $r0, 99
	st.b $t0, $sp, 0
	syscall1_r NR_output, $sp	# c
	syscall0 NR_exit
father:
#	syscall0 NR_pause		# 子进程先运行
	syscall1_r NR_output, $sp	# a
	ori $t0, $r0, 98
	st.b $t0, $sp, 0
	syscall1_r NR_output, $sp	# b
	syscall0 NR_pause		# 父进程先运行
exit:
	syscall1_a NR_output, str
	syscall0 NR_exit

str:
	.string "\n"