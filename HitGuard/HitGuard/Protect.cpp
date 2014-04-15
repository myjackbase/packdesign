#include "stdafx.h"
#include "Protect.h"
#include "shell.h"


CProtect::CProtect(void)
{
}


CProtect::~CProtect(void)
{
}

DWORD CProtect::AlignData(DWORD dwSize, DWORD dwAlign)
{
	return (dwSize + dwAlign - 1)/dwAlign * dwAlign;
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

int CProtect::AddSection(char* SectionName,DWORD SectionSize,DWORD SectionCharacteristics)
{
	int iSectionNum = m_pNtHeader->FileHeader.NumberOfSections;
	PIMAGE_SECTION_HEADER pLastSecHeader = &m_pSecHeader[iSectionNum - 1];
	PIMAGE_SECTION_HEADER pNewSecHeader = &m_pSecHeader[iSectionNum];

	DWORD dwSectionAlignment = m_pNtHeader->OptionalHeader.SectionAlignment;
	DWORD dwFileAlignment = m_pNtHeader->OptionalHeader.FileAlignment;
	DWORD dwLastVirtualSize = AlignData(pLastSecHeader->Misc.VirtualSize, dwSectionAlignment);
	DWORD dwLastSizeOfRawData = AlignData(pLastSecHeader->SizeOfRawData, dwFileAlignment);

	pNewSecHeader->VirtualAddress = pLastSecHeader->VirtualAddress + dwLastVirtualSize;
	pNewSecHeader->PointerToRawData = pLastSecHeader->PointerToRawData + dwLastSizeOfRawData;
	pNewSecHeader->Misc.VirtualSize = AlignData(SectionSize, dwSectionAlignment);
	pNewSecHeader->SizeOfRawData = AlignData(SectionSize, dwFileAlignment);
	pNewSecHeader->Characteristics = SectionCharacteristics;
	if (strlen(SectionName) < 8)
	{
		#pragma warning( disable : 4996 )
		strncpy((char*)pNewSecHeader->Name, SectionName, strlen(SectionName));
		#pragma warning( default : 4996 ) 
	}
	else
	{
		for(int n = 0;n < 8;n++)
		{
			pNewSecHeader->Name[n] = SectionName[n];
		}
	}

	m_pNtHeader->FileHeader.NumberOfSections++;

	m_pNtHeader->OptionalHeader.SizeOfImage = pNewSecHeader->VirtualAddress + pNewSecHeader->Misc.VirtualSize;

	SetFilePointer(m_hFile, AlignData(SectionSize, dwFileAlignment),NULL,FILE_END);
	SetEndOfFile(m_hFile);

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

	AddSection("jackbase", 500, 0xE0000020);
	
	DWORD dwWrite;
	int isectionNum = m_pNtHeader->FileHeader.NumberOfSections;
	SetFilePointer(m_hFile, m_pSecHeader[isectionNum-1].PointerToRawData, 0, FILE_BEGIN);

	DWORD dwStart = (DWORD)Loader_Start;
	DWORD dwEnd = (DWORD)Loader_End;
	DWORD dwDataLen = dwEnd-dwStart;

	WriteFile(m_hFile, (LPVOID)Loader_Start, dwDataLen, &dwWrite, NULL);

	DWORD dwOEP = m_pNtHeader->OptionalHeader.AddressOfEntryPoint;

	SetFilePointer(m_hFile, m_pSecHeader[isectionNum - 1].PointerToRawData + 0x131, 0, FILE_BEGIN);
	WriteFile(m_hFile, &dwOEP, sizeof(DWORD), &dwWrite, NULL);

	m_pNtHeader->OptionalHeader.AddressOfEntryPoint = m_pSecHeader[isectionNum - 1].VirtualAddress +\
		(DWORD)start -\
		(DWORD)Loader_Start;

	UnmapViewOfFile(m_lpBase);
	CloseHandle(m_hFile);
	CloseHandle(hMap);
	return 0;
}
