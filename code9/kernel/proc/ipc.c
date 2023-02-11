#include <xtos.h>

extern struct process *current;
extern struct process *process[NR_PROCESS];

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
