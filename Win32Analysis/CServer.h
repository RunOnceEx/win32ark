#pragma once
#include <atlstr.h>
#include <vector>
using std::vector;
#include"tchar.h"

class CServerInfo {
public:
	CString m_Name;
	CString m_StartName;
	CString m_Status;
	CString m_Type;
	CString m_StartType;
	CString m_DisplayName;
	CString m_Path;
};

/*�ض��巵������*/
typedef vector<CServerInfo>  VServerAll;

class CServer
{
public:
	CServer();
	~CServer();

	/*��ȡ��ǰ����*/
	bool GetServerList(VServerAll *vServerInfos);
	/*��ȡ��������*/
	const TCHAR* GetServerStartType(DWORD StartGen);
	/*��ȡ��ǰ����״̬*/
	const TCHAR* GetServerStatus(DWORD ServerGen);
	/*��ȡ��ǰ����*/
	const TCHAR* GetServerType(DWORD GenreNow);


	/*��������*/
	bool CreateTheServer(
		TCHAR *szName,
		TCHAR *szDispalyName,
		TCHAR *szPath,
		DWORD StartType,
		DWORD ServiceType,
		SC_HANDLE hSCManager = 0);

	/*�رշ���*/
	bool DeleteTheServer(TCHAR *szName, SC_HANDLE hSCManager = 0);

	/*��������*/
	bool StartTheServer(TCHAR *szName, SC_HANDLE hSCManager = 0);

	/*ֹͣ����*/
	bool StopTheServer(TCHAR *szName,SC_HANDLE hSCManager = 0);


};

