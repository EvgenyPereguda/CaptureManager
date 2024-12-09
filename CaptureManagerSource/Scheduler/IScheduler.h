#pragma once

#pragma warning(suppress: 28251)
typedef long HRESULT;

namespace CaptureManager
{
	namespace Core
	{
		struct ISchedulerCallback
		{
			virtual void callback() = 0;

			virtual ~ISchedulerCallback(){};
		};

		struct IScheduler
		{
			virtual HRESULT start() = 0;

			virtual HRESULT stop() = 0;

			virtual ~IScheduler(){};
		};
	}
}