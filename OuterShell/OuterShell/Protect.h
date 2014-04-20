#ifndef _PROTTHEFILE_H_
#define _PROTTHEFILE_H_
UINT ProtTheFileProc(HWND);

/*-------------------------------------------------------------*/
/*  ProtTheFile ��     */
/*-------------------------------------------------------------*/
UINT ProtTheFile(HWND hDlg )
{
	ProtTheFileProc(hDlg);
	ISWORKING=FALSE;
	return TRUE;
}


UINT ProtTheFileProc(HWND hDlg )
{


	DWORD				    NumberOfBytesRW;
	PCHAR					MapOfSData = NULL;	

	PCHAR					pMapOfPackRes = NULL; //���ض�����Դ�Ƶ��˴�
	UINT					nNoPackResSize =NULL;

	PCHAR					m_pImportTable =NULL;//����������ʱ�Ļ���
	UINT					m_pImportTableSize=NULL; //ת����������С

	IMAGE_DOS_HEADER		dosHeader;
	IMAGE_NT_HEADERS		ntHeaders;
	UINT					nFileSize = 0;
	UINT					nSectionNum = 0;
	UINT					nImageSize = 0;
	UINT					nFileAlign = 0;
	UINT					nSectionAlign = 0;
	UINT					nHeaderSize = 0;
	UINT					nNtHeaderSize = 0;
	PIMAGE_SECTION_HEADER 	psecHeader = NULL;
	UINT					nRawDataOffset = 0;
	UINT					nRawDataSize = 0;
	UINT					nVirtualAddress = 0;
	UINT					nVirtualSize = 0;
	UINT					nIndex = 0;
	UINT                    nMapOfSDataSize =NULL; //�������ݴ�С
	DWORD					nbWritten;
	UINT					FirstResADDR =NULL;
	TCHAR					szFilebakName[MAX_PATH];						

	try
	{
		ISWORKING=TRUE;

		memset(szFilebakName,0,MAX_PATH);

		//���������ļ�

		if(IsCreateBak)
		{	
			strcpy(szFilebakName,szFilePath);
			strcat(szFilebakName,".bak");
			CopyFile(szFilePath,szFilebakName,FALSE);
		}


		//��ʼ��
		m_pImageBase  = NULL;
		m_nImageSize  = 0;
		m_pntHeaders  = NULL;
		m_psecHeader  = NULL;


		//����ռ䱣��ѹ������������Ϣ���������������ʱ��ѹ��

		m_pInfoData = NULL;
		m_pInfoSize = 0;
		m_pInfoData = new CHAR[SIZE_OF_PACKINFO];
		if (m_pInfoData == NULL)
		{
			AddLine(hDlg,"����!�ڴ治��!");
			return FALSE;
		}

		ZeroMemory(m_pInfoData, SIZE_OF_PACKINFO); 
		*(WORD *)(m_pInfoData) = 0;

		
  //�ѳ����ļ���������ʱӳ����ص���뵽�ڴ���      	
		HANDLE hFile = CreateFile(
			szFilePath,
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE, 
			NULL,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if ( hFile == INVALID_HANDLE_VALUE ) {
			 AddLine(hDlg,"����!�ļ���ʧ��!");
			 return  FALSE;
		}
		// ��DOSͷ
		ReadFile(hFile,&dosHeader, sizeof(dosHeader), &NumberOfBytesRW, NULL);

		// ��λ��PEͷ��ʼ��e_lfanew
		SetFilePointer(hFile,dosHeader.e_lfanew,  NULL,  FILE_BEGIN);
		// ����PEͷ
		ReadFile(hFile,&ntHeaders, sizeof(ntHeaders), &NumberOfBytesRW, NULL);

		// ��ȡ�ļ���С����Ϣ
		nFileSize     = GetFileSize(hFile,NULL);
		nSectionNum   = ntHeaders.FileHeader.NumberOfSections;
		nImageSize    = ntHeaders.OptionalHeader.SizeOfImage;
		nFileAlign    = ntHeaders.OptionalHeader.FileAlignment;
		nSectionAlign = ntHeaders.OptionalHeader.SectionAlignment;
		nHeaderSize   = ntHeaders.OptionalHeader.SizeOfHeaders;


		// �������ܴ��ڵ�ӳ���Сû�ж�������
		m_nImageSize = AlignSize(nImageSize, nSectionAlign);
		
		// �����ڴ����ڱ���ӳ��
		m_pImageBase = new char[m_nImageSize];
			
		if (m_pImageBase == NULL)
		{
			AddLine(hDlg,"����!�ڴ治��!");
			return FALSE;
		}

		memset(m_pImageBase, 0, m_nImageSize);//��������ڴ������ʼ��0
		//ZeroMemory(m_pImageBase, m_nImageSize);       


		// ���ȶ�λ����PE�ļ�ͷ���ڴ���
		SetFilePointer(hFile, 0, NULL,  FILE_BEGIN);
		ReadFile(hFile, m_pImageBase, nHeaderSize,&NumberOfBytesRW, NULL);

		m_pntHeaders = (PIMAGE_NT_HEADERS)((DWORD)m_pImageBase + dosHeader.e_lfanew);

		// ע: ���ڳ����ļ���IMAGE_DATA_DIRECTORY���������Զ���(��һ���ǵö���16��)��
		// �������ͨ���������õ�׼ȷ��IMAGE_NT_HEADERS�Ĵ�С
		nNtHeaderSize = sizeof(ntHeaders.FileHeader) + sizeof(ntHeaders.Signature) + ntHeaders.FileHeader.SizeOfOptionalHeader;
		m_psecHeader = (PIMAGE_SECTION_HEADER)((DWORD)m_pntHeaders + nNtHeaderSize);

		// �ȶ�SECTION���ݽ�������
		for (nIndex = 0, psecHeader = m_psecHeader; nIndex < nSectionNum; ++ nIndex, ++ psecHeader)
		{
			nRawDataSize    = psecHeader->SizeOfRawData;
			nRawDataOffset  = psecHeader->PointerToRawData;
			nVirtualAddress = psecHeader->VirtualAddress;
			nVirtualSize    = psecHeader->Misc.VirtualSize;

			// �������ܴ��ڵĶ�������
			psecHeader->SizeOfRawData    = AlignSize(nRawDataSize, nFileAlign);
			psecHeader->Misc.VirtualSize = AlignSize(nVirtualSize, nSectionAlign);

			if (nIndex == nSectionNum - 1 && psecHeader->VirtualAddress + psecHeader->SizeOfRawData > m_nImageSize)
			{
				psecHeader->SizeOfRawData = m_nImageSize - psecHeader->VirtualAddress;
			}
		}

		// ѭ�����ζ���SECTION���ݵ�ӳ���е������ַ��
		for (nIndex = 0, psecHeader = m_psecHeader; nIndex < nSectionNum; ++ nIndex, ++ psecHeader)
		{
			nRawDataSize    = psecHeader->SizeOfRawData;
			nRawDataOffset  = psecHeader->PointerToRawData;
			nVirtualAddress = psecHeader->VirtualAddress;
			nVirtualSize    = psecHeader->Misc.VirtualSize;

			// ��λ��SECTION������ʼ��
			SetFilePointer(hFile, nRawDataOffset, NULL,  FILE_BEGIN);

			// ��SECTION���ݵ�ӳ����
			ReadFile(hFile, &m_pImageBase[nVirtualAddress], nRawDataSize,&NumberOfBytesRW, NULL);
			
			if(IsDelShare)//�������鹲��
			psecHeader->Characteristics = psecHeader->Characteristics & ~IMAGE_SCN_MEM_SHARED;

		}

		psecHeader -- ;

		// �������ܴ��ڵ�ImageSizeû�ж�������
		m_pntHeaders->OptionalHeader.SizeOfImage = psecHeader->VirtualAddress + psecHeader->Misc.VirtualSize;

	//***********�������ݶ�ȡ********************/
	/*�����ַ��MapOfSData						*/
	/*��С��nMapOfSDataSize						*/
	/********************************************/
		if(IsSaveSData)
		{
			nMapOfSDataSize=nFileSize - (psecHeader->PointerToRawData + psecHeader->SizeOfRawData);
			if(nMapOfSDataSize>0)
			{
				MapOfSData = new char[nMapOfSDataSize];
				memset(MapOfSData , 0, nMapOfSDataSize);

				ReadFile(hFile, MapOfSData, nMapOfSDataSize,&NumberOfBytesRW, NULL);
				AddLine(hDlg,"�������ݶ�ȡ���.");	
			}
			else
			{
				AddLine(hDlg,"û�ж�������.");
			}
		}
	
		CloseHandle(hFile);

		AddLine(hDlg,"�ļ��������.");	
	//***********ȥ���ض�λ����******************
		// �����ǰ�ļ�����DLL�ļ�������ѡ����"�����ض�λ����"ʱ������RELOC����
		if (!(m_pntHeaders->FileHeader.Characteristics & IMAGE_FILE_DLL)  && IsNoRelocation)
		{
			ClsRelocData();
		}
	//***********�����ض�λ���ݣ���Ҫ�����DLL��******************
		if(SaveReloc())
			AddLine(hDlg,"�ض�λ����ɹ�.");

	//***********��������******************

		if(IsProtImpTable)
		{
			//Ϊ�򵥣��˴�����0xa0000�ڴ�����������������ɵ��������ṹ�ߴ�С��0xa0000��
			m_pImportTable = new char[0xa0000]; 
			if (m_pImportTable == NULL)
			{
				AddLine(hDlg,"�ڴ治��.");
				return FALSE;
			}
			ZeroMemory(m_pImportTable, 0xa0000); 
			m_pImportTableSize=MoveImpTable(m_pImportTable);

			if(m_pImportTableSize==FALSE){
				AddLine(hDlg,"���������ʱָ�����ڴ治������û�����.");
				return FALSE;
			}
		
			ClsImpTable();
			AddLine(hDlg,"�����������.");	

		}


	//***********�ƶ���ѹ����Դ******************

		if(isPackRes)
		{
			FirstResADDR = FindFirstResADDR();

			//����������ԴRT_ICON��RT_GROUP_ICON��RT_VERSION��XP���Ĵ�С�����뻺�����������ƹ���
			//pMapOfPackRes = new char[0x10000];

			nNoPackResSize   = MoveRes(0x3,0,0) + MoveRes(0x0e,0,0) + MoveRes(0x10,0,0) + MoveRes(0x18,0,0);

			pMapOfPackRes = new char[nNoPackResSize];
			if (pMapOfPackRes == NULL)
			{
				AddLine(hDlg,"�ڴ治��.");
				return FALSE;
			}
			ZeroMemory(pMapOfPackRes, nNoPackResSize); 

			UINT MoveResSize =NULL;
			MoveResSize = MoveRes(0x3 ,pMapOfPackRes,MoveResSize);//;RT_ICON	
			MoveResSize = MoveRes(0x0e,pMapOfPackRes,MoveResSize);//RT_GROUP_ICON
			MoveResSize = MoveRes(0x10,pMapOfPackRes,MoveResSize);//RT_VERSION
			MoveRes(0x18,pMapOfPackRes,MoveResSize);//XP���
			AddLine(hDlg,"��Դ�������.");				
		}

	 //***********�ϲ�����******************
		if(IsMergeSection)
			MergeSection();

	 //***********ѹ����������******************
		AddLine(hDlg,"��ʼѹ�����ݡ���");
		if(!PackFile(szFilePath,FirstResADDR,hDlg))
		{
			AddLine(hDlg,"�ļ�ѹ��ʧ��!");
			return FALSE;
		}

	//*************������ǲ�д��******
		DisposeShell(pMapOfPackRes,nNoPackResSize,m_pImportTable,m_pImportTableSize,hDlg);

		
	//*************�����������******
		if(IsClsSecName)
			ClsSectionName();

	//*************��д�ļ�ͷ******
		SetFilePointer(hPackFile,0x0,  NULL,  FILE_BEGIN);
		nHeaderSize=m_pntHeaders->OptionalHeader.SizeOfHeaders ;// ����PEͷ��С
		
		if(!WriteFile(hPackFile,(PCHAR)m_pImageBase,nHeaderSize,&nbWritten,NULL))
		{
			AddLine(hDlg,"����!�ļ�дʧ��!");
			CloseHandle(hPackFile);
			return FALSE;
		}

	//*************д��������******

		if(IsSaveSData)
		{
			SetFilePointer(hPackFile, 0x0, NULL,  FILE_END);
			WriteFile(hPackFile,MapOfSData,nMapOfSDataSize,&nbWritten, NULL);
			AddLine(hDlg,"����д�����.");	

		}
	//*************�������,����******	
		EnableWindow(GetDlgItem(hDlg,IDC_OPEN_BUTTON), TRUE);
		EnableWindow(GetDlgItem(hDlg,IDC_SET_PROPERTY), TRUE);
		
		CloseHandle(hPackFile);
		delete m_pInfoData;
		delete m_pImageBase;
		if(nMapOfSDataSize>0)
			delete MapOfSData;

		if(IsProtImpTable)
		{
			delete m_pImportTable;
			m_pImportTable = NULL;
		}

		if(isPackRes)
			pMapOfPackRes;
	}

	catch (...)
	{
	    AddLine(hDlg,"�ļ�����ʧ��!");
		return FALSE;
	}

	AddLine(hDlg,"������ɣ�ллʹ�ã�");	
	return TRUE;
}

#endif