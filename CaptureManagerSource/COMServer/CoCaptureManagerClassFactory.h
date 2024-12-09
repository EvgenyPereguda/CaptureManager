#pragma once

#include <windows.h>
#include "../Common/BaseUnknown.h"

namespace CaptureManager
{
	namespace COMServer
	{
		class CoCaptureManagerClassFactory : 
			public BaseUnknown<IClassFactory>
		{
		public:
			CoCaptureManagerClassFactory();
			virtual ~CoCaptureManagerClassFactory();

			// IClassFactory interface
			STDMETHODIMP LockServer(
				BOOL aLock);
			STDMETHODIMP CreateInstance(
				LPUNKNOWN aPtrUnkOuter,
				REFIID aRefIID,
				void** aPtrPtrVoidObject);
		};
	}
}