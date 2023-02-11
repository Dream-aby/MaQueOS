#include <xtos.h>

#define NR_PIX_X 1280
#define NR_PIX_Y 800
#define CHAR_HEIGHT 16
#define CHAR_WIDTH 8
#define NR_CHAR_X (NR_PIX_X / CHAR_WIDTH)
#define NR_CHAR_Y (NR_PIX_Y / CHAR_HEIGHT)
#define NR_BYTE_PIX 4
#define VRAM_BASE (0x40000000UL | DMW_MASK)
#define L7A_I8042_DATA (0x1fe00060UL | DMW_MASK)

extern char fonts[];
int x, y;
int sum_char_x[NR_CHAR_Y];
char digits_map[] = "0123456789abcdef";
char keys_map[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '`', 0,
	0, 0, 0, 0, 0, 'q', '1', 0, 0, 0, 'z', 's', 'a', 'w', '2', 0,
	0, 'c', 'x', 'd', 'e', '4', '3', 0, 0, 32, 'v', 'f', 't', 'r', '5', 0,
	0, 'n', 'b', 'h', 'g', 'y', '6', 0, 0, 0, 'm', 'j', 'u', '7', '8', 0,
	0, ',', 'k', 'i', 'o', '0', '9', 0, 0, '.', '/', 'l', ';', 'p', '-', 0,
	0, 0, '\'', 0, '[', '=', 0, 0, 0, 0, 13, ']', 0, '\\', 0, 0,
	0, 0, 0, 0, 0, 0, 127, 0, 0, 0, 0, 0, 0, 0, '`', 0};

void write_char(char ascii, int xx, int yy)
{
	char *font_byte;
	int row, col;
	char *pos;

	font_byte = &fonts[(ascii - 32) * CHAR_HEIGHT];
	pos = (char *)(VRAM_BASE + (yy * CHAR_HEIGHT * NR_PIX_X + xx * CHAR_WIDTH) * NR_BYTE_PIX);
	for (row = 0; row < CHAR_HEIGHT; row++, font_byte++)
	{
		for (col = 0; col < CHAR_WIDTH; col++)
		{
			if (*font_byte & (1 << (7 - col)))
			{
				*pos++ = 0;
				*pos++ = 0;
				*pos++ = 255;
				*pos++ = 0;
			}
			else
				pos += NR_BYTE_PIX;
		}
		pos += (NR_PIX_X - CHAR_WIDTH) * NR_BYTE_PIX;
	}
}
void erase_char(int xx, int yy)
{
	int row, col;
	char *pos;

	pos = (char *)(VRAM_BASE + (yy * CHAR_HEIGHT * NR_PIX_X + xx * CHAR_WIDTH) * NR_BYTE_PIX);
	for (row = 0; row < CHAR_HEIGHT; row++)
	{
		for (col = 0; col < CHAR_WIDTH; col++)
		{
			*pos++ = 0;
			*pos++ = 0;
			*pos++ = 0;
			*pos++ = 0;
		}
		pos += (NR_PIX_X - CHAR_WIDTH) * NR_BYTE_PIX;
	}
}
void scrup()
{
	int i;
	char *from, *to;

	to = (char *)VRAM_BASE;
	from = (char *)(VRAM_BASE + (CHAR_HEIGHT * NR_PIX_X * NR_BYTE_PIX));
	for (i = 0; i < (NR_PIX_Y - CHAR_HEIGHT) * NR_PIX_X * NR_BYTE_PIX; i++, to++, from++)
		*to = *from;
	for (i = 0; i < NR_CHAR_X; i++)
		erase_char(i, NR_CHAR_Y - 1);
	for (i = 0; i < NR_CHAR_Y - 1; i++)
		sum_char_x[i] = sum_char_x[i + 1];
	sum_char_x[i] = 0;
}
void cr_lf()
{
	x = 0;
	if (y < NR_CHAR_Y - 1)
		y++;
	else
		scrup();
}
void del()
{
	if (x)
	{
		x--;
		sum_char_x[y] = x;
	}
	else if (y)
	{
		sum_char_x[y] = 0;
		y--;
		x = sum_char_x[y];
	}
	erase_char(x, y);
}
void printk(char *buf)
{
	char c;
	int nr = 0;

	while (buf[nr] != '\0')
		nr++;
	erase_char(x, y);
	while (nr--)
	{
		c = *buf++;
		if (c > 31 && c < 127)
		{
			write_char(c, x, y);
			sum_char_x[y] = x;
			x++;
			if (x >= NR_CHAR_X)
				cr_lf();
		}
		else if (c == 10 || c == 13)
			cr_lf();
		else if (c == 127)
			del();
		else
			panic("panic: unsurpported char!\n");
	}
	write_char('_', x, y);
}
void panic(char *s)
{
	printk(s);
	while (1)
		;
}
void con_init()
{
	x = 0;
	y = 0;
}
void print_debug(char *str, unsigned long val)
{
	int i, j;
	char buffer[20];

	printk(str);
	buffer[0] = '0';
	buffer[1] = 'x';
	for (j = 0, i = 17; j < 16; j++, i--)
	{
		buffer[i] = (digits_map[val & 0xfUL]);
		val >>= 4;
	}
	buffer[18] = '\n';
	buffer[19] = '\0';
	printk(buffer);
}
void do_keyboard(unsigned char c)
{
	static unsigned long stack[10];
	static int index = 0;

	c = keys_map[c];
	if (c == 'a' && index < 10)
	{
		stack[index] = get_page();
		print_debug(" get a page: ", stack[index]);
		index++;
	}
	else if (c == 's' && index > 0)
	{
		index--;
		free_page(stack[index]);
		print_debug("free a page: ", stack[index]);
	}
}
void keyboard_interrupt()
{
	unsigned char c;

	c = *(volatile unsigned char *)L7A_I8042_DATA;
	if (c == 0xf0)
	{
		c = *(volatile unsigned char *)L7A_I8042_DATA;
		return;
	}
	do_keyboard(c);
}
