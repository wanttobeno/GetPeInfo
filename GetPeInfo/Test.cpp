#include <Windows.h>
#include <stdio.h>
#include <tchar.h>
#include "FileHelper.h"
#include "PEHelper.h"


int main(int agrc, char** agrv)
{
	TCHAR szPEFile[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPEFile, MAX_PATH);

	CPEHelper peHelper;
	if (peHelper.ReadPeFile(szPEFile))
	{
		peHelper.PrintDllInfo(TRUE);
	}
	else
	{
		printf("Get PE Info Failed!\n");
	}
	system("pause");
	return 0;
}