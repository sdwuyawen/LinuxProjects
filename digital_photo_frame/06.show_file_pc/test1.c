/*
 * =====================================================================================
 *
 *       Filename:  test1.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/27/2015 08:56:15 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <vga.h>
#include <vgagl.h>

int main(void)
{
	int x, y;
	
	vga_init();
	vga_setmode(G320x200x256);
	
	gl_setpalettecolor(4, 0xE7>>2, 0xDB>>2, 0xB5>>2);    // 设置要显示的颜色
	vga_setcolor(4);
	for (x = 0; x < 160; x++)
		for (y = 0; y < 100; y++)
			vga_drawpixel(x, y);
			
	sleep(5);
	vga_setmode(TEXT);
	
	return EXIT_SUCCESS;
}
