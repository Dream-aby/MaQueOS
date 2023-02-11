#include <xtos.h>

#define CSR_PGDL 0x19
#define CSR_SAVE0 0x30
#define NR_PROCESS 64
#define PTE_V (1UL << 0)
#define PTE_D (1UL << 1)
#define PTE_PLV (3UL << 2)

struct process *process[NR_PROCESS];
struct process *current;
char proc0_code[] = {0x00, 0x00, 0x00, 0x50};

void process_init()
{
	unsigned long page;
	int i;

	for (i = 0; i < NR_PROCESS; i++)
		process[i] = 0;
	process[0] = (struct process *)get_page();
	write_csr_64((unsigned long)process[0] + PAGE_SIZE, CSR_SAVE0);
	process[0]->page_directory = get_page();
	write_csr_64(process[0]->page_directory & ~DMW_MASK, CSR_PGDL);
	page = get_page();
	copy_mem((void *)page, proc0_code, sizeof(proc0_code));
	put_page(process[0], 0, page, PTE_PLV | PTE_D | PTE_V);
	process[0]->pid = 0;
	process[0]->exe_end = PAGE_SIZE;
	current = process[0];
}