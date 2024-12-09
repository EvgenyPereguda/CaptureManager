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

#include "MediaSinkFinalizeProcessor.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/Common.h"


namespace CaptureManager
{
	namespace Core
	{
		MediaSinkFinalizeProcessor::MediaSinkFinalizeProcessor()
		{
		}

		MediaSinkFinalizeProcessor::~MediaSinkFinalizeProcessor()
		{
		}

		HRESULT MediaSinkFinalizeProcessor::finalizeMediaSink(
			IMFMediaSink* aPtrMediaSink)
		{
			HRESULT lresult;

			do
			{
				std::unique_lock<std::mutex> lLock(mMutex);

				LOG_CHECK_PTR_MEMORY(aPtrMediaSink);
				
				CComPtrCustom<IMFFinalizableMediaSink> lFinalizableMediaSink;							

				LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrMediaSink, &lFinalizableMediaSink);
				
				LOG_CHECK_PTR_MEMORY(lFinalizableMediaSink);
				
				try
				{
					LOG_INVOKE_MF_METHOD(BeginFinalize, lFinalizableMediaSink, this, lFinalizableMediaSink);
					
					auto lconditionResult = mConditionVariable.wait_for(lLock, std::chrono::seconds(2));

					LOG_CHECK_STATE_DESCR(lconditionResult == std::cv_status::timeout, CONTEXT_E_SYNCH_TIMEOUT);
					
				}
				catch (...)
				{

				}

			} while (false);
			
			return lresult;
		}

		// IMFAsyncCallback implements

		HRESULT STDMETHODCALLTYPE MediaSinkFinalizeProcessor::GetParameters(
			DWORD* aPtrFlags,
			DWORD* aPtrQueue)
		{
			return S_OK;
		}

		HRESULT STDMETHODCALLTYPE MediaSinkFinalizeProcessor::Invoke(
			IMFAsyncResult* aPtrAsyncResult)
		{
			HRESULT lresult;

			do
			{
				std::unique_lock<std::mutex> lLock(mMutex);

				LOG_CHECK_PTR_MEMORY(aPtrAsyncResult);
								
				LOG_INVOKE_MF_METHOD(GetStatus, aPtrAsyncResult);
								
				CComPtrCustom<IUnknown> lMediaSinkUnk;

				LOG_INVOKE_POINTER_METHOD(aPtrAsyncResult, GetState, &lMediaSinkUnk);
				
				CComQIPtrCustom<IMFFinalizableMediaSink> lFinalizableMediaSink = lMediaSinkUnk;

				LOG_CHECK_PTR_MEMORY(lFinalizableMediaSink);
				
				LOG_INVOKE_MF_METHOD(EndFinalize, lFinalizableMediaSink, aPtrAsyncResult);
												
			} while (false);
			
			mConditionVariable.notify_one();

			return lresult;
		}
	}
}