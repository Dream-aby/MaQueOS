#define DMW_MASK 0x9000000000000000UL
#define PAGE_SIZE 4096
#define VMEM_SIZE (1UL << (9 + 9 + 12))
#define BLOCK_SIZE 512
#define NAME_LEN 9
#define NR_PROCESS 64
#define NR_SHMEM 16
#define NR_FILE 10
#define TASK_RUNNING 0
#define TASK_UNINTERRUPTIBLE 1
#define TASK_INTERRUPTIBLE 2
#define TASK_EXIT 3
#define PTE_V (1UL << 0)
#define PTE_D (1UL << 1)
#define PTE_PLV (3UL << 2)

struct inode
{
	int size;
	short index_table_blocknr;
	char type;
	char filename[NAME_LEN];
};
struct file
{
	struct inode *inode;
	short pos_r;
	short pos_w;
};
struct context
{
	unsigned long ra, sp;
	unsigned long s0, s1, s2, s3, s4, s5, s6, s7, s8, fp;
	unsigned long csr_save0;
};
struct process
{
	int state;
	int pid;
	int counter;
	int signal_exit;
	unsigned long exe_end;
	unsigned long shmem_end;
	unsigned long page_directory;
	struct inode *executable;
	struct process *father;
	struct process *wait_next;
	struct file file_table[NR_FILE];
	struct context context;
};
struct shmem
{
	char name[NAME_LEN];
	unsigned long mem;
	int count;
};

void printk(char *);
void con_init();
void panic(char *);
void print_debug(char *, unsigned long);
void keyboard_interrupt();
int sys_output(char *);
int sys_input(char *);

void excp_init();
void int_on();
void exception_handler();
void tlb_handler();
void fork_ret();
int sys_timer(int);

void mem_init();
unsigned long get_page();
void free_page(unsigned long);
void share_page(unsigned long);
void put_page(struct process *, unsigned long, unsigned long, unsigned long);
void copy_page_table(struct process *, struct process *);
void free_page_table(struct process *);
void do_no_page();
void do_wp_page();

void process_init();
void schedule();
int sys_fork();
int sys_exit();
int sys_pause();
int sys_exe(char *, char *);
void sleep_on(struct process **);
void wake_up(struct process **);
void get_exe_page(unsigned long, unsigned long);
void free_process(struct process *);
void swtch(struct context *, struct context *);
void tell_father();
void do_signal();
void shmem_init();
int sys_shmem(char *, unsigned long *);

void disk_interrupt();
void disk_init();
char *read_block(short);
void write_block(short, char *);
void clear_block(short);
void free_block(short);
int sys_sync();

int sys_mount();
struct inode *find_inode(char *);
void read_inode_block(struct inode *, short, char *, int);
int sys_open(char *);
int sys_close(int);
int sys_read(int, char *);
int sys_create(char *);
int sys_destroy(char *);
int sys_write(int, char *);
void write_first_two_blocks();
void close_files();

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
static inline unsigned long read_csr_64(unsigned int csr)
{
	unsigned long val;

	asm volatile("csrrd %0, %1"
				 : "=r"(val)
				 : "i"(csr));
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
static inline unsigned int read_cpucfg(int cfg_num)
{
	unsigned int val;

	asm volatile("cpucfg %0, %1"
				 : "=r"(val)
				 : "r"(cfg_num));
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
static inline void copy_string(char *to, char *from)
{
	int nr = 0;

	while (from[nr++] != '\0')
		;
	copy_mem(to, from, nr);
}
static inline int match(char *str1, char *str2, int nr)
{
	for (int i = 0; i < nr; i++)
	{
		if (str1[i] != str2[i])
			return 0;
		if (str1[i] == '\0')
			return 1;
	}
	return 0;
}