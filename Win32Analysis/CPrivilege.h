#pragma once
#include <Windows.h>
#include <atlstr.h>
#include <vector>
using std::vector;
#include"tchar.h"

class CPrivilegeInfo {
public:
	CString m_Name;
	CString m_Status;
};

/*�ض��巵������*/
typedef vector<CPrivilegeInfo>  VPrivilegeAll;



class CPrivilege
{
public:
	CPrivilege();
	~CPrivilege();

	/*����Ȩ��*/
	BOOL EnumPrivileges(VPrivilegeAll *PrivilegeInfos,HANDLE hanlde = INVALID_HANDLE_VALUE);

	/*Ȩ�޲�ѯ �Ƿ�ӵ�й���Ա����Ȩ��*/
	BOOL QueryPrivileges(HANDLE hanlde = INVALID_HANDLE_VALUE);

	/*����Ȩ��*/
	BOOL EnablePrivileges(BOOL Enable,const TCHAR *szPrivilege, HANDLE hanlde = INVALID_HANDLE_VALUE);

	/*����Ա����*/
	BOOL StartAdminRun(TCHAR *szPath);

	/*��ӹ���Աͼ��*/
	BOOL AddAdminIcon(HWND hWnd, DWORD dwId);

	/*����Ա������ק*/
	BOOL AddAdminDrag();



};

