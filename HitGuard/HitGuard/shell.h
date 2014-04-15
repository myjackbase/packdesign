#pragma once

#include "windows.h"
#include "string.h"

inline FARPROC MyGetProcAddress(HMODULE hModule, LPCSTR lpProcName)
{
	PIMAGE_DOS_HEADER pDOSHeader;
	PIMAGE_NT_HEADERS pNTHeader;
	PIMAGE_EXPORT_DIRECTORY pExportDir;
	LPCSTR *pFunctionName; 
	LPCSTR pszFunName;
	LPDWORD pFunction;
	LPWORD pIndex;
	DWORD n; 
	FARPROC ret = NULL;
	if ((INVALID_HANDLE_VALUE == hModule) || (NULL == lpProcName))
	{
		goto end;
	}
	pDOSHeader = (PIMAGE_DOS_HEADER)hModule;
	if (pDOSHeader->e_magic != IMAGE_DOS_SIGNATURE)  
	{
		goto end;
	}
	pNTHeader = (PIMAGE_NT_HEADERS)((DWORD)pDOSHeader + (DWORD)pDOSHeader->e_lfanew);
	if (pNTHeader->Signature != IMAGE_NT_SIGNATURE) 
	{
		goto end;
	}
	pExportDir = (PIMAGE_EXPORT_DIRECTORY)((DWORD)pDOSHeader + \
		(DWORD)pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	if ((DWORD)pExportDir == (DWORD)pDOSHeader) 
	{
		goto end;
	}
	pFunctionName = (LPCSTR *)((DWORD)pExportDir->AddressOfNames + (DWORD)pDOSHeader);
    pFunction = (LPDWORD)((DWORD)pExportDir->AddressOfFunctions + (DWORD)pDOSHeader);
    pIndex = (LPWORD)((DWORD)pExportDir->AddressOfNameOrdinals + (DWORD)pDOSHeader);
    n = pExportDir->NumberOfNames;
    while (n--)
	{
		pszFunName = (LPCSTR)((DWORD)*pFunctionName + (DWORD)pDOSHeader);
		if (strcmp(pszFunName, lpProcName) == 0)
		{
			ret = (FARPROC)(pFunction[*pIndex] + (DWORD)pDOSHeader);
			break;
		}
		pFunctionName++;
		pIndex++;
	}
end:	
	return ret;
}

typedef HMODULE (WINAPI *PFNGetModuleHandle)(LPCTSTR lpModuleName); 


__declspec(naked) void Loader_Start()
{
}


__declspec(naked) void start()
{
	DWORD dwGetModuleHandleA;
	DWORD dwKernel32;
	PFNGetModuleHandle GetModuleHandle;
	DWORD dwImageBase,dwCodeStart,dwCodeSize;
	PIMAGE_DOS_HEADER pDosHeader;
	PIMAGE_NT_HEADERS pNtHeaders;
	PIMAGE_SECTION_HEADER pSecHeader;
	BYTE Tmp;
	unsigned n;
	__asm
	{
		jmp begin 
		__emit 'S'
		__emit 'q'
		__emit 'u'
		__emit 'n'
		__emit 'G'
		__emit 'u' 
		__emit 'a'
		__emit 'r'
		__emit 'd'
begin:
		call stub
stub:
		pop esi
		sub esi,offset stub
		lea edx,[offset szGetModuleHandle+esi]
		mov dwGetModuleHandleA,edx
	}
	if (dwGetModuleHandleA==0)
	{
		goto gotooep;
	}

	__asm
	{
		xor eax,eax
		mov eax,fs:[eax+0x30]
		mov eax,[eax+0x0c]
		mov esi,[eax+0x1c]
		lodsd
		mov eax,[eax+0x08]
		mov dwKernel32,eax
	}

	GetModuleHandle=(PFNGetModuleHandle)MyGetProcAddress((HMODULE)dwKernel32,(LPCSTR)dwGetModuleHandleA);
	if (!(DWORD)GetModuleHandle)
	{
		goto  gotooep;
	}

	dwImageBase=(DWORD)GetModuleHandle(NULL);
	if (!dwImageBase)
	{
		goto gotooep;
	}

	pDosHeader = (PIMAGE_DOS_HEADER)dwImageBase;
	pNtHeaders = (PIMAGE_NT_HEADERS)(dwImageBase+pDosHeader->e_lfanew);
	pSecHeader = (PIMAGE_SECTION_HEADER)((long)pNtHeaders+\
		sizeof(DWORD)+\
		sizeof(IMAGE_FILE_HEADER)+\
		(long)pNtHeaders->FileHeader.SizeOfOptionalHeader);

	dwCodeStart = pSecHeader->VirtualAddress + dwImageBase;
	dwCodeSize = pSecHeader->SizeOfRawData;
	for(n=0;n<dwCodeSize;n++)
	{
		Tmp = (BYTE)(*(DWORD*)(dwCodeStart+n));
		*(BYTE*)(dwCodeStart+n) = Tmp ^ 0x2B;
	}

gotooep:
	__asm mov eax,0x12345678
	__asm add eax,dwImageBase
	__asm call eax

szGetModuleHandle:
	__asm
	{
		__emit 'G'
		__emit 'e'
		__emit 't'
		__emit 'M'
		__emit 'o'
		__emit 'd'
		__emit 'u'
		__emit 'l'
		__emit 'e'
		__emit 'H'
		__emit 'a'
		__emit 'n'
		__emit 'd'
		__emit 'l'
		__emit 'e'
		__emit 'A'
	    __emit '\0'
	}
}


__declspec(naked) void Loader_End()
{
}