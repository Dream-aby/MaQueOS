#include <xtos.h>

#define CSR_PRMD 0x1
#define CSR_ERA 0x6
#define CSR_PRMD_PPLV (3UL << 0)
#define CSR_PRMD_PIE (1UL << 2)
#define VMEM_SIZE (1UL << (9 + 9 + 12))

void main()
{
	mem_init();
	con_init();
	excp_init();
	process_init();
	int_on();
	asm volatile(
		"csrwr %0, %1\n"
		"csrwr $r0, %2\n"
		"li.d $sp, %3\n"
		"ertn\n"
		:
		: "r"(CSR_PRMD_PPLV | CSR_PRMD_PIE), "i"(CSR_PRMD), "i"(CSR_ERA), "i"(VMEM_SIZE));
}