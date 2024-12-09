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

#include <strsafe.h>
#include <memory>

#include "RegisterManager.h"
#include "CaptureManagerTypeInfo_i.c"
#include "../Common/ComPtrCustom.h"

#define FILEPATHLENGTH MAX_PATH * 100

namespace CaptureManager
{
	namespace COMServer
	{

		HRESULT RegisterManager::registerServer(
			HINSTANCE aModuleInstance)
		{
			HRESULT lresult = E_FAIL;
			
			do
			{
				if (aModuleInstance == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				std::unique_ptr<wchar_t[]>lFullFilePath(new (std::nothrow) wchar_t[FILEPATHLENGTH]);

				lresult = getModuleFilePath(
					aModuleInstance,
					lFullFilePath.get(),
					FILEPATHLENGTH);
				
				if (FAILED(lresult))
				{
					break;
				}

				// Register CoLogPrintOut component.
				lresult = registerInprocServer(
					lFullFilePath.get(),
					L"Capture Manager Log Print Out Module",
					L"Both",
					L"CaptureManager.CoLogPrintOut.1",
					L"CaptureManager.CoLogPrintOut",
					CLSID_CoLogPrintOut,
					LIBID_CaptureManagerLibrary);

				if (FAILED(lresult))
				{
					break;
				}

				// Register CoCaptureManager component.
				lresult = registerInprocServer(
					lFullFilePath.get(),
					L"Capture Manager Main Module",
					L"Both",
					L"CaptureManager.CoCaptureManager.1",
					L"CaptureManager.CoCaptureManager",
					CLSID_CoCaptureManager,
					LIBID_CaptureManagerLibrary);

				if (FAILED(lresult))
				{
					break;
				}

				lresult = registerTypeLib(
					lFullFilePath.get());

			} while (false);

			return lresult;
		}

		HRESULT RegisterManager::unregisterServer(
			HINSTANCE aModuleInstance)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (aModuleInstance == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				std::unique_ptr<wchar_t[]>lFullFilePath(new (std::nothrow) wchar_t[FILEPATHLENGTH]);

				lresult = getModuleFilePath(
					aModuleInstance,
					lFullFilePath.get(),
					FILEPATHLENGTH);

				if (FAILED(lresult))
				{
					break;
				}

				// Unregister CoLogPrintOut component.
				lresult = unregisterInprocServer(
					L"CaptureManager.CoLogPrintOut.1",
					L"CaptureManager.CoLogPrintOut",
					CLSID_CoLogPrintOut);

				if (FAILED(lresult))
				{
					break;
				}

				// Unregister CoCaptureManager component.
				lresult = unregisterInprocServer(
					L"CaptureManager.CoCaptureManager.1",
					L"CaptureManager.CoCaptureManager",
					CLSID_CoCaptureManager);

				if (FAILED(lresult))
				{
					break;
				}

				lresult = unregisterTypeLib(
					lFullFilePath.get());

			} while (false);

			return lresult;
		}

		HRESULT RegisterManager::getModuleFilePath(
			HINSTANCE aModuleInstance, 
			LPWSTR aPtrFilename,
			DWORD aSize)
		{
			HRESULT lresult = S_OK;

			do
			{
				if (aModuleInstance == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrFilename == nullptr)
				{
					lresult = E_POINTER;

					break;
				}
				
				{				
					if (GetModuleFileName(
						aModuleInstance, 
						aPtrFilename,
						aSize) == 0)
					{
						lresult = HRESULT_FROM_WIN32(GetLastError());
					}
				}							

			} while (false);

			return lresult;
		}

		HRESULT RegisterManager::registerInprocServer(
			PCWSTR aFilePath,
			PCWSTR aFriendlyName,
			PCWSTR aThreadModel,
			PCWSTR aProgID,
			PCWSTR aVerIndProgID,
			REFCLSID aRefCLSID,
			REFIID aRefLibID)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (aFilePath == nullptr)
				{
					lresult = E_INVALIDARG;

					break;
				}

				if (aFriendlyName == nullptr)
				{
					lresult = E_INVALIDARG;

					break;
				}

				if (aThreadModel == nullptr)
				{
					lresult = E_INVALIDARG;

					break;
				}

				if (aProgID == nullptr)
				{
					lresult = E_INVALIDARG;

					break;
				}

