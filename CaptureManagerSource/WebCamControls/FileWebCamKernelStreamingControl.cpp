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

//#define __STREAMS__
#define WIN32_LEAN_AND_MEAN
#include <Unknwnbase.h>
#include <windows.h>
#include <thread>
//#include <fileapi.h>
#include <winioctl.h>
#include <ks.h>
#include <ksmedia.h>
#include <map>
#include <memory>
#include <Ksproxy.h>

#include "../LogPrintOut/LogPrintOut.h"
#include "FileWebCamKernelStreamingControl.h"


namespace CaptureManager
{
	namespace Controls
	{
		namespace WebCamControls
		{
			namespace CustomisedWebCamControl
			{

				using namespace pugi;
				
				FileWebCamKernelStreamingControl::FileWebCamKernelStreamingControl(HANDLE aDevice):
					mDevice(aDevice)
				{
				}
				
				FileWebCamKernelStreamingControl::~FileWebCamKernelStreamingControl()
				{
					if (mDevice != nullptr)
						CloseHandle(mDevice);
				}
				
				HRESULT STDMETHODCALLTYPE FileWebCamKernelStreamingControl::processProperty(
					LPVOID aInputPtrBuffer, LONGLONG aInputLengthBuffer,
					LPVOID aOutputPtrBuffer, LONGLONG aOutputLengthBuffer)
				{
					HRESULT lresult;

					do
					{

						auto lInputPair = std::make_pair(aInputPtrBuffer, aInputLengthBuffer);

						auto lOutputPair = std::make_pair(aOutputPtrBuffer, aOutputLengthBuffer);


						auto lDevice = mDevice;


						std::mutex lDeviceReadyMutex;

						std::condition_variable lDeviceReadyCondition;

						std::unique_lock<std::mutex> lLock(lDeviceReadyMutex);

						auto lWaitResult = lDeviceReadyCondition.wait_for(lLock, std::chrono::milliseconds(200),
							[lDevice,
							lInputPair,
							lOutputPair,
							&lresult]
						{
							BOOL lcheckResult;

							ULONG lBytesReturned = 0;

							lcheckResult = DeviceIoControl(
								lDevice,                       // device to be queried
								IOCTL_KS_PROPERTY, // operation to perform
								lInputPair.first, lInputPair.second,// no input buffer
								lOutputPair.first, lOutputPair.second,// output buffer
								&lBytesReturned,                         // # bytes returned
								(LPOVERLAPPED)NULL);

							lresult = HRESULT_FROM_WIN32(GetLastError());

							if (lresult == 0x80070490)
							{
								return true;
							}

							//if (FAILED(lresult))
							//{
							//	LogPrintOut::getInstance().printOutln(
							//		LogPrintOut::ERROR_LEVEL,
							//		__FUNCTIONW__,
							//		L" Error code: ",
							//		(HRESULT)lresult);
							//}

							return lcheckResult != FALSE;
						});

						if (FAILED(lresult))
						{
							lresult = S_FALSE;

							break;
						}

						if (!lWaitResult)
						{
							lresult = S_FALSE;

							break;
						}

						lresult = S_OK;

					} while (false);

					return lresult;
				}
			}
		}
	}
}