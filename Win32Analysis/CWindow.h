#pragma once
#include<atlstr.h>
#include <vector>
using std::vector;


class WndInfo{
public:
	CString m_Name;
	CString m_ClassName;
	bool m_IsShowWnd;
	vector<WndInfo> m_ChildWnds;
};
typedef vector<WndInfo> VWndAll;




class CWindow
{
public:
	CWindow();
	~CWindow();

	/*��ȡ������Ϣ*/
	bool GetWindowInfo(VWndAll& WndInfos);

	/*ö���Ӵ��ڻص�����*/
	static BOOL CALLBACK EnumChildProc( HWND   hwnd,  LPARAM lParam);

	/*ö�ٶ������ڻص�����*/
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);



};

