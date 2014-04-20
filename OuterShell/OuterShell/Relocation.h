#ifndef _RELOCATION_H_
#define _RELOCATION_H_

/*-------------------------------------------------------------*/
/*  ClsRelocation                                              */
/* ����ض�λ����                                              */
/*-------------------------------------------------------------*/

void ClsRelocData( )
{
	
	PIMAGE_BASE_RELOCATION		pBaseReloc = NULL;
	PIMAGE_DATA_DIRECTORY		pRelocDir = NULL;
	UINT						nSize = 0;

	pRelocDir = &m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	pBaseReloc =(PIMAGE_BASE_RELOCATION)RVAToPtr(pRelocDir->VirtualAddress);

	// ���û���ض�λ���ݣ���ֱ�ӷ���
	if (pRelocDir->VirtualAddress == 0)
	{
		return ;
	}

	// �������е��ض�λ����
	while (pBaseReloc->VirtualAddress != 0)
	{
		nSize = pBaseReloc->SizeOfBlock;

		memset(pBaseReloc, 0 , nSize);

		pBaseReloc = (PIMAGE_BASE_RELOCATION)((DWORD)pBaseReloc + nSize);
	}
	m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress = 0;
	m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size = 0;

}
/*-------------------------------------------------------------*/
/*  SaveReloc                                                  */
/* �����ض�λ���ݣ���Ҫ��֧��DLL�ļ�                           */
/*-------------------------------------------------------------*/

BOOL SaveReloc()
{


	//�����ض�λ��ṹ
	typedef struct _IMAGE_BASE_RELOCATION2 {
		DWORD   VirtualAddress;
		DWORD   SizeOfBlock;
		WORD    TypeOffset[1];
	} IMAGE_BASE_RELOCATION2;
	typedef IMAGE_BASE_RELOCATION2 UNALIGNED * PIMAGE_BASE_RELOCATION2;

	//�¹�����ض�λ��ṹ
/*	typedef struct _NEWIMAGE_BASE_RELOCATION {
		BYTE   type;
		DWORD  FirstTypeRVA;
		BYTE   nNewItemOffset[1];
	} 
*/

	PIMAGE_DATA_DIRECTORY		pRelocDir = NULL;
	PIMAGE_BASE_RELOCATION2		pBaseReloc = NULL;

	PCHAR						pRelocBufferMap =NULL;
	PCHAR						pData = NULL;
	UINT						nRelocSize = NULL;
	UINT						nSize = 0;
	UINT						nType = 0;
	UINT						nIndex = 0;
	UINT						nTemp = 0;
	UINT						nNewItemOffset =0;
	UINT						nNewItemSize =0;


	pRelocDir = &m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	nRelocSize = pRelocDir->Size;
	pBaseReloc =(PIMAGE_BASE_RELOCATION2)RVAToPtr(pRelocDir->VirtualAddress);

	// ���û���ض�λ���ݣ���ֱ�ӷ���
	if (pRelocDir->VirtualAddress == 0)
	{
		return TRUE;
	}
	//������ʱ����ռ�
	pRelocBufferMap = new char[nRelocSize];
	if (pRelocBufferMap == NULL)
		{
			return FALSE;
		}
	ZeroMemory(pRelocBufferMap, nRelocSize); 

	// 
	pData =pRelocBufferMap;
	
	while (pBaseReloc->VirtualAddress != 0)
	{
		nNewItemSize = (pBaseReloc->SizeOfBlock-8)/2;//������������Ҫ���ֽڳ�
		
		while (nNewItemSize != 0)
		{
			nType = pBaseReloc->TypeOffset[nIndex] >> 0x0c;//ȡtype

			if(nType ==0x3 )
			{
				//ȡ��ItemOffset�����ϱ����ض�λ��ʼ��ַ ����ȥnTemp,�õ���ֵ׼���ŵ����ض�λ��ṹ��
				nNewItemOffset = ((pBaseReloc->TypeOffset[nIndex] & 0x0fff) + pBaseReloc->VirtualAddress) - nTemp;
				if(nNewItemOffset > 0xff)//����Ǳ����ض�λ���ݵ�һ��
				{
					*(BYTE *)(pData) = 3;
					pData += sizeof(BYTE);
					*(DWORD *)pData = (DWORD)(nNewItemOffset);
					pData += sizeof(DWORD);
					
				}
				else
				{
					*(BYTE *)(pData) =(BYTE)(nNewItemOffset);
					pData += sizeof(BYTE);					
				}
				nTemp += nNewItemOffset;				
			}
			nNewItemSize--;	
			nIndex++;
		}

		nIndex     = 0;
		pBaseReloc = (PIMAGE_BASE_RELOCATION2)((DWORD)pBaseReloc + pBaseReloc->SizeOfBlock);
	}
	
	memset((PCHAR)RVAToPtr(pRelocDir->VirtualAddress),0,nRelocSize);
	memcpy((PCHAR)RVAToPtr(pRelocDir->VirtualAddress),pRelocBufferMap,nRelocSize);
	delete pRelocBufferMap;

	return TRUE;	
}

#endif