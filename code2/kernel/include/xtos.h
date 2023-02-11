void printk(char *);
void con_init();
void panic(char *);
void print_debug(char *, unsigned long);

void excp_init();
void int_on();
void exception_handler();

static inline void write_csr_32(unsigned int val, unsigned int csr)
{
	asm volatile("csrwr %0, %1"
				 :
				 : "r"(val), "i"(csr));
}
static inline unsigned int read_csr_32(unsigned int csr)
{
	unsigned int val;

	asm volatile("csrrd %0, %1"
				 : "=r"(val)
				 : "i"(csr));
	return val;
}
static inline void write_csr_64(unsigned long val, unsigned int csr)
{
	asm volatile("csrwr %0, %1"
				 :
				 : "r"(val), "i"(csr));
}
static inline unsigned int read_cpucfg(int cfg_num)
{
	unsigned int val;

	asm volatile("cpucfg %0, %1"
				 : "=r"(val)
				 : "r"(cfg_num));
	return val;
}