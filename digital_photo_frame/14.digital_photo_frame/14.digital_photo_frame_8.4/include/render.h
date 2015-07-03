
#ifndef _RENDER_H
#define _RENDER_H

#include "page_manager.h"
#include "pic_operation.h"

int PicZoom(PT_PixelDatas ptOriginPic, PT_PixelDatas ptZoomPic);
int PicMerge(int iX, int iY, PT_PixelDatas ptSmallPic, PT_PixelDatas ptBigPic);
void FlushVideoMemToDev(PT_VideoMem ptVideoMem);
int GetPixelDatasForIcon(char *strFileName, PT_PixelDatas ptPixelDatas);
void FreePixelDatasForIcon(PT_PixelDatas ptPixelDatas);
void ReleaseButton(PT_Layout ptLayout);
void PressButton(PT_Layout ptLayout);

#endif /* _RENDER_H */

