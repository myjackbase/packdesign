#ifndef _IMPORTTABLE_H_
#define _IMPORTTABLE_H_


/*------------------------------------------------------------- */
/*  �������ܲ���												*/
/* MoveImpTable                                                 */
/* ��һ����������������ݻ��ֽṹ��ʽ�洢,�Դﵽ���ܵ�Ŀ��.   */
/* 																*/
/*  -----------------------------------------------------		*/
/* | DWORD | byte | STRING | 00 |DWORD |BYTE |STRING|00|��		*/
/*  -----------------------------------------------------		*/
/* FirstThunk      DLL��      ��ʼ������Ŀ   ������				*/
/*                          								    */
/*------------------------------------------------------------- */
// ��������ֵΪ�����������ݵĴ�С

UINT MoveImpTable(PCHAR m_pImportTable)
{

	PIMAGE_IMPORT_DESCRIPTOR	pImportDescriptor = NULL, pDescriptor = NULL;
	PIMAGE_DATA_DIRECTORY		pImportDir = NULL;
	PCHAR						pszDllName = NULL;
	UINT						nSize = 0;
	PCHAR						pData = NULL;
	PCHAR						pFunNum =NULL;
	PIMAGE_THUNK_DATA32			pFirstThunk = NULL;
	PIMAGE_IMPORT_BY_NAME		pImportName = NULL;

	try
	{
		pImportDir = &m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
		if(0==pImportDir->VirtualAddress)
					return FALSE;			
		pImportDescriptor =(PIMAGE_IMPORT_DESCRIPTOR)RVAToPtr(pImportDir->VirtualAddress);

		// ����ԭʼ�����
		for (pData = m_pImportTable, pDescriptor = pImportDescriptor; pDescriptor->Name != 0; pDescriptor ++)
		{
			// ������THUNK���ݵ�RVA��ַ
			*(DWORD *)pData = pDescriptor->FirstThunk;
			pData += sizeof(DWORD);
			// ����DLL���Ƴ���(WORD)
			pszDllName = (PCHAR)RVAToPtr(pDescriptor->Name);
			*(BYTE *)(pData) = (BYTE)(strlen(pszDllName) ); //DLL�ַ�������
			pData += sizeof(BYTE);
			//����DLL�ַ���
			memcpy(pData, pszDllName, strlen(pszDllName) + 1);
			pData += strlen(pszDllName) + 1;
			pFunNum = pData; //pFunNum ָ����Ҫ��ʼ��������Ŀ
			*(DWORD *)pFunNum =0;

			pData += sizeof(DWORD);//ָ��BYTE |STRING|00|����

			
			// ע: ͨ��������FirstThunk�б��溯�����Ƶ���Ϣ(��׼��ʽ)�����ǲ��ֳ������Ϊ�˼ӿ����������
			// ��FirstThunk��ֱ�ӱ��溯����ַ����OriginalFirstThunk�ű��溯�����Ƶ���Ϣ���������ֻ��OrginalFirstThunk
			// ��Чʱ����ȡFirstThunk�е�������Ϊ�������Ƶ���Ϣ�Ĵ��λ�á�
			if (pDescriptor->OriginalFirstThunk != 0)
			{
				pFirstThunk = (PIMAGE_THUNK_DATA32)RVAToPtr(pDescriptor->OriginalFirstThunk);
			}
			else
			{
				
				pFirstThunk = (PIMAGE_THUNK_DATA32)RVAToPtr(pDescriptor->FirstThunk);
			}

			//����BYTE |STRING|00|����
			/*
			typedef struct _IMAGE_THUNK_DATA32 {
				union {
					PBYTE  ForwarderString;
					PDWORD Function;
				    DWORD  Ordinal;
					PIMAGE_IMPORT_BY_NAME  AddressOfData;
					} u1;
			} IMAGE_THUNK_DATA32;
			typedef IMAGE_THUNK_DATA32 * PIMAGE_THUNK_DATA32;*/

			while (pFirstThunk->u1.AddressOfData != NULL)
			{
				if (IMAGE_SNAP_BY_ORDINAL32(pFirstThunk->u1.Ordinal))//��������ŷ�ʽ
				{
					*(BYTE *)pData = 0;
					pData += sizeof(BYTE);

					*(DWORD *)pData = (DWORD)(pFirstThunk->u1.Ordinal & ~IMAGE_ORDINAL_FLAG32);//�����Ԫ��ֵ����߶���λΪ1�� ��ô������
					pData += sizeof(DWORD)+1;
					(*(DWORD *)pFunNum) ++;//������������������1
				}
				else//�������ַ�������
				{
					pImportName = (PIMAGE_IMPORT_BY_NAME)RVAToPtr((DWORD)(pFirstThunk->u1.AddressOfData));

						*(BYTE *)pData = (BYTE)(strlen((char *)pImportName->Name) );//�������ַ�����
						pData += sizeof(BYTE);

						memcpy(pData, pImportName->Name, strlen((char *)pImportName->Name) + 1);

						(*(DWORD *)pFunNum) ++;//������������������1

						pData += strlen((char *)pImportName->Name) + 1;
				
				}

				pFirstThunk ++;
				
			}
			
		}

		// DLL�ṹ�Ľ�����
		*(DWORD *)pData = (DWORD)0;
		pData += sizeof(DWORD);

		// ����ʵ�ʴ�С
		return(pData - m_pImportTable);

	}
	catch (...)
	{
		return FALSE;
	}

}
/*-------------------------------------------------------------*/
/*  ClsImpTable                                                */
/*  ���ԭ�����                                               */
/*-------------------------------------------------------------*/

