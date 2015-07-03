#ifndef _FILE_H
#define _FILE_H

typedef struct FileMap {
	char strFileName[128];
	int iFd;
	int iFileSize;
	unsigned char *pucFileMapMem;
}T_FileMap, *PT_FileMap;

int MapFile(PT_FileMap ptFileMap);
void UnMapFile(PT_FileMap ptFileMap);


#endif /* _FILE_H */

