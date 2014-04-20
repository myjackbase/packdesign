#ifndef _SECTION_H_
#define _SECTION_H_

/*-------------------------------------------------------------*/
/*  CalcMinSizeOfData                                   ��     */
/* ������ȥ��β�����õ����ֽڣ����¼�������Ĵ�С             */
/*-------------------------------------------------------------*/

UINT CalcMinSizeOfData(PCHAR pSectionData, UINT nSectionSize)
{

	if (IsBadReadPtr(pSectionData, nSectionSize))
	{
		return nSectionSize;
	}

	PCHAR	pData = &pSectionData[nSectionSize - 1];
	UINT	nSize = nSectionSize;

	while (nSize > 0 && *pData == 0)
	{
		pData --;
		nSize --;
	}

	return nSize;
}

/*-------------------------------------------------------------*/
/*  IsSectionCanPacked                                         */
/* �жϵ�ǰ���������ܷ�ѹ��                                  */
/*-------------------------------------------------------------*/

BOOL IsSectionCanPacked(PIMAGE_SECTION_HEADER psecHeader)
{
//	ASSERT(psecHeader != NULL);

	UINT  nExportAddress=0;
	const UINT	nListNum = 6;
	LPCTSTR		lpszSecNameList[nListNum] =
	{
		".text",
		".data",
		".rdata",
		"CODE",
		"DATA",
		".reloc",
	};

	nExportAddress = m_pntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;


	// �������ƥ����������ƣ����ʾ���������ѹ��
	for (UINT nIndex = 0; nIndex < nListNum; nIndex ++)
	{

		//��Щ�������ܻ���.rdata�����飬�������ϲ��˾Ͳ��������ж���
		if(!IsMergeSection)
		{
			if((nExportAddress >= psecHeader->VirtualAddress) && ( nExportAddress < (psecHeader->VirtualAddress+psecHeader->Misc.VirtualSize) ) )
			return FALSE;
		}

		if (strncmp((char *)psecHeader->Name, lpszSecNameList[nIndex], strlen(lpszSecNameList[nIndex])) == 0)
		{
			return TRUE;
		}
	}


	// �����������鶼����ѹ��
	return FALSE;
}

/*-------------------------------------------------------------*/
/*  MergeSection ���ϲ�����
����ʼ��һЩ����ѹ�������κϲ�,������СһЩѹ�����ļ��Ĵ�С
�����˺������ں����ɵ�����ֻ��ӳ���С��ӳ��ƫ������,�ļ���
С���ļ�ƫ����ѹ����дʱ����.
/*-------------------------------------------------------------*/
BOOL MergeSection()
{
	UINT					nSectionNum = 0;
	PIMAGE_SECTION_HEADER	psecHeader = m_psecHeader;
	UINT					nspareSize=NULL;
	UINT					nMergeVirtualSize =0;
	UINT					nIndex = 0;
	try
	{
		nSectionNum   = m_pntHeaders->FileHeader.NumberOfSections;
		
		for (nIndex = 0; nIndex < nSectionNum; nIndex ++, psecHeader ++)
		{
			if ((m_psecHeader->Characteristics & IMAGE_SCN_MEM_SHARED) != 0)//�������鲻�ں�
				break;		

			if ((strcmp((char *)psecHeader->Name, ".edata") == 0))
				break;

			if ((strcmp((char *)psecHeader->Name, ".rsrc") == 0))
				break;			

			nMergeVirtualSize += psecHeader->Misc.VirtualSize;
		}
		m_psecHeader->Misc.VirtualSize = nMergeVirtualSize;
		m_pntHeaders->FileHeader.NumberOfSections = nSectionNum - nIndex+1;// ���ڵ�������
		//(nIndex+1)*sizeof(IMAGE_SECTION_HEADER)//;ʣ�������ĳ���
		memcpy(m_psecHeader+1,psecHeader,(nSectionNum - nIndex)*sizeof(IMAGE_SECTION_HEADER));//��ʣ���������ǰ
		nspareSize=(nSectionNum - m_pntHeaders->FileHeader.NumberOfSections)*sizeof(IMAGE_SECTION_HEADER);//�������鳤��
		memset(m_psecHeader+nSectionNum - nIndex+1,0,nspareSize);		


	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;

}
/*-------------------------------------------------------------*/
/*  ClsSectionName�� ���������								   */
/*-------------------------------------------------------------*/
void ClsSectionName()
{
	UINT					nSectionNum = 0;
	PIMAGE_SECTION_HEADER	psecHeader = m_psecHeader;
	
	nSectionNum   = m_pntHeaders->FileHeader.NumberOfSections;		
	for (UINT nIndex = 0; nIndex < nSectionNum; nIndex ++, psecHeader ++)
	{
		memset(psecHeader->Name, 0, 8);
	}
}

#endif