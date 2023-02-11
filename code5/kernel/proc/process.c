#include <xtos.h>

#define CSR_PGDL 0x19
#define CSR_SAVE0 0x30
#define NR_PROCESS 64
#define PROC_COUNTER 5
#define PTE_V (1UL << 0)
#define PTE_D (1UL << 1)
#define PTE_PLV (3UL << 2)

struct process *process[NR_PROCESS];
struct process *current;
char proc0_code[] = {
	0x0b, 0x00, 0x80, 0x03, 0x00, 0x00, 0x2b, 0x00, 0x80, 0x08, 0x00, 0x44, 0x00, 0x00, 0x00, 0x50,
	0x00, 0x00, 0x00, 0x50};

int sys_fork()
{
	int i;

	for (i = 1; i < NR_PROCESS; i++)
		if (!process[i])
			break;
	if (i == NR_PROCESS)
		panic("panic: process[] is empty!\n");
	process[i] = (struct process *)get_page();
	copy_mem((char *)process[i], (char *)current, PAGE_SIZE);
	process[i]->page_directory = get_page();
	copy_page_table(current, process[i]);
	process[i]->context.ra = (unsigned long)fork_ret;
	process[i]->context.sp = (unsigned long)process[i] + PAGE_SIZE;
	process[i]->context.csr_save0 = read_csr_64(CSR_SAVE0);
	process[i]->pid = i;
	process[i]->counter = PROC_COUNTER;
	return i;
}
void schedule()
{
	int pid = 0;
	int i;
	struct process *old;

	for (i = 0; i < NR_PROCESS; i++)
	{
		if (!process[i] || process[i]->counter == 0)
			continue;
		pid = process[i]->pid;
		break;
	}
	if (i == NR_PROCESS)
	{
		for (i = 0; i < NR_PROCESS; i++)
		{
			if (!process[i])
				continue;
			process[i]->counter = PROC_COUNTER;
			pid = process[i]->pid;
		}
	}
	if (current->pid == pid)
		return;
	old = current;
	current = process[pid];
	write_csr_64(current->page_directory & ~DMW_MASK, CSR_PGDL);
	invalidate();
	swtch(&old->context, &current->context);
}
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
	process[0]->counter = PROC_COUNTER;
	current = process[0];
}