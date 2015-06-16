#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/fb.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

int fd_fb;
struct fb_var_screeninfo var;	/* Current var */
struct fb_fix_screeninfo fix;	/* Current fix */
int screen_size;
unsigned char *fbmem;
unsigned int line_width;
unsigned int pixel_width;

int fd_hzk16;
struct stat hzk_stat;
unsigned char *hzkmem;

extern const unsigned char fontdata_8x16[];	/* font ascii 8x16 */

/* color : 0x00RRGGBB */
void lcd_put_pixel(int x, int y, unsigned int color)
{
	unsigned char *pen_8 = fbmem+y*line_width+x*pixel_width;
	unsigned short *pen_16;	
	unsigned int *pen_32;	

	unsigned int red, green, blue;	

	pen_16 = (unsigned short *)pen_8;
	pen_32 = (unsigned int *)pen_8;

	switch (var.bits_per_pixel)
	{
		case 8:
		{
			*pen_8 = color;
			break;
		}
		case 16:
		{
			/* 565 */
			red   = (color >> 16) & 0xff;
			green = (color >> 8) & 0xff;
			blue  = (color >> 0) & 0xff;
			color = ((red >> 3) << 11) | ((green >> 2) << 5) | (blue >> 3);
			*pen_16 = color;
			break;
		}
		case 32:
		{
			*pen_32 = color;
			break;
		}
		default:
		{
			printf("can't surport %dbpp\n", var.bits_per_pixel);
			break;
		}
	}
}

void lcd_put_ascii(int x, int y, unsigned char c)
{
	unsigned char *dots = (unsigned char *)&fontdata_8x16[c*16];
	int i, b;
	unsigned char byte;

	for (i = 0; i < 16; i++)
	{
		byte = dots[i];
		for (b = 7; b >= 0; b--)
		{
			if (byte & (1<<b))
			{
				/* show */
				lcd_put_pixel(x+7-b, y+i, 0xffffff); /* 白 */
			}
			else
			{
				/* hide */
				lcd_put_pixel(x+7-b, y+i, 0); /* 黑 */
			}
		}
	}
}

void lcd_put_chinese(int x, int y, unsigned char *str)
{
	unsigned int area  = str[0] - 0xA1;
	unsigned int where = str[1] - 0xA1;
	unsigned char *dots = hzkmem + (area * 94 + where)*32;
	unsigned char byte;

	int i, j, b;
	for (i = 0; i < 16; i++)
		for (j = 0; j < 2; j++)
		{
			byte = dots[i*2 + j];
			for (b = 7; b >=0; b--)
			{
				if (byte & (1<<b))
				{
					/* show */
					lcd_put_pixel(x+j*8+7-b, y+i, 0xffffff); /* 白 */
				}
				else
				{
					/* hide */
					lcd_put_pixel(x+j*8+7-b, y+i, 0); /* 黑 */
				}
				
			}
		}
	
}

int main(int argc, char **argv)
{
	unsigned char str[] = "中";
	

	fd_fb = open("/dev/fb0", O_RDWR);
	if (fd_fb < 0)
	{
		printf("can't open /dev/fb0\n");
		return -1;
	}

	if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &var))
	{
		printf("can't get var\n");
		return -1;
	}

	if (ioctl(fd_fb, FBIOGET_FSCREENINFO, &fix))
	{
		printf("can't get fix\n");
		return -1;
	}

	line_width  = var.xres * var.bits_per_pixel / 8;
	pixel_width = var.bits_per_pixel / 8;
	screen_size = var.xres * var.yres * var.bits_per_pixel / 8;
	fbmem = (unsigned char *)mmap(NULL , screen_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0);
	if (fbmem == (unsigned char *)-1)
	{
		printf("can't mmap\n");
		return -1;
	}

	fd_hzk16 = open("HZK16", O_RDONLY);
	if (fd_hzk16 < 0)
	{
		printf("can't open HZK16\n");
		return -1;
	}
	if(fstat(fd_hzk16, &hzk_stat))
	{
		printf("can't get fstat\n");
		return -1;
	}
	hzkmem = (unsigned char *)mmap(NULL , hzk_stat.st_size, PROT_READ, MAP_SHARED, fd_hzk16, 0);
	if (hzkmem == (unsigned char *)-1)
	{
		printf("can't mmap for hzk16\n");
		return -1;
	}

	/* 清屏: 全部设为黑色 */
	memset(fbmem, 0, screen_size);

	lcd_put_ascii(var.xres/2, var.yres/2, 'A');

	printf("chinese code: %02x %02x\n", str[0], str[1]);
	lcd_put_chinese(var.xres/2 + 8,  var.yres/2, str);

	return 0;	
}

