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

#include "ScheduleWorkImpl.h"

#include "../MediaFoundationManager/MediaFoundationManager.h"

#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"

namespace EVRMultiSink
{
	namespace Sinks
	{
		namespace EVR
		{
			namespace Scheduler
			{
				using namespace CaptureManager;
				using namespace CaptureManager::Core;

				ScheduleWorkImpl::ScheduleWorkImpl(
					ISchedulerCallback* aPtrCallback) :
					mPtrCallback(aPtrCallback)
				{
				}

				ScheduleWorkImpl::~ScheduleWorkImpl()
				{
				}

				HRESULT ScheduleWorkImpl::init(
					INT64 aFrameDuration100nseconds)
				{
					HRESULT lresult;

					do
					{
						lCycleMax = 4;

						lCycleCount = 0;

						LOG_INVOKE_MF_FUNCTION(MFCreateAsyncResult,
							nullptr,
							this,
							nullptr,
							&mAsyncResult);

						mFrameDuration100nseconds = aFrameDuration100nseconds;

						mPartFrameDuration100nsecondsTimeout = mFrameDuration100nseconds / lCycleMax;

						mPartFrameDurationMillSecTimeout = mPartFrameDuration100nsecondsTimeout / 10000;


					} while (false);

					return lresult;
				}

				HRESULT ScheduleWorkImpl::start()
				{
					HRESULT lresult;

					do
					{
						auto lInitTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

						if (lInitTime == 0)
						{
							lInitTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();
						}

						LOG_CHECK_STATE(lInitTime == 0);

						mLastTme = 0;

						mInitTime = 0;

						mShiftTime = 0;

						mStopFlag = false;

						LOG_INVOKE_MF_FUNCTION(MFScheduleWorkItemEx,
							mAsyncResult,
							mPartFrameDurationMillSecTimeout,
							&mCancelKey);

					} while (false);

					return lresult;

				}

				HRESULT ScheduleWorkImpl::stop()
				{
					std::unique_lock<std::mutex> lLock(mStopMutex);

					mStopFlag = true;

					HRESULT lresult(E_FAIL);

					do
					{
						LOG_INVOKE_MF_FUNCTION(MFCancelWorkItem,
							mCancelKey);

					} while (false);

					mStopCondition.wait_for(lLock, std::chrono::milliseconds(100));

					return S_OK;
				}

				STDMETHODIMP ScheduleWorkImpl::GetParameters(DWORD*, DWORD*)
				{
					return E_NOTIMPL;
				}

				//int j = 0;

				STDMETHODIMP ScheduleWorkImpl::Invoke(IMFAsyncResult* pAsyncResult)
				{
					if (mStopFlag)
					{
						std::lock_guard<std::mutex> lLock(mStopMutex);

						mStopCondition.notify_one();

						return S_OK;
					}

					//auto lInitTime = MediaFoundation::MediaFoundationManager::MFGetSystemTime();

					//if (mLastTme == 0)
					//{
					//	HRESULT lresult;

					//	mLastTme = lInitTime;

					//	mInitTime = lInitTime;

					//	lCycleCount = 0;

					//	do
					//	{

					//		LOG_INVOKE_MF_FUNCTION(MFScheduleWorkItemEx,
					//			mAsyncResult,
					//			-mPartFrameDurationMillSecTimeout,
					//			&mCancelKey);

					//	} while (false);

					//	return S_OK;
					//}

					//if (++lCycleCount >= lCycleMax)
					//{
					//	if (mPtrCallback != nullptr)
					//		mPtrCallback->callback();

					//	lCycleCount = 0;

					//	auto lduration = (lInitTime - mLastTme);

					//	mShiftTime += lduration - mFrameDuration100nseconds;

					//	mLastTme = lInitTime;

					//	auto lTemp = mShiftTime;


					//	while (lTemp >= mPartFrameDuration100nsecondsTimeout)
					//	{
					//		if (++lCycleCount >= lCycleMax)
					//			break;
					//		lTemp -= mPartFrameDuration100nsecondsTimeout;
					//	}

					//	//LogPrintOut::getInstance().printOutln(
					//	//	LogPrintOut::ERROR_LEVEL,
					//	//	L" mShiftTime: ",
					//	//	mShiftTime,
					//	//	L", lduration: ",
					//	//	lduration,
					//	//	L", Time: ",
					//	//	lInitTime - mInitTime,
					//	//	L", lCycleCount: ",
					//	//	lCycleCount,
					//	//	L", J: ",
					//	//	++j);
					//}

					HRESULT lresult;

					do
					{
						if (mPtrCallback != nullptr)
							mPtrCallback->callback();

						LOG_INVOKE_MF_FUNCTION(MFScheduleWorkItemEx,
							mAsyncResult,
							mPartFrameDurationMillSecTimeout,
							&mCancelKey);

					} while (false);

					return S_OK;
				}
			}
		}
	}
}