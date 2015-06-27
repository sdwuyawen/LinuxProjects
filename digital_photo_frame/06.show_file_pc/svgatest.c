
#include <stdlib.h>
#include <vga.h>
#include <vgagl.h>

/* gcc -o svgatest svgatest.c -lvga -lvgagl */
#if 0
int main(void)
{
	int x, y;
	
	vga_init();
	vga_setmode(G320x200x256);
//	gl_setpalettecolor(4, 0xE7>>2, 0xDB>>2, 0xB5>>2);   /* 0xE7DBB5  */ /* ·º»ÆµÄÖ½ */
	vga_setcolor(2);

	for (x = 0; x < 320; x++)
		for (y = 0; y < 200; y++)
			vga_drawpixel(x, y);

	sleep(5);
	vga_setmode(TEXT);

	return EXIT_SUCCESS;
}
#endif


#include <stdlib.h>
#include <vga.h>
 
int main(void)
{
   vga_init();
   vga_setmode(G320x200x256);
   vga_setcolor(4);
   vga_drawpixel(10, 10);
 
   sleep(5);
   vga_setmode(TEXT);

   return EXIT_SUCCESS;
}
