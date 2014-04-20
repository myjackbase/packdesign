#ifndef _PACKFILE_H_
#define _PACKFILE_H_

/*-------------------------------------------------------------*/
/*  pack_callback�� ����������                                 */
/*-------------------------------------------------------------*/
int __cdecl pack_callback(UINT nSourcePos, UINT nDestinationPos,UINT nOutputbytes ,void *cbparam )
{
	UINT nRadio = 100-((nSourcePos - nDestinationPos)) * 100 /nSourcePos;

	SendMessage(hProgress, PBM_SETPOS, nRadio, 0);
		return 1;
	
}

/*-------------------------------------------------------------*/
/*  FillZero                                                   */
/* �����                                                      */
/*-------------------------------------------------------------*/

void FillZero(HANDLE hFile, UINT nLength)
{
	CHAR	ch = 0;
	DWORD   nbWritten;

	while (nLength -- > 0)
	{
		WriteFile(hFile,&ch,1,&nbWritten,NULL);
	}
}

/*-------------------------------------------------------------
 AddPackInfo                                                   
 ��¼ѹ������������Ϣ���������������ʱ��ѹ��            
���ݴ����ʽ��
DWORD  ��������ԭ��С__��ѹ����ռ��С
DWORD  ��������ԭƫ��__��ѹ���
DWORD  ����ѹ�����С__��ѹ����

�Ժ�ᱣ����shell.asm������
S_PackSection	DB	0a0h dup (?)
-------------------------------------------------------------*/
BOOL AddPackInfo(UINT OriginalOffset,UINT OriginalSize,UINT nPackSize, HWND hDlg)
{
	try
	{
		if (m_pInfoData == NULL )
		{
			AddLine(hDlg,"��Ч�Ĳ�������.");
			return FALSE;
		}

		if (m_pInfoSize + sizeof(UINT) * 2 > SIZE_OF_PACKINFO)
		{
			AddLine(hDlg,"����������ռ�̫С.");
			return FALSE;
		}


		*(UINT *)(&m_pInfoData[m_pInfoSize]) = OriginalSize ;//��������ԭ��С__��ѹ����ռ��С
		m_pInfoSize += sizeof(UINT);

		//��������ԭƫ��__��ѹ���
		*(UINT *)(&m_pInfoData[m_pInfoSize]) = OriginalOffset ;
		m_pInfoSize += sizeof(UINT);
		
		// ��¼ѹ�����ݵĴ�С
		*(UINT *)(&m_pInfoData[m_pInfoSize]) = nPackSize;
		m_pInfoSize += sizeof(UINT);
	}
	catch (...)
	{
		AddLine(hDlg,"δ֪�쳣.");
		return FALSE;
	}

	return TRUE;
}

/*-------------------------------------------------------------*/
/* PackData 												   */
/* ����aplibѹ������ѹ������                                   */
/* ѹ����ĵ�ַ:m_pPackData ��С��m_nPackSize (ȫ�ֱ���)       */
/*-------------------------------------------------------------*/

BOOL PackData(PCHAR pData, UINT nSize,HWND hDlg)
{
	PCHAR		pCloneData = NULL;
	UINT        m_nSpaceSize=NULL;

	try
	{
		if (pData == NULL || nSize <= 0 || IsBadReadPtr(pData, nSize))
		{
		    AddLine(hDlg,"��Ч�Ĳ�������.");
			return FALSE;

		}

    	//	��ʼ��

     	if (m_pWorkSpace != NULL)
		{
			delete[] m_pWorkSpace;	m_pWorkSpace = NULL;
		}

		if (m_pPackData != NULL)
		{
			delete[] m_pPackData;	m_pPackData = NULL;
		}

		m_nSpaceSize = 0;
		m_nPackSize = 0;

		// ���㹤���ռ��С
		m_nSpaceSize = aP_workmem_size(nSize);

		// ���빤���ռ�
		m_pWorkSpace = new CHAR[m_nSpaceSize];

		if (m_pWorkSpace == NULL)
		{
			AddLine(hDlg,"�ڴ治��.");
			return FALSE;
		}

		// ���뱣��ѹ�����ݵĿռ�
		m_pPackData = new CHAR[nSize * 2];

		if (m_pPackData == NULL)
		{
			AddLine(hDlg,"�ڴ治��.");
			return FALSE;
		}

		// ����ռ�
		pCloneData = (PCHAR)GlobalAlloc(GMEM_FIXED, nSize);

		if (pCloneData == NULL)
		{
			AddLine(hDlg,"�ڴ治��.");
			return FALSE;
		}

		// ����ԭʼ���ݵ��¿ռ�
		memcpy(pCloneData, pData, nSize);

		// ��ԭʼ���ݽ���ѹ��

		m_nPackSize = aP_pack((PBYTE)pCloneData, (PBYTE)m_pPackData, nSize, (PBYTE)m_pWorkSpace,pack_callback,0);

		// �ͷ��¿ռ�
		GlobalFree(pCloneData);
		pCloneData = NULL;

		if (m_nPackSize == 0)
		{
			AddLine(hDlg,"ѹ�������з��ִ���.");
			return FALSE;
		}
	}
	catch (...)
	{
		if (pCloneData != NULL)
		{
			GlobalFree(pCloneData);		pCloneData = NULL;
		}

			AddLine(hDlg,"δ֪�쳣.");
			return FALSE;

	}

			return TRUE;
}

