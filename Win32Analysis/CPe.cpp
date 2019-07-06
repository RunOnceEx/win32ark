#include "pch.h"
#include "CPe.h"


const WCHAR* RES[20] = {
	L"���",
	L"λͼ",
	L"ͼ��",
	L"�˵�",
	L"�Ի���",
	L"�ַ����б�",
	L"����Ŀ¼",
	L"����",
	L"��ݼ�",
	L"�Ǹ�ʽ����Դ",
	L"��Ϣ�б�",
	L"���ָ������",
	L"NULL",
	L"ͼ����",
	L"NULL",
	L"�汾��Ϣ",
};



CPe::CPe(TCHAR * pSzName)
{
	m_szExePath = pSzName;
	if (!AnalysisPe())
	{
		MessageBox(NULL, L"����ʧ��", NULL, NULL);
	}
}

BOOL CPe::AnalysisPe()
{
	//1.���ļ�
	HANDLE hfile = CreateFile(m_szExePath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	);
	if (hfile == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	//2. ��ȡ��С����ȡ����
	DWORD dwsize = GetFileSize(hfile, 0);
	m_pbuff = new BYTE[dwsize]{};
	DWORD dwReadSize;

	ReadFile(hfile, m_pbuff, dwsize, &dwReadSize, 0);

	//3.�ж��Ƿ���PE�ļ�
	if (GetDosHeader()->e_magic != IMAGE_DOS_SIGNATURE)
	{
		delete[]m_pbuff;
		m_pbuff = NULL;
		//3.�ر��ļ����
		CloseHandle(hfile);
		return FALSE;
	}
	if (GetNtHeader()->Signature != IMAGE_NT_SIGNATURE)
	{
		delete[]m_pbuff;
		m_pbuff = NULL;
		//3.�ر��ļ����
		CloseHandle(hfile);
		return FALSE;
	}
	CloseHandle(hfile);

	return 1;
}

PIMAGE_DOS_HEADER CPe::GetDosHeader()
{
	return (PIMAGE_DOS_HEADER)m_pbuff;
}

PIMAGE_NT_HEADERS CPe::GetNtHeader()
{
	// nt = dos.e_lfnew + pbuff;
	return  (PIMAGE_NT_HEADERS)(GetDosHeader()->e_lfanew + (LONG)m_pbuff);
}

PIMAGE_SECTION_HEADER CPe::GetSectionHeader()
{
	return IMAGE_FIRST_SECTION(GetNtHeader());
}

DWORD CPe::RvaToOfset(DWORD RVA)
{
	// foa = rva - ��������rva + �����������ļ�λ��

	//�ж�rva����ʲô�ĸ�����
	PIMAGE_NT_HEADERS  pNt = GetNtHeader();
	PIMAGE_SECTION_HEADER pSection = GetSectionHeader();

	for (int i = 0; i < pNt->FileHeader.NumberOfSections; i++)
	{
		if (RVA >= pSection[i].VirtualAddress
			&& (RVA < pSection[i].VirtualAddress + pSection[i].SizeOfRawData))
		{
			return RVA - pSection[i].VirtualAddress + pSection[i].PointerToRawData;
		}
	}
	return -1;
}

PIMAGE_EXPORT_DIRECTORY CPe::GetExportDirectory()
{
	//����Ŀ¼���0��
	IMAGE_DATA_DIRECTORY dirdata = GetNtHeader()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

	//ת���ļ�ƫ��
	DWORD Foa = RvaToOfset(dirdata.VirtualAddress);

	//��ȡ�����λ��
	return (PIMAGE_EXPORT_DIRECTORY)(Foa + (DWORD)m_pbuff);
}

PIMAGE_IMPORT_DESCRIPTOR CPe::GetImportDescriptor()
{
	//����Ŀ¼���0��
	IMAGE_DATA_DIRECTORY dirdata = GetNtHeader()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];

	//ת���ļ�ƫ��
	DWORD Foa = RvaToOfset(dirdata.VirtualAddress);

	//��ȡ�����λ��
	return (PIMAGE_IMPORT_DESCRIPTOR)(Foa + (DWORD)m_pbuff);
}

