#pragma once
#include <Windows.h>
#include <atlstr.h>
#include <vector>
using std::vector;
#include"tchar.h"

class CSoftwareInfo {
public:
	CString m_Name;
	CString m_SoftUniPath;
	CString m_SoftInsPath;
	CString m_SoftDate;
	CString m_SoftVer;
	CString m_SoftIconPath;
	CString m_SoftUrl;

};

/*�ض��巵������*/
typedef vector<CSoftwareInfo>  VSoftAll;


class CSoftware
{
public:
	CSoftware();
	~CSoftware();
	/*��ȡ����б�*/
	bool GetSoftwareList(VSoftAll *vSoftInfos);
	/*ж�����*/
	bool UnInstallSoftware(CString cSoftUnPath);
	/*ָ��Ŀ¼��ʾ��Դ������*/
	bool ShowSoftwareDir(CString cPath);

	/*��ȡ�ĵ�ǰ����ϵͳ�汾*/
	BOOL Is64BitSystem();

};

