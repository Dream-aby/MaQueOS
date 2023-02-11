#include <xtos.h>

void main()
{
	con_init();
	excp_init();
	int_on();
	while (1)
		;
}