/*-------------------------------------------------------------*/
/*  PackFile �� ѹ��PE    */
/*-------------------------------------------------------------*/
BOOL PackFile(TCHAR *szFilePath,UINT FirstResADDR,HWND hDlg)
{


	PIMAGE_SECTION_HEADER	psecHeader = m_psecHeader;

	UINT					nSectionNum = 0;
	UINT					nFileAlign = 0;
	UINT					nSectionAlign = 0;
	UINT					nSize = 0;
					
	DWORD                   nbWritten;
	UINT					nIndex = 0;
	PCHAR					pData = NULL;
	UINT					nNewSize = 0;
	UINT					nRawSize = 0;	
	
	try
	{		
		hPackFile = CreateFile(
			szFilePath,
			GENERIC_READ|GENERIC_WRITE,
			FILE_SHARE_READ|FILE_SHARE_WRITE, 
			NULL,
			CREATE_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,
			NULL);
		if ( hPackFile == INVALID_HANDLE_VALUE ) {
			AddLine(hDlg,"����!�ļ���ʧ��!");
			return  FALSE;
			}

		if(IsFileAlignment200){
			m_pntHeaders->OptionalHeader.FileAlignment = 0x200;// ǿ�ư���С200�ֽڶ���
		}

		nSectionNum   = m_pntHeaders->FileHeader.NumberOfSections;
		nFileAlign    = m_pntHeaders->OptionalHeader.FileAlignment;
		nSectionAlign = m_pntHeaders->OptionalHeader.SectionAlignment;

		//�� ������ ����
		PIMAGE_DATA_DIRECTORY			pBoundImportDir = NULL;
		PIMAGE_BOUND_IMPORT_DESCRIPTOR	pBoundImport = NULL;
		pBoundImportDir = &m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT];
		if (pBoundImportDir->VirtualAddress != NULL && pBoundImportDir->Size > 0)
		{
			pBoundImport = (PIMAGE_BOUND_IMPORT_DESCRIPTOR)RVAToPtr(pBoundImportDir->VirtualAddress);
			memset(pBoundImport,0, pBoundImportDir->Size);
			pBoundImportDir->VirtualAddress=0;
			pBoundImportDir->Size=0;	
		}
		
		//���IAT��Ϣ
		PIMAGE_DATA_DIRECTORY			pIATDir = NULL;
		pIATDir=&m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT];

		if(pIATDir->VirtualAddress != NULL)
		{
			memset((PCHAR)RVAToPtr(pIATDir->VirtualAddress ),0,pIATDir->Size);
			pIATDir->VirtualAddress=0;
			pIATDir->Size=0;
		}
		
	
		nSize = (PCHAR)(&psecHeader[nSectionNum + 1]) - (PCHAR)m_pImageBase;// �����µ��ļ�ͷ�Ĵ�С(����������һ�����Σ�

		nSize = AlignSize(nSize,nFileAlign);//����			
		m_pntHeaders->OptionalHeader.SizeOfHeaders = nSize;// Ҫ�����ļ�ͷ�е�SizeOfHeaders��С
		
		psecHeader->PointerToRawData = nSize;// ͬʱ��Ҫ������һ�������RAW��ַ


		if(!WriteFile(hPackFile,(PCHAR)m_pImageBase,nSize,&nbWritten,NULL))// д���ļ�
		{
			AddLine(hDlg,"����!�ļ�дʧ��!");
			CloseHandle(hPackFile);
			return FALSE;
		}

