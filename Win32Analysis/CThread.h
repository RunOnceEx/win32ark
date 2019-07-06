#pragma once
#include<atlstr.h>
#include <vector>
using std::vector;
#include"UnDocFunation.h"



/*������Ϣ�ṹ��*/
class ThInfo {
public:
	CString m_Tid;
	CString m_Start;
	CString m_Teb;
	CString m_Status;

};

/*�ض��巵������*/
typedef vector<ThInfo>  VThAll;

class CThread
{
public:
	CThread();
	~CThread();

	/*��ȡ�߳���Ϣ*/
	bool GetThreadInfo(VThAll * threadinfos, int pid);

	/*��ȡ�߳���ڵ�*/
	DWORD GetThreadStartAddress(HANDLE hThread);

	/*��ȡ�߳�Teb*/
	DWORD GetThreadTed(HANDLE hThread);

	/*�����߳�*/
	bool SuspendTheThread(DWORD dwTid);

	/*�ָ��߳�*/
	bool ResumeTheThread(DWORD dwTid);

	/*��ȡ��ǰ״̬ true �������У�false ����״̬*/
	bool GetThreadStatus(DWORD dwTid);

};

