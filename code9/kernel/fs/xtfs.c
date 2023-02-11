#include <xtos.h>

#define NR_INODE BLOCK_SIZE / sizeof(struct inode)

struct inode inode_table[NR_INODE];
char block_map[BLOCK_SIZE];

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
int sys_mount()
{
	char *block;

	block = read_block(0);
	copy_mem((char *)inode_table, block, BLOCK_SIZE);
	block = read_block(1);
	copy_mem(block_map, block, BLOCK_SIZE);
	return 0;
}