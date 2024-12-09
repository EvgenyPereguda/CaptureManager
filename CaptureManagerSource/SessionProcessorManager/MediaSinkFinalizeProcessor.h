#pragma once

#include <condition_variable>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"

namespace CaptureManager
{
	namespace Core
	{
		class MediaSinkFinalizeProcessor :
			public BaseUnknown<IMFAsyncCallback>
		{
		public:
			MediaSinkFinalizeProcessor();
			virtual ~MediaSinkFinalizeProcessor();

			HRESULT finalizeMediaSink(
				IMFMediaSink* aPtrMediaSink);

			// IMFAsyncCallback implements

			virtual HRESULT STDMETHODCALLTYPE GetParameters(
				DWORD* aPtrFlags,
				DWORD* aPtrQueue);

			virtual HRESULT STDMETHODCALLTYPE Invoke(
				IMFAsyncResult* aPtrAsyncResult);
			
		private:

			std::mutex mMutex;

			std::condition_variable mConditionVariable;
		};
	}
}