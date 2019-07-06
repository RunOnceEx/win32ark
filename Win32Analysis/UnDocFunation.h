#pragma once
#include <windows.h>
#include<TlHelp32.h>
#include <winternl.h>
#pragma comment(lib,"ntdll.lib")

//�̻߳�����Ϣ�ṹ��
typedef struct _THREAD_BASIC_INFORMATION
{
	NTSTATUS  ExitStatus;
	PTEB  TebBaseAddress;
	CLIENT_ID  ClientId;
	ULONG_PTR  AffinityMask;
	KPRIORITY  Priority;
	LONG  BasePriority;
}THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;

//�߳���Ϣö��ֵ
typedef enum _MYTHREADINFOCLASS
{
	UnThreadBasicInformation,				//�̻߳�����Ϣ���̻߳�����
	UnThreadTimes,
	UnThreadPriority,
	UnThreadBasePriority,
	UnThreadAffinityMask,
	UnThreadImpersonationToken,
	UnThreadDescriptorTableEntry,
	UnThreadEnableAlignmentFaultFixup,
	UnThreadEventPair_Reusable,
	UnThreadQuerySetWin32StartAddress = 9, //�߳���ڵ�ַ
	UnThreadZeroTlsCell,
	UnThreadPerformanceCount,
	UnThreadAmILastThread,
	UnThreadIdealProcessor,
	UnThreadPriorityBoost,
	UnThreadSetTlsArrayAddress,
	UnThreadIsIoPending1,
	UnThreadHideFromDebugger,
	UnThreadBreakOnTermination,
	UnThreadSwitchLegacyState,
	UnThreadIsTerminated,
	UnMaxThreadInfoClass
}MYTHREADINFOCLASS;

//������Ϣö��ֵ
typedef enum _MYPROCESSINFOCLASS {
	UnProcessBasicInformation,				//���̻�����Ϣ  PROCESS_BASIC_INFORMATION
	UnProcessQuotaLimits,
	UnProcessIoCounters,
	UnProcessVmCounters,
	UnProcessTimes,
	UnProcessBasePriority,
	UnProcessRaisePriority,
	UnProcessDebugPort,
	UnProcessExceptionPort,
	UnProcessAccessToken,
	UnProcessLdtInformation,
	UnProcessLdtSize,
	UnProcessDefaultHardErrorMode,
	UnProcessIoPortHandlers,
	UnProcessPooledUsageAndLimits,
	UnProcessWorkingSetWatch,
	UnProcessUserModeIOPL,
	UnProcessEnableAlignmentFaultFixup,
	UnProcessPriorityClass,
	UnProcessWx86Information,
	UnProcessHandleCount,					  //���̾������
	UnProcessAffinityMask,
	UnProcessPriorityBoost,
	UnProcessDeviceMap,
	UnProcessSessionInformation,
	UnProcessForegroundInformation,
	UnProcessWow64Information,
	UnProcessImageFileName,
	UnProcessLUIDDeviceMapsEnabled,
	UnProcessBreakOnTermination,
	UnProcessDebugObjectHandle,			//���Զ�����
	UnProcessDebugFlags,					//���Ա�־
	UnProcessHandleTracing,
	UnProcessIoPriority,
	UnProcessExecuteFlags,
	UnProcessResourceManagement,
	UnProcessCookie,
	UnProcessImageInformation,
	UnMaxProcessInfoClass

}MYPROCESSINFOCLASS, *PMYPROCESSINFOCLASS;

