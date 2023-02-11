#define CSR_PRMD 0x1
#define CSR_PGD 0x1b
#define CSR_SAVE0 0x30
#define CSR_SAVE1 0x31
#define CSR_TLBRSAVE 0x8b

.macro store_load_regs cmd
	\cmd $ra, $sp, 0x0
	\cmd $tp, $sp, 0x8
	\cmd $a0, $sp, 0x10
	\cmd $a1, $sp, 0x18
	\cmd $a2, $sp, 0x20
	\cmd $a3, $sp, 0x28
	\cmd $a4, $sp, 0x30
	\cmd $a5, $sp, 0x38
	\cmd $a6, $sp, 0x40
	\cmd $a7, $sp, 0x48
	\cmd $t0, $sp, 0x50
	\cmd $t1, $sp, 0x58
	\cmd $t2, $sp, 0x60
	\cmd $t3, $sp, 0x68
	\cmd $t4, $sp, 0x70
	\cmd $t5, $sp, 0x78
	\cmd $t6, $sp, 0x80
	\cmd $t7, $sp, 0x88
	\cmd $t8, $sp, 0x90
	\cmd $r21, $sp,0x98
	\cmd $fp, $sp, 0xa0
	\cmd $s0, $sp, 0xa8
	\cmd $s1, $sp, 0xb0
	\cmd $s2, $sp, 0xb8
	\cmd $s3, $sp, 0xc0
	\cmd $s4, $sp, 0xc8
	\cmd $s5, $sp, 0xd0
	\cmd $s6, $sp, 0xd8
	\cmd $s7, $sp, 0xe0
	\cmd $s8, $sp, 0xe8
.endm

	.globl exception_handler
	.globl tlb_handler
exception_handler:
	csrwr $t0, CSR_SAVE1
	csrrd $t0, CSR_PRMD
	andi $t0, $t0, 0x3
	beqz $t0, kernel_exception
	b user_exception

tlb_handler:
	csrwr $t0, CSR_TLBRSAVE
	csrrd $t0, CSR_PGD
	lddir $t0, $t0, 1
	ldpte $t0, 0
	ldpte $t0, 1
	tlbfill
	csrrd $t0, CSR_TLBRSAVE
	ertn

user_exception:
	csrrd $t0, CSR_SAVE1
	csrwr $sp, CSR_SAVE0
	addi.d $sp, $sp, -0xf0
	store_load_regs st.d
	bl do_exception

user_exception_ret:
	ori $t0, $r0, 0x7
	csrwr $t0, CSR_PRMD
	store_load_regs ld.d
	addi.d $sp, $sp, 0xf0
	csrwr $sp, CSR_SAVE0
	ertn

kernel_exception:
	csrrd $t0, CSR_SAVE1
	addi.d $sp, $sp, -0xf0
	store_load_regs st.d
	bl do_exception
	store_load_regs ld.d
	addi.d $sp, $sp, 0xf0
	ertn
