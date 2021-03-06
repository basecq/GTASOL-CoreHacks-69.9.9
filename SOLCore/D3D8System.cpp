#include "StdInc.h"

bool bWindowedMode = false;

class CEvents
{
private:
	std::vector<void(*)(void)> fnList;
	void RunEvent();
public:
	void AttachAt(unsigned int ea);
	void RegisterFn(void(*fn)(void));
	void UnRegisterAll();
};

void CEvents::AttachAt(unsigned int ea)
{

}

void CEvents::RegisterFn(void(*fn)(void))
{
	fnList.push_back(fn);
}

void CEvents::RunEvent()
	{
	std::vector<void (*)(void)>::iterator itr;
		for(itr = fnList.begin(); itr != fnList.end(); itr++)
	{
	(*itr)();
	}
}

class CEventsManager
{
public:
	CEvents* CreateNewEvent(DWORD ea);
	void DeleteEvent(CEvents* ev);
};

CEvents* CEventsManager::CreateNewEvent(DWORD ea)
{
	return new CEvents();
}

void CEventsManager::DeleteEvent(CEvents* ev)
{
	delete ev;
}

IDirect3DDevice8* pGameD3DDevice = (IDirect3DDevice8*)0x7897A8;

DWORD OldDirect3DCreate8 = 0x66355C;


DWORD pfnOldReset;
void D3D8HookInit(bool bWindowed)
{

	bWindowedMode = bWindowed;

	D3DPRESENT_PARAMETERS& pParams = *(D3DPRESENT_PARAMETERS*)0xA0FD04;
	pParams.Windowed = true;
	pParams.Flags = 0;
	pParams.FullScreen_RefreshRateInHz = 0;
	pParams.FullScreen_PresentationInterval = 0;

	// if the launcher is not present, to get window mode override the reset method in the original game device
	/**/
	IDirect3DDevice8* pGameD3DDevice = (IDirect3DDevice8*)0x7897A8;
	DWORD AddrDeviceVtbl = **(DWORD**)pGameD3DDevice;

	void *pOb = malloc(sizeof(IDirect3DDevice8));
	memchr(pOb, 0, sizeof(IDirect3DDevice8));
	IDirect3DDevice8 *pDeviceFake = (IDirect3DDevice8*) pOb;

	DWORD fnNewReset = (DWORD)*(DWORD*)(*((DWORD*)(pDeviceFake)) + 14 * 4);
	pParams = *(D3DPRESENT_PARAMETERS*)0xA0FD04;
	DWORD Protect = CMemory::UnProtect(AddrDeviceVtbl + 14 * 4, 4);
	pfnOldReset = ((DWORD)*(DWORD**)(AddrDeviceVtbl + 14 * 4));
	*(DWORD**)(AddrDeviceVtbl + 14 * 4) = (DWORD*)fnNewReset;
	CMemory::RestoreProtection(AddrDeviceVtbl + 14 * 4, 4, Protect);
	pParams.Windowed = true;

	pGameD3DDevice->Reset(&pParams);
}

void CustomInitialize(IDirect3DDevice8* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
}

void CustomRender(IDirect3DDevice8* pDevice)
{
}

void CustomReset()
{
}