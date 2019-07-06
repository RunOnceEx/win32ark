#include "pch.h"

#include "CServer.h"


CServer::CServer()
{
}


CServer::~CServer()
{
}

/*��ȡ�����б�*/
bool CServer::GetServerList(VServerAll * vServerInfos)
{
	//1.�򿪷�������������ط���Ĭ�����ݿ⣬Ȩ������
	SC_HANDLE hSCM = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	//2.��һ�ε��� ��Ҫ�ڴ��С
	DWORD dwServiceNum = 0;
	DWORD dwSize = 0;

	EnumServicesStatusEx(
		hSCM,										//������
		SC_ENUM_PROCESS_INFO,		//���ص����� Ĭ�ϵĶ������ֵ
		SERVICE_WIN32,						//��������   Ӧ�ó���ķ���
		SERVICE_STATE_ALL,				//����״̬������
		NULL,										//������
		0,												//��������С
		&dwSize,									//��Ҫ�Ĵ�С
		&dwServiceNum,						//�������ķ������
		NULL,										//Ĭ��0
		NULL										//Ĭ��0
	);
	//3 ������Ҫ���ڴ��С
	LPENUM_SERVICE_STATUS_PROCESS pEnumSerice = (LPENUM_SERVICE_STATUS_PROCESS)new char[dwSize];

	//4.�ڶ���ö��
	BOOL bstatus = FALSE;
	bstatus = EnumServicesStatusEx(
		hSCM,										//������
		SC_ENUM_PROCESS_INFO,		//���ص����� Ĭ�ϵĶ������ֵ
		SERVICE_WIN32,						//��������   Ӧ�ó���ķ���
		SERVICE_STATE_ALL,				//����״̬������
		(PBYTE)pEnumSerice,				//������
		dwSize,									//��������С
		&dwSize,
		&dwServiceNum,
		NULL, NULL
	);

	//5������Ϣ
	for (DWORD i = 0; i < dwServiceNum; i++)
	{

		CServerInfo ServiceInfo;
		////��ȡ������Ϣ
		////������
		//pEnumSerice[i].lpServiceName;
		////����״̬   ״̬��ֹͣ ����  ��ͣ
		//pEnumSerice[i].ServiceStatusProcess.dwCurrentState;
		////��������  ���ͣ��ļ��������� �������� �������̷���
		//pEnumSerice[i].ServiceStatusProcess.dwServiceType;


		//6.������ϸ��Ϣ
			//�򿪷���
		SC_HANDLE hService = OpenService(hSCM,
			pEnumSerice[i].lpServiceName,		//������
			SERVICE_QUERY_CONFIG				//��Ȩ��
		);
		//��һ��1��ȡ��������С
		QueryServiceConfig(hService, NULL, 0, &dwSize);
		//�����ڴ�
		LPQUERY_SERVICE_CONFIG pServiceConfig =
			(LPQUERY_SERVICE_CONFIG)new char[dwSize];
		//�ڶ��ε���
		QueryServiceConfig(hService, pServiceConfig, dwSize, &dwSize);
		//����״̬����
		ServiceInfo.m_Status.Format(L"%s", GetServerStatus(pEnumSerice[i].ServiceStatusProcess.dwCurrentState));
		//������������
		ServiceInfo.m_StartType.Format(L"%s", GetServerStartType(pServiceConfig->dwStartType));

		ServiceInfo.m_Path.Format(L"%s", pServiceConfig->lpBinaryPathName);							//����ȫ·��

		ServiceInfo.m_DisplayName.Format(L"%s", pServiceConfig->lpDisplayName);										//������ʾ��

		ServiceInfo.m_StartName.Format(L"%s", pServiceConfig->lpServiceStartName);							//����������

		ServiceInfo.m_Name.Format(L"%s", pEnumSerice[i].lpServiceName);											//�����ķ�����

		vServerInfos->push_back(ServiceInfo);
		CloseServiceHandle(hService);
	}

	//�رշ������
	CloseServiceHandle(hSCM);

	return true;
}

