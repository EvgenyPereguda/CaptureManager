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

#include "SessionAsyncCallback.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"

namespace CaptureManager
{
	namespace Core
	{
		STDMETHODIMP SessionAsyncCallback::GetParameters(
			__RPC__out DWORD *pdwFlags,
			__RPC__out DWORD *pdwQueue)
		{
			HRESULT lresult;

			do
			{
				lresult = E_NOTIMPL;

			} while (false);

			return lresult;
		}

		STDMETHODIMP SessionAsyncCallback::Invoke(
			IMFAsyncResult *aPtrAsyncResult)
		{
			HRESULT lresult;;

			do
			{				
				LOG_CHECK_PTR_MEMORY(aPtrAsyncResult);
				
				std::lock_guard<std::mutex> lLock(mInvokeMutex);

				CComPtrCustom<IUnknown> lUnkState;

				CComQIPtrCustom<ISessionAsyncCallbackRequest> lRequest;

				LOG_INVOKE_MF_METHOD(GetStatus, aPtrAsyncResult);

				LOG_INVOKE_MF_METHOD(GetState, aPtrAsyncResult, &lUnkState);
				
				lRequest = lUnkState;

				LOG_CHECK_PTR_MEMORY(lRequest);
				
				lRequest->invoke();

			} while (false);
			
			return lresult;
		}
	}
}