				if (aVerIndProgID == nullptr)
				{
					lresult = E_INVALIDARG;

					break;
				}

				wchar_t lstringCLSID[MAX_PATH];
				auto lwriteSize = StringFromGUID2(aRefCLSID, lstringCLSID, ARRAYSIZE(lstringCLSID));

				if (lwriteSize == 0)
				{
					lresult = E_INVALIDARG;

					break;
				}

				wchar_t lstringLIBID[MAX_PATH];
				lwriteSize = StringFromGUID2(aRefLibID, lstringLIBID, ARRAYSIZE(lstringLIBID));

				if (lwriteSize == 0)
				{
					lresult = E_INVALIDARG;

					break;
				}

				wchar_t lSubkeyString[MAX_PATH];


				lresult = StringCchPrintf(
					lSubkeyString, 
					ARRAYSIZE(lSubkeyString), 
					L"CLSID\\%s", lstringCLSID);

				if (FAILED(lresult))
				{
					break;
				}

				lresult = setRegistryKeyAndValue(
					lSubkeyString, 
					aFilePath);

				if (FAILED(lresult))
				{
					break;
				}

				lresult = StringCchPrintf(
					lSubkeyString, 
					ARRAYSIZE(lSubkeyString),
					L"CLSID\\%s\\ProgID", 
					lstringCLSID);

				if (FAILED(lresult))
				{
					break;
				}

				lresult = setRegistryKeyAndValue(
					lSubkeyString, 
					aProgID);

				if (FAILED(lresult))
				{
					break;
				}

				lresult = StringCchPrintf(
					lSubkeyString, 
					ARRAYSIZE(lSubkeyString),
					L"CLSID\\%s\\VersionIndependentProgID", 
					lstringCLSID);

				if (FAILED(lresult))
				{
					break;
				}				

				lresult = setRegistryKeyAndValue(
					lSubkeyString, 
					aVerIndProgID);

				if (FAILED(lresult))
				{
					break;
				}


				lresult = StringCchPrintf(
					lSubkeyString, 
					ARRAYSIZE(lSubkeyString),
					L"CLSID\\%s\\TypeLib", 
					lstringCLSID);

				if (FAILED(lresult))
				{
					break;
				}
				
				lresult = setRegistryKeyAndValue(
					lSubkeyString, 
					lstringLIBID);

				if (FAILED(lresult))
				{
					break;
				}


				lresult = StringCchPrintf(
					lSubkeyString, 
					ARRAYSIZE(lSubkeyString),
					L"CLSID\\%s\\InprocServer32", 
					lstringCLSID);

				if (FAILED(lresult))
				{
					break;
				}
				
				lresult = setRegistryKeyAndValue(
					lSubkeyString, 
					aFilePath);

				if (FAILED(lresult))
				{
					break;
				}

				lresult = setRegistryKeyAndValue(
					lSubkeyString, 
					aThreadModel, 
					L"ThreadingModel");

				if (FAILED(lresult))
				{
					break;
				}
				
				lresult = setRegistryKeyAndValue(
					aProgID, 
					aFriendlyName);

				if (FAILED(lresult))
				{
					break;
				}

				lresult = StringCchPrintf(
					lSubkeyString, 
					ARRAYSIZE(lSubkeyString), 
					L"%s\\CLSID",
					aProgID);

				if (FAILED(lresult))
				{
					break;
				}

				lresult = setRegistryKeyAndValue(
					lSubkeyString, 
					lstringCLSID);

				if (FAILED(lresult))
				{
					break;
				}

				lresult = setRegistryKeyAndValue(
					aVerIndProgID, 
					aFriendlyName);

				if (FAILED(lresult))
				{
					break;
				}

				lresult = StringCchPrintf(
					lSubkeyString, 
					ARRAYSIZE(lSubkeyString), 
					L"%s\\CLSID",
					aVerIndProgID);

				if (FAILED(lresult))
				{
					break;
				}

				lresult = setRegistryKeyAndValue(
					lSubkeyString, 
					lstringCLSID);

				if (FAILED(lresult))
				{
					break;
				}

				lresult = StringCchPrintf(
					lSubkeyString, 
					ARRAYSIZE(lSubkeyString), 
					L"%s\\CurVer",
					aVerIndProgID);

