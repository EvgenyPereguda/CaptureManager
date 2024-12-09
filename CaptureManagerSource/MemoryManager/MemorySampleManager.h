#pragma once
#include "IMemorySampleManager.h"

struct IMFMediaType;

namespace CaptureManager
{
	namespace Core
	{
		class MemorySampleManager
		{
		public:

			static HRESULT createVideoMemorySampleManager(
				IMFMediaType* aPtrMediaType,
				IMemorySampleManager** aPtrPtrMemorySampleManager);

		private:
			MemorySampleManager() = delete;
			~MemorySampleManager() = delete;
		};
	}
}



