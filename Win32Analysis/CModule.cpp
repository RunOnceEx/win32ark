#include "pch.h"
#include "CModule.h"
#include <Psapi.h>
#include <shlwapi.h>
#pragma comment(lib,"Psapi.lib")
#pragma comment(lib,"shlwapi.lib")


CModule::CModule()
{
}


CModule::~CModule()
{
}

bool CModule::GetModuleInfo(VModAll * ModuleInfos, DWORD dwPid)
{
	HMODULE Modlist[1000];
	DWORD nSize = 0;
	wchar_t fileName[260] = { 0 };
	MODULEINFO mdinfo = {};

	/*�򿪽���*/
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);

	if (hProcess != NULL && hProcess != INVALID_HANDLE_VALUE)
	{
		/*��ȡ��ǰ����ģ���б�*/
		EnumProcessModules(hProcess, 0,0, &nSize);
		EnumProcessModules(hProcess, Modlist, nSize, &nSize);
		/*����ģ����Ϣ*/
		for (int i = 0; i < nSize / sizeof(HMODULE); i++)
		{
			ModuleInfo mdInfo;
			/*��ȡģ�������Ϣ*/
			GetModuleFileNameEx(hProcess, Modlist[i], fileName, sizeof(fileName));


			//dll����
			mdInfo.m_Name = PathFindFileName(fileName);
			//·����
			mdInfo.m_Path = fileName;
			//�����ַ��С��ڵ�ַ
			GetModuleInformation(hProcess, Modlist[i], &mdinfo, sizeof(mdinfo));
			mdInfo.m_BaseImage.Format(L"0X%08X",mdinfo.lpBaseOfDll);
			mdInfo.m_Size.Format(L"0X%08X", mdinfo.SizeOfImage);
			mdInfo.m_Entry.Format(L"0X%08X", mdinfo.EntryPoint);

			//���浽�б���
			ModuleInfos->push_back(mdInfo);
		}

	}
	CloseHandle(hProcess);


	return false;
}
