#pragma once

/*-------------------------------------------------------------*/
/* �����ӳ�����ȫ�ֱ���������                                  */
/*-------------------------------------------------------------*/

#define				    MAXINPUTLEN 80

BOOL	CALLBACK MainDlg   (HWND, UINT, WPARAM, LPARAM) ;

HINSTANCE				hInst;
HWND					hProgress;

TCHAR					szFilePath[MAX_PATH];
TCHAR					*pMessageBuffer;		// ��Ϣ�򻺳�
TCHAR					 IniFileName[] = TEXT (".\\settings.ini");// �������ü�¼�ļ�

PCHAR					m_pInfoData= NULL;      // ���ڼ�¼ѹ������������Ϣ
UINT					m_pInfoSize;
const UINT				SIZE_OF_PACKINFO = 0x2000;// ��¼ѹ������������Ϣ


UINT					m_nImageSize = 0;		// ӳ���С
PIMAGE_NT_HEADERS 		m_pntHeaders = 0;		// PE�ṹָ��
PIMAGE_SECTION_HEADER 	m_psecHeader = 0;		// ��һ��SECTION�ṹָ��
PCHAR					m_pImageBase = 0;		// ӳ���ַ


PCHAR					m_pWorkSpace=NULL;		// ѹ���������ʱ�ڴ�
PCHAR					m_pPackData=NULL;		// ѹ����ĵ�ַ
UINT					m_nPackSize;			// ѹ�������ݴ�С

HANDLE				    hPackFile ;				// ����ѹ���ļ��ľ��

BOOL					ISWORKING=0;              // �Ƿ�����ѹ����־

//���shell.asm�еı���
extern "C" DWORD ShellStart0;
extern "C" DWORD ShellEnd0;
extern "C" DWORD ShellStart;
extern "C" DWORD ShellEnd;
extern "C" DWORD ImportTableBegin;
extern "C" DWORD ImportTableEnd;
extern "C" DWORD SHELL_DATA_0;
extern "C" DWORD SHELL_DATA_1;
extern "C" BYTE  TlsTable;
extern "C" BYTE  RelocBaseBegin;

//���ԶԻ������
BOOL IsFileAlignment200 = FALSE;
BOOL IsProtImpTable     = FALSE;//����Ƿ������
BOOL isPackRes          = FALSE;//�Ƿ�ѹ����Դ
BOOL IsSaveSData        = FALSE;//�Ƿ�����������
BOOL IsMergeSection     = FALSE;//�Ƿ�ϲ�����
BOOL IsClsSecName       = FALSE;//���������
BOOL IsNoRelocation     = FALSE;//���������
BOOL IsCreateBak        = FALSE;//�����ļ�
BOOL IsDelShare         = FALSE;//����ƥ�鹲��

/*-------------------------------------------------------------*/
/* RVAToPtr �� RVAת��ǰƫ�Ƶ�ַ����						   */
/*-------------------------------------------------------------*/
PCHAR RVAToPtr(DWORD dwRVA)
{
	if ((UINT)dwRVA < m_nImageSize)
	{
		return (PCHAR)((DWORD)m_pImageBase + dwRVA);
	}
	else
	{
		return NULL;
	}
}




/*-------------------------------------------------------------*/
/* AddLine �� ����Ϣ��������һ����Ϣ���                       */
/*-------------------------------------------------------------*/


void AddLine(HWND hDlg,TCHAR *szMsg)
{
	if(GetDlgItemText(hDlg,IDC_MESSAGEBOX_EDIT,pMessageBuffer,MAX_PATH)!=0){
		strcat(pMessageBuffer, "\r\n"); 
	}
	strcat(pMessageBuffer, szMsg); 

	SendDlgItemMessage(hDlg,IDC_MESSAGEBOX_EDIT,WM_SETTEXT,0,(LPARAM)pMessageBuffer);
	SendDlgItemMessage(hDlg, IDC_MESSAGEBOX_EDIT, EM_LINESCROLL, 0, SendDlgItemMessage(hDlg, IDC_MESSAGEBOX_EDIT, EM_GETLINECOUNT, 0, 0));
}

/*-------------------------------------------------------------*/
/* AlignSize �� ȡ�����뺯��								   */
/*-------------------------------------------------------------*/
UINT AlignSize(UINT nSize, UINT nAlign)
{
	return ((nSize + nAlign - 1) / nAlign * nAlign);
}
