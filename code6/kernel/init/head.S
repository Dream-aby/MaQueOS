
	.global _start
_start:
	la $t0, go
	jirl $r0, $t0, 0
go:
	la $sp, kernel_init_stack
	b main

	.fill 4096,1,0
kernel_init_stack:

