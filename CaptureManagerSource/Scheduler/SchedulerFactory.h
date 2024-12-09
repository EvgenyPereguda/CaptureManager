#pragma once

typedef signed __int64      INT64;
#pragma warning(suppress: 28251)
typedef long HRESULT;

namespace CaptureManager
{
	namespace Core
	{
		struct IScheduler;

		struct ISchedulerCallback;

		class SchedulerFactory
		{
		public:

			static HRESULT createScheduler(
				ISchedulerCallback* aPtrCallback,
				INT64 aFrameDuration100nseconds,
				IScheduler** aPtrPtrIScheduler);

		private:
			SchedulerFactory() = delete;
			~SchedulerFactory() = delete;
		};
	}
}