#include "constants.h"

HMODULE hModule = GetModuleHandle(nullptr);
uintptr_t exeBaseAddress = (uintptr_t)GetModuleHandle(nullptr);


extern Player* localPlayerPtr = *(Player**)(exeBaseAddress + 0x0017E0A8);

uintptr_t playersAddr = exeBaseAddress + 0x18AC04;
extern EntityList* entityList = *(EntityList**)playersAddr;

extern int noPlayers = *(int*)(exeBaseAddress + 0x191FD4);
extern float* viewMatrix = (float*) ( exeBaseAddress + 0x17DFD0 );

void resetPointers() {
	localPlayerPtr = *(Player**)(exeBaseAddress + 0x0017E0A8);
	playersAddr = exeBaseAddress + 0x18AC04;
	noPlayers = *(int*)(exeBaseAddress + 0x191FD4);
}

_wglSwapBuffers originalSwapBuffers = (_wglSwapBuffers)GetProcAddress(GetModuleHandle(L"opengl32.dll"), "wglSwapBuffers");
_SDL_SetRelativeMouseMode originalSetRelativeMouseMode = (_SDL_SetRelativeMouseMode)( GetProcAddress( GetModuleHandle( L"SDL2.dll" ) , "SDL_SetRelativeMouseMode" ) );