BOOL CPe::GetExprotInfo(vector<ExportInfo>& vec_info)
{
	//����PE���� 
	PIMAGE_EXPORT_DIRECTORY pExprot = GetExportDirectory();

	//������ַ��  //������ű�    //�������Ʊ�
	// Order Rva  Foa  FunName

	DWORD dwFunsCount = pExprot->NumberOfFunctions;
	DWORD dwNameCount = pExprot->NumberOfNames;

	//���к�����ַ
	for (int i = 0; i < dwFunsCount; i++)
	{
		//������ַ
		DWORD *pFunaddr = (DWORD*)(RvaToOfset(pExprot->AddressOfFunctions) + (DWORD)m_pbuff); //i 
		ExportInfo info;


		bool Find = 0;  //û���ҵ�������
		//�����ֺ�����ַ
		for (int j = 0; j < dwNameCount; j++)
		{
			WORD * pOrderName = (WORD*)(RvaToOfset(pExprot->AddressOfNameOrdinals) + (DWORD)m_pbuff);
			if (i == pOrderName[j])
			{
				//�����ֺ���
				info.Order = i + pExprot->Base;
				info.Rva = pFunaddr[i];
				info.Foa = (RvaToOfset(info.Rva) + (DWORD)m_pbuff);
				DWORD * pFunAddr = (DWORD*)(RvaToOfset(pExprot->AddressOfNames) + (DWORD)m_pbuff);
				char * pName = (char*)(RvaToOfset(pFunAddr[i]) + (DWORD)m_pbuff);
				strcpy_s(info.FunName, pName);
				Find = 1;
				break;
			}
		}

		if (Find == 0)
		{
			//û�����ֺ���
			info.Order = i + pExprot->Base;
			info.Rva = pFunaddr[i];
			info.Foa = (RvaToOfset(info.Rva) + (DWORD)m_pbuff);
			strcpy_s(info.FunName, "");
		}
		//����
		vec_info.push_back(info);
	}
	return 0;
}

BOOL CPe::GetIATInfo(vector<IATInfo>& vec_info)
{
	IMAGE_IMPORT_DESCRIPTOR * pImport = GetImportDescriptor();

	//��������dll
	while (pImport->OriginalFirstThunk != 0)
	{
		//dll����
		IATInfo info;
		char * pdllname = (char*)(RvaToOfset(pImport->Name) + m_pbuff);
		strcpy_s(info.dllName, pdllname);

		//IAT����
		IMAGE_THUNK_DATA *pIat = (IMAGE_THUNK_DATA *)(RvaToOfset(GetImportDescriptor()->FirstThunk) + m_pbuff);
		while (pIat->u1.Ordinal != 0)
		{
			IATInfo_Item item;
			//�������
			if (pIat->u1.Ordinal & 0x80000000 == 1)
			{

				item.RVA = pIat->u1.AddressOfData;
				strcpy_s(item.FunName, "");
				item.Order = pIat->u1.Ordinal & 0xFFFF;
			}
			//���Ƶ���
			else {
				item.RVA = pIat->u1.AddressOfData;
				PIMAGE_IMPORT_BY_NAME pBn = (PIMAGE_IMPORT_BY_NAME)(RvaToOfset(pIat->u1.AddressOfData) + m_pbuff);
				item.Order = pBn->Hint;
				strcpy_s(item.FunName, pBn->Name);


			}
			info.vec_item.push_back(item);
			pIat++;
		}

		vec_info.push_back(info);
		pImport++;
	}

	return 0;
}

PIMAGE_RESOURCE_DIRECTORY CPe::GetResourceDirectory()
{
	DWORD dwRva = GetNtHeader()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress;
	return (PIMAGE_RESOURCE_DIRECTORY)(RvaToOfset(dwRva) + m_pbuff);
}

