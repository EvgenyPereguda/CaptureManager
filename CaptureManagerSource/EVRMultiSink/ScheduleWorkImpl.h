#pragma once

#include <mutex>
#include <condition_variable>

#include "IScheduler.h"
#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"


namespace EVRMultiSink
{
	namespace Sinks
	{
		namespace EVR
		{
			namespace Scheduler
			{
				class ScheduleWorkImpl :
					public IScheduler,
					public CaptureManager::BaseUnknown < IMFAsyncCallback >
				{
				private:

					ISchedulerCallback* mPtrCallback;

					CaptureManager::CComPtrCustom<IMFAsyncResult> mAsyncResult;

					MFWORKITEM_KEY mCancelKey;

					INT64 mPartFrameDurationMillSecTimeout;

					INT64 mPartFrameDuration100nsecondsTimeout;

					INT64 mFrameDuration100nseconds;

					MFTIME mLastTme;

					MFTIME mInitTime;

					MFTIME mShiftTime;

					INT64 lCycleCount;

					INT64 lCycleMax;

					bool mStopFlag;

					std::mutex mStopMutex;

					std::condition_variable mStopCondition;

				public:
					ScheduleWorkImpl(
						ISchedulerCallback* aPtrCallback);
					virtual ~ScheduleWorkImpl();
					HRESULT init(
						INT64 aFrameDuration100nseconds);

					virtual HRESULT start();
					virtual HRESULT stop();

					STDMETHODIMP GetParameters(DWORD*, DWORD*);
					STDMETHODIMP Invoke(IMFAsyncResult* pAsyncResult);
				};
			}
		}
	}
}