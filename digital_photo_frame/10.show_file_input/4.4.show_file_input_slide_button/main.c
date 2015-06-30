#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <config.h>
#include <draw.h>
#include <encoding_manager.h>
#include <fonts_manager.h>
#include <disp_manager.h>
#include <input_manager.h>
#include <string.h>


/* ./show_file [-s Size] [-f freetype_font_file] [-h HZK] <text_file> */
int main(int argc, char **argv)
{
	int iError;
	unsigned int dwFontSize = 16;		/* 字体大小 */
	char acHzkFile[128];
	char acFreetypeFile[128];
	char acTextFile[128];

	char acDisplay[128];

	int bList = 0;

	T_InputEvent tInputEvent;

	acHzkFile[0]  = '\0';				/* 汉字库文件名 */
	acFreetypeFile[0] = '\0';
	acTextFile[0] = '\0';

	strcpy(acDisplay, "fb");
	
	while ((iError = getopt(argc, argv, "ls:f:h:d:")) != -1)
	{
		switch(iError)
		{
			case 'l':
			{
				  bList = 1;
				  break;
			}
			case 's':
			{
				  dwFontSize = strtoul(optarg, NULL, 0);
				  break;
			}
			case 'f':
			{
				  strncpy(acFreetypeFile, optarg, 128);
				  acFreetypeFile[127] = '\0';
				  break;
			}			
			case 'h':
			{
					strncpy(acHzkFile, optarg, 128);
					acHzkFile[127] = '\0';
					break;
			}
			case 'd':
			{
				strncpy(acDisplay, optarg, 128);
				acDisplay[127] = '\0';
				break;
			}
			default:
			{
					printf("Usage: %s [-s Size] [-d display] [-f font_file] [-h HZK] <text_file>\n", argv[0]);
					printf("Usage: %s -l\n", argv[0]);
					return -1;
					break;
			}
		}
	}

	/* optind < argc说明含有不是选项的参数
	 * optind >= argc说明所有参数都是选项
	 */
	if (!bList && (optind >= argc))
	{
		printf("Usage: %s [-s Size] [-d display] [-f font_file] [-h HZK] <text_file>\n", argv[0]);
		printf("Usage: %s -l\n", argv[0]);
		return -1;
	}
		
	iError = DisplayInit();
	if (iError)
	{
		printf("DisplayInit error!\n");
		return -1;
	}

	iError = FontsInit();
	if (iError)
	{
		printf("FontsInit error!\n");
		return -1;
	}

	iError = EncodingInit();
	if (iError)
	{
		printf("EncodingInit error!\n");
		return -1;
	}

	iError = InputInit();
	if (iError)
	{
		printf("InputInit error!\n");
		return -1;
	}

	if (bList)
	{
		printf("supported display:\n");
		ShowDispOpr();

		printf("supported font:\n");
		ShowFontOpr();

		printf("supported encoding:\n");
		ShowEncodingOpr();

		printf("supported input:\n");
		ShowInputOpr();
		return 0;
	}

	strncpy(acTextFile, argv[optind], 128);
	acTextFile[127] = '\0';

	/* 打开文本文件，选择合适的编码，并把文件映射到内存 */
	iError = OpenTextFile(acTextFile);
	if (iError)
	{
		printf("OpenTextFile error!\n");
		return -1;
	}

	iError = SetFontDetail(acHzkFile, acFreetypeFile, dwFontSize);
	if (iError)
	{
		printf("SetFontDetail error!\n");
		return -1;
	}

	DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);

	iError = SelectAndInitDisplay(acDisplay);
	if (iError)
	{
		printf("SelectAndInitDisplay error!\n");
		return -1;
	}

	/* touchscreen的初始化函数中需要获得fb的分辨率，所以AllInputDevicesInit()必须在SelectAndInitDisplay()之后调用 */
	AllInputDevicesInit();
	if (iError)
	{
		DBG_PRINTF("Error to AllInputDevicesInit\n");
		return -1;
	}
	
	DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	iError = ShowNextPage();
	DBG_PRINTF("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	if (iError)
	{
		printf("Error to show first page\n");
		return -1;
	}

	printf("Enter 'n' to show next page, 'u' to show previous page, 'q' to exit: \n");

	while (1)
	{
		if(0 == GetInputEvent(&tInputEvent))
		{
			if (tInputEvent.iVal == INPUT_VALUE_DOWN)
			{
				ShowNextPage();
			}
			else if (tInputEvent.iVal == INPUT_VALUE_UP)
			{
				ShowPrePage();			
			}
			else if (tInputEvent.iVal == INPUT_VALUE_UNKNOWN)
			{
		
			}
			else 
			{
				/* 程序退出前清屏 */
				DispClearScreen(COLOR_BLACK);
				return 0;
			}	
		}

				
	}
	return 0;
}

