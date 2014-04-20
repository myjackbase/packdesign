#include "res/resource.h"
#include <windows.h> 
#include "aplib\aplib.h"
#include <commctrl.h>
#include "OuterShell.h"
#include "ImportTable.h"
#include "ResourceDir.h"
#include "IsPEFile.h"
#include "Relocation.h"
#include "Section.h"
#include "PackFile.h"
#include "DisposeShell.h"
#include "Protect.h"
#include <process.h> 

#pragma comment(lib, "comctl32.lib")

/*-------------------------------------------------------------*/
/* WinMain �� ����WIN32�ĳ�������                            */
/*-------------------------------------------------------------*/

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    PSTR szCmdLine, int iCmdShow)
{
	hInst = hInstance;
	DialogBoxParam (hInstance, MAKEINTRESOURCE(IDD_MAINDLG), NULL, MainDlg, NULL);
	return 0;	
}

/*-------------------------------------------------------------*/
/* AboutDlgProc �� ���ڴ���                                    */
/*-------------------------------------------------------------*/

BOOL CALLBACK AboutDlgProc (HWND hDlg, UINT message, 
                            WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
   case  WM_LBUTTONDOWN:
		PostMessage(hDlg, WM_NCLBUTTONDOWN, HTCAPTION, 0);
		return TRUE ;
    
	case WM_COMMAND :
		switch (LOWORD (wParam))  		            
		{
		case IDOK :
		case IDCANCEL :
			EndDialog (hDlg, 0) ;
			return TRUE ;
		 }
		  break ;
	 }
     return FALSE ;
}



/*-------------------------------------------------------------*/
/* OpenFileDlg �� �򿪶Ի�����                               */
/*-------------------------------------------------------------*/
BOOL  OpenFileDlg(TCHAR *szFilePath,HWND hwnd)   
{
	OPENFILENAME ofn;
    memset(szFilePath,0,MAX_PATH);
	memset(&ofn, 0, sizeof(ofn));

	ofn.lStructSize      =sizeof(ofn);
	ofn.hwndOwner        =hwnd;
	ofn.hInstance        =hInst;
	ofn.nMaxFile         =MAX_PATH;
	ofn.lpstrInitialDir  =NULL;
	ofn.lpstrFile        =szFilePath;
	ofn.lpstrTitle       ="Open ...";
	ofn.Flags            = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_LONGNAMES|OFN_EXPLORER | OFN_HIDEREADONLY;
	ofn.lpstrFilter      =TEXT ("PE Files (*.exe;*.dll)\0*.EXE;*.DLL\0")\
						  TEXT ("All Files (*.*)\0*.*\0\0") ;
	if(!GetOpenFileName(&ofn))
		return FALSE;
	
	SendDlgItemMessage(hwnd,IDC_MESSAGEBOX_EDIT,WM_SETTEXT,0,0);//�����Ϣ���е���ʾ
	return TRUE;

}



/*-------------------------------------------------------------*/
/* properinitDlgProc�� ѡ��Ի����ʼ��                            */
/*-------------------------------------------------------------*/
void properinitDlgProc(HWND hDlg)
{

	//ǿ���ļ���0x200����
	if(GetPrivateProfileInt("ProtSet","IsFileAlignment200",0,IniFileName)==1) 
	{
		IsFileAlignment200=TRUE;
		CheckDlgButton(hDlg,IDC_ALIGN_200,BST_CHECKED);
	}

	//�Ƿ��������
	if(GetPrivateProfileInt("ProtSet","IsProtImpTable",0,IniFileName)==1) 
	{
		IsProtImpTable=TRUE;
		CheckDlgButton(hDlg,IDC_APIPROT,BST_CHECKED);
	}

	//�Ƿ�ѹ����Դ
   	if(GetPrivateProfileInt("ProtSet","isPackRes",0,IniFileName)==1) 
	{
		isPackRes =TRUE;
		CheckDlgButton(hDlg,IDC_PACKRES,BST_CHECKED);
	}

	//�Ƿ�����������
   	if(GetPrivateProfileInt("ProtSet","IsSaveSData",0,IniFileName)==1) 
	{
		isPackRes =TRUE;
		CheckDlgButton(hDlg,IDC_SDATA,BST_CHECKED);
	}

	//�Ƿ�ϲ�����
   	if(GetPrivateProfileInt("ProtSet","IsMergeSection",0,IniFileName)==1) 
	{
		IsMergeSection =TRUE;
		CheckDlgButton(hDlg,IDC_MERGESECTION,BST_CHECKED);
	}

	//���������
   	if(GetPrivateProfileInt("ProtSet","IsClsSecName",0,IniFileName)==1) 
	{
		IsClsSecName =TRUE;
		CheckDlgButton(hDlg,IDC_CLSSECNAME,BST_CHECKED);
	}

	//����ض�λ����
   	if(GetPrivateProfileInt("ProtSet","IsNoRelocation",0,IniFileName)==1) 
	{
		IsNoRelocation =TRUE;
		CheckDlgButton(hDlg,IDC_NOREL0,BST_CHECKED);
	}

	//�����ļ�
   	if(GetPrivateProfileInt("ProtSet","IsCreateBak",0,IniFileName)==1) 
	{
		IsCreateBak =TRUE;
		CheckDlgButton(hDlg,IDC_CREATEBAK,BST_CHECKED);
	}

	//�������鹲��
   	if(GetPrivateProfileInt("ProtSet","IsDelShare",0,IniFileName)==1) 
	{
		IsDelShare =TRUE;
		CheckDlgButton(hDlg,IDC_DELSHSRE,BST_CHECKED);
	}

}

