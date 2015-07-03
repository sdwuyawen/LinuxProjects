#include "file.h"

int MapFile(PT_FileMap ptFileMap)
{
	int iFd;
	struct stat tStat;

	/* ���ļ� */
	iFd = open(ptFileMap->strFileName, O_RDWR);
	if (iFd == -1)
	{
		printf("can't open %s\n", ptFileMap->strFileName);
		return -1;
	}

	ptFileMap->iFd = iFd;

	if(fstat(iFd, &tStat))
	{
		printf("can't get fstat\n");
		return -1;
	}

	ptFileMap->iFileSize = tStat.st_size;
	ptFileMap->pucFileMapMem = (unsigned char *)mmap(NULL , tStat.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, iFd, 0);
	if (ptFileMap->pucFileMapMem == (unsigned char *)-1)
	{
		printf("can't mmap for bmp file\n");
		return -1;
	}

	return 0;
}

void UnMapFile(PT_FileMap ptFileMap)
{
	unmap(ptFileMap->pucFileMapMem);
	close(ptFileMap->iFd);
}