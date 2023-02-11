#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCK_SIZE 512
#define INODE_SIZE sizeof(struct inode)
#define NR_INODE (BLOCK_SIZE / INODE_SIZE)
#define NAME_LEN 9

struct inode
{
	int size;
	short index_table_blocknr;
	char type;
	char filename[NAME_LEN];
};

struct inode inode_table[NR_INODE];
char block_map[BLOCK_SIZE];
FILE *fp_xtfs;

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
			return blocknr;
		}
	}
	printf("block_map is empty.\n");
	exit(0);
}

void write_block(FILE *fp, long int offset, char *buffer, int size)
{
	fseek(fp, offset, SEEK_SET);
	fwrite(buffer, 1, size, fp);
}
void read_first_two_blocks()
{
	fp_xtfs = fopen("xtfs.img", "r+");
	fread((char *)inode_table, 1, BLOCK_SIZE, fp_xtfs);
	fread(block_map, 1, BLOCK_SIZE, fp_xtfs);
}
int copy_blocks(char *filename, short *index_table)
{
	FILE *fp;
	int filesize;
	int i, j;
	size_t size;
	int blocknr;
	char buffer[BLOCK_SIZE];

	fp = fopen(filename, "r");
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	memset((char *)index_table, 0, BLOCK_SIZE);
	for (i = 0, j = 0; i < filesize; i += BLOCK_SIZE, j++)
	{
		blocknr = get_block();
		size = fread(buffer, 1, BLOCK_SIZE, fp);
		write_block(fp_xtfs, blocknr * BLOCK_SIZE, buffer, size);
		index_table[j] = blocknr;
	}
	fclose(fp);
	return filesize;
}
short write_index_table(char *index_table)
{
	short index_table_blocknr;

	index_table_blocknr = get_block();
	write_block(fp_xtfs, index_table_blocknr * BLOCK_SIZE, (char *)index_table, BLOCK_SIZE);
	return index_table_blocknr;
}
void get_empty_inode(char *filename, int filesize, short index_table_blocknr, char type)
{
	int i;

	for (i = 0; i < NR_INODE; i++)
	{
		if (inode_table[i].type != 0)
			continue;
		inode_table[i].size = filesize;
		inode_table[i].type = type;
		inode_table[i].index_table_blocknr = index_table_blocknr;
		strcpy(inode_table[i].filename, filename);
		return;
	}
	if (i == NR_INODE)
	{
		printf("inode_table is empty.\n");
		exit(0);
	}
}
void write_first_two_blocks()
{
	write_block(fp_xtfs, 0, (char *)inode_table, BLOCK_SIZE);
	write_block(fp_xtfs, 512, block_map, BLOCK_SIZE);
	fclose(fp_xtfs);
}
void main(int argc, char **argv)
{
	int filesize;
	short index_table_blocknr;
	short index_table[BLOCK_SIZE / 2];
	char *filename;
	char type;

	filename = argv[1];
	type = atoi(argv[2]);
	read_first_two_blocks();
	filesize = copy_blocks(filename, index_table);
	index_table_blocknr = write_index_table((char *)index_table);
	get_empty_inode(filename, filesize, index_table_blocknr, type);
	write_first_two_blocks();
}