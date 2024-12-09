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

#include "MCSSManager.h"
#include <VersionHelpers.h>
#include <Avrt.h>
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"

namespace CaptureManager
{
	namespace Core
	{
		const wchar_t* MCSSManager::mProAudioTaskName = L"Pro Audio";

		const wchar_t* MCSSManager::mCaptureTaskName = L"Capture";

		const wchar_t* MCSSManager::mLowLatencyTaskName = L"Low Latency";

		DWORD MCSSManager::mProAudioTaskID = 0;

		DWORD MCSSManager::mCaptureTaskID = 0;

		MCSSManager::MCSSManager()
		{
		}

		MCSSManager::~MCSSManager()
		{
		}

		HRESULT MCSSManager::createCaptureWorkQueue(DWORD* aPtrWorkQueue)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (IsWindows8OrGreater())
				{
					if (IsWindows8OrGreater())
					{
						LOG_INVOKE_MF_FUNCTION(MFLockSharedWorkQueue,
							mCaptureTaskName,
							AVRT_PRIORITY_CRITICAL,
							&mCaptureTaskID,
							aPtrWorkQueue);
					}
					else
					{
						LOG_INVOKE_MF_FUNCTION(MFAllocateWorkQueueEx,
							_MF_MULTITHREADED_WORKQUEUE,
							aPtrWorkQueue);
					}
				}

			} while (false);
			
			return lresult;
		}

		HRESULT MCSSManager::createProAudioWorkQueue(DWORD* aPtrWorkQueue)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (IsWindows8OrGreater())
				{
					LOG_INVOKE_MF_FUNCTION(MFLockSharedWorkQueue,
						mProAudioTaskName,
						AVRT_PRIORITY_CRITICAL,
						&mProAudioTaskID,
						aPtrWorkQueue);
				}
				else
				{
					LOG_INVOKE_MF_FUNCTION(MFAllocateWorkQueueEx,
						_MF_MULTITHREADED_WORKQUEUE,
						aPtrWorkQueue);
				}

			} while (false);

			if (FAILED(lresult))
			{
				*aPtrWorkQueue = MFASYNC_CALLBACK_QUEUE_MULTITHREADED;

				lresult = S_OK;
			}

			return lresult;
		}
		
		HRESULT MCSSManager::unlockWorkQueue(DWORD aWorkQueue)
		{
			HRESULT lresult = E_FAIL;

			do
			{

				LOG_INVOKE_MF_FUNCTION(MFUnlockWorkQueue,
					aWorkQueue);

			} while (false);

			return lresult;
		}

		// Initialize MCSS support
		HRESULT MCSSManager::initialize()
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (IsWindows8OrGreater())
				{
					//LOG_INVOKE_MF_FUNCTION(MFRegisterPlatformWithMMCSS,
					//	L"Pro Audio",
					//	&mTaskId,
					//	AVRT_PRIORITY_CRITICAL);
				}

			} while (false);

			lresult = S_OK;

			//if (FAILED(lresult))
			//{
			//	LogPrintOut::getInstance().printOutln(LogPrintOut::ERROR_LEVEL, L"DMOManager: DMO cannot be initialized!!!");
			//}
			//else
			//{
			//	LogPrintOut::getInstance().printOutln(LogPrintOut::INFO_LEVEL, L"DMOManager: DMO is initialized!!!");
			//}

			return lresult;
		}
	}
}