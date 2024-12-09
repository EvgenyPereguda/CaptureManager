#pragma once
#include <thread>
#include <memory>
#include <queue>
#include <mutex>
#include <string>
#include <functional>
#include <condition_variable>
#include "../Common/BaseUnknown.h"
#include "../AvrtManager/AvrtManager.h"


namespace CaptureManager
{
	namespace Core
	{
		class CaptureInvoker
		{
		public:
			CaptureInvoker(AVRT_PRIORITY_AvrtManager aAVRT_PRIORITY_AvrtManager, std::wstring aTaskName = L"Capture");
			virtual ~CaptureInvoker();
			
		protected:
			
			HRESULT start();

			HRESULT stop(std::function<void(void)> PostStopCallback = nullptr);

			virtual HRESULT STDMETHODCALLTYPE invoke();

			virtual HRESULT STDMETHODCALLTYPE starting();

		private:

			AVRT_PRIORITY_AvrtManager mAVRT_PRIORITY_AvrtManager;

			std::mutex mAccessMutex;

			std::unique_ptr<std::thread> mCaptureThread;

			const std::wstring mTaskName;

			enum CaptureInvokerState
			{
				Started,
				Stopped
			};

			CaptureInvokerState mCaptureInvokerState;
		};
	}
}