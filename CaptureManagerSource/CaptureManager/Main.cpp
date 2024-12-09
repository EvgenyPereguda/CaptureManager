/*
MIT License

Copyright(c) 2020 Evgeny Pereguda

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#define WIN32_LEAN_AND_MEAN

#include "../Common/Singleton.h"
#include "CommercialConfig.h"
#include "Libraries.h"
#include "../COMServer/RegisterManager.h"
#include "../COMServer/ClassFactory.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Macros.h"

 

using namespace CaptureManager;
using namespace CaptureManager::COMServer;

HINSTANCE gModuleInstance = NULL;

BOOL APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case DLL_PROCESS_ATTACH:
		gModuleInstance = hInstance;
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}

STDAPI DllCanUnloadNow()
{
	return Singleton<ClassFactory>::getInstance().checkLock();
}

STDAPI DllGetClassObject(
	REFCLSID aRefCLSID,
	REFIID aRefIID,
	void** aPtrPtrVoidObject)
{
	return Singleton<ClassFactory>::getInstance().getClassObject(
		aRefCLSID,
		aRefIID,
		aPtrPtrVoidObject);
}

STDAPI DllRegisterServer()
{
	return Singleton<RegisterManager>::getInstance().registerServer(gModuleInstance);
}

STDAPI DllUnregisterServer()
{
	return Singleton<RegisterManager>::getInstance().unregisterServer(gModuleInstance);	
}

void InitLogOut()
{
	LogPrintOut::getInstance().printOutln(
		LogPrintOut::INFO_LEVEL,
		L"***** CAPTUREMANAGER SDK ",
		(int)VERSION_MAJOR,
		L".",
		(int)VERSION_MINOR,
		L".",
		(int)VERSION_PATCH,
		L" ",
		WSTRINGIZE(ADDITIONAL_LABEL),
		L" - ",
		__DATE__,
		L" (Author: Evgeny Pereguda) *****\n");
}

void UnInitLogOut()
{
	LogPrintOut::getInstance().printOutlnUnlock(
		LogPrintOut::INFO_LEVEL,
		L"***** CAPTUREMANAGER SDK ",
		(int)VERSION_MAJOR,
		L".",
		(int)VERSION_MINOR,
		L".",
		(int)VERSION_PATCH,
		L" ",
		WSTRINGIZE(ADDITIONAL_LABEL),
		L" - ",
		__DATE__,
		L" (Author: Evgeny Pereguda) - is closed *****\n");
}