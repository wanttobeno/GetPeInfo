#ifndef Pe_H__
#define  PE_H__
#include <Windows.h>


// 数据表，保存各个资源的虚拟地址和大小
typedef struct _IMAGE_DATA_DIRECTORY_ARRAY
{
	IMAGE_DATA_DIRECTORY Export;
	IMAGE_DATA_DIRECTORY Import;
	IMAGE_DATA_DIRECTORY Resource;
	IMAGE_DATA_DIRECTORY Exception;
	IMAGE_DATA_DIRECTORY Security;
	IMAGE_DATA_DIRECTORY BaseRelocationTable;
	IMAGE_DATA_DIRECTORY DebugDirectory;
	IMAGE_DATA_DIRECTORY CopyrightOrArchitectureSpecificData;
	IMAGE_DATA_DIRECTORY GlobalPtr;
	IMAGE_DATA_DIRECTORY TLSDirectory;
	IMAGE_DATA_DIRECTORY LoadConfigurationDirectory;
	IMAGE_DATA_DIRECTORY BoundImportDirectory;
	IMAGE_DATA_DIRECTORY ImportAddressTable;
	IMAGE_DATA_DIRECTORY DelayLoadImportDescriptors;
	IMAGE_DATA_DIRECTORY COMRuntimedescriptor;
	IMAGE_DATA_DIRECTORY Reserved;
}IMAGE_DATA_DIRECTORY_ARRAY, *PIMAGE_DATA_DIRECTORY_ARRAY;


struct PeSt
{
	PIMAGE_DOS_HEADER dosHeader;
	PIMAGE_NT_HEADERS  ntHeader;
	PIMAGE_FILE_HEADER fileHeader;
	PIMAGE_NT_HEADERS32 ntheader32;
	PIMAGE_NT_HEADERS64 ntheader64;
	PIMAGE_OPTIONAL_HEADER optionheader;
	PIMAGE_OPTIONAL_HEADER32 optoinHeader32;
	PIMAGE_OPTIONAL_HEADER64 optionHeader64;
	PIMAGE_SECTION_HEADER sectionHeader;
	PIMAGE_IMPORT_DESCRIPTOR importSescriptor;
	PIMAGE_DATA_DIRECTORY_ARRAY imageDataDirArray;
	PeSt()
	{
		memset(this, 0, sizeof(PeSt));
	}
};

class CPEHelper
{
public:
	CPEHelper();
	~CPEHelper();
	BOOL ReadPeFile(TCHAR * pFilenPath);

	
	PIMAGE_SECTION_HEADER GetImageSectionHeader(unsigned int nNum);
	void PrintDllInfo(BOOL bPrintApi);
	void PrinDllApi(PIMAGE_IMPORT_DESCRIPTOR importSescriptor);
public:
	// 虚拟地址转文件偏移地址
	ULONG RVA2FOA(ULONG ulRVA);
	CONST char* GetFileBuf();
	void FreeFileDataBuf();
private:
	PeSt m_pe;
	char* m_pFileDataBuf;
	BOOL  m_bI386;
	int m_numOfSection;
};








#endif // !Pe_H__
