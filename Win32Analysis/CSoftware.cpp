#include "pch.h"
#include "CSoftware.h"

//64λ��װ·��
#define  X86_SUBKEY  _T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall")
//32λ��װ·��
#define  X64_SUBKEY _T("Software\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall")



CSoftware::CSoftware()
{
}


CSoftware::~CSoftware()
{
}

bool CSoftware::GetSoftwareList(VSoftAll * vSoftInfos)
{
	//1.����λ��
	HKEY RootKey = HKEY_LOCAL_MACHINE;
	//���ս�Ҫ�򿪵ļ����
	HKEY hkResult = 0;


	//2.1Ĭ��64λϵͳ�Ӽ�λ��
	LPCTSTR  lpSubKey = X64_SUBKEY;
	//2.2���ϵͳλ��
	if (!Is64BitSystem())
		lpSubKey = X86_SUBKEY;

	//3.��ע����
	LONG lReturn = RegOpenKeyEx(
		RootKey,
		lpSubKey,
		0,
		KEY_ENUMERATE_SUB_KEYS | KEY_QUERY_VALUE,
		&hkResult
	);

	//�����±�
	DWORD dwIndex = 0;

	//4.ѭ������UninstallĿ¼�µ��Ӽ�
	while (TRUE)
	{
		CSoftwareInfo info;
		DWORD dwkeyLen = 255;
		//ע�������
		WCHAR szNewKeyName[MAX_PATH] = {};
		//ö��ע�������
		LONG lReturn = RegEnumKeyEx(
			hkResult,									//	�Ӽ����
			dwIndex,									//�ӽ�������
			szNewKeyName,						//ָ�򱣴��ӽ�����
			&dwkeyLen,								//�ӽ����Ƴ���
			0, NULL, NULL, NULL);
		if (lReturn != ERROR_SUCCESS)
		{
			break;
		}


		//5.����ӽ�����������
		WCHAR strMidReg[MAX_PATH] = {};
		swprintf_s(strMidReg, L"%s%s%s", lpSubKey, L"\\", szNewKeyName);

		//6.�����Ӽ� ��þ��
		HKEY hkValueKey = 0;
		RegOpenKeyEx(RootKey, strMidReg, 0, KEY_QUERY_VALUE, &hkValueKey);
		if (hkValueKey == NULL)
		{
			dwIndex++;
			continue;
		}

		//7.��ȡ��ֵ
		DWORD dwNameLen = 0;
		DWORD dwType = 0;
		//8.��ȡ�������
		TCHAR szBuff[260] = {};
		lReturn = RegQueryValueEx(hkValueKey, L"DisplayName", 0, 0, 0, &dwNameLen);
		if (lReturn == ERROR_SUCCESS) {
			RegQueryValueEx(hkValueKey, L"DisplayName", 0, &dwType, (PBYTE)szBuff, &dwNameLen);
			info.m_Name = szBuff;
		}
		else {
			info.m_Name = szNewKeyName;
		}
		//ж��·��
		dwType = 0;
		RegQueryValueEx(hkValueKey, L"Uninstallstring", 0, 0, 0, &dwNameLen);
		if (dwNameLen != 0) {
		RegQueryValueEx(hkValueKey, L"Uninstallstring", 0, &dwType, (LPBYTE)szBuff, &dwNameLen);
		info.m_SoftUniPath = szBuff;
	}

		//���·��
		dwType = 0;
		RegQueryValueEx(hkValueKey, L"DisplayIcon", 0, 0, 0, &dwNameLen);
		if (dwNameLen != 0) {
			RegQueryValueEx(hkValueKey, L"DisplayIcon", 0, &dwType, (LPBYTE)szBuff, &dwNameLen);
			info.m_SoftIconPath = szBuff;
		}

		dwType = 0;
		RegQueryValueEx(hkValueKey, L"DisplayVersion", 0,0,0, &dwNameLen);
		if (dwNameLen != 0) {
			RegQueryValueEx(hkValueKey, L"DisplayVersion", 0, &dwType, (LPBYTE)szBuff, &dwNameLen);//�汾��
			info.m_SoftVer = szBuff;
		}

		dwType = 0;
		RegQueryValueEx(hkValueKey, L"InstallDate", 0, 0, 0, &dwNameLen);
		if (dwNameLen != 0) {
			RegQueryValueEx(hkValueKey, L"InstallDate", 0, &dwType, (LPBYTE)szBuff, &dwNameLen);//��װ����
			info.m_SoftDate = szBuff;
		}

		dwType = 0;
		RegQueryValueEx(hkValueKey, L"URLInfoAbout", 0,0,0, &dwNameLen);
		if (dwNameLen != 0) {
			RegQueryValueEx(hkValueKey, L"URLInfoAbout", 0, &dwType, (LPBYTE)szBuff, &dwNameLen);//��ַ
			info.m_SoftUrl = szBuff;
		}

		dwType = 0;
		RegQueryValueEx(hkValueKey, L"InstallLocation", 0,0,0, &dwNameLen);
		if (dwNameLen != 0) {
			RegQueryValueEx(hkValueKey, L"InstallLocation", 0, &dwType, (LPBYTE)szBuff, &dwNameLen);//��װ·��
			info.m_SoftInsPath = szBuff;
		}
		//������һ����
		dwIndex++;
		RegCloseKey(hkValueKey);
		vSoftInfos->push_back(info);

	}

	RegCloseKey(hkResult);

	return false;
}

bool CSoftware::UnInstallSoftware(CString cSoftUnPath)
{
	ShellExecute(NULL, L"open", cSoftUnPath, 0, 0, SW_SHOW);
	return false;
}

bool CSoftware::ShowSoftwareDir(CString cPath)
{
	ShellExecute(NULL, L"explore", cPath, 0, 0, SW_SHOW);
	return false;
}

BOOL CSoftware::Is64BitSystem()
{

	SYSTEM_INFO si = { 0 };
	typedef void (WINAPI *LPFN_PGNSI)(LPSYSTEM_INFO);
	LPFN_PGNSI pGNSI = (LPFN_PGNSI)GetProcAddress(GetModuleHandleA(("kernel32.dll")), "GetNativeSystemInfo");
		if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 ||
			si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64)
		{
			return TRUE;
		}
		return FALSE;

	return 0;
}
