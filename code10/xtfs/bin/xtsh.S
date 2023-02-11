#include "asm.h"

	.globl start
start:
	syscall1_a NR_output, str
	la $t0, cmd
read:
	syscall1_r NR_input, $t0
	ld.b $t1, $t0, 0
	li.d $t2, 13			
	beq $t1, $t2, fork
	li.d $t2, 127			
	beq $t1, $t2, delete
	st.b $r0, $t0, 1
	syscall1_r NR_output, $t0	
	addi.d $t0, $t0, 1
	b read      
delete:
	la $t1, cmd
	beq $t1, $t0, read
	st.b $r0, $t0, 1
	syscall1_r NR_output, $t0	
	addi.d $t0, $t0, -1
	st.b $r0, $t0, 0
	b read 
fork:
	st.b $r0, $t0, 1
	syscall1_r NR_output, $t0	
	st.b $r0, $t0, 0
	la $t0, cmd				
	ld.b $t0, $t0, 0 
	beqz $t0, start
	syscall0 NR_fork
	bnez $a0, pause
exe:
	la $t0, cmd
loop: 
	ld.b $t1, $t0, 0
	li.d $t2, 32			
	beq $t1, $t2, space
	li.d $t2, 0				
	beq $t1, $t2, zero
	addi.d $t0, $t0, 1
	b loop
space:
	st.b $r0, $t0, 0
	addi.d $t0, $t0, 1
zero:
	syscall2_ar NR_exe, cmd, $t0
	syscall1_a NR_output, str1
	syscall0 NR_exit
pause:
	syscall0 NR_pause
	b start

str:
	.string "xtsh# "
str1:
	.string "no such cmd!\n"
cmd:
	.fill 1024,1,0
