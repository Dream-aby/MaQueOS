#include <xtos.h>

#define HBA_REGS_BASE (0x41044000UL | DMW_MASK)
#define HBA_GHC (HBA_REGS_BASE + 0x4)
#define HBA_PORT0_BASE (HBA_REGS_BASE + 0x100)
#define HBA_PORT0_CLB (HBA_PORT0_BASE + 0x0)
#define HBA_PORT0_IS (HBA_PORT0_BASE + 0x10)
#define HBA_PORT0_IE (HBA_PORT0_BASE + 0x14)
#define HBA_PORT0_CMD (HBA_PORT0_BASE + 0x18)
#define HBA_PORT0_CI (HBA_PORT0_BASE + 0x38)
#define HBA_PORT0_CL_BASE (*(unsigned long *)(HBA_PORT0_CLB) | DMW_MASK)
#define HBA_PORT0_CL_HEADER0_BASE (HBA_PORT0_CL_BASE + 0x0)
#define HBA_PORT0_CL_HEADER0_CTBA (HBA_PORT0_CL_HEADER0_BASE + 0x8)
#define HBA_PORT0_CL_HEADER0_CT_BASE (*(unsigned long *)(HBA_PORT0_CL_HEADER0_CTBA) | DMW_MASK)
#define HBA_PORT0_CL_HEADER0_CT_CFIS_BASE (HBA_PORT0_CL_HEADER0_CT_BASE + 0x0)
#define HBA_PORT0_CL_HEADER0_CT_CFIS_CMD (HBA_PORT0_CL_HEADER0_CT_CFIS_BASE + 0x2)
#define HBA_PORT0_CL_HEADER0_CT_CFIS_LBA (HBA_PORT0_CL_HEADER0_CT_CFIS_BASE + 0x4)
#define HBA_PORT0_CL_HEADER0_CT_ITEM0_BASE (HBA_PORT0_CL_HEADER0_CT_BASE + 0x80)
#define HBA_PORT0_CL_HEADER0_CT_ITEM0_DBA (HBA_PORT0_CL_HEADER0_CT_ITEM0_BASE + 0x0)
#define NR_BUFFER 16
#define BLOCK_SIZE 512
#define READ 0x25
#define WRITE 0x35
#define HBA_PORT0_CMD_ST (1UL << 0)
#define HBA_PORT0_CMD_FRE (1UL << 4)
#define HBA_GHC_IE (1UL << 1)
#define HBA_PORT0_IE_DHRE (1UL << 0)
#define HBA_PORT0_IS_DHRS (1UL << 0)

struct buffer
{
	char *data;
	short blocknr;
};
struct request
{
	int update;
	struct process *wait;
};

struct buffer buffer_table[NR_BUFFER];
struct request request;
int disk_lock = 0;
struct process *disk_wait = 0;

void lock_disk()
{
	while (disk_lock)
		sleep_on(&disk_wait);
	disk_lock = 1;
}
void unlock_disk()
{
	disk_lock = 0;
	wake_up(&disk_wait);
}
void rw_disk(unsigned short blocknr, char *buf, int rw)
{
	*(unsigned int *)(HBA_PORT0_IS) = HBA_PORT0_IS_DHRS;
	*(unsigned long *)(HBA_PORT0_CL_HEADER0_CT_ITEM0_DBA) = (unsigned long)buf & ~DMW_MASK;
	*(unsigned short *)(HBA_PORT0_CL_HEADER0_CT_CFIS_LBA) = blocknr;
	*(unsigned char *)(HBA_PORT0_CL_HEADER0_CT_CFIS_CMD) = rw;
	*(unsigned int *)(HBA_PORT0_CI) = 1;
}
void disk_interrupt()
{
	request.update = 1;
	wake_up(&request.wait);
}
void rw_disk_block(int rw, short blocknr, char *buf)
{
	request.update = 0;
	rw_disk(blocknr, buf, rw);
	if (!request.update)
		sleep_on(&request.wait);
}
struct buffer *find_buffer(short blocknr)
{
	int i;

	for (i = 0; i < NR_BUFFER; i++)
	{
		if (buffer_table[i].blocknr == blocknr)
			return &buffer_table[i];
	}
	return 0;
}
struct buffer *get_buffer(short blocknr)
{
	int i;

	for (i = 0; i < NR_BUFFER; i++)
	{
		if (buffer_table[i].blocknr == -1)
		{
			buffer_table[i].blocknr = blocknr;
			return &buffer_table[i];
		}
	}
	for (i = 0; i < NR_BUFFER; i++)
	{
		rw_disk_block(WRITE, buffer_table[i].blocknr, buffer_table[i].data);
		buffer_table[i].blocknr = -1;
	}
	buffer_table[0].blocknr = blocknr;
	return &buffer_table[0];
}
void write_block(short blocknr, char *buf)
{
	struct buffer *bf;

	lock_disk();
	bf = find_buffer(blocknr);
	if (!bf)
		bf = get_buffer(blocknr);
	copy_mem(bf->data, buf, BLOCK_SIZE);
	unlock_disk();
}
char *read_block(short blocknr)
{
	struct buffer *bf;

	lock_disk();
	bf = find_buffer(blocknr);
	if (bf)
	{
		unlock_disk();
		return bf->data;
	}
	bf = get_buffer(blocknr);
	rw_disk_block(READ, bf->blocknr, bf->data);
	unlock_disk();
	return bf->data;
}
void disk_init()
{
	char *block_data = 0;
	int i;

	for (i = 0; i < NR_BUFFER; i++, block_data += BLOCK_SIZE)
	{
		buffer_table[i].blocknr = -1;
		if (i % 8 == 0)
			block_data = (char *)get_page();
		buffer_table[i].data = block_data;
	}
	*(unsigned int *)(HBA_PORT0_CMD) |= HBA_PORT0_CMD_FRE;
	*(unsigned int *)(HBA_PORT0_CMD) |= HBA_PORT0_CMD_ST;
	*(unsigned int *)(HBA_GHC) |= HBA_GHC_IE;
	*(unsigned int *)(HBA_PORT0_IE) |= HBA_PORT0_IE_DHRE;
}
int sys_sync()
{
	int i;

	lock_disk();
	for (i = 0; i < NR_BUFFER; i++)
	{
		if (buffer_table[i].blocknr != -1)
			rw_disk_block(WRITE, buffer_table[i].blocknr, buffer_table[i].data);
	}
	unlock_disk();
	return 0;
}
int sys_write_disk(char *buffer)
{
	write_block(0, buffer);
	sys_sync();
	return 0;
}
int sys_read_disk(char *buffer)
{
	char *block;

	block = read_block(0);
	copy_mem(buffer, block, BLOCK_SIZE);
	return 0;
}