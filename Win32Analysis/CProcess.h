#pragma once
#include <Windows.h>
#include <vector>
#include <atlstr.h>
#include "UnDocFunation.h"
#include<TlHelp32.h>
using std::vector;


/*������Ϣ�ṹ��*/
class PSInfo {
public:
	CString m_Path;
	CString m_Name;
	CString m_Pid;
	CString m_Ppid;
	CString m_Cmdline;
	CString m_Peb;
	
};

/*�ض��巵������*/
typedef vector<PSInfo>  VPsAll;

class CProcess
{
	
public:
	/*��ȡ������Ϣ*/
	bool GetProcessInfo(VPsAll *psinfo);
	/*��ȡ����������*/
	CString GetProcessCmdline(HANDLE hProcess);
	/*��ȡ����PEB*/
	CString GetProcessPeb(HANDLE hProcess);


};

