#include "asm.h"

	.globl start
start:
	syscall2_aa NR_shmem,share_name,share_vaddr_f
	la $t0, share_vaddr_f
	ld.d $t0, $t0, 0
	ori $t1,$r0,48
	st.d $t1, $t0, 0
	syscall0 NR_fork
	bnez $a0, father
child:
	syscall2_aa NR_shmem,share_name,share_vaddr_c
	la $t0, share_vaddr_c
	ld.d $t0, $t0, 0  
	ori $t1,$r0,49
	st.d $t1, $t0, 0 
	syscall0 NR_exit
father:
	syscall1_p NR_output, share_vaddr_f
	syscall0 NR_pause
	syscall1_p NR_output, share_vaddr_f
	syscall1_a NR_output, str
	syscall0 NR_exit

share_name:
	.string "plane"
share_vaddr_f:
	.quad 0
share_vaddr_c:
	.quad 0
str:
	.string "\n"
