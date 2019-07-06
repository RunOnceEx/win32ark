#include "pch.h"
#include "CPrivilege.h"
#include <Shlobj.h>
#pragma comment(lib,"Shell32.lib")


CPrivilege::CPrivilege()
{
}


CPrivilege::~CPrivilege()
{
}

BOOL CPrivilege::EnumPrivileges(VPrivilegeAll *PrivilegeInfos, HANDLE hanlde)
{
	//��ȡ�������ƾ��
	HANDLE hToken;
	CPrivilegeInfo info;
	OpenProcessToken(hanlde, TOKEN_QUERY, &hToken);
	if (!hToken)
	{
		return FALSE;
	}
	//��ѯ�����е�Ȩ��
	DWORD dwSize;
	//��һ�λ�ȡ��ѯ���ݵĴ�С
	GetTokenInformation(hToken,
		TokenPrivileges, NULL, NULL, &dwSize);
	//�ڶ��θ����ϴλ�ȡ�Ĵ�С�����㹻����ڴ��
	//���ڴ��ַ����ȥ��ȡ��Ҫ������
	char* pBuf = new char[dwSize] {};
	GetTokenInformation(hToken,
		TokenPrivileges, pBuf, dwSize, &dwSize);
	//���ڴ��е�������Ҫ��ѯ���ݽṹ�����
	//������Ȩ�޵Ľṹ��
	TOKEN_PRIVILEGES* pTp = (TOKEN_PRIVILEGES*)pBuf;

	//Ȩ�޸���
	DWORD dwCount = pTp->PrivilegeCount;
	//ָ��Ȩ�������׵�ַ
	LUID_AND_ATTRIBUTES* pLaa = pTp->Privileges;
	//���Ȩ��
	for (int i = 0; i < dwCount; i++, pLaa++)
	{
		TCHAR szName[100] = {};
		DWORD dwLen = sizeof(szName);
		//��ѯ��Ȩ�޶�Ӧ���ַ���
		LookupPrivilegeName(0, &pLaa->Luid, szName, &dwLen);
		//״̬,0:�رգ�1��Ĭ��ֵ��2:������3��Ĭ�Ͽ��� 
		//printf("[%s] -- ״̬[%d]\n", szName, pLaa->Attributes);
		info.m_Name = szName;
		info.m_Status.Format(L"%d", pLaa->Attributes);
		PrivilegeInfos->push_back(info);
	}
	delete pBuf;
	return TRUE;
}

BOOL CPrivilege::QueryPrivileges(HANDLE hanlde)
{
	// 1. ��ñ����̵�����
	HANDLE hToken = NULL;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken))
		return false;
	// 2. ��ȡ��������
	TOKEN_ELEVATION_TYPE ElevationType = TokenElevationTypeDefault;
	BOOL                 bIsAdmin = false;
	DWORD                dwSize = 0;


	if (GetTokenInformation(hToken, TokenElevationType, &ElevationType,
		sizeof(TOKEN_ELEVATION_TYPE), &dwSize)) {
		// 2.1 ��������Ա��Ķ�ӦSID
		BYTE adminSID[SECURITY_MAX_SID_SIZE];
		dwSize = sizeof(adminSID);
		CreateWellKnownSid(WinBuiltinAdministratorsSid, NULL, &adminSID, &dwSize);


		// 2.2 �жϵ�ǰ���������û���ɫ�Ƿ�Ϊ����Ա
		if (ElevationType == TokenElevationTypeLimited) {

			// a. ��ȡ�������Ƶľ��
			HANDLE hUnfilteredToken = NULL;
			GetTokenInformation(hToken, TokenLinkedToken, (PVOID)&hUnfilteredToken,
				sizeof(HANDLE), &dwSize);

			// b. ������ԭʼ�������Ƿ��������Ա��SID
			if (!CheckTokenMembership(hUnfilteredToken, &adminSID, &bIsAdmin))
				return false;
			CloseHandle(hUnfilteredToken);
		}
		else {
			bIsAdmin = IsUserAnAdmin();
		}
		CloseHandle(hToken);
	}
	// 3. �жϾ����Ȩ��״��
	BOOL bFullToken = false;
	switch (ElevationType) {
	case TokenElevationTypeDefault: /* Ĭ�ϵ��û���UAC������ */
		if (IsUserAnAdmin())  bFullToken = true; // Ĭ���û��й���ԱȨ��
		else                  bFullToken = false;// Ĭ���û����ǹ���Ա��
		break;
	case TokenElevationTypeFull:    /* �Ѿ��ɹ���߽���Ȩ�� */
		if (IsUserAnAdmin())  bFullToken = true; //��ǰ�Թ���ԱȨ������
		else                  bFullToken = false;//��ǰδ�Թ���ԱȨ������
		break;
	case TokenElevationTypeLimited: /* �����������޵�Ȩ������ */
		if (bIsAdmin)  bFullToken = false;//�û��й���ԱȨ�ޣ�������Ȩ������
		else           bFullToken = false;//�û����ǹ���Ա�飬�ҽ���Ȩ������
	}
	return bFullToken;
}

BOOL CPrivilege::EnablePrivileges(BOOL Enable,const TCHAR * szPrivilege, HANDLE hanlde)
{
	BOOL fOk = FALSE;    
	HANDLE hToken;
	// ���޸�Ȩ�޵ķ�ʽ���򿪽��̵�����
	if (OpenProcessToken(hanlde, TOKEN_ADJUST_PRIVILEGES,
		&hToken)) {
		// ����Ȩ�޽ṹ��
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount = 1;
		//���LUID
		LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &tp.Privileges[0].Luid);
		//�������߹ر�
		tp.Privileges[0].Attributes = Enable ? SE_PRIVILEGE_ENABLED : 0;
		//�޸�Ȩ��
		AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
		fOk = (GetLastError() == ERROR_SUCCESS);
		CloseHandle(hToken);
	}
	return(fOk);
}

BOOL CPrivilege::StartAdminRun(TCHAR *szPath)
{
	//  �Թ���ԱȨ�����´򿪽���
	SHELLEXECUTEINFO sei = { sizeof(SHELLEXECUTEINFO) };
	sei.lpVerb = L"runas";      // ��������Ȩ��
	sei.lpFile = szPath;		// ��ִ���ļ�·��
	sei.lpParameters = NULL;          // ����Ҫ����
	sei.nShow = SW_SHOWNORMAL; // ������ʾ����
	return ShellExecuteEx(&sei);
}

BOOL CPrivilege::AddAdminIcon(HWND hWnd, DWORD dwId)
{
	Button_SetElevationRequiredState(::GetDlgItem(hWnd, dwId), 1);
	return 0;
}

BOOL CPrivilege::AddAdminDrag()
{
	ChangeWindowMessageFilter(WM_DROPFILES, MSGFLT_ADD);
	ChangeWindowMessageFilter(0x0049, MSGFLT_ADD);
	return 1;
}
