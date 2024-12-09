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

#include "WebCamKernelStreamingControlManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "./FileWebCamKernelStreamingControl.h"
#include "./KsControlWebCamKernelStreamingControl.h"
#define WIN32_LEAN_AND_MEAN
#include <Unknwnbase.h>
#include <windows.h>
#include <thread>
//#include <fileapi.h>
#include <winioctl.h>
#include <ks.h>
#include <ksmedia.h>
#include <ksproxy.h>

namespace CaptureManager
{
	namespace Controls
	{
		namespace WebCamControls
		{
			using namespace CustomisedWebCamControl;

			HRESULT WebCamKernelStreamingControlManager::createIWebCamKernelStreamingControlFromFile(
				std::wstring aSymbolicLink,
				IWebCamKernelStreamingControl** aPtrPtrIWebCamKernelStreamingControl)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					if (aSymbolicLink.empty())
						break;

					if (aPtrPtrIWebCamKernelStreamingControl == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					HANDLE lDevice = INVALID_HANDLE_VALUE;  // handle to the drive to be examined 

					CREATEFILE2_EXTENDED_PARAMETERS extendedParams = { 0 };
					extendedParams.dwSize = sizeof(CREATEFILE2_EXTENDED_PARAMETERS);
					extendedParams.dwFileAttributes = FILE_ATTRIBUTE_NORMAL;
					extendedParams.dwFileFlags = FILE_FLAG_SEQUENTIAL_SCAN;
					extendedParams.dwSecurityQosFlags = SECURITY_ANONYMOUS;
					extendedParams.lpSecurityAttributes = nullptr;
					extendedParams.hTemplateFile = nullptr;

					//lDevice = CreateFile2(
					//	aSymbolicLink.c_str(),
					//	0,
					//	FILE_SHARE_READ | FILE_SHARE_WRITE,
					//	OPEN_EXISTING,
					//	&extendedParams
					//	);

					lDevice = CreateFile(aSymbolicLink.c_str(),          // drive to open
						0,                // no access to the drive
						FILE_SHARE_READ | // share mode
						FILE_SHARE_WRITE,
						NULL,             // default security attributes
						OPEN_EXISTING,    // disposition
						0,                // file attributes
						NULL);            // do not copy file attributes

					if (lDevice == INVALID_HANDLE_VALUE)    // cannot open the drive
					{
						lresult = S_FALSE;

						break;
					}

					*aPtrPtrIWebCamKernelStreamingControl =
						new (std::nothrow) FileWebCamKernelStreamingControl(lDevice);

					if (*aPtrPtrIWebCamKernelStreamingControl == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			HRESULT WebCamKernelStreamingControlManager::createIWebCamKernelStreamingControlFromKsControl(
				IUnknown* aUnkIKsControl,
				IWebCamKernelStreamingControl** aPtrPtrIWebCamKernelStreamingControl)
			{
				HRESULT lresult(E_FAIL);

				do
				{

					LOG_CHECK_PTR_MEMORY(aUnkIKsControl);

					CComPtrCustom<IKsControl> lIKsControl;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aUnkIKsControl, &lIKsControl);

					LOG_CHECK_PTR_MEMORY(lIKsControl);
									   
					*aPtrPtrIWebCamKernelStreamingControl =
						new (std::nothrow) KsControlWebCamKernelStreamingControl(lIKsControl.get());

					if (*aPtrPtrIWebCamKernelStreamingControl == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					lresult = S_OK;

				} while (false);


				return lresult;
			}
		}
	}
}
