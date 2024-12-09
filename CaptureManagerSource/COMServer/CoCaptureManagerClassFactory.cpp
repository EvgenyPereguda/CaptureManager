/*
MIT License

Copyright(c) 2020 Evgeny Pereguda

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <memory>
#include "CoCaptureManagerClassFactory.h"
#include "../Common/Singleton.h"
#include "../Common/ComPtrCustom.h"
#include "ClassFactory.h"
#include "CoCaptureManager.h"
#include "../CaptureManagerBroker/CaptureManagerBroker.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"

namespace CaptureManager
{
	namespace COMServer
	{
		CoCaptureManagerClassFactory::CoCaptureManagerClassFactory()
		{
			Singleton<ClassFactory>::getInstance().lock();

			Singleton<CaptureManagerBroker>::getInstance();
		}

		CoCaptureManagerClassFactory::~CoCaptureManagerClassFactory()
		{
			Singleton<ClassFactory>::getInstance().unlock();
		}


		// IClassFactory interface implementation
		STDMETHODIMP CoCaptureManagerClassFactory::LockServer(BOOL aLock)
		{
			if (aLock)
				Singleton<ClassFactory>::getInstance().lock();
			else
				Singleton<ClassFactory>::getInstance().unlock();

			return S_OK;
		}

		STDMETHODIMP CoCaptureManagerClassFactory::CreateInstance(
			LPUNKNOWN aPtrUnkOuter,
			REFIID aRefIID,
			void** aPtrPtrVoidObject)
		{
			HRESULT lresult = E_NOINTERFACE;

			do
			{
				LOG_CHECK_STATE_DESCR(aPtrUnkOuter != nullptr, CLASS_E_NOAGGREGATION);
				
				CComPtrCustom<CoCaptureManager> lCoCaptureManager(new (std::nothrow) CoCaptureManager());

				LOG_CHECK_PTR_MEMORY(lCoCaptureManager);
				
				LOG_INVOKE_POINTER_METHOD(lCoCaptureManager, QueryInterface,
					aRefIID,
					aPtrPtrVoidObject);
				
			} while (false);

			return lresult;

		}
	}
}