void ClsImpTable( )
{

	PIMAGE_IMPORT_DESCRIPTOR	pImportDescriptor = NULL, pDescriptor = NULL;
	PIMAGE_DATA_DIRECTORY		pImportDir = NULL;
	PCHAR						pszDllName = NULL;
	PIMAGE_THUNK_DATA32			pFirstThunk = NULL;
	PIMAGE_IMPORT_BY_NAME		pImportName = NULL;

	pImportDir = &m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	pImportDescriptor =(PIMAGE_IMPORT_DESCRIPTOR)RVAToPtr(pImportDir->VirtualAddress);

	// ����ԭʼ�����
	for (pDescriptor = pImportDescriptor; pDescriptor->Name != 0; pDescriptor ++)//ÿѭ��һ�Σ����һ��DLL����Ϣ
	{
		pszDllName = (PCHAR)RVAToPtr(pDescriptor->Name);

		// ����DLL�ַ�����Ϣ
		memset(pszDllName, 0, strlen(pszDllName));

		// ����ԭʼTHUNK����
		if (pDescriptor->OriginalFirstThunk != 0)
		{
			pFirstThunk = (PIMAGE_THUNK_DATA32)RVAToPtr(pDescriptor->OriginalFirstThunk);

			while (pFirstThunk->u1.AddressOfData != NULL)//���OriginalFirstThunk
			{
				if (IMAGE_SNAP_BY_ORDINAL32(pFirstThunk->u1.Ordinal))
				{
					memset(pFirstThunk, 0 , sizeof(DWORD));
				}
				else
				{
					pImportName = (PIMAGE_IMPORT_BY_NAME)RVAToPtr((DWORD)(pFirstThunk->u1.AddressOfData));
					memset(pImportName, 0,strlen((char *)pImportName->Name) + sizeof(WORD));
					memset(pFirstThunk, 0 ,sizeof(DWORD));
				}

				pFirstThunk ++;
			}

		}
		
		pFirstThunk = (PIMAGE_THUNK_DATA32)RVAToPtr(pDescriptor->FirstThunk);
		//	���FirstThunk
		while (pFirstThunk->u1.AddressOfData != NULL)
		{
			memset(pFirstThunk, 0 ,sizeof(DWORD));
			pFirstThunk ++;
		}

		memset(pDescriptor, 0 ,sizeof(IMAGE_IMPORT_DESCRIPTOR));
	}
}

#endif