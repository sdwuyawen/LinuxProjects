
#ifndef _PIC_OPERATION_H
#define _PIC_OPERATION_H

typedef struct PixelDatas {
	int iWidth;
	int iHeight;
	int iBpp;
	int iLineBytes;
	int iTotalBytes;
	unsigned char *aucPixelDatas;
}T_PixelDatas, *PT_PixelDatas;


typedef struct PicFileParser {
	char *name;
	int (*isSupport)(unsigned char *aFileHead);
	int (*GetPixelDatas)(unsigned char *aFileHead, PT_PixelDatas ptPixelDatas);
	int (*FreePixelDatas)(PT_PixelDatas ptPixelDatas);
}T_PicFileParser, *PT_PicFileParser;

#endif /* _PIC_OPERATION_H */


