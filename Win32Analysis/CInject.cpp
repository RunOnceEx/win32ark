#include "pch.h"
#include <Windows.h>
#include "CInject.h"
#include ".\keystone\keystone.h"
#pragma comment (lib,"keystone/x86/keystone_x86.lib")


CInject::CInject():m_CurMode(),m_CurType(),m_InitInject(false)
{

}


CInject::~CInject()
{

}

/*��ʼ��ע��*/
bool CInject::InitInjectObj(int nPid, INJECT_TYPE Type, INJECT_MODE Mode)
{
	m_InitInject = true;
	m_pid = nPid;
	m_CurType = Type;
	m_CurMode = Mode;
	return true;
}

/*��ʼע��*/
bool CInject::StartInject(char * pBuff, unsigned int size)
{

	/*ע��ģʽ*/
	switch (m_CurMode)
	{
		/*Զ���߳�ע��*/
	case INJECT_MODE::Mode_Remote:
		{
			/*ע������*/
			switch (m_CurType)
			{
				/*ָ��ע��*/
			case INJECT_TYPE::Inject_Asm:
				AsmCodeInject(pBuff);
	
				break;
				/*dllע��*/
			case INJECT_TYPE::Inject_Dll:
				DllInject(pBuff);
				break;
				/*������ע��*/
			case INJECT_TYPE::Inject_Opcode:
				OpcodeInject(pBuff, size);
				break;
			default:
				break;
		}
	
		}


		break;
	}


	return false;
}

/*���ָ��ע��*/
bool CInject::AsmCodeInject(char * szAsmCode)
{
	/*ת�����ָ�Opcode �ĳ���*/
	int nCodeLen = GetAsmLength(szAsmCode);

	/*�򿪽���*/
	HANDLE hProcess = OpenProcess(
		PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
		FALSE,
		m_pid);
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/*��Ŀ���������ռ�*/
	SIZE_T Count = 0;
	LPVOID pMem = VirtualAllocEx(hProcess, NULL, nCodeLen, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	/*�����ָ� ������Ŀռ��ַ��Ϊ�������ʼλ��*/
	ks_engine *pengine = NULL;
	if (KS_ERR_OK != ks_open(KS_ARCH_X86, KS_MODE_32, &pengine))
	{
		printf("����������ʼ��ʧ��\n");
		return 0;
	}
	unsigned char* opcode = NULL; // ���õ���opcode�Ļ������׵�ַ
	unsigned int nOpcodeSize = 0; // ��������opcode���ֽ���

	int nRet = 0; // ���溯���ķ���ֵ�������жϺ����Ƿ�ִ�гɹ�
	size_t stat_count = 0; // ����ɹ�����ָ�������
	nRet = ks_asm(pengine, /* �����������ͨ��ks_open�����õ�*/
		szAsmCode, /*Ҫת���Ļ��ָ��*/
		(uint64_t)pMem, /*���ָ�����ڵĵ�ַ*/
		&opcode,/*�����opcode*/
		&nOpcodeSize,/*�����opcode���ֽ���*/
		&stat_count /*����ɹ�����ָ�������*/
	);

	// ����ֵ����-1ʱ��������
	if (nRet == -1)
	{
		printf("������Ϣ��%s\n", ks_strerror(ks_errno(pengine)));
		return 0;
	}

	/*д��ָ��*/
	WriteProcessMemory(hProcess, pMem, opcode, nOpcodeSize, &Count);

	/*����Զ���߳�*/
	HANDLE hTread = CreateRemoteThread(
		hProcess,
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)pMem,
		NULL,
		NULL,
		NULL
	);

	/*�ȴ��߳̽���*/
	WaitForSingleObject(hTread, -1);
	/*�رվ��*/
	CloseHandle(hTread);
	CloseHandle(hProcess);
	// �ͷſռ�
	ks_free(opcode);
	// �رվ��
	ks_close(pengine);
	return true;
}

/*Opcodeע��*/
bool CInject::OpcodeInject(char * pOpcode, unsigned int nSize)
{
	/*�򿪽���*/
	HANDLE hProcess = OpenProcess(
		PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
		FALSE,
		m_pid);
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	/*��Ŀ���������ռ�*/
	SIZE_T Count = 0;
	LPVOID pMem = VirtualAllocEx(hProcess, NULL, nSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	/*д��opocde*/
	WriteProcessMemory(hProcess, pMem, pOpcode, nSize, &Count);

	/*����Զ���߳�*/
	HANDLE hTread = CreateRemoteThread(
		hProcess,
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)pMem,
		NULL,
		NULL,
		NULL
	);
	if (hTread == INVALID_HANDLE_VALUE || hTread == NULL)
	{
		return FALSE;
	}

	/*�ȴ��߳̽���*/
	WaitForSingleObject(hTread, -1);
	/*�رվ��*/
	CloseHandle(hTread);
	CloseHandle(hProcess);
	return true;
}

bool CInject::DllInject(char *szDllPath)
{
	/*�򿪽���*/
	HANDLE hProcess = OpenProcess(
		PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ,
		FALSE,
		m_pid);
	if (hProcess == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	//��ָ���Ľ���������ռ�
	LPVOID pMem = VirtualAllocEx(hProcess, NULL, 2* MAX_PATH, MEM_COMMIT, PAGE_READWRITE);


	//������ڴ�д��dll·��
	SIZE_T Count;
	WriteProcessMemory(hProcess, pMem, szDllPath, MAX_PATH, &Count);

		//���ý����е��̻߳ص� �������Լ���dll
		HANDLE hTread = CreateRemoteThread(hProcess,
			NULL,
			NULL,
			(LPTHREAD_START_ROUTINE)LoadLibraryA,
			pMem,
			NULL,
			NULL
		);
		if (hTread == INVALID_HANDLE_VALUE || hTread == NULL)
		{
			return FALSE;
		}
		/*�ȴ��߳̽���*/
		WaitForSingleObject(hTread, -1);
		CloseHandle(hProcess);
		CloseHandle(hTread);
		return TRUE;


	return false;
}

/*��ȡָ���*/
unsigned int CInject::GetAsmLength(char * szAsmCode)
{
	ks_engine *pengine = NULL;
	if (KS_ERR_OK != ks_open(KS_ARCH_X86, KS_MODE_32, &pengine))
	{
		printf("����������ʼ��ʧ��\n");
		return 0;
	}

	unsigned char* opcode = NULL; // ���õ���opcode�Ļ������׵�ַ
	unsigned int nOpcodeSize = 0; // ��������opcode���ֽ���

	int nRet = 0; // ���溯���ķ���ֵ�������жϺ����Ƿ�ִ�гɹ�
	size_t stat_count = 0; // ����ɹ�����ָ�������

	nRet = ks_asm(pengine, /* �����������ͨ��ks_open�����õ�*/
		szAsmCode, /*Ҫת���Ļ��ָ��*/
		0x401000, /*���ָ�����ڵĵ�ַ*/
		&opcode,/*�����opcode*/
		&nOpcodeSize,/*�����opcode���ֽ���*/
		&stat_count /*����ɹ�����ָ�������*/
	);

	// ����ֵ����-1ʱ��������
	if (nRet == -1)
	{
		// ���������Ϣ
		printf("������Ϣ��%s\n", ks_strerror(ks_errno(pengine)));
		return 0;
	}

	// �ͷſռ�
	ks_free(opcode);

	// �رվ��
	ks_close(pengine);
	return nOpcodeSize;
}
