#include "pch.h"
#include "CProcess.h"



/*��ȡ������Ϣ*/
bool CProcess::GetProcessInfo(VPsAll* psinfo)
{
	//1�������̿���
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	//2��ʱ������Ϣ
	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	//3�������н���
	Process32First(hSnap, &pe);

	do
	{
		PSInfo ps;
		//���������Ϣ
		ps.m_Name.Format(L"%s", pe.szExeFile);
		ps.m_Pid.Format(L"%8d",pe.th32ProcessID);
		ps.m_Ppid.Format(L"%8d", pe.th32ParentProcessID);


		//��ȡ����·��
		HANDLE hprocess = OpenProcess(
			PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ,
			FALSE, pe.th32ProcessID);
		//�򿪽���ʧ��
		if (hprocess == INVALID_HANDLE_VALUE || hprocess == NULL)
		{
			//δ֪
			ps.m_Cmdline = "";
			ps.m_Path = "";
		}
		else {
			//��ȡ�����в���
			ps.m_Cmdline = GetProcessCmdline(hprocess);
			//��ȡ����·��
			wchar_t exepath[260] = {};
			DWORD dwSize = 260;
			QueryFullProcessImageNameW(hprocess, 0, exepath, &dwSize);
			ps.m_Path = exepath;

			//��ȡ����PEB
			ps.m_Peb = GetProcessPeb(hprocess);

		}
		psinfo->push_back(ps);
	} while (Process32Next(hSnap, &pe));

	CloseHandle(hSnap);



	return true;
}

/*��ȡ����������*/
CString CProcess::GetProcessCmdline(HANDLE hProcess)
{
	// ��ȡPEB
	PROCESS_BASIC_INFORMATION info;
	ULONG uSize;
	NtQueryInformationProcess(
		hProcess,
		ProcessBasicInformation,
		&info, sizeof(info),
		&uSize
	);

	LPVOID puser_addr;
	_RTL_USER_PROCESS_PARAMETERS user_info;
	DWORD dwSize;

	//��ȡ�����û���Ϣ��ַ
	LPVOID pAddr = (LPVOID)((int)info.PebBaseAddress + 0x10);
	if (info.PebBaseAddress == 0)
	{
		return "";
	}
	if (!ReadProcessMemory(hProcess, pAddr, &puser_addr, sizeof(puser_addr), &dwSize))
	{
		return "";
	}

	// ��ȡ�����û���Ϣ����
	if (!ReadProcessMemory(hProcess, puser_addr, &user_info, sizeof(user_info), &dwSize))
	{
		return "";
	}


	//��ȡ�ַ���
	wchar_t wPath[260] = {};

	pAddr = (LPVOID)((int)user_info.CommandLine.Buffer);
	if (!ReadProcessMemory(hProcess, pAddr, &wPath, 260, &dwSize))
	{
		return "";
	}

	//��ȡ������
	return wPath;
}

CString CProcess::GetProcessPeb(HANDLE hProcess)
{
	// ��ȡPEB
	PROCESS_BASIC_INFORMATION info;
	ULONG uSize;
	NtQueryInformationProcess(
		hProcess,
		ProcessBasicInformation,
		&info, sizeof(info),
		&uSize
	);
	CString csPeb;
	csPeb.Format(L"%08X", info.PebBaseAddress);
	return csPeb;
}
