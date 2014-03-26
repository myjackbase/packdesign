#include "stdafx.h"
#include "Protect.h"


CProtect::CProtect(void)
{
}


CProtect::~CProtect(void)
{
}

int CProtect::Init(CString filename)
{
	CString temp;
	m_FileName=filename;
	if(ISPE()!=0)
	{
		return 1;
	}
	temp = m_FileName + ".bak";
	CopyFile(m_FileName, temp, TRUE);
	return 0;
}

int CProtect::ISPE()
{
	CFile f;
	PIMAGE_DOS_HEADER pDOsHeader;
	DWORD PeSign;
	BOOL b=f.Open(m_FileName,CFile::modeRead);
	if (b==FALSE)
	{
		return 1;
	}
	pDOsHeader=(PIMAGE_DOS_HEADER)malloc(sizeof(IMAGE_DOS_HEADER));
	if (pDOsHeader==NULL)
	{
		f.Close();
		return 2;
	}
	f.Read(pDOsHeader,sizeof(IMAGE_DOS_HEADER));
	if (pDOsHeader->e_magic!=IMAGE_DOS_SIGNATURE)
	{
		free(pDOsHeader);
		f.Close();
		return 3;
	}
	f.Seek(pDOsHeader->e_lfanew,SEEK_SET);
	f.Read(&PeSign,sizeof(DWORD));
	if (PeSign!=IMAGE_NT_SIGNATURE)
	{
		free(pDOsHeader);
		f.Close();
		return 4;
	}
	return 0;
}

int CProtect::Protect()
{
	HANDLE hFile=CreateFile(m_FileName,
		GENERIC_WRITE|GENERIC_READ,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return 1;
	}
	HANDLE hMap = CreateFileMapping(hFile,NULL,PAGE_READWRITE,NULL,NULL,NULL);
	if (hMap==INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		return 2;
	}
	LPVOID lpBase=MapViewOfFile(hMap, FILE_MAP_WRITE,0,0,0);
	if (lpBase==0)
	{
		CloseHandle(hFile);
		CloseHandle(hMap);
		return 3;
	}
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)lpBase;
	PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((long)lpBase + pDosHeader->e_lfanew);
	PIMAGE_SECTION_HEADER pSecHeader=(PIMAGE_SECTION_HEADER)((long)pNtHeaders +
		sizeof(DWORD) +
		sizeof(IMAGE_FILE_HEADER)+
		(long)pNtHeaders->FileHeader.SizeOfOptionalHeader);

	pSecHeader->Characteristics |=0x80000000;

	DWORD dwCodeStart = pSecHeader->PointerToRawData + (DWORD)lpBase;
	DWORD dwCodeSize = pSecHeader->SizeOfRawData;
	for(unsigned i=0;i<dwCodeSize;i++)
	{
		*(BYTE*)(dwCodeStart+i) ^= 0x2B;
	}
	UnmapViewOfFile(lpBase);
	CloseHandle(hFile);
	CloseHandle(hMap);
	return 0;
}
