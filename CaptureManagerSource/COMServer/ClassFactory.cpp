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

#include "ClassFactory.h"
#include "CaptureManagerTypeInfo_i.c"
#include "CoLogPrintOutClassFactory.h"
#include "CoCaptureManagerClassFactory.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/Common.h"


namespace CaptureManager
{
	namespace COMServer
	{
		ClassFactory::ClassFactory()
		{
		}

		ClassFactory::~ClassFactory()
		{
		}

		HRESULT ClassFactory::getClassObject(
			REFCLSID aRefCLSID,
			REFIID aRefIID,
			void** aPtrPtrVoidObject)
		{
			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrVoidObject);
				
				lresult = CLASS_E_CLASSNOTAVAILABLE;
				
				if (aRefCLSID == CLSID_CoLogPrintOut)
				{
					CComPtrCustom<CoLogPrintOutClassFactory> lClassFactory(
						new (std::nothrow) CoLogPrintOutClassFactory());

					LOG_CHECK_PTR_MEMORY(lClassFactory);
										
					LOG_INVOKE_POINTER_METHOD(lClassFactory, QueryInterface,
						aRefIID,
						aPtrPtrVoidObject);
				}
				else if (aRefCLSID == CLSID_CoCaptureManager)
				{
					CComPtrCustom<CoCaptureManagerClassFactory> lClassFactory(
						new (std::nothrow) CoCaptureManagerClassFactory());

					LOG_CHECK_PTR_MEMORY(lClassFactory);

					LOG_INVOKE_POINTER_METHOD(lClassFactory, QueryInterface,
						aRefIID,
						aPtrPtrVoidObject);
				}

			} while (false);

			return lresult;
		}

		HRESULT ClassFactory::checkLock()
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (mLockCount == 0)
				{
					lresult = S_OK;
				}
				else
				{
					lresult = S_FALSE;
				}

			} while (false);

			return lresult;
		}

		ULONG ClassFactory::lock()
		{
			return ++mLockCount;
		}

		ULONG ClassFactory::unlock()
		{
			return --mLockCount;
		}
	}
}