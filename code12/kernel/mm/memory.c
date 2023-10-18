#include <xtos.h>

#define CSR_BADV 0x7
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
#define ENTRYS 512

char mem_map[NR_PAGE];
extern struct process *current;
extern struct shmem shmem_table[NR_SHMEM];

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
void share_page(unsigned long page)
{
	unsigned long i;

	i = (page & ~DMW_MASK) >> 12;
	if (!mem_map[i])
		panic("panic: try to share free page!\n");
	mem_map[i]++;
}
int is_share_page(unsigned long page)
{
	unsigned long i;

	i = (page & ~DMW_MASK) >> 12;
	if (mem_map[i] > 1)
		return 1;
	else
		return 0;
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
void free_page_table(struct process *p)
{
	unsigned long pd, pt;
	unsigned long *pde, *pte;
	unsigned long page;

	pd = p->page_directory;
	pde = (unsigned long *)pd;
	for (int i = 0; i < ENTRYS; i++, pde++)
	{
		if (*pde == 0)
			continue;
		pt = *pde | DMW_MASK;
		pte = (unsigned long *)pt;
		for (int j = 0; j < ENTRYS; j++, pte++)
		{
			if (*pte == 0)
				continue;
			page = (~0xfffUL & *pte) | DMW_MASK;
			if (is_share_page(page) && (*pte & PTE_D))
			{
				for (i = 0; i < NR_SHMEM; i++)
				{
					if (shmem_table[i].mem == page)
					{
						shmem_table[i].count--;
						break;
					}
				}
			}
			free_page(page);
			*pte = 0;
		}
		free_page(*pde | DMW_MASK);
		*pde = 0;
	}
}
void copy_page_table(struct process *from, struct process *to)
{
	unsigned long from_pd, to_pd, from_pt, to_pt;
	unsigned long *from_pde, *to_pde, *from_pte, *to_pte;
	unsigned long page;
	int i, j;

	from_pd = from->page_directory;
	from_pde = (unsigned long *)from_pd;
	to_pd = to->page_directory;
	to_pde = (unsigned long *)to_pd;
	for (i = 0; i < ENTRYS; i++, from_pde++, to_pde++)
	{
		if (*from_pde == 0)
			continue;
		from_pt = *from_pde | DMW_MASK;
		from_pte = (unsigned long *)from_pt;
		to_pt = get_page();
		to_pte = (unsigned long *)to_pt;
		*to_pde = to_pt & ~DMW_MASK;
		for (j = 0; j < ENTRYS; j++, from_pte++, to_pte++)
		{
			if (*from_pte == 0)
				continue;
			page = (~0xfffUL & *from_pte) | DMW_MASK;
			if (is_share_page(page) && (*from_pte & PTE_D))
				continue;
			share_page(page);
			*from_pte &= ~PTE_D;
			*to_pte = *from_pte;
		}
	}
	invalidate();
}
void do_wp_page()
{
	unsigned long *pte;
	unsigned long u_vaddr;
	unsigned long old_page, new_page;

	u_vaddr = read_csr_64(CSR_BADV);
	pte = get_pte(current, u_vaddr);
	old_page = (~0xfff & *pte) | DMW_MASK;
	if (is_share_page(old_page))
	{
		new_page = get_page();
		*pte = (new_page & ~DMW_MASK) | PTE_PLV | PTE_D | PTE_V;
		copy_mem((char *)new_page, (char *)old_page, PAGE_SIZE);
		free_page(old_page);
	}
	else
		*pte |= PTE_D;
	invalidate();
}
void do_no_page()
{
	unsigned long page;
	unsigned long u_vaddr;

	u_vaddr = read_csr_64(CSR_BADV);
	u_vaddr &= ~0xfffUL;
	page = get_page();
	if (u_vaddr < current->exe_end)
		get_exe_page(u_vaddr, page);
	put_page(current, u_vaddr, page, PTE_PLV | PTE_D | PTE_V);
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
	shmem_init();
}