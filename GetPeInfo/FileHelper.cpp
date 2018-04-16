#include "FileHelper.h"
#include <stdio.h>
#include <tchar.h>

BOOL ReadFileBuf(TCHAR* pFilePath,char** pOutBuf,size_t *nFileSize)
{
	BOOL bRet = FALSE;
	do 
	{
		//if (!PathFileExists(pFilePath))  break;

		//FILE *pFile = _tfopen(pFilePath,_T("rb"));
		FILE *pFile = NULL;
		_tfopen_s(&pFile,pFilePath, _T("rb"));

		if (!pFile) break;

		fseek(pFile, 0, SEEK_END);
		size_t nSize = ftell(pFile);
		fseek(pFile, 0, SEEK_SET);
		char * pFilebuf = (char*)malloc(nSize + 1024); // 越界保护
		if (!pFilebuf)
		{
			fclose(pFile);
			break;
		}
		memset(pFilebuf, 0, nSize+1024);
		*nFileSize = fread(pFilebuf, 1, nSize, pFile);
		*pOutBuf = pFilebuf;
		fclose(pFile);
		bRet = TRUE;
	} while (0);
	return bRet;
}