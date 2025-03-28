// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "headers\globals.h"
#include "headers\headers.h"
#include "headers\utilities.h"
#include "headers\hooks.h"

BOOL APIENTRY DllMain( HMODULE hModule,  DWORD  ul_reason_for_call,  LPVOID lpReserved )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        prepare_setup();
        return installHooks();
        break;
    }
    return TRUE;
}