// д��ԭ����������
	
		for (nIndex = 0; nIndex < nSectionNum; nIndex ++, psecHeader ++)
		{

			pData = RVAToPtr(psecHeader->VirtualAddress);
			//nSize = psecHeader->SizeOfRawData;������ں�������������
			nSize=psecHeader->Misc.VirtualSize;
		

			// ע: ����ĳЩ��������֮ǰ��һЩ���������Ѿ�����˲������ݣ�����������ܱ�С��
			// �������ͨ��������ȥ��β�����õ����ֽڣ����¼�������Ĵ�С
			nNewSize = CalcMinSizeOfData(pData, nSize);

			// ������������Ѿ�ֻʡ���ֽڣ�����Բ���Ҫ��������������
			if (nNewSize == 0)
			{
				psecHeader->SizeOfRawData = 0;
				psecHeader->Characteristics |= IMAGE_SCN_MEM_WRITE;

				// ����ѹ����ԭ����˱���ÿ�ζ�������һ���������ʼƫ�Ƶ�ַ
				if (nIndex != nSectionNum - 1)
				{
					psecHeader[1].PointerToRawData = psecHeader->PointerToRawData + psecHeader->SizeOfRawData;
				}

				continue;
			}

			// �жϵ�ǰ���������ܷ�ѹ��
			if (IsSectionCanPacked(psecHeader))
			{
				
				// ���������ݽ���ѹ��  
				/*ѹ����ĵ�ַ 	  m_pPackData  *****/
				/*ѹ�������ݴ�С  m_nPackSize; *****/
				if(PackData(pData, nNewSize,hDlg))
					AddLine(hDlg,"����ѹ���ɹ�");
				else
				{
					AddLine(hDlg,"ѹ������ʧ��");
					return FALSE;
				}
			
				nRawSize  = AlignSize(m_nPackSize, nFileAlign);

				// д��ѹ���������
				if(!WriteFile(hPackFile,(PCHAR)m_pPackData,m_nPackSize,&nbWritten,NULL))// д���ļ�
				{
					AddLine(hDlg,"����!�ļ�дʧ��!");
					CloseHandle(hPackFile);
					return FALSE;
				}


				// д��Ϊ���������������
				if (nRawSize - m_nPackSize > 0)
				{
					FillZero(hPackFile, nRawSize - m_nPackSize);
				}

				// ��������Ĵ�С
				psecHeader->SizeOfRawData = nRawSize;

				// ��¼ѹ������������Ϣ���������������ʱ��ѹ��

				if(!AddPackInfo(psecHeader->VirtualAddress,psecHeader->Misc.VirtualSize,psecHeader->SizeOfRawData, hDlg))
					return FALSE;
			}
			else
			{
				//����Դ�������⴦��
				if ((strcmp((char *)psecHeader->Name, ".rsrc") == 0)&& isPackRes)
				{
					PIMAGE_DATA_DIRECTORY			pResourceDir = NULL;
					UINT							nResourceDirSize = NULL;
					PCHAR							pResourcePtr = NULL;
					UINT							nResourceSize;
					pResourceDir = &m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];

					if (pResourceDir->VirtualAddress != NULL)
					{
						pResourcePtr = (PCHAR)RVAToPtr(pResourceDir->VirtualAddress);
						nResourceSize = pResourceDir->Size;

						//д����Դ�β���ѹ���Ĳ���
						UINT					nFirstResSize;						
						PCHAR		pFirstResADDR = NULL;
						
						nResourceDirSize =  FirstResADDR  -pResourceDir->VirtualAddress ;//;�������ַ��ò�ѹ�����ֳ���
						if(!WriteFile(hPackFile,(PCHAR)pResourcePtr,nResourceDirSize,&nbWritten,NULL))// д���ļ�
						{
							AddLine(hDlg,"����!�ļ�дʧ��!");
							CloseHandle(hPackFile);
							return FALSE;
						}
						pFirstResADDR = RVAToPtr(FirstResADDR);  //��ѹ����Դ��ַ
						nFirstResSize = nResourceSize-nResourceDirSize; //��Ҫѹ����Դ�Ĵ�С
						  
						/*ѹ����ĵ�ַ 	  m_pPackData  *****/
						/*ѹ�������ݴ�С  m_nPackSize; *****/
						if(PackData(pFirstResADDR,nFirstResSize,hDlg))
							AddLine(hDlg,"ѹ����Դ�ɹ�.");
						else
							return FALSE;


	
						nRawSize  = AlignSize(m_nPackSize+nResourceDirSize, nFileAlign);//��������Դ�δ�С

						// д��ѹ���������
						if(!WriteFile(hPackFile,(PCHAR)m_pPackData,m_nPackSize,&nbWritten,NULL))// д���ļ�
						{
							AddLine(hDlg,"����!�ļ�дʧ��!");
							CloseHandle(hPackFile);
							return FALSE;
						}


						// д��Ϊ���������������
						if (nRawSize - m_nPackSize -nResourceDirSize > 0)
						{
							FillZero(hPackFile, nRawSize - m_nPackSize -nResourceDirSize);
						}

						// ��������Ĵ�С
						psecHeader->SizeOfRawData = nRawSize;

						// ��¼ѹ������������Ϣ���������������ʱ��ѹ��

						if(!AddPackInfo(FirstResADDR,nFirstResSize,m_nPackSize, hDlg))
							return FALSE;					

						
					}
				}
				else
				{
				
					nRawSize  = AlignSize(nNewSize, nFileAlign);

					// ����ѹ�������飬��ֱ�ӱ������������
					if(!WriteFile(hPackFile,(PCHAR)pData,nRawSize,&nbWritten,NULL))// д���ļ�
					{
						AddLine(hDlg,"����!�ļ�дʧ��!");
						CloseHandle(hPackFile);
						return FALSE;
					}
					psecHeader->SizeOfRawData = nRawSize;
				}
			}

			// ����ѹ����ԭ����˱���ÿ�ζ�������һ���������ʼƫ�Ƶ�ַ
			if (nIndex != nSectionNum - 1)
			{
				psecHeader[1].PointerToRawData = psecHeader->PointerToRawData + psecHeader->SizeOfRawData;
			}

			psecHeader->Characteristics |= IMAGE_SCN_MEM_WRITE;

	  }	
	}
	catch (...)
	{
		return FALSE;
	}

		return TRUE;
}
#endif