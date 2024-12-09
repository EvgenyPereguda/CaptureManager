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

#include "CaptureInvoker.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Singleton.h"


namespace CaptureManager
{
	namespace Core
	{
		CaptureInvoker::CaptureInvoker(AVRT_PRIORITY_AvrtManager aAVRT_PRIORITY_AvrtManager, std::wstring aTaskName) :
			mCaptureInvokerState(CaptureInvokerState::Stopped),
			mAVRT_PRIORITY_AvrtManager(aAVRT_PRIORITY_AvrtManager),
			mTaskName(aTaskName)
		{
		}

		CaptureInvoker::~CaptureInvoker()
		{
		}

		HRESULT CaptureInvoker::start()
		{

			HRESULT lresult(E_NOTIMPL);

			do
			{
				LOG_INVOKE_FUNCTION(Singleton<Core::AvrtManager>::getInstance().initialize);				

				std::lock_guard<std::mutex> lLock(mAccessMutex);
				
				mCaptureInvokerState = CaptureInvokerState::Started;
				
				mCaptureThread.reset(new std::thread(
					[this]()
				{
					HANDLE mmcssHandle = NULL;

					DWORD mmcssTaskIndex = 0;

					mmcssHandle = AvrtManager::AvSetMmThreadCharacteristicsW(mTaskName.c_str(), &mmcssTaskIndex);

					if (mmcssHandle != nullptr)
						AvrtManager::AvSetMmThreadPriority(mmcssHandle, mAVRT_PRIORITY_AvrtManager);

					starting();
															
					while (mCaptureInvokerState == CaptureInvokerState::Started)
					{
						this->invoke();
					}

					if (mmcssHandle != nullptr)
						AvrtManager::AvRevertMmThreadCharacteristics(mmcssHandle);
				}
				));

			} while (false);

			return lresult;
		}

		HRESULT CaptureInvoker::stop(std::function<void(void)> PostStopCallback)
		{

			HRESULT lresult(E_NOTIMPL);

			do
			{
				lresult = S_OK;

				if (mCaptureInvokerState != CaptureInvokerState::Started)
				{
					break;
				}

				mCaptureInvokerState = CaptureInvokerState::Stopped;

				if (PostStopCallback != nullptr)
					PostStopCallback();

				if (mCaptureThread)
					mCaptureThread->join();

				mCaptureThread.reset();
				
			} while (false);

			return lresult;
		}
		
		HRESULT STDMETHODCALLTYPE CaptureInvoker::invoke()
		{
			return E_NOTIMPL;
		}
		HRESULT STDMETHODCALLTYPE CaptureInvoker::starting()
		{
			return E_NOTIMPL;
		}
	}
}