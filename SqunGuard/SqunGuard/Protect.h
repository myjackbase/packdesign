#pragma once
class CProtect
{
public:
	CProtect(void);
	~CProtect(void);
	int Init(CString filename);
	int ISPE();
	int Protect();
	CString m_FileName;
};

