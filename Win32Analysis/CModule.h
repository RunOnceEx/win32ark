#pragma once
#include<atlstr.h>
#include <vector>
using std::vector;
#include"UnDocFunation.h"

/*ģ����Ϣ�ṹ��*/
class ModuleInfo {
public:
	CString m_Name;
	CString m_Path;
	CString m_BaseImage;
	CString m_Size;
	CString m_Entry;

};

/*�ض��巵������*/
typedef vector<ModuleInfo>  VModAll;



class CModule
{
public:
	CModule();
	~CModule();

	/*��ȡģ����Ϣ*/
	bool GetModuleInfo(VModAll *ModuleInfos,DWORD dwPid);

};

