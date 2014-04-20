#ifndef _RESOURCEDIR_H_
#define _RESOURCEDIR_H_


/*-------------------------------------------------------------*/
/*  FindFirstResADDR                                          */
/* �Ҹ�����Դ�����ַ��ǰ��һ��,�Ժ󲿷ֿ���ѹ��              */
/*-------------------------------------------------------------*/

UINT FindFirstResADDR()
{
	UINT							FirstResAddr = NULL;
	
	PIMAGE_DATA_DIRECTORY			pResourceDir = NULL;
	PIMAGE_RESOURCE_DIRECTORY		pResource = NULL;

	PIMAGE_RESOURCE_DIRECTORY		pTypeRes = NULL;
	PIMAGE_RESOURCE_DIRECTORY		pNameIdRes = NULL;
	PIMAGE_RESOURCE_DIRECTORY		pLanguageRes = NULL;

	PIMAGE_RESOURCE_DIRECTORY_ENTRY	pTypeEntry = NULL;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY	pNameIdEntry = NULL;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY	pLanguageEntry = NULL;

	PIMAGE_RESOURCE_DATA_ENTRY		pResData = NULL;

	UINT							nTypeNum = 0;
	UINT							nTypeIndex = 0;
	UINT							nNameIdNum = 0;
	UINT							nNameIdIndex = 0;
	UINT							nLanguageNum = 0;
	UINT							nLanguageIndex = 0;

	try
	{
		FirstResAddr = m_pntHeaders->OptionalHeader.SizeOfImage;//�ַ���һ�����ֵ������ȡӳ��ߴ磩��Ȼ����ݱȽ��𽥼�С

		pResourceDir = &m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];
		if (pResourceDir->VirtualAddress == NULL)
		{
			return FALSE;
		}

		pResource = (PIMAGE_RESOURCE_DIRECTORY)RVAToPtr(pResourceDir->VirtualAddress);//��Դ����ַ
		pTypeRes    = pResource;
		nTypeNum    = pTypeRes->NumberOfIdEntries + pTypeRes->NumberOfNamedEntries;//���������м�����Դ
		pTypeEntry  = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pTypeRes + sizeof(IMAGE_RESOURCE_DIRECTORY));

		for (nTypeIndex = 0; nTypeIndex < nTypeNum; nTypeIndex ++, pTypeEntry ++)
		{
			
			//������Ŀ¼��ַ
			pNameIdRes = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pResource + (DWORD)pTypeEntry->OffsetToDirectory);
			//���������м�����Ŀ
			nNameIdNum = pNameIdRes->NumberOfIdEntries + pNameIdRes->NumberOfNamedEntries;
			pNameIdEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pNameIdRes + sizeof(IMAGE_RESOURCE_DIRECTORY));


			for (nNameIdIndex = 0; nNameIdIndex < nNameIdNum; nNameIdIndex ++, pNameIdEntry ++)
			{
			
		
				//����ĿĿ¼��ַ
				pLanguageRes = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pResource + (DWORD)pNameIdEntry->OffsetToDirectory);
				nLanguageNum = pLanguageRes->NumberOfIdEntries + pLanguageRes->NumberOfNamedEntries;
				pLanguageEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pLanguageRes + sizeof(IMAGE_RESOURCE_DIRECTORY));

				for (nLanguageIndex = 0; nLanguageIndex < nLanguageNum; nLanguageIndex ++, pLanguageEntry ++)
				{
					pResData = (PIMAGE_RESOURCE_DATA_ENTRY)((DWORD)pResource + (DWORD)pLanguageEntry->OffsetToData);

					if((pResData->OffsetToData < FirstResAddr) && (pResData->OffsetToData>pResourceDir->VirtualAddress))
					{
						FirstResAddr = pResData->OffsetToData;
					}
				}				
			}
		}

		
	}
	catch (...)
	{
		return FALSE;
	}
	return FirstResAddr;
	
}


/*-------------------------------------------------------------*/
/*  MoveRes													    */
/* �������͵���Դ�ƶ���ָ����λ��	
ResType:��Դ��ID�ţ�
MoveADDR��ΪĿ���ַ,��Ϊ0���������ƶ����ݣ�ֻ�������ݴ�С   
MoveResSize:Ϊ�ϴ��ƶ���Դ�Ĵ�С */
/*-------------------------------------------------------------*/

