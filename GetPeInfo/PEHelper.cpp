#include "PEHelper.h"
#include <stdio.h>
#include <tchar.h>
#include "FileHelper.h"


CPEHelper::CPEHelper()
:m_pFileDataBuf(NULL)
{
}

CPEHelper::~CPEHelper()
{
	this->FreeFileDataBuf();
}

ULONG CPEHelper::RVA2FOA(ULONG ulRVA)
{
	int nFileOffset = 0;
	for (int i = 0; i < m_pe.fileHeader->NumberOfSections; i++)
	{
		PIMAGE_SECTION_HEADER pSec = GetImageSectionHeader(i);
		// 判断虚拟地址是否在本区段内
		if ((ulRVA >= pSec->VirtualAddress) && (ulRVA <= pSec->VirtualAddress + pSec->SizeOfRawData))
		{
			nFileOffset = pSec->PointerToRawData + (ulRVA - pSec->VirtualAddress);
			break;
		}
	}
	return nFileOffset;
}

const char * RVA2Ptr(unsigned rva)
{
	//if ((m_pModule != NULL) && rva)
	//{
	//	return m_pModule + rva;
	//}
	//else
	//{
	//	return NULL;
	//}
	return 0;
}

//功能：得到某个API函数（导入函数）代码地址
//参数：
//pImport: PIMAGE_IMPORT_DESCRIPTOR
//pProcName: 函数名或序号

const FARPROC GetFunctionFARPROC(
	PIMAGE_IMPORT_DESCRIPTOR pImport, LPCSTR pProcName)
{
	PIMAGE_THUNK_DATA pThunk;

	pThunk = (PIMAGE_THUNK_DATA)RVA2Ptr(pImport->OriginalFirstThunk);

	for (int i = 0; pThunk->u1.Function; i++)
	{
		bool match;

		if (pThunk->u1.Ordinal & 0x80000000) // by ordinal
			match = (pThunk->u1.Ordinal & 0xFFFF) ==
			((DWORD)pProcName);
		else
			match = _stricmp(pProcName, RVA2Ptr((unsigned)
			pThunk->u1.AddressOfData) + 2) == 0;

		if (match)
			return (FARPROC)*((unsigned *)RVA2Ptr(pImport->FirstThunk) + i);
		//pImport->FirstThunk:
		//该字段是指向一32位以00结束的RVA偏移地址串，此地址串中每个地址描述一个输入函数(函数在内存中的地址的地址)，
		//它在输入表中的顺序是可变的。 
		pThunk++;
	}
	return NULL;
}

BOOL CPEHelper::ReadPeFile(TCHAR * pFilePath)
{
	BOOL bRet = FALSE;
	do 
	{
		char* pFileBuf = NULL;
		size_t nFileSize = 0;
		int nNtHeaderSize = 0;
		ReadFileBuf(pFilePath, &pFileBuf, &nFileSize);
		if (!pFileBuf) break;
		m_pFileDataBuf = pFileBuf;
		m_pe.dosHeader = (PIMAGE_DOS_HEADER)pFileBuf;
		m_pe.ntHeader = (PIMAGE_NT_HEADERS)(pFileBuf + m_pe.dosHeader->e_lfanew);
		{
			// 子集合
			DWORD dsSignature = m_pe.ntHeader->Signature;
			m_pe.fileHeader = (PIMAGE_FILE_HEADER)((char*)m_pe.ntHeader + 4);
			m_pe.fileHeader->Machine == 0x14C ? m_bI386 = TRUE : m_bI386 = FALSE;
			m_bI386 == TRUE ? nNtHeaderSize = sizeof(IMAGE_NT_HEADERS32) : nNtHeaderSize = sizeof(IMAGE_NT_HEADERS64);
			m_numOfSection = m_pe.fileHeader->NumberOfSections;
			// 最常用的信息所在
			m_pe.optionheader = (PIMAGE_OPTIONAL_HEADER)((char*)m_pe.fileHeader + sizeof(IMAGE_FILE_HEADER));
		}
		m_pe.sectionHeader = (PIMAGE_SECTION_HEADER)((char*)m_pe.ntHeader + nNtHeaderSize);
		m_pe.imageDataDirArray = (PIMAGE_DATA_DIRECTORY_ARRAY)(m_pe.optionheader->DataDirectory);
		bRet = TRUE;
	} while (0);
	return bRet;
}

PIMAGE_SECTION_HEADER CPEHelper::GetImageSectionHeader(unsigned int nNum)
{
	PIMAGE_SECTION_HEADER  pSec = NULL;
	if (nNum >= 0 && nNum < m_numOfSection)
		pSec = (PIMAGE_SECTION_HEADER)((char*)m_pe.sectionHeader + sizeof(IMAGE_SECTION_HEADER) * nNum);
	return pSec;
}

void CPEHelper::PrintDllInfo(BOOL bPrintApi)
{
	if (m_pe.imageDataDirArray->Import.Size <= 0)
		return;
	int nDllCount = 0;
	do
	{
		int nImpFileOffset = RVA2FOA(m_pe.imageDataDirArray->Import.VirtualAddress + 0x14 * nDllCount);
		PIMAGE_IMPORT_DESCRIPTOR importSescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(m_pFileDataBuf + nImpFileOffset);
		if (importSescriptor->FirstThunk == 0) break;
		if (m_pe.imageDataDirArray->Import.Size > 0)
		{
			char * pStr4 = m_pFileDataBuf + RVA2FOA(importSescriptor->Name);
			printf("%s \n", pStr4);
				if (bPrintApi)
					PrinDllApi(importSescriptor);
		}
		nDllCount++;
	} while (1);
}

void CPEHelper::PrinDllApi(PIMAGE_IMPORT_DESCRIPTOR importSescriptor)
{
	// 打印API的名字
	ULONG ulOriginalFirstThunkFOA = RVA2FOA(importSescriptor->OriginalFirstThunk);
	int nNameIndex = 0;
	ULONG ulThrunk = 0;
	while (1)
	{
		ulThrunk = *((DWORD*)((char*)m_pFileDataBuf + ulOriginalFirstThunkFOA + 4 * nNameIndex));
		if (ulThrunk == 0)
		{
			break;
		}
		if (ulThrunk & 0x80000000)
		{
			printf("ID:%5d Hint: -  Name= %08x\n", nNameIndex + 1, ulThrunk - 0x80000000);
		}
		else
		{
			PIMAGE_IMPORT_BY_NAME  pImageImportByName = (PIMAGE_IMPORT_BY_NAME)((char*)m_pFileDataBuf + RVA2FOA(ulThrunk));
			printf("ID:%5d Hint:%5d Name= %s\n", nNameIndex + 1, pImageImportByName->Hint, pImageImportByName->Name);;
		}
		nNameIndex++;
	};
}

CONST char* CPEHelper::GetFileBuf()
{
	return m_pFileDataBuf;
}

void CPEHelper::FreeFileDataBuf()
{
	if (m_pFileDataBuf)
	{
		free(m_pFileDataBuf);
		m_pFileDataBuf = NULL;
	}
}