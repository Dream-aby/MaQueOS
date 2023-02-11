#define DMW_MASK 0x9000000000000000UL
#define PAGE_SIZE 4096

struct process
{
	int pid;
	unsigned long exe_end;
	unsigned long page_directory;
};

void printk(char *);
void con_init();
void panic(char *);
void print_debug(char *, unsigned long);
void keyboard_interrupt();

void excp_init();
void int_on();
void exception_handler();
void tlb_handler();

void mem_init();
unsigned long get_page();
void free_page(unsigned long);
void put_page(struct process *, unsigned long, unsigned long, unsigned long);

void process_init();

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
static inline void write_iocsr(unsigned long val, unsigned long reg)
{
	asm volatile("iocsrwr.d %0, %1"
				 :
				 : "r"(val), "r"(reg));
}
static inline unsigned long read_iocsr(unsigned long reg)
{
	unsigned long val;

	asm volatile("iocsrrd.d %0, %1"
				 : "=r"(val)
				 : "r"(reg));
	return val;
}
static inline void invalidate()
{
	asm volatile("invtlb 0x0,$r0,$r0");
}
static inline void set_mem(char *to, int c, int nr)
{
	for (int i = 0; i < nr; i++)
		to[i] = c;
}
static inline void copy_mem(char *to, char *from, int nr)
{
	for (int i = 0; i < nr; i++)
		to[i] = from[i];
}