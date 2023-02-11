#include <stdio.h>
#include <stdlib.h>

void main()
{
	FILE *fp;

	fp = fopen("xtfs.img", "r+");
	fseek(fp, 512, SEEK_SET);
	fputc(3, fp);
	fclose(fp);
}
