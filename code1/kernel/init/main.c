#include <xtos.h>

void main()
{
	con_init();
	printk("hello, world.\n");
	while (1)
		;
}