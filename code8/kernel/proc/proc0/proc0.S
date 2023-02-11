#define NR_fork 0
#define NR_input 1
#define NR_output 2
#define NR_exit 3
#define NR_pause 4
#define NR_read_disk 5
#define NR_write_disk 6

.macro syscall0 A7
	ori $a7, $r0, \A7
	syscall 0
.endm
.macro syscall1_a A7, A0
	la $a0, \A0
	ori $a7, $r0, \A7
	syscall 0
.endm

	.globl start
start:
	syscall0 NR_fork
	bnez $a0, father
child:
	syscall1_a NR_read_disk, read_buffer
	syscall1_a NR_output, read_buffer
input:
	syscall1_a NR_input, write_buffer
	syscall1_a NR_write_disk, write_buffer
	syscall1_a NR_read_disk, read_buffer
	syscall1_a NR_output, read_buffer
	b input
father:
	syscall0 NR_pause
	b father

read_buffer:
	.fill 512, 1, 0
write_buffer:
	.fill 512, 1, 0