const TCHAR * CServer::GetServerStartType(DWORD StartGen)
{
	switch (StartGen)
	{
	case SERVICE_AUTO_START:
		return _T("������ƹ�������ϵͳ����ʱ�Զ���������");
	case SERVICE_BOOT_START:
		return _T("��ϵͳװ�س����������豸��������,��ֵ�����������������");
	case SERVICE_DEMAND_START:
		return _T("����ʼ�ɷ�����ƹ�����,��һ�����̵���startService����");
	case SERVICE_DISABLED:
		return _T("���������ķ���.������������,���´���Ĵ���error_service_disabled");
	default:
		break;
	}
	return _T("��ioinitsystem���������豸�������򡣴�ֵ�����������������");
}

const TCHAR * CServer::GetServerStatus(DWORD ServerGen)
{
	switch (ServerGen)
	{
	case SERVICE_CONTINUE_PENDING:
		return _T("��������");
	case SERVICE_PAUSE_PENDING:
		return _T("��ͣ����");
	case SERVICE_PAUSED:
		return _T("��ͣ");
	case SERVICE_RUNNING:
		return _T("��������");
	case SERVICE_START_PENDING:
		return _T("��������");
	case SERVICE_STOP_PENDING:
		return _T("����ֹͣ");
	default:
		break;
	}
	return _T("δ����");
}

const TCHAR * CServer::GetServerType(DWORD GenreNow)
{
	switch (GenreNow)
	{
	case SERVICE_FILE_SYSTEM_DRIVER:
		return _T("�ļ�ϵͳ��������");
	case SERVICE_KERNEL_DRIVER:
		return _T("�豸��������");
	case SERVICE_WIN32_OWN_PROCESS:
		return _T("�÷������������Լ��Ĺ�����");
	default:
		break;
	}
	return _T("�÷���������������һ������");
}


bool CServer::CreateTheServer(TCHAR * szName, TCHAR * szDispalyName, TCHAR * szPath, DWORD StartType, DWORD ServiceType, SC_HANDLE hSCManager)
{
	if (hSCManager == NULL)
	{
		 hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
	}
	DWORD ErrorControl = 0;
	if (hSCManager != NULL) {
		SC_HANDLE hService = CreateService(hSCManager, szName, szDispalyName, SERVICE_ALL_ACCESS, ServiceType, StartType, ErrorControl, szPath, NULL, NULL, NULL, NULL, TEXT(""));
		if (hService != NULL) {
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return TRUE;
		}
		CloseServiceHandle(hSCManager);
		return FALSE;
	}
	else return FALSE;
}

bool CServer::DeleteTheServer(TCHAR * szName, SC_HANDLE hSCManager)
{
	if (hSCManager == NULL)
	{
		hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	}
	/*�򿪷��������*/

	if (hSCManager != NULL) {
		/*�򿪷���*/
		SC_HANDLE hService = OpenService(hSCManager, szName, DELETE);
		if (hService != NULL) {
			/*ɾ������*/
			if (DeleteService(hService))
			{
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				return TRUE;
			}
			/*ɾ��ʧ��*/
			/*ֹͣ������ɾ��*/
			StopTheServer(szName, hSCManager);
			Sleep(500);
			DeleteService(hService);
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return FALSE;
		}
		CloseServiceHandle(hSCManager);
		return FALSE;
	}
	else return FALSE;
}

bool CServer::StartTheServer(TCHAR * szName, SC_HANDLE hSCManager)
{
	if (hSCManager == NULL)
	{
		 hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CONNECT);
	}
	if (hSCManager != NULL) {
		SC_HANDLE hService = OpenService(hSCManager, szName, SERVICE_START);
		if (hService != NULL) {
			if (StartService(hService, NULL, NULL))
			{
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				return TRUE;
			}
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return FALSE;
		}
		CloseServiceHandle(hSCManager);
		return FALSE;
	}
	else return FALSE;
}

bool CServer::StopTheServer(TCHAR * szName, SC_HANDLE hSCManager)
{
	if (hSCManager == NULL)
	{
		 hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	}

	if (hSCManager != NULL) {
		SC_HANDLE hService = OpenService(hSCManager, szName, SERVICE_STOP);
		if (hService != NULL)
		{
			SERVICE_STATUS status;
			if (ControlService(hService, SERVICE_CONTROL_STOP, &status))
			{
				CloseServiceHandle(hService);
				CloseServiceHandle(hSCManager);
				return TRUE;
			}
			CloseServiceHandle(hService);
			CloseServiceHandle(hSCManager);
			return FALSE;
		}
		CloseServiceHandle(hSCManager);
		return FALSE;
	}
	else return FALSE;
}
