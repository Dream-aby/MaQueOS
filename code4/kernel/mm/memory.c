#include <xtos.h>

#define CSR_PWCL 0x1c
#define CSR_DMW0 0x180
#define CSR_DMW3 0x183
#define CSR_DMW0_PLV0 (1UL << 0)
#define MEMORY_SIZE 0x8000000
#define NR_PAGE (MEMORY_SIZE >> 12)
#define KERNEL_START_PAGE (0x200000UL >> 12)
#define KERNEL_END_PAGE (0x300000UL >> 12)
#define ENTRY_SIZE 8
#define PWCL_PTBASE 12
#define PWCL_PTWIDTH 9
#define PWCL_PDBASE 21
#define PWCL_PDWIDTH 9
#define PWCL_EWIDTH 0

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
unsigned long *get_pte(struct process *p, unsigned long u_vaddr)
{
	unsigned long pd, pt;
	unsigned long *pde, *pte;

	pd = p->page_directory;
	pde = (unsigned long *)(pd + ((u_vaddr >> 21) & 0x1ff) * ENTRY_SIZE);
	if (*pde)
		pt = *pde | DMW_MASK;
	else
	{
		pt = get_page();
		*pde = pt & ~DMW_MASK;
	}
	pte = (unsigned long *)(pt + ((u_vaddr >> 12) & 0x1ff) * ENTRY_SIZE);
	return pte;
}
void put_page(struct process *p, unsigned long u_vaddr, unsigned long k_vaddr, unsigned long attr)
{
	unsigned long *pte;

	pte = get_pte(p, u_vaddr);
	if (*pte)
		panic("panic: try to remap!\n");
	*pte = (k_vaddr & ~DMW_MASK) | attr;
	invalidate();
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
	write_csr_64(0, CSR_DMW3);
	write_csr_64((PWCL_EWIDTH << 30) | (PWCL_PDWIDTH << 15) | (PWCL_PDBASE << 10) | (PWCL_PTWIDTH << 5) | (PWCL_PTBASE << 0), CSR_PWCL);
	invalidate();
}