//*****************ȡ������**********************************
void GetOption(HWND hDlg)
{
	TCHAR string[32];

	//ǿ���ļ�����Ϊ200��?
	if(SendDlgItemMessage(hDlg, IDC_ALIGN_200, BM_GETCHECK, 0, 0)==BST_CHECKED)
		IsFileAlignment200=TRUE;
	else
		IsFileAlignment200=FALSE;
	wsprintf(string,"%d",IsFileAlignment200); 
	WritePrivateProfileString("ProtSet","IsFileAlignment200",string,IniFileName);
	
	//�Ƿ��������
	if(SendDlgItemMessage(hDlg, IDC_APIPROT, BM_GETCHECK, 0, 0)==BST_CHECKED)
		IsProtImpTable=TRUE;
	else
		IsProtImpTable=FALSE;
	wsprintf(string,"%d",IsProtImpTable); 
	WritePrivateProfileString("ProtSet","IsProtImpTable",string,IniFileName);
	
	//�Ƿ�ѹ����Դ
	if(SendDlgItemMessage(hDlg, IDC_PACKRES, BM_GETCHECK, 0, 0)==BST_CHECKED)
		isPackRes=TRUE;
	else
		isPackRes=FALSE;
	wsprintf(string,"%d",isPackRes); 
	WritePrivateProfileString("ProtSet","isPackRes",string,IniFileName);
	
	//�Ƿ�����������
	if(SendDlgItemMessage(hDlg, IDC_SDATA, BM_GETCHECK, 0, 0)==BST_CHECKED)
		IsSaveSData=TRUE;
	else
		IsSaveSData=FALSE;
	wsprintf(string,"%d",IsSaveSData); 
	WritePrivateProfileString("ProtSet","IsSaveSData",string,IniFileName);
	
	//�Ƿ�ϲ�����
	if(SendDlgItemMessage(hDlg, IDC_MERGESECTION, BM_GETCHECK, 0, 0)==BST_CHECKED)
		IsMergeSection=TRUE;
	else
		IsMergeSection=FALSE;
	wsprintf(string,"%d",IsMergeSection); 
	WritePrivateProfileString("ProtSet","IsMergeSection",string,IniFileName);
	
	//���������
	if(SendDlgItemMessage(hDlg, IDC_CLSSECNAME, BM_GETCHECK, 0, 0)==BST_CHECKED)
		IsClsSecName=TRUE;
	else
		IsClsSecName=FALSE;
	wsprintf(string,"%d",IsClsSecName); 
	WritePrivateProfileString("ProtSet","IsClsSecName",string,IniFileName);
	
	//����ض�λ����
	if(SendDlgItemMessage(hDlg, IDC_NOREL0, BM_GETCHECK, 0, 0)==BST_CHECKED)
		IsNoRelocation=TRUE;
	else
		IsNoRelocation=FALSE;
	wsprintf(string,"%d",IsNoRelocation); 
	WritePrivateProfileString("ProtSet","IsNoRelocation",string,IniFileName);
	
	//�����ļ�
	if(SendDlgItemMessage(hDlg, IDC_CREATEBAK , BM_GETCHECK, 0, 0)==BST_CHECKED)
		IsCreateBak=TRUE;
	else
		IsCreateBak=FALSE;
	wsprintf(string,"%d",IsCreateBak); 
	WritePrivateProfileString("ProtSet","IsCreateBak",string,IniFileName);
	
	//�������鹲��
	if(SendDlgItemMessage(hDlg, IDC_DELSHSRE , BM_GETCHECK, 0, 0)==BST_CHECKED)
		IsDelShare=TRUE;
	else
		IsDelShare=FALSE;
	wsprintf(string,"%d",IsDelShare); 
	WritePrivateProfileString("ProtSet","IsDelShare",string,IniFileName);
	
	
}



