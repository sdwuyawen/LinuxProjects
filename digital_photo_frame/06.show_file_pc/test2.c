/*
 * =====================================================================================
 *
 *       Filename:  test2.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/27/2015 09:13:25 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <vga.h>

int main(void)
{
	int x;
	int y;

	vga_init();
	vga_setmode(G320x200x256);
	vga_setcolor(4);

	for (x = 0; x < 100; x++)
		for (y = 0; y < 50; y++)
			vga_drawpixel(x, y);

//	vga_drawpixel(10, 10);

	sleep(5);
	vga_setmode(TEXT);

	return EXIT_SUCCESS;
}