//ϵͳ��Ϣö��ֵ
typedef enum _MYSYSTEMINFOCLASS {
	UnSystemBasicInformation,
	UnSystemProcessorInformation,				//SYSTEM_PROCESSOR_INFORMATION
	UnSystemPerformanceInformation,
	UnSystemTimeOfDayInformation,
	UnSystemPathInformation,
	UnSystemProcessInformation,
	UnSystemCallCountInformation,
	UnSystemDeviceInformation,
	UnSystemProcessorPerformanceInformation,
	UnSystemFlagsInformation,
	UnSystemCallTimeInformation,
	UnSystemModuleInformation,				//ϵͳģ����Ϣ�����н�����Ϣ
	UnSystemLocksInformation,
	UnSystemStackTraceInformation,
	UnSystemPagedPoolInformation,
	UnSystemNonPagedPoolInformation,
	UnSystemHandleInformation,
	UnSystemObjectInformation,
	UnSystemPageFileInformation,
	UnSystemVdmInstemulInformation,
	UnSystemVdmBopInformation,
	UnSystemFileCacheInformation,
	UnSystemPoolTagInformation,
	UnSystemInterruptInformation,
	UnSystemDpcBehaviorInformation,
	UnSystemFullMemoryInformation,
	UnSystemLoadGdiDriverInformation,
	UnSystemUnloadGdiDriverInformation,
	UnSystemTimeAdjustmentInformation,
	UnSystemSummaryMemoryInformation,
	UnSystemMirrorMemoryInformation,
	UnSystemPerformanceTraceInformation,
	UnSystemObsolete0,
	UnSystemExceptionInformation,
	UnSystemCrashDumpStateInformation,
	UnSystemKernelDebuggerInformation,
	UnSystemContextSwitchInformation,
	UnSystemRegistryQuotaInformation,
	UnSystemExtendServiceTableInformation,
	UnSystemPrioritySeperation,
	UnSystemVerifierAddDriverInformation,
	UnSystemVerifierRemoveDriverInformation,
	UnSystemProcessorIdleInformation,
	UnSystemLegacyDriverInformation,
	UnSystemCurrentTimeZoneInformation,
	UnSystemLookasideInformation,
	UnSystemTimeSlipNotification,
	UnSystemSessionCreate,
	UnSystemSessionDetach,
	UnSystemSessionInformation,
	UnSystemRangeStartInformation,
	UnSystemVerifierInformation,
	UnSystemVerifierThunkExtend,
	UnSystemSessionProcessInformation,
	UnSystemLoadGdiDriverInSystemSpace,
	UnSystemNumaProcessorMap,
	UnSystemPrefetcherInformation,
	UnSystemExtendedProcessInformation,
	UnSystemRecommendedSharedDataAlignment,
	UnSystemComPlusPackage,
	UnSystemNumaAvailableMemory,
	UnSystemProcessorPowerInformation,
	UnSystemEmulationBasicInformation,
	UnSystemEmulationProcessorInformation,
	UnSystemExtendedHandleInformation,
	UnSystemLostDelayedWriteInformation,
	UnSystemBigPoolInformation,
	UnSystemSessionPoolTagInformation,
	UnSystemSessionMappedViewInformation,
	UnSystemHotpatchInformation,
	UnSystemObjectSecurityMode,
	UnSystemWatchdogTimerHandler,
	UnSystemWatchdogTimerInformation,
	UnSystemLogicalProcessorInformation,
	UnSystemWow64SharedInformation,
	UnSystemRegisterFirmwareTableInformationHandler,
	UnSystemFirmwareTableInformation,
	UnSystemModuleInformationEx,
	UnSystemVerifierTriageInformation,
	UnSystemSuperfetchInformation,
	UnSystemMemoryListInformation,
	UnSystemFileCacheInformationEx,
	UnMaxSystemInfoClass
}MYSYSTEMINFOCLASS, *PMYSYSTEMINFOCLASS;

//����ģ����Ϣ
typedef struct _SYSTME_MODULE_INFORMATION
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR  FullPathName[256];
} SYSTME_MODULE_INFORMATION, *PSYSTME_MODULE_INFORMATION;

//��ѯϵͳģ�鷵�صĽṹ
typedef struct _SYSTEM_PROCESS_MODULES
{
	ULONG NumberOfModules;
	SYSTME_MODULE_INFORMATION Modules[1];
} SYSTEM_PROCESS_MODULES, *PSYSTEM_PROCESS_MODULES;
