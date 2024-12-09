#pragma once

#include <windows.h>
#include "../Common/BaseUnknown.h"

namespace CaptureManager
{
	namespace COMServer
	{
		class CoLogPrintOutClassFactory :
			public BaseUnknown<IClassFactory>
		{
		public:
			CoLogPrintOutClassFactory();
			virtual ~CoLogPrintOutClassFactory();


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