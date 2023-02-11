	.global _start
_start:
	la $sp, kernel_init_stack
	b main

	.fill 4096,1,0
kernel_init_stack:
