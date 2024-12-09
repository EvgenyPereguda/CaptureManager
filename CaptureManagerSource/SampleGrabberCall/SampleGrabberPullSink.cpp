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

#include "SampleGrabberPullSink.h"
#include "ReadWriteBuffer.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"

namespace CaptureManager
{
	namespace Sinks
	{
		namespace SampleGrabberPull
		{
			SampleGrabberPullSink::SampleGrabberPullSink()
			{
			}

			SampleGrabberPullSink::~SampleGrabberPullSink()
			{
			}

						
			// IMFMediaSink interface implementation
			STDMETHODIMP SampleGrabberPullSink::GetCharacteristics(
				DWORD* aPtrCharacteristics)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrCharacteristics);
					
					*aPtrCharacteristics = MEDIASINK_RATELESS | MEDIASINK_FIXED_STREAMS;

					lresult = S_OK;

				} while (false);

				return lresult;
			}
			
			STDMETHODIMP SampleGrabberPullSink::AddStreamSink(
				DWORD aStreamSinkIdentifier,
				IMFMediaType* aPtrMediaType,
				IMFStreamSink** aPtrPtrStreamSink)
			{
				HRESULT lresult;

				do
				{
					lresult = MF_E_STREAMSINKS_FIXED;

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullSink::RemoveStreamSink(
				DWORD aStreamSinkIdentifier)
			{
				HRESULT lresult;

				do
				{
					lresult = MF_E_STREAMSINKS_FIXED;
				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullSink::GetStreamSinkCount(
				DWORD* aPtrStreamSinkCount)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrStreamSinkCount);

					*aPtrStreamSinkCount = 1;

					lresult = S_OK;

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullSink::GetStreamSinkByIndex(
				DWORD aIndex,
				IMFStreamSink** aPtrPtrStreamSink)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_STATE_DESCR(aIndex != 0, E_BOUNDS);

					LOG_CHECK_PTR_MEMORY(aPtrPtrStreamSink);

					LOG_CHECK_PTR_MEMORY(mStreamSink);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mStreamSink, aPtrPtrStreamSink);

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullSink::GetStreamSinkById(
				DWORD aStreamSinkIdentifier,
				IMFStreamSink** aPtrPtrStreamSink)
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mMutex);

					LOG_CHECK_STATE_DESCR(aStreamSinkIdentifier != 0, E_BOUNDS);
				
					LOG_CHECK_PTR_MEMORY(aPtrPtrStreamSink);
					
					LOG_CHECK_PTR_MEMORY(mStreamSink);
					
					LOG_INVOKE_QUERY_INTERFACE_METHOD(mStreamSink, aPtrPtrStreamSink);

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullSink::SetPresentationClock(
				IMFPresentationClock* aPtrPresentationClock)
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPresentationClock);

					if (mClock)
					{
						LOG_INVOKE_POINTER_METHOD(mClock, RemoveClockStateSink, this);
					}
					
					LOG_INVOKE_POINTER_METHOD(aPtrPresentationClock, AddClockStateSink, this);
															
					mClock = aPtrPresentationClock;

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullSink::GetPresentationClock(
				IMFPresentationClock** aPtrPtrPresentationClock)
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mMutex);
					
					LOG_CHECK_PTR_MEMORY(aPtrPtrPresentationClock);

					LOG_CHECK_STATE_DESCR(!mClock, MF_E_NO_CLOCK);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mClock, aPtrPtrPresentationClock);
					
				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullSink::Shutdown()
			{
				HRESULT lresult;

				do
				{
					CComPtrCustom<IMFShutdown> lShutdown;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mStreamSink, &lShutdown);

					if (lShutdown)
						lShutdown->Shutdown();

					if (mClock)
						mClock->RemoveClockStateSink(this);

					lresult = S_OK;

				} while (false);

				return lresult;
			}
			
			// IMFClockStateSink interface
			STDMETHODIMP SampleGrabberPullSink::OnClockStart(
				MFTIME aSystemTime,
				LONGLONG aClockStartOffset)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(mStreamSink);

					CComPtrCustom<ISampleGrabberPullStream> lStreamSink;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mStreamSink, &lStreamSink);
					
					LOG_CHECK_PTR_MEMORY(lStreamSink);

					LOG_INVOKE_POINTER_METHOD(lStreamSink, start);

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullSink::OnClockStop(
				MFTIME aSystemTime)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(mStreamSink);

					CComPtrCustom<ISampleGrabberPullStream> lStreamSink;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mStreamSink, &lStreamSink);

					LOG_CHECK_PTR_MEMORY(lStreamSink);

					LOG_INVOKE_POINTER_METHOD(lStreamSink, stop);

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullSink::OnClockPause(
				MFTIME aSystemTime)
			{
				HRESULT lresult;

				do
				{
					lresult = E_NOTIMPL;

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullSink::OnClockRestart(
				MFTIME aSystemTime)
			{
				HRESULT lresult;

				do
				{
					lresult = E_NOTIMPL;

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullSink::OnClockSetRate(
				MFTIME aSystemTime,
				float aRate)
			{
				HRESULT lresult;

				do
				{
					lresult = E_NOTIMPL;

				} while (false);

				return lresult;
			}


			// IMFFinalizableMediaSink interface
			STDMETHODIMP SampleGrabberPullSink::BeginFinalize(
				IMFAsyncCallback* aPtrAsyncCallback,
				IUnknown* aPtrUnkState)
			{
				HRESULT lresult;

				do
				{
					lresult = E_NOTIMPL;

				} while (false);

				return lresult;
			}

			STDMETHODIMP SampleGrabberPullSink::EndFinalize(
				IMFAsyncResult* aPtrAsyncResult)
			{
				HRESULT lresult;

				do
				{
					lresult = E_NOTIMPL;

				} while (false);

				return lresult;
			}



			// IMFAsyncCallback interface implementation
			STDMETHODIMP SampleGrabberPullSink::GetParameters(
				DWORD* aPtrFlags,
				DWORD* aPtrQueue)
			{
				HRESULT lresult;

				do
				{
					lresult = E_NOTIMPL;

				} while (false);

				return lresult;
			}
			
			STDMETHODIMP SampleGrabberPullSink::Invoke(
				IMFAsyncResult* aPtrAsyncResult)
			{
				HRESULT lresult;

				do
				{
					lresult = E_NOTIMPL;

				} while (false);

				return lresult;
			}
			
			STDMETHODIMP SampleGrabberPullSink::setStream(
				IUnknown* aPtrUnkSampleGrabberPullStream)
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrUnkSampleGrabberPullStream);

					if (mStreamSink)
					{
						mStreamSink.Release();
					}

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrUnkSampleGrabberPullStream, &mStreamSink);
					
				} while (false);

				return lresult;
			}
		}
	}
}