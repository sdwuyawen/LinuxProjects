/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  06/26/2015 10:12:46 PM
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

int main(void)
{
	printf("%s %d\n", __FUNCTION__, __LINE__);
	vga_init();
	printf("%s %d\n", __FUNCTION__, __LINE__);
	vga_setmode(G320x200x256);
	printf("%s %d\n", __FUNCTION__, __LINE__);
	vga_setcolor(4);
	printf("%s %d\n", __FUNCTION__, __LINE__);
	vga_drawpixel(10, 10);
	printf("%s %d\n", __FUNCTION__, __LINE__);

	sleep(5);
	vga_setmode(TEXT);
	printf("%s %d\n", __FUNCTION__, __LINE__);

	return EXIT_SUCCESS;
}
