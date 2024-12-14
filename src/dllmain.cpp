// dllmain.cpp : Defines the entry point for the DLL application.
#include <iostream>
#include <Windows.h>
#include "constants.h"
#include "genCode.h"

#include "esp.h"
#include "detours/detours.h"
#include "menu.h"

void aimbot() {
    while ( true ) {
        resetPointers();
        ESP::aimbot();
        Sleep( 50 );
        if ( GetAsyncKeyState( VK_DELETE ) ) {
            Menu::toggleMenu();
        }
    }
}

void hook() {
    Sleep( 1000 );
    DisableThreadLibraryCalls( hModule );
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)originalSwapBuffers, Menu::newSwapBuffers);
    DetourTransactionCommit();
}

void console() {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONIN$", "r", stdin);

    std::cout << "Console Created" << std::endl;

    while (true) {
        std::string input;
        std::cin >> input;

        if (input == "exit") {
            break;
        }
        if(input == "up") { 
            localPlayerPtr->velocity.y += 1;
        }

        if (input == "no") {
            std::cout << "Local Player ptr: " << localPlayerPtr << std::endl;
        }
        if (input == "amount") {
            std::cout << "No Of Players: " << noPlayers << std::endl;
        }
        if (input == "ent") {
            for (int i = 1; i < noPlayers + 1; i++) {
                if ( entityList->players[i] == nullptr ) break;
                entityList->players[ i ]->print();
            }
        }
        if (input == "view") {
            std::cout << "View Matrix: " << viewMatrix << std::endl;
            for (int i = 0; i < 16; i++) {
                std::cout << "View Matrix: " << viewMatrix[i] << std::endl;
            }
        }
        if (input == "eye") {
            std::cout << "Eye Height: " << localPlayerPtr->eyeHeight << std::endl;
        }
    }
    FreeConsole();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)console, nullptr, 0, nullptr);
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)hook, nullptr, 0, nullptr);
        CreateThread( nullptr , 0 , ( LPTHREAD_START_ROUTINE )aimbot , nullptr , 0 , nullptr );
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

