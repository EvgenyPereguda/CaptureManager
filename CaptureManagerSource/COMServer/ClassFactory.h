#pragma once

#include <windows.h>
#include <atomic>


namespace CaptureManager
{
	namespace COMServer
	{
		class ClassFactory
		{
		public:

			HRESULT getClassObject(
				REFCLSID aRefCLSID,
				REFIID aRefIID,
				void** aPtrPtrVoidObject);

			HRESULT checkLock();

			ULONG lock();

			ULONG unlock();

		protected:
			ClassFactory();
			virtual ~ClassFactory();

		private:
			
			std::atomic<ULONG> mLockCount = 0;
		};
	}
}