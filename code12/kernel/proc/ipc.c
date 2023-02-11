#include <xtos.h>

struct shmem shmem_table[NR_SHMEM];
extern struct process *current;
extern struct process *process[NR_PROCESS];

int sys_shmem(char *name, unsigned long *u_vaddr)
{
	int i;

	for (i = 0; i < NR_SHMEM; i++)
	{
		if (match(name, shmem_table[i].name, NAME_LEN))
		{
			shmem_table[i].count++;
			break;
		}
	}
	if (i == NR_SHMEM)
	{
		for (i = 0; i < NR_SHMEM; i++)
		{
			if (shmem_table[i].count != 0)
				continue;
			shmem_table[i].count = 1;
			copy_string(shmem_table[i].name, name);
			break;
		}
		if (i == NR_SHMEM)
			panic("shmem_table[NR_SHMEM] is empty!\n");
	}
	share_page(shmem_table[i].mem);
	put_page(current, current->shmem_end, shmem_table[i].mem, PTE_PLV | PTE_D | PTE_V);
	*u_vaddr = current->shmem_end;
	current->shmem_end += PAGE_SIZE;
	return 0;
}
void do_signal()
{
	int i;

	if (current->signal_exit)
	{
		for (i = 1; i < NR_PROCESS; i++)
			if (process[i] && process[i]->father == current && process[i]->state == TASK_EXIT)
				free_process(process[i]);
		current->signal_exit = 0;
	}
}
void tell_father()
{
	current->father->signal_exit = 1;
	if (current->father->state == TASK_INTERRUPTIBLE)
		current->father->state = TASK_RUNNING;
}
void shmem_init()
{
	int i;

	for (i = 0; i < NR_SHMEM; i++)
	{
		shmem_table[i].mem = get_page();
		shmem_table[i].count = 0;
	}
}