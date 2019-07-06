#include "pch.h"
#include "CThread.h"


CThread::CThread()
{
}


CThread::~CThread()
{
}

bool CThread::GetThreadInfo(VThAll * threadinfos, int pid)
{
	//1�������̿���
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	//2��ʱ������Ϣ
	THREADENTRY32 th = { sizeof(THREADENTRY32) };
	//3�������н���
	Thread32First(hSnap, &th);

	do {
		//����ָ������
		if (th.th32OwnerProcessID == pid)
		{
			ThInfo thInfo;
			thInfo.m_Tid.Format(L"%8d",th.th32ThreadID);

			HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, th.th32ThreadID);
			if (hThread != INVALID_HANDLE_VALUE && hThread != NULL)
			{
				//��ȡ�߳���ڵ�
				thInfo.m_Start.Format(L"0X%08X", GetThreadStartAddress(hThread));
				/*��ȡTEB*/
				thInfo.m_Teb.Format(L"0X%08X", GetThreadTed(hThread));
				/*��ȡ״̬*/
				if (GetThreadStatus(th.th32ThreadID))
					thInfo.m_Status = L"������";
				else
					thInfo.m_Status = L"����";

			}	
			threadinfos->push_back(thInfo);
			CloseHandle(hThread);
		}
	} while (Thread32Next(hSnap, &th));



	return false;
}

DWORD CThread::GetThreadStartAddress(HANDLE hThread)
{
	/*����δ�ĵ�����*/
	DWORD lpAddr;
	ULONG ulSize;
	/*��ȡ�߳����ַ*/
	NtQueryInformationThread(
		hThread,
		(THREADINFOCLASS)UnThreadQuerySetWin32StartAddress,
		&lpAddr,
		sizeof(lpAddr),
		&ulSize);

	return lpAddr;
}

DWORD CThread::GetThreadTed(HANDLE hThread)
{
	/*����δ�ĵ�����*/
	THREAD_BASIC_INFORMATION info = { 0 };
	ULONG ulSize;
	/*��ȡ�߳����ַ*/
	NtQueryInformationThread(
		hThread,
		(THREADINFOCLASS)UnThreadBasicInformation,
		&info,
		sizeof(info),
		&ulSize);

	return (DWORD)info.TebBaseAddress;
}

bool CThread::SuspendTheThread(DWORD dwTid)
{
	/*��ȡ���̾��*/
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwTid);
	if (hThread != INVALID_HANDLE_VALUE && hThread != NULL)
	{
		SuspendThread(hThread);
		CloseHandle(hThread);
		return true;
	}
	return false;
}

bool CThread::ResumeTheThread(DWORD dwTid)
{
	/*��ȡ���̾��*/
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwTid);
	if (hThread != INVALID_HANDLE_VALUE && hThread != NULL)
	{
		ResumeThread(hThread);
		CloseHandle(hThread);
		return true;
	}
	return false;
}

bool CThread::GetThreadStatus(DWORD dwTid)
{
	/*��ȡ���̾��*/
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwTid);
	if (hThread != INVALID_HANDLE_VALUE && hThread != NULL)
	{
		/*���ù���ʱ�����ϴ����ô������ж��Ƿ񱻹���*/
		DWORD dwSuspendCount = SuspendThread(hThread);
		ResumeThread(hThread);
		CloseHandle(hThread);
		/*���ص�ǰ״̬ 0 ��ʾ�������У������ʾ����*/
		return !dwSuspendCount;
	}
	return true;
}
