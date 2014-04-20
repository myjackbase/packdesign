#ifndef _ISPEFFILE_H_
#define _ISPEFFILE_H_

/*-------------------------------------------------------------*/
/* IsPEFile �� �ļ�����Ч��PE�ļ���                            */
/*-------------------------------------------------------------*/

BOOL IsPEFile(TCHAR *szFilePath,HWND hDlg)
{

	DWORD					fileSize;
	HANDLE					hMapping;
	LPVOID					ImageBase;
    PIMAGE_DOS_HEADER	    pDosHeader=NULL;
    PIMAGE_NT_HEADERS       pNtHeader=NULL;
    PIMAGE_FILE_HEADER      pFilHeader=NULL;
	PIMAGE_OPTIONAL_HEADER  pOptHeader=NULL;
    PIMAGE_SECTION_HEADER   pSecHeader=NULL;


	//���ļ�
  	HANDLE hFile = CreateFile(
		szFilePath,
		GENERIC_READ,
		FILE_SHARE_READ, 
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if ( hFile == INVALID_HANDLE_VALUE ) {
		 AddLine(hDlg,"����!�ļ���ʧ��!");
		 return  FALSE;
	}
		
	//����ļ����� :
	fileSize = GetFileSize(hFile,NULL);
	if (fileSize == 0xFFFFFFFF) {
		AddLine(hDlg,"����!�ļ���ʧ��!");
		return FALSE;
	}

    hMapping=CreateFileMapping(hFile,NULL,PAGE_READONLY,0,0,NULL);
	if(!hMapping)
	{									
		CloseHandle(hFile);
		AddLine(hDlg,"����!�ļ���ʧ��!");
		return FALSE;
	}
	ImageBase=MapViewOfFile(hMapping,FILE_MAP_READ,0,0,0);
    if(!ImageBase)
	{									
		CloseHandle(hMapping);
		CloseHandle(hFile);
	    AddLine(hDlg,"����!�ļ���ʧ��!");
		return FALSE;
	}
   
    pDosHeader=(PIMAGE_DOS_HEADER)ImageBase;
    if(pDosHeader->e_magic!=IMAGE_DOS_SIGNATURE){
		AddLine(hDlg,"����!���ǿ�ִ���ļ�!");
         return FALSE;
	}

    pNtHeader=(PIMAGE_NT_HEADERS32)((DWORD)pDosHeader+pDosHeader->e_lfanew);
    if (pNtHeader->Signature != IMAGE_NT_SIGNATURE ){
		AddLine(hDlg,"����!���ǿ�ִ���ļ�!");
        return FALSE;
	}

    pFilHeader=&pNtHeader->FileHeader;
    if (pFilHeader->NumberOfSections== 1 ){
		AddLine(hDlg,"�ļ������ѱ�ѹ��,����!");
        return FALSE;
	}

	 pOptHeader=&pNtHeader->OptionalHeader;//�õ�IMAGE_OPTIONAL_HEADER�ṹָ��ĺ���
	// pOptHeader->AddressOfEntryPoint;

    //�õ���һ���������ʼ��ַ  
   	pSecHeader=IMAGE_FIRST_SECTION(pNtHeader);
    pSecHeader++;//�õ��ڶ����������ʼ��ַ
	if((pOptHeader->AddressOfEntryPoint) > (pSecHeader->VirtualAddress)){
		AddLine(hDlg,"�ļ������ѱ�ѹ��,����!");
        return FALSE;
	}

	 if (((pFilHeader->Characteristics) & IMAGE_FILE_DLL )!=0){
		 AddLine(hDlg,"��PE-DLL�ļ�,����ѹ��.");
	 }
	 else{
		 AddLine(hDlg,"��PE-EXE�ļ�,����ѹ��.");
	 }
	 UnmapViewOfFile(ImageBase);
	 CloseHandle(hMapping);
	 CloseHandle(hFile);
	
	 return TRUE;
}

#endif