BOOL CPe::GetResourceInfo(vector<ResourceInfo>& vec_ResInfo)
{
	PIMAGE_RESOURCE_DIRECTORY pRes = GetResourceDirectory();

	//1. ����1�� ��Դ���� -��Դ����		 typename

	DWORD dwCount = pRes->NumberOfIdEntries + pRes->NumberOfNamedEntries;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pResEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pRes + 1);
	for (int i = 0; i < dwCount; i++)
	{
		ResourceInfo Resinfo;

		//��Դ���ַ���
		if (pResEntry[i].NameIsString == 1)
		{
			PIMAGE_RESOURCE_DIR_STRING_U wSzName = (PIMAGE_RESOURCE_DIR_STRING_U)(pResEntry[i].NameOffset + (DWORD)pRes);
			strncpy_s(Resinfo.ResTypeName, CW2A(wSzName->NameString), wSzName->Length);


		}
		//��Դ������ID
		else {
			//����ϵͳ�����
			if (pResEntry[i].Id > 16)
			{
				sprintf_s(Resinfo.ResTypeName, "%04x", pResEntry[i].Id);
			}
			//ϵͳ�����
			else {
				strcpy_s(Resinfo.ResTypeName, CW2A(RES[pResEntry[i].Id - 1]));
			}

		}


		//2. ����2�� ������Դ����   - ��Դ����  Resname
		if (pResEntry[i].DataIsDirectory == 1)
		{


			PIMAGE_RESOURCE_DIRECTORY pResTwo = (PIMAGE_RESOURCE_DIRECTORY)(pResEntry[i].OffsetToDirectory + (DWORD)pRes);
			DWORD dwCount2 = pResTwo->NumberOfIdEntries + pResTwo->NumberOfNamedEntries;

			PIMAGE_RESOURCE_DIRECTORY_ENTRY pResEntryTwo = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResTwo + 1);
			for (int j = 0; j < dwCount2; j++)
			{
				ResourceInfo_Item Resitem;

				//��Դ��ID���ַ���
				if (pResEntryTwo[j].NameIsString == 1)
				{
					PIMAGE_RESOURCE_DIR_STRING_U wSzName = (PIMAGE_RESOURCE_DIR_STRING_U)(pResEntryTwo[j].NameOffset + (DWORD)pRes);
					strncpy_s(Resitem.ResName, CW2A(wSzName->NameString), wSzName->Length);
				}
				//��Դ��ID������
				else {

					sprintf_s(Resitem.ResName, "%04x", pResEntryTwo[j].Id);
				}


				//3. ����3�� ������Դ��ʲôλ��		 RVA FOA Size
				if (pResEntryTwo[j].DataIsDirectory == 1)
				{
					PIMAGE_RESOURCE_DIRECTORY pResThred = (PIMAGE_RESOURCE_DIRECTORY)(pResEntryTwo[j].OffsetToDirectory + (DWORD)pRes);
					PIMAGE_RESOURCE_DIRECTORY_ENTRY pResThredEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(pResThred + 1);
					PIMAGE_RESOURCE_DATA_ENTRY pResData = (PIMAGE_RESOURCE_DATA_ENTRY)(pResThredEntry->OffsetToData + (DWORD)pRes);

					Resitem.Rva = pResData->OffsetToData;
					Resitem.Foa = RvaToOfset(pResData->OffsetToData);
					Resitem.Size = pResData->Size;


				}

				//����2����������
				Resinfo.ItemInfo.push_back(Resitem);
			}


		}
		//������������
		vec_ResInfo.push_back(Resinfo);
	}


	return 0;
}

PIMAGE_BASE_RELOCATION CPe::GetRelocation()
{
	DWORD dwRva = GetNtHeader()->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
	return (PIMAGE_BASE_RELOCATION)(RvaToOfset(dwRva) + m_pbuff);
}

BOOL CPe::GetRelocationInfo(vector<RelocationInfo>& vec_RelInfo)
{
	typedef struct DataType {

		WORD offset : 12;
		WORD type : 4;
	}*PDATATYPE;

	//��ȡ�ض�λ
	PIMAGE_BASE_RELOCATION pBaseRel = GetRelocation();
	while (1)
	{

		if (pBaseRel->VirtualAddress == 0 && pBaseRel->SizeOfBlock == 0)
		{
			break;
		}
		DWORD dwCount = (pBaseRel->SizeOfBlock - 8) / 2;
		PDATATYPE  pdata = (PDATATYPE)(pBaseRel + 1);
		for (int i = 0; i < dwCount; i++)
		{
			RelocationInfo relinfo;
			if (pdata[i].type == 3)
			{
				relinfo.BaseRVA = pBaseRel->VirtualAddress;
				relinfo.offset = pdata[i].offset;
				relinfo.offsetFoa = RvaToOfset(relinfo.offset + relinfo.BaseRVA);
				relinfo.RelocationData = *(DWORD*)(relinfo.offsetFoa + (DWORD)m_pbuff);
				vec_RelInfo.push_back(relinfo);
			}
		}

	}

	return 1;
}




CPe::~CPe()
{
	if (m_pbuff)
	{
		delete[]m_pbuff;
		m_pbuff = NULL;
	}

}
