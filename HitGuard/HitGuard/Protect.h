#pragma once
class CProtect
{
public:
	CProtect(void);
	~CProtect(void);

public:
	int Init(CString fileName);
	int IsPe();
	int Protect();
	int AddSection(char* SectionName,DWORD SectionSize,DWORD SectionCharacteristics);
	DWORD AlignData(DWORD dwSize, DWORD dwAlign);

private:
	HANDLE m_hFile;
	LPVOID m_lpBase;
	CString m_fileName;
	PIMAGE_DOS_HEADER m_pDosHeader;
	PIMAGE_NT_HEADERS m_pNtHeader;
	PIMAGE_SECTION_HEADER m_pSecHeader;
};
