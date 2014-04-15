// Protect.h: interface for the CProtect class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROTECT_H__C6707A71_7A5F_43FC_BACF_93D6D69A165F__INCLUDED_)
#define AFX_PROTECT_H__C6707A71_7A5F_43FC_BACF_93D6D69A165F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProtect  
{
public:
	CProtect();
	virtual ~CProtect();

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

#endif // !defined(AFX_PROTECT_H__C6707A71_7A5F_43FC_BACF_93D6D69A165F__INCLUDED_)
