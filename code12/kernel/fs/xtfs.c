#include <xtos.h>

#define NR_INODE BLOCK_SIZE / sizeof(struct inode)

struct inode inode_table[NR_INODE];
char block_map[BLOCK_SIZE];
extern struct process *current;
extern struct process *process[NR_PROCESS];

short get_block()
{
	short blocknr;
	int i, j;

	for (i = 0; i < BLOCK_SIZE; i++)
	{
		if (block_map[i] == 255)
			continue;
		for (j = 0; j < 8; j++)
		{
			if ((block_map[i] & (1 << j)) != 0)
				continue;
			block_map[i] |= 1 << j;
			blocknr = i * 8 + j;
			clear_block(blocknr);
			return blocknr;
		}
	}
	panic("panic: block_map[] is empty!\n");
	return 0;
}
void put_block(short blocknr)
{
	int i, j;

	i = blocknr / 8;
	j = blocknr % 8;
	block_map[i] &= ~(1 << j);
	free_block(blocknr);
}
struct inode *find_inode(char *filename)
{
	int i;

	for (i = 0; i < NR_INODE; i++)
	{
		if (inode_table[i].type == 0)
			continue;
		if (match(filename, inode_table[i].filename, NAME_LEN))
			return &inode_table[i];
	}
	return 0;
}
void read_inode_block(struct inode *inode, short file_blocknr, char *buf, int size)
{
	char *block;
	short blocknr;
	short *index_table;

	index_table = (short *)read_block(inode->index_table_blocknr);
	blocknr = index_table[file_blocknr];
	block = read_block(blocknr);
	copy_mem(buf, block, size);
}
int sys_read(int fd, char *buf)
{
	struct file *file;
	short file_blocknr;

	file = &current->file_table[fd];
	file_blocknr = file->pos_r++;
	read_inode_block(file->inode, file_blocknr, buf, BLOCK_SIZE);
	return 0;
}
int sys_write(int fd, char *buf)
{
	struct file *file;
	short file_blocknr;
	short blocknr;
	short *index_table;

	file = &current->file_table[fd];
	file_blocknr = file->pos_w++;
	index_table = (short *)read_block(file->inode->index_table_blocknr);
	blocknr = index_table[file_blocknr];
	if (blocknr == 0)
	{
		blocknr = get_block();
		index_table[file_blocknr] = blocknr;
	}
	write_block(blocknr, buf);
	file->inode->size += BLOCK_SIZE;
	return 0;
}
int sys_create(char *filename)
{
	int i;

	if (find_inode(filename))
		return -1;
	for (i = 0; i < NR_INODE; i++)
		if (inode_table[i].type == 0)
			break;
	if (i == NR_INODE)
		panic("panic: inode_table[] is empty!\n");
	inode_table[i].type = 2;
	inode_table[i].index_table_blocknr = get_block();
	inode_table[i].size = 0;
	copy_string(inode_table[i].filename, filename);
	return 0;
}
int sys_destroy(char *filename)
{
	int i, j;
	struct inode *inode;
	short *index_table;

	inode = find_inode(filename);
	if (!inode)
		return -1;
	for (i = 1; i < NR_PROCESS; i++)
	{
		if (process[i] == 0)
			continue;
		if (process[i]->executable == inode)
			panic("panic: can not destroy opened executable!\n");
		for (j = 0; j < NR_FILE; j++)
		{
			if (process[i]->file_table[j].inode == inode)
				panic("panic: can not destroy opened file!\n");
		}
	}
	index_table = (short *)read_block(inode->index_table_blocknr);
	for (i = 0; i < 256; i++)
	{
		if (index_table[i] == 0)
			break;
		put_block(index_table[i]);
	}
	put_block(inode->index_table_blocknr);
	inode->type = 0;
	return 0;
}
int sys_open(char *filename)
{
	int i;
	struct inode *inode;

	inode = find_inode(filename);
	if (!inode)
		return -1;
	if (inode->type != 2)
		panic("panic: wrong type!\n");
	for (i = 0; i < NR_FILE; i++)
		if (current->file_table[i].inode == 0)
			break;
	if (i == NR_FILE)
		panic("panic: current->file_table[] is empty!\n");
	current->file_table[i].inode = inode;
	current->file_table[i].pos_r = 0;
	current->file_table[i].pos_w = 0;
	return i;
}
int sys_close(int i)
{
	current->file_table[i].inode = 0;
	current->file_table[i].pos_r = 0;
	current->file_table[i].pos_w = 0;
	return 0;
}
int sys_mount()
{
	char *block;

	block = read_block(0);
	copy_mem((char *)inode_table, block, BLOCK_SIZE);
	block = read_block(1);
	copy_mem(block_map, block, BLOCK_SIZE);
	return 0;
}
void close_files()
{
	int i;

	for (i = 0; i < NR_FILE; i++)
	{
		if (current->file_table[i].inode)
			sys_close(i);
	}
}
void write_first_two_blocks()
{
	write_block(0, (char *)inode_table);
	write_block(1, block_map);
}
