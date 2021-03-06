#include "CMemory.h"

void CMemory::NoOperation(DWORD dwAddress, DWORD dwNopBytes){
    DWORD dwProt;
	VirtualProtect((void*)dwAddress, dwNopBytes, PAGE_EXECUTE_READWRITE, &dwProt);
	memset((void*)(dwAddress), 0x90, dwNopBytes);
	VirtualProtect((void*)dwAddress, dwNopBytes, dwProt, &dwProt);
}

DWORD CMemory::UnProtect(DWORD dwAddress, DWORD dwUnProtectBytes){
    DWORD dwProt;
	VirtualProtect((void*)dwAddress, dwUnProtectBytes, PAGE_EXECUTE_READWRITE, &dwProt);
	return dwProt;
}

void CMemory::RestoreProtection(DWORD dwAddress, DWORD dwProtBytes, DWORD dwProt){
    DWORD dwPrevProt;
	VirtualProtect((void*)dwAddress, dwProtBytes, dwProt, &dwPrevProt);
}

void CMemory::InstallCallHook(DWORD dwAddress, void* pfnFunc, BYTE bCallByte){
	DWORD dwProt;
	VirtualProtect((void*)dwAddress, 5, PAGE_EXECUTE_READWRITE, &dwProt);
	*(BYTE*)dwAddress = bCallByte;
	*(DWORD*)(dwAddress+1) = ((DWORD)pfnFunc)-dwAddress-0x5;
	VirtualProtect((void*)dwAddress, 5, dwProt, &dwProt);
}

void InstallHook(DWORD dwAddress, DWORD dwFunctionAddr, DWORD dwFunctionAddrPtr, BYTE* bCode, int iCodeSize){ //Needs thorough testing
    DWORD dwProt;
	VirtualProtect((void*)dwAddress, iCodeSize, PAGE_EXECUTE_READWRITE, &dwProt);
	for (int i = 0; i<iCodeSize; i++){
	    *(BYTE*)(dwAddress + i) = bCode[i];
	}
	VirtualProtect((void*)dwFunctionAddr, 4, PAGE_EXECUTE_READWRITE, &dwProt);
	*(DWORD*)dwFunctionAddrPtr = dwFunctionAddr;
	VirtualProtect((void*)dwAddress, iCodeSize, dwProt, &dwProt);
}

void UnProtect(DWORD dwAddress, DWORD dwUnProtectBytes){
    DWORD dwProt;
	VirtualProtect((void*)dwAddress, dwUnProtectBytes, PAGE_EXECUTE_READWRITE, &dwProt);
}

void InstallMethodHook(DWORD dwAddress, DWORD dwFunctionAddr){ //Need to check this dwFunctionAddr should actually be an offset not address directly
    DWORD dwProt;
	VirtualProtect((void*)dwAddress, 5, PAGE_EXECUTE_READWRITE, &dwProt);
	*(DWORD*)dwAddress =  dwFunctionAddr;
	VirtualProtect((void*)dwAddress, 5, dwProt, &dwProt);
}

enum eHookType
{
	hkSysJUMP, hkSysCALL
};

struct hkSysDecriptorIA32
{
	unsigned int ea;
	eHookType hookType;
	char* pNewBytes;
	char* pOldBytes;
	int nLength;
};

std::vector<hkSysDecriptorIA32> _myHookList;

void _hkSysRegisterHook(hkSysDecriptorIA32& stHook)
{
	_myHookList.push_back(stHook);
}

void _hkSysInstallFnByCall(unsigned int uiAddress, void* pFn)
{
	hkSysDecriptorIA32 stHookDescriptor;

	char* pNewBytes = (char*)malloc(stHookDescriptor.nLength);
	char* pOldBytes = (char*)malloc(stHookDescriptor.nLength);

	*(byte*)pNewBytes = 0xE8;
	*(DWORD*)(pNewBytes+1)=(DWORD)pFn - uiAddress - 0x05;

	DWORD dwProt;
	VirtualProtect((void*)uiAddress, 5, PAGE_EXECUTE_READWRITE, &dwProt);
	// grab the old bytes
	memcpy(pOldBytes, (void*)uiAddress, 5);
	// install the new bytes
	memcpy((void*)uiAddress, pNewBytes, 5);
	VirtualProtect((void*)uiAddress, 5, dwProt, &dwProt);

	stHookDescriptor.ea = uiAddress;
	stHookDescriptor.hookType = hkSysCALL;
	stHookDescriptor.nLength = 5;
	stHookDescriptor.pNewBytes = pNewBytes;
	stHookDescriptor.pOldBytes = pOldBytes;
	
	_hkSysRegisterHook(stHookDescriptor);
}

//void _hkSysMemCpy(void* dest, void* src, size_t length);

//void _hkSysInstallFnByJump;
//void _hkSysUninstallAll;