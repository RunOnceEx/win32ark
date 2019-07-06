#pragma once
class CInject
{
public:
	/*ע������*/
	enum INJECT_TYPE{
		Inject_Dll,
		Inject_Asm,
		Inject_Opcode
	};
	/*ע�뷽ʽ*/
	enum INJECT_MODE {
		Mode_Remote,
		Mode_Message,
		Mode_Apc
	};

	CInject();
	~CInject();
	/*��ʼ��ע��*/
	bool InitInjectObj(int nPid, INJECT_TYPE Type, INJECT_MODE Mode);

	/*��ʼע��*/
	bool StartInject(char *pBuff, unsigned int size);

	/*����ע��*/
	bool AsmCodeInject(char * szAsmCode);

	/*Opcodeע��*/
	bool OpcodeInject(char * pOpcode, unsigned int size);

	/*dllע��*/
	bool DllInject(char * szDllPath);

private:

	unsigned int GetAsmLength(char * pOpcode);





	INJECT_TYPE m_CurType;	/*��ǰע������*/
	INJECT_MODE m_CurMode;  /*��ǰע��ģʽ*/
	bool m_InitInject;		/*��ʼ��ע�����*/
	int m_pid;				/*Ŀ��pid*/


};

