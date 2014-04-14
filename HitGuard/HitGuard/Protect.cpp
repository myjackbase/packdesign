#include "stdafx.h"
#include "Protect.h"


CProtect::CProtect(void)
{
}


CProtect::~CProtect(void)
{
}

int CProtect::Init(CString fileName)
{
	CString temp;
	m_fileName = fileName;
	if(IsPe() != 0)
	{
		return 1;
	}
	temp = m_fileName + ".bak";
	CopyFile(m_fileName, temp, TRUE);

	return 0;
}

int CProtect::IsPe()
{
	CFile f;
	DWORD PeSign;
	if(!f.Open(m_fileName, CFile::modeRead))
	{
		return 1;
	}
	m_pDosHeader = (PIMAGE_DOS_HEADER)malloc(sizeof(IMAGE_DOS_HEADER));
	if(m_pDosHeader == NULL)
	{
		f.Close();
		return 2;
	}
	f.Read(m_pDosHeader, sizeof(IMAGE_DOS_HEADER));
	if(m_pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
	{
		free(m_pDosHeader);
		f.Close();
		return 3;
	}
	f.Seek(m_pDosHeader->e_lfanew, SEEK_SET);
	f.Read(&PeSign, sizeof(DWORD));
	if(PeSign != IMAGE_NT_SIGNATURE)
	{
		free(m_pDosHeader);
		f.Close();
		return 4;
	}
	return 0;
}

int CProtect::Protect()
{
	m_hFile = CreateFile(m_fileName,
		GENERIC_READ|GENERIC_WRITE,
		NULL,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if(INVALID_HANDLE_VALUE == m_hFile)
	{
		return 1;
	}

	HANDLE hMap = CreateFileMapping(m_hFile,
		NULL,
		PAGE_READWRITE,
		NULL,
		NULL,
		NULL);
	if(INVALID_HANDLE_VALUE == hMap)
	{
		CloseHandle(m_hFile);
		return 2;
	}

	m_lpBase = MapViewOfFile(hMap,
		FILE_MAP_WRITE,
		0,
		0,
		0);
	if(0 == m_lpBase)
	{
		CloseHandle(m_hFile);
		CloseHandle(hMap);
		return 3;
	}

	m_pDosHeader = (PIMAGE_DOS_HEADER)m_lpBase;
	m_pNtHeader = (PIMAGE_NT_HEADERS)((long)m_lpBase + m_pDosHeader->e_lfanew);
	m_pSecHeader = (PIMAGE_SECTION_HEADER)((long)m_pNtHeader+\
		sizeof(DWORD)+\
		sizeof(IMAGE_FILE_HEADER)+\
		(long)m_pNtHeader->FileHeader.SizeOfOptionalHeader);

	m_pSecHeader->Characteristics |= 0x80000000;

	DWORD dwCodeStart = m_pSecHeader->PointerToRawData + (DWORD)m_lpBase;
	DWORD dwCodeSize = m_pSecHeader->SizeOfRawData;
	BYTE bTmp;
	for (DWORD i = 0; i < dwCodeSize; i++)
	{
		bTmp = (BYTE)(*(DWORD *)(dwCodeStart + i));
		*(BYTE*)(dwCodeStart + i) = bTmp ^ 0x2B;
	}
	return 0;
}
