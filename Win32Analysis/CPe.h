#pragma once
#include <Windows.h>
#include <atlstr.h>
#include <vector>
using std::vector;

class CPe
{
public:

	// ������
	struct ExportInfo {
		DWORD Order;
		DWORD Rva;
		DWORD Foa;
		char FunName[100];
	}*PEXPORTINFO;


	//IAT��
	struct IATInfo_Item {
		char FunName[250];
		DWORD Order;
		DWORD RVA;
	};

	struct IATInfo {
		char dllName[250];
		vector<IATInfo_Item> vec_item;
	}*PIATINFO;


	// ��Դname ,��Դ��RVA,FOA,Size
	struct ResourceInfo_Item {
		char  ResName[100];
		DWORD Rva;
		DWORD Foa;
		DWORD Size;
	};

	struct ResourceInfo {
		char ResTypeName[100];
		vector<ResourceInfo_Item> ItemInfo;
	};


	//�ض�λ������
	struct RelocationInfo {
		DWORD BaseRVA;
		DWORD offset;
		DWORD offsetFoa;
		DWORD RelocationData; //�ض�λ������
	};





	CPe(TCHAR *pSzName);

	//����pe
	BOOL AnalysisPe();

	//��ȡDOSͷ
	PIMAGE_DOS_HEADER GetDosHeader();

	//��ȡNTͷ
	PIMAGE_NT_HEADERS GetNtHeader();

	//��ȡ����ͷ��
	PIMAGE_SECTION_HEADER GetSectionHeader();

	//rva =>  foa
	DWORD RvaToOfset(DWORD RVA);


	//��ȡ������export
	PIMAGE_EXPORT_DIRECTORY GetExportDirectory();

	//��ȡ�����import
	PIMAGE_IMPORT_DESCRIPTOR GetImportDescriptor();

	//��ȡ��������Ϣ  Order  - Rva  - Foa  - FunName
	BOOL GetExprotInfo(vector<ExportInfo> & vec_info);

	//����IAT
	BOOL GetIATInfo(vector< IATInfo> &vec_info);

	//��ȡ��Դ��
	PIMAGE_RESOURCE_DIRECTORY GetResourceDirectory();

	//������Դ��
	BOOL GetResourceInfo(vector<ResourceInfo>& vec_ResInfo);


	//��ȡ�ض�λ��
	PIMAGE_BASE_RELOCATION GetRelocation();

	//�����ض�λ��
	BOOL GetRelocationInfo(vector<RelocationInfo> & vec_RelInfo);






	~CPe();
private:
	//·��
	CString m_szExePath;
	//�ļ�����
	BYTE * m_pbuff;

};