				if (FAILED(lresult))
				{
					break;
				}

				lresult = setRegistryKeyAndValue(
					lSubkeyString, 
					aProgID);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			return lresult;
		}

		HRESULT RegisterManager::setRegistryKeyAndValue(
			PCWSTR aSubkeyString,
			PCWSTR aData,
			PCWSTR aName)
		{
			HRESULT lresult;

			do
			{
				HKEY lKey = nullptr;

				// Creates the specified registry key. If the key already exists, the 
				// function opens it. 
				lresult = HRESULT_FROM_WIN32(RegCreateKeyEx(
					HKEY_CLASSES_ROOT, 
					aSubkeyString,
					0,
					nullptr, 
					REG_OPTION_NON_VOLATILE, 
					KEY_WRITE, 
					nullptr, 
					&lKey,
					nullptr));

				if (FAILED(lresult))
				{
					break;
				}


				if (aData != nullptr)
				{
					// Set the specified value of the key.
					DWORD cbData = lstrlen(aData) * sizeof(*aData);

					lresult = HRESULT_FROM_WIN32(RegSetValueEx(
						lKey,
						aName,
						0,
						REG_SZ,
						reinterpret_cast<const BYTE *>(aData),
						cbData));
				}

				RegCloseKey(lKey);

			} while (false);

			return lresult;
		}

		HRESULT RegisterManager::registerTypeLib(
			PCWSTR aFilePath)
		{
			HRESULT lresult;

			do
			{
				CComPtrCustom<ITypeLib> lITypeLib;

				lresult = LoadTypeLibEx(
					aFilePath, 
					REGKIND_REGISTER, 
					&lITypeLib);
			
			} while (false);

			return lresult;
		}

		HRESULT RegisterManager::unregisterTypeLib(
			PCWSTR aFilePath)
		{
			HRESULT lresult;

			do
			{

				CComPtrCustom<ITypeLib> lITypeLib;
				lresult = LoadTypeLibEx(aFilePath, REGKIND_NONE, &lITypeLib);

				if (FAILED(lresult))
				{
					break;
				}
				
				TLIBATTR *lPtrAttr = nullptr;

				lresult = lITypeLib->GetLibAttr(&lPtrAttr);

				if (FAILED(lresult))
				{
					break;
				}				

				lresult = UnRegisterTypeLib(
					lPtrAttr->guid,
					lPtrAttr->wMajorVerNum,
					lPtrAttr->wMinorVerNum,
					lPtrAttr->lcid,
					lPtrAttr->syskind);

				lITypeLib->ReleaseTLibAttr(lPtrAttr);

			} while (false);

			return lresult;
		}

		HRESULT RegisterManager::unregisterInprocServer(
			PCWSTR aProgID,
			PCWSTR aVerIndProgID,
			REFCLSID aRefCLSID)
		{
			HRESULT lresult = E_FAIL;

			do
			{

				if (aProgID == nullptr)
				{
					lresult = E_INVALIDARG;

					break;
				}

				if (aVerIndProgID == nullptr)
				{
					lresult = E_INVALIDARG;

					break;
				}

				wchar_t lstringCLSID[MAX_PATH];
				auto lwriteSize = StringFromGUID2(
					aRefCLSID, 
					lstringCLSID, 
					ARRAYSIZE(lstringCLSID));

				if (lwriteSize == 0)
				{
					lresult = E_INVALIDARG;

					break;
				}


				wchar_t lSubkeyString[MAX_PATH];

				lresult = StringCchPrintf(
					lSubkeyString, 
					ARRAYSIZE(lSubkeyString), 
					L"CLSID\\%s", 
					lstringCLSID);

				if (FAILED(lresult))
				{
					break;
				}

				lresult = HRESULT_FROM_WIN32(RegDeleteTree(
					HKEY_CLASSES_ROOT, 
					lSubkeyString));

				if (FAILED(lresult))
				{
					break;
				}
				
				lresult = HRESULT_FROM_WIN32(RegDeleteTree(
					HKEY_CLASSES_ROOT, 
					aProgID));

				if (FAILED(lresult))
				{
					break;
				}
				
				lresult = HRESULT_FROM_WIN32(RegDeleteTree(
					HKEY_CLASSES_ROOT, 
					aVerIndProgID));

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			return lresult;
		}

	}
}