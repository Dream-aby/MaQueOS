#include <xtos.h>

#define CSR_DMW0 0x180
#define CSR_DMW0_PLV0 (1UL << 0)
#define MEMORY_SIZE 0x8000000
#define PAGE_SIZE 4096
#define NR_PAGE (MEMORY_SIZE >> 12)
#define KERNEL_START_PAGE (0x200000UL >> 12)
#define KERNEL_END_PAGE (0x300000UL >> 12)

char mem_map[NR_PAGE];

unsigned long get_page()
{
	unsigned long page;
	unsigned long i;

	for (i = NR_PAGE - 1; i >= 0; i--)
	{
		if (mem_map[i] != 0)
			continue;
		mem_map[i] = 1;
		page = (i << 12) | DMW_MASK;
		set_mem((char *)page, 0, PAGE_SIZE);
		return page;
	}
	panic("panic: out of memory!\n");
	return 0;
}
void free_page(unsigned long page)
{
	unsigned long i;

	i = (page & ~DMW_MASK) >> 12;
	if (!mem_map[i])
		panic("panic: try to free free page!\n");
	mem_map[i]--;
}
void mem_init()
{
	int i;

	for (i = 0; i < NR_PAGE; i++)
	{
		if (i >= KERNEL_START_PAGE && i < KERNEL_END_PAGE)
			mem_map[i] = 1;
		else
			mem_map[i] = 0;
	}
	write_csr_64(CSR_DMW0_PLV0 | DMW_MASK, CSR_DMW0);
}