/*-------------------------------------------------------------*/
/* PropertDlgProc �� �������ô���                            */
/*-------------------------------------------------------------*/

BOOL CALLBACK PropertDlgProc (HWND hDlg, UINT message, 
                            WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{

	case WM_INITDIALOG:
		//���öԻ����ʼ��
		properinitDlgProc(hDlg);
		break;
	 }
     return FALSE ;
}


/*-------------------------------------------------------------*/
/*  IDD_STATUS �� ���ܴ���                                      */
/*-------------------------------------------------------------*/

BOOL CALLBACK StatusDlgProc (HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)

{	
	HANDLE					hThread ;
	//DWORD                 ProtFileThreadID;
	UINT                    ProtFileThreadID;

	switch (message) 
	{
	case WM_INITDIALOG:

		//������Ϣ�򻺳�
		pMessageBuffer = new TCHAR [0x10000];  
		ZeroMemory(pMessageBuffer, 0x10000); //����Ϣ��������������

		hProgress = GetDlgItem(hDlg, IDC_PROGRESS) ;

		DragAcceptFiles(hDlg,TRUE);



		break; 

	case WM_DROPFILES://֧���ļ��Ϸ�
		
		if(FALSE==ISWORKING){
			
			ZeroMemory(pMessageBuffer, 0x10000); //����Ϣ��������������
			ZeroMemory(szFilePath,MAX_PATH);//����ļ�������
			DragQueryFile((HDROP)wParam, 0, szFilePath, sizeof(szFilePath));
			DragFinish((HDROP)wParam);

			SendDlgItemMessage(hDlg,IDC_MESSAGEBOX_EDIT,WM_SETTEXT,0,0);//�����Ϣ���е���ʾ
			SendDlgItemMessage(hDlg,IDC_FILEPATH_EDIT,WM_SETTEXT,MAX_PATH,(LPARAM)szFilePath);
			AddLine(hDlg,szFilePath);
			if(!IsPEFile(szFilePath,hDlg)) 
				EnableWindow(GetDlgItem(hDlg,IDC_PROT_BUTTON), FALSE);
			else
				EnableWindow(GetDlgItem(hDlg,IDC_PROT_BUTTON), TRUE);
			SendMessage(hProgress,PBM_SETPOS, 0, 0);					
			}		
		break;

	case WM_COMMAND: 
		switch (LOWORD (wParam))
		{
		case IDC__EXIT_BUTTON:
			SendMessage(GetParent(GetParent(hDlg)), WM_CLOSE, 0, 0);

			break;
			
		//�ö�
		case IDC_ONTOP:	
			
			if(SendDlgItemMessage(hDlg, IDC_ONTOP, BM_GETCHECK, 0, 0)==BST_CHECKED)
				SetWindowPos(GetParent(GetParent(hDlg)),HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			else
				SetWindowPos(GetParent(GetParent(hDlg)),HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		
			break;

        //����			
		case IDC_PROT_BUTTON:
			EnableWindow(GetDlgItem(hDlg,IDC_PROT_BUTTON), FALSE);
			EnableWindow(GetDlgItem(hDlg,IDC_OPEN_BUTTON), FALSE);//ѹ�������д򿪰�ť������
			//ProtTheFile(hDlg);
			//����һ��������ѹ������
		//	hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)ProtTheFile, (VOID *)hDlg, NORMAL_PRIORITY_CLASS, &ProtFileThreadID);
			hThread = (HANDLE)_beginthreadex( NULL, 0, (unsigned int (_stdcall*)(void*))&ProtTheFile, (LPVOID)hDlg, 0, &ProtFileThreadID );
			CloseHandle(hThread);
	
			break;
        
	    //��Ԥ����
		case IDC_OPEN_BUTTON:
			if(!OpenFileDlg(szFilePath,hDlg))
				break;
			SendDlgItemMessage(hDlg,IDC_FILEPATH_EDIT,WM_SETTEXT,MAX_PATH,(LPARAM)szFilePath);
			AddLine(hDlg,szFilePath);
			if(!IsPEFile(szFilePath,hDlg)) 
				EnableWindow(GetDlgItem(hDlg,IDC_PROT_BUTTON), FALSE);
			else
				EnableWindow(GetDlgItem(hDlg,IDC_PROT_BUTTON), TRUE);
			SendMessage(hProgress,PBM_SETPOS, 0, 0);
			break;
		}

		return TRUE;
		break;
	}
	return FALSE;
}


//****************���Ի�����Ϣѭ��****************
int CALLBACK MainDlg (HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static int     i;
	static HWND    hwndTab;           //TAB�ؼ����
	static HWND    Child1hWnd;        //3���ӶԻ�����
	static HWND    Child2hWnd;
	static HWND    Child3hWnd;
	TC_ITEM ItemStruct;



	switch (uMsg)
	{
	case WM_CLOSE:
		if(FALSE==ISWORKING){
			delete pMessageBuffer;  // �ͷ��ڴ�
			DestroyWindow(hDlg); 		                       
		}
		else
		    MessageBox(NULL, TEXT ("����ѹ���������˳���"), TEXT ("����"), MB_OK) ;


		break;

	case WM_COMMAND: 
		switch (LOWORD (wParam))
		{
			case IDM_HELP_ABOUT :
				DialogBox (hInst, MAKEINTRESOURCE (IDD_ABOUT), hDlg, AboutDlgProc) ;
				break;

			case ID_MENU_OPEN:
				SendMessage(Child1hWnd, WM_COMMAND, (WPARAM)IDC_OPEN_BUTTON, 0);

				break;

			case ID_MENU_EXIT:
				SendMessage(hDlg, WM_CLOSE, 0, 0);
				break;


		}
		break;

	case WM_INITDIALOG:

		properinitDlgProc(hDlg);//���öԻ����ʼ��

		SendMessage(hDlg,WM_SETICON,ICON_BIG,LPARAM(LoadIcon(hInst,MAKEINTRESOURCE(IDI_ICON)))); //����ͼ��
		EnableWindow(GetDlgItem(hDlg,IDC_PROT_BUTTON), FALSE);


		InitCommonControls();
		hwndTab = GetDlgItem(hDlg, IDC_TAB1);
		ItemStruct.mask        = TCIF_TEXT;
  		ItemStruct.iImage      = 0;
 		ItemStruct.lParam      = 0;
		ItemStruct.pszText     = "����";
		ItemStruct.cchTextMax  = 4;
		SendMessage(hwndTab, TCM_INSERTITEM, 0, (LPARAM)&ItemStruct);

		ItemStruct.pszText     = "ѡ��";
		ItemStruct.cchTextMax  = 4;
		SendMessage(hwndTab, TCM_INSERTITEM, 1, (LPARAM)&ItemStruct);
		
		Child1hWnd = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_PROTECT), hwndTab, StatusDlgProc, 0);
		Child2hWnd = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_SETTING), hwndTab, PropertDlgProc , 0);

		ShowWindow(Child1hWnd, SW_SHOWDEFAULT);
		break;

	case WM_NOTIFY:      
	//2���ӶԻ������л�
		if ( *(LPDWORD)((LPBYTE)lParam+8)==TCN_SELCHANGE )
		{
		//�����������ӶԻ���
			ShowWindow(Child1hWnd, SW_HIDE);  
			ShowWindow(Child2hWnd, SW_HIDE);

			i = SendMessage(hwndTab, TCM_GETCURSEL, 0, 0);
			if (i == 0)
			{
				GetOption(Child2hWnd);//ȡ�����ò����浽�����ļ�
				ShowWindow(Child1hWnd, SW_SHOWDEFAULT);
			}
			else if (i == 1)
			{
				ShowWindow(Child2hWnd, SW_SHOWDEFAULT);
			}
	
		}
		break;

	default:
		break;
	}

	return 0;
}