BOOL MoveRes(UINT ResType,PCHAR MoveADDR,UINT MoveResSize)
{
	PIMAGE_DATA_DIRECTORY			pResourceDir = NULL;

	PIMAGE_RESOURCE_DIRECTORY		pResource = NULL;
	PIMAGE_RESOURCE_DIRECTORY		pTypeRes = NULL;
	PIMAGE_RESOURCE_DIRECTORY		pNameIdRes = NULL;
	PIMAGE_RESOURCE_DIRECTORY		pLanguageRes = NULL;

	PIMAGE_RESOURCE_DIRECTORY_ENTRY	pTypeEntry = NULL;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY	pNameIdEntry = NULL;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY	pLanguageEntry = NULL;

	PIMAGE_RESOURCE_DATA_ENTRY		pResData = NULL;
	UINT							nTypeNum = 0;
	UINT							nTypeIndex = 0;
	UINT							nNameIdNum = 0;
	UINT							nNameIdIndex = 0;
	UINT							nLanguageNum = 0;
	UINT							nLanguageIndex = 0;
	DWORD							mShell0_nSize = NULL;//��������εĳߴ�
	PCHAR							pOffsetToDataPtr;

	try
	{
		mShell0_nSize =(DWORD) (&ShellEnd0) - (DWORD)(&ShellStart0) ;//������������γߴ�
		
		pResourceDir = &m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];
		if (pResourceDir->VirtualAddress == NULL)
		{
			return FALSE;
		}

		pResource = (PIMAGE_RESOURCE_DIRECTORY)RVAToPtr(pResourceDir->VirtualAddress);//��Դ����ַ
		pTypeRes    = pResource;
		nTypeNum    = pTypeRes->NumberOfIdEntries + pTypeRes->NumberOfNamedEntries;//���������м�����Դ
		pTypeEntry  = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pTypeRes + sizeof(IMAGE_RESOURCE_DIRECTORY));

		for (nTypeIndex = 0; nTypeIndex < nTypeNum; nTypeIndex ++, pTypeEntry ++)
		{
			if(pTypeEntry->NameIsString==0)
			{
			
				if((DWORD)pTypeEntry->NameOffset ==ResType)
				{
					//������Ŀ¼��ַ
					pNameIdRes = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pResource + (DWORD)pTypeEntry->OffsetToDirectory);
					//���������м�����Ŀ
					nNameIdNum = pNameIdRes->NumberOfIdEntries + pNameIdRes->NumberOfNamedEntries;
					pNameIdEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pNameIdRes + sizeof(IMAGE_RESOURCE_DIRECTORY));

					for (nNameIdIndex = 0; nNameIdIndex < nNameIdNum; nNameIdIndex ++, pNameIdEntry ++)
					{			
					
						//����ĿĿ¼��ַ
						pLanguageRes = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pResource + (DWORD)pNameIdEntry->OffsetToDirectory);
						nLanguageNum = pLanguageRes->NumberOfIdEntries + pLanguageRes->NumberOfNamedEntries;
						pLanguageEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pLanguageRes + sizeof(IMAGE_RESOURCE_DIRECTORY));

						for (nLanguageIndex = 0; nLanguageIndex < nLanguageNum; nLanguageIndex ++, pLanguageEntry ++)
						{
							//OffsetToData (RVA):
							pResData = (PIMAGE_RESOURCE_DATA_ENTRY)((DWORD)pResource + (DWORD)pLanguageEntry->OffsetToData);
						
							if(MoveADDR)
							{				
							
								pOffsetToDataPtr = RVAToPtr(pResData->OffsetToData);
							
								pResData->OffsetToData = m_nImageSize+mShell0_nSize+MoveResSize; //��OffsetToData�ֶ�ָ���������������Դ��

								memcpy(MoveADDR+MoveResSize,pOffsetToDataPtr,pResData->Size);
								ZeroMemory(pOffsetToDataPtr, pResData->Size); 				
							}
					
							MoveResSize+=pResData->Size;
						}											
					}
				return MoveResSize;
				}				
			}
		}	
	}
	catch (...)
	{
		return FALSE;
	}
	return 0;


}
#endif
