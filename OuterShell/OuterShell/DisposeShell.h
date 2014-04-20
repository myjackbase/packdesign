#ifndef _DISPOSESHELL_H_
#define _DISPOSESHELL_H_

/*-------------------------------------------------------------*/
/*  DisposeShell--��ʼ������ǲ��ֵ�����   

���������壺
pMapOfPackRes:      ͼ��Ȳ���ѹ������Դ�µ�ַ
nNoPackResSize:     ��Щ��Դ��С
m_pImportTable:     ���������ĵ�ַ
m_pImportTableSize�����������Ĵ�С
/*-------------------------------------------------------------*/

BOOL DisposeShell(PCHAR pMapOfPackRes,UINT nNoPackResSize,PCHAR m_pImportTable,UINT m_pImportTableSize ,HWND hDlg)
{

	PIMAGE_SECTION_HEADER	plastsecHeader = NULL;
	UINT					nBasePoint = 0;
	PIMAGE_DATA_DIRECTORY	pImportDir = NULL;
	UINT					nImportTableOffset=NULL;//Shell.asm��ImportTableBeginƫ��ֵ

	PIMAGE_SECTION_HEADER	psecHeader = m_psecHeader;
	PIMAGE_DATA_DIRECTORY	pTlsDir = NULL;
    PIMAGE_TLS_DIRECTORY	pTlsDirectory = NULL;


	PCHAR					mShell_pData= NULL;//�������ĵ�ַ�����������κ͵ڶ��Σ�
	UINT					mShell_nSize = NULL;//��ǳߴ�

	PDWORD					pShell0Data = NULL;
	DWORD					mShell0_nSize = NULL;//��������εĳߴ�

 	PCHAR					mShell1_pData= NULL;//��������οռ�
	UINT					mShell1_nSize = NULL;//��������γߴ�

	PDWORD					pShellData = NULL;

	
	UINT					nRawSize = 0;
	UINT					nVirtualSize = 0;
	UINT					nFileAlign = 0;
	UINT					nSectionAlign = 0;
	DWORD                   nbWritten;

	/************************************************************************/
	/* 
	/* -------------- <-ShellStart0
	/* |  �������  |
	/* -------------- <-ShellEnd0
	/* |  ͼ�����Դ| <------------pMapOfPackRes
	/* -------------- <-ShellStart����������ѹ����
	/* |  ��ǵڶ���|
	/* -------------- <-ShellEnd
	/* |  ���������|
	/* --------------
	/************************************************************************/
	try
	{

        // �ȴ�����ǵڶ���
		mShell1_nSize =(DWORD) (&ShellEnd) - (DWORD)(&ShellStart);//������ǵڶ��γߴ�

		mShell1_pData = new CHAR[mShell1_nSize + m_pImportTableSize];//�����С����ǵڶ��Σ�����������С

		if (mShell1_pData == NULL)
		{
			AddLine(hDlg,"����!�ڴ治��!");
			return FALSE;
		}
		ZeroMemory(mShell1_pData, mShell1_nSize+ m_pImportTableSize); 
		
		memcpy(mShell1_pData, (&ShellStart), mShell1_nSize);//����ǵڶ��ζ��뻺��
		
		//�ɼӿǳ��������ı���(��ǵڶ���)
		/************************************************************************/
		/* shell.asm�еı���
		SHELL_DATA_1		LABEL	DWORD
		0 OEP			    DD	0
		1 S_IsProtImpTable	DD	0
		2 ImpTableAddr		DD	0
		3 S_FileIsDll		DD	0
		4 S_RelocADDR		DD	0
		5 S_PeImageBase     DD	0  ;ԭʼӳ���ַ
		6 S_PackSection		DB	0a0h dup (?)
		/************************************************************************/

		pShellData=(PDWORD)((DWORD)(&SHELL_DATA_1)-(DWORD)(&ShellStart)+mShell1_pData);//ָ�򻺳�����shell.asm�ı���	
		pShellData[0] =m_pntHeaders->OptionalHeader.AddressOfEntryPoint;//��ԭʼ��ڵ㱣�浽�����		
		pShellData[1] =IsProtImpTable;//���Ƿ��������ı�־�ŵ������

		if(IsProtImpTable)
		{		
			memcpy(mShell1_pData+mShell1_nSize,m_pImportTable,m_pImportTableSize);
			pShellData[2]	= mShell1_nSize;//�����������ĵ�ַ���˴��������ǵڶ���ƫ�Ƶ�ַ������ 
			
		}
		else//��ԭʼ������ַ���浽�����	
		{
			pImportDir		= &m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
			pShellData[2]	= pImportDir->VirtualAddress;		
		}

		//���Ƿ�ΪDLL�ı�־�Ž������
		if(m_pntHeaders->FileHeader.Characteristics & IMAGE_FILE_DLL)
			pShellData[3] = 1;

		//���ض�λ��ַ�Ž������
		pShellData[4] = m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress ;

		//����ԭʼӳ���ַ�������
		pShellData[5] = m_pntHeaders->OptionalHeader.ImageBase;

		//����ѹ�������Ϣ�������		
		memcpy((PCHAR)((PBYTE)(&pShellData[6])), m_pInfoData, m_pInfoSize);

		//����ǵڶ��μ�������������ѹ��
		//ѹ����ĵ�ַ 	  m_pPackData 
		//ѹ�������ݴ�С  m_nPackSize; 
		//mShell1_nSize+ m_pImportTableSize����ǵڶ��Σ�����������С
		PackData(mShell1_pData, (mShell1_nSize+ m_pImportTableSize),hDlg);


		/******����������Ƕ�*******/



		mShell_nSize =(DWORD) (&ShellEnd0) - (DWORD)(&ShellStart0) + m_nPackSize + nNoPackResSize;//�������������Ҫ�ߴ�

		mShell_pData = new CHAR[mShell_nSize];

		if (mShell_pData == NULL)
		{
			AddLine(hDlg,"����!�ڴ治��!");
			return FALSE;
		}
		ZeroMemory(mShell_pData, mShell_nSize); 

		mShell0_nSize    = (DWORD) (&ShellEnd0) - (DWORD)(&ShellStart0);
		memcpy(mShell_pData, &ShellStart0, mShell0_nSize);//����������ζ��뻺��

		if(isPackRes)
		{
			memcpy((mShell_pData+mShell0_nSize),pMapOfPackRes,nNoPackResSize);//������ѹ������Դ�����ȣ����뻺��	
		}
		
		//��ѹ��������ݶ��뻺��(������ǵ���������ڶ��κϲ�)
		memcpy((mShell_pData+mShell0_nSize +nNoPackResSize), m_pPackData, m_nPackSize);


		/***������������***/
		PIMAGE_IMPORT_DESCRIPTOR	pImportDescriptor = NULL;
		PIMAGE_IMPORT_DESCRIPTOR	pDescriptor = NULL;
		UINT						nFirstThunk = 0;
		PIMAGE_THUNK_DATA32			pFirstThunk = NULL;
		UINT						nITRVA = 0;

	    psecHeader     = psecHeader + m_pntHeaders->FileHeader.NumberOfSections;//ָ��������β��
		plastsecHeader = psecHeader - 1;
		nBasePoint     = plastsecHeader->VirtualAddress + plastsecHeader->Misc.VirtualSize;//���������

		nImportTableOffset=(DWORD)(&ImportTableBegin)-(DWORD)(&ShellStart0);//ImportTableBegin�����������ƫ��ֵ

		pImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(mShell_pData+nImportTableOffset);//ָ����������
		nITRVA = nBasePoint +nImportTableOffset;//У���Ĳ���

		for (pDescriptor = pImportDescriptor; pDescriptor->FirstThunk != 0; pDescriptor ++)
		{
			pDescriptor->OriginalFirstThunk += nITRVA;
			pDescriptor->Name += nITRVA;
			nFirstThunk = pDescriptor->FirstThunk;
			pDescriptor->FirstThunk = nFirstThunk + nITRVA;

			pFirstThunk = ( PIMAGE_THUNK_DATA32)(&(mShell_pData+nImportTableOffset)[nFirstThunk]);

			while (pFirstThunk->u1.AddressOfData != 0)
			{
				nFirstThunk = pFirstThunk->u1.Ordinal;
				pFirstThunk->u1.Ordinal = nFirstThunk + nITRVA;
				pFirstThunk ++;
			}
	
		}

		//�ɼӿǳ��������ı���(���������)
		/************************************************************************/
		/* shell.asm�еı���
		SHELL_DATA_0		LABEL	DWORD
		0 ShellBase			DD	0
		1 ShellPackSize		DD	0
		2 TlsTable			DB	18h dup (?)

		  ImportTableEnd:
		0 ShellBase			DD	0 //��ǲ������ ��ShellStart0ƫ��
		1 ShellPackSize		DD	0 //���ѹ�����ֵ�ԭʼ��С+����������С
		2 Virtualalloc		DB	'VirtualAlloc',0
		3 VirtualallocADDR	DD	0
		4 TlsTable			DB	18h dup (?)
		/************************************************************************/

		pShell0Data=(PDWORD)((DWORD)(&SHELL_DATA_0)-(DWORD)(&ShellStart0)+mShell_pData);//ָ�򻺳�����shell.asm�ı���	
		pShell0Data[0] = mShell0_nSize + nNoPackResSize;
		pShell0Data[1] = mShell1_nSize + m_pImportTableSize;


		// ���ԭ����TLS���ݣ�������

		pTlsDir =&m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS];
		
		if (pTlsDir->VirtualAddress != NULL)
		{
			PDWORD		pShellTlsTable = NULL;
			pTlsDirectory = (PIMAGE_TLS_DIRECTORY32)RVAToPtr(pTlsDir->VirtualAddress);
			memcpy((PCHAR)(&pShell0Data[2]), pTlsDirectory, sizeof(IMAGE_TLS_DIRECTORY));
		
			m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress =nBasePoint + (DWORD)(&TlsTable)-(DWORD)(&ShellStart0);
			m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size = sizeof(IMAGE_TLS_DIRECTORY32);
						
		}



		nFileAlign    = m_pntHeaders->OptionalHeader.FileAlignment;
		nSectionAlign = m_pntHeaders->OptionalHeader.SectionAlignment;


		nRawSize     = AlignSize(mShell_nSize, nFileAlign);
		nVirtualSize = AlignSize(mShell_nSize, nSectionAlign);

		// ������������Ϣ
		memset(psecHeader, 0, sizeof(IMAGE_SECTION_HEADER));
		memcpy(psecHeader->Name, ".shell", 6);
		psecHeader->PointerToRawData = plastsecHeader->PointerToRawData + plastsecHeader->SizeOfRawData;
		psecHeader->SizeOfRawData    = nRawSize;
		psecHeader->VirtualAddress   = plastsecHeader->VirtualAddress + plastsecHeader->Misc.VirtualSize;
		psecHeader->Misc.VirtualSize = nVirtualSize;
		psecHeader->Characteristics  = 0xE0000040;


		// �޸��ļ�ͷ
		m_pntHeaders->FileHeader.NumberOfSections ++;
		m_pntHeaders->OptionalHeader.CheckSum = 0;
		m_pntHeaders->OptionalHeader.SizeOfImage = psecHeader->VirtualAddress + psecHeader->Misc.VirtualSize;
		m_pntHeaders->OptionalHeader.AddressOfEntryPoint = nBasePoint;
		m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress = nBasePoint + nImportTableOffset;
		m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size = (DWORD)(&ImportTableEnd)-(DWORD)(&ImportTableBegin);
		m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].VirtualAddress = 0;
		m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT].Size = 0;
		m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].VirtualAddress = 0;
		m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG].Size = 0;
		m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].VirtualAddress = 0;
		m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT].Size = 0;
		m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].VirtualAddress = 0;
		m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG].Size = 0;

		//IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)
		m_pntHeaders->OptionalHeader.DataDirectory[7].VirtualAddress = 0;
		m_pntHeaders->OptionalHeader.DataDirectory[7].Size = 0;

		//����DLL������ǵ��ض�λָ�����鹹���ض�λ��
		if(m_pntHeaders->FileHeader.Characteristics & IMAGE_FILE_DLL)
		{
			m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress = nBasePoint + (DWORD)(&RelocBaseBegin)-(DWORD)(&ShellStart0);
			m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size = 0x08;
		}

		if(!WriteFile(hPackFile,(PCHAR)mShell_pData,mShell_nSize,&nbWritten,NULL))// ����ǲ���д���ļ�
		{
			AddLine(hDlg,"����!�ļ�дʧ��!");
			CloseHandle(hPackFile);
			return FALSE;
		}
		// д��Ϊ���������������
		if (nRawSize - mShell_nSize > 0)
		{
			FillZero(hPackFile, nRawSize - mShell_nSize);
		}

		delete[] mShell1_pData;		
		delete[] mShell_pData;


	}
	catch (...)
	{
		return FALSE;
	}

	return TRUE;
}

#endif