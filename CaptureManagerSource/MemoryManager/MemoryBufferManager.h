#pragma once

#include <stack>
#include <list>
#include <mutex>

#include "IMemoryBufferManager.h"


namespace CaptureManager
{
	namespace Core
	{
		class MemoryBufferManager
		{
		public:
			
			static HRESULT createSystemMemoryBufferManager(
				DWORD aMaxBufferLength, 
				IMemoryBufferManager** aPtrPtrMemoryBufferManager);

		private:
			MemoryBufferManager() = delete;
			~MemoryBufferManager() = delete;
		};
	}
}

