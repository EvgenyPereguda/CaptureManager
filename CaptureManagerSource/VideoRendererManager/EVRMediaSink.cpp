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

#include "EVRMediaSink.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "EVRStreamSink.h"
#include "IPresenter.h"

namespace CaptureManager
{
	namespace Sinks
	{
		namespace EVR
		{
			using namespace Core;

			EVRMediaSink::EVRMediaSink():
				mIsShutdown(false),
				mStreamID(0)
			{
			}
			
			EVRMediaSink::~EVRMediaSink()
			{
			}


			HRESULT EVRMediaSink::AddStreamSink(
				DWORD aStreamSinkIdentifier,
				IMFMediaType* aPtrMediaType,
				IMFStreamSink** aPtrPtrStreamSink)
			{
				return MF_E_STREAMSINKS_FIXED;
			}

			HRESULT EVRMediaSink::GetCharacteristics(
				DWORD* aPtrCharacteristics)
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_CHECK_PTR_MEMORY(aPtrCharacteristics);

					LOG_INVOKE_FUNCTION(checkShutdown);

					*aPtrCharacteristics = MEDIASINK_FIXED_STREAMS | MEDIASINK_CAN_PREROLL;

				} while (false);

				return lresult;
			}


			HRESULT EVRMediaSink::GetPresentationClock(
				IMFPresentationClock** aPtrPtrPresentationClock)
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPtrPresentationClock);

					LOG_INVOKE_FUNCTION(checkShutdown);

					LOG_CHECK_STATE_DESCR(!mClock, MF_E_NO_CLOCK);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mClock, aPtrPtrPresentationClock);
					
				} while (false);

				return lresult;
			}

			HRESULT EVRMediaSink::GetStreamSinkById(
				DWORD aIdentifier,
				IMFStreamSink** aPtrPtrStreamSink) 
			{
				HRESULT lresult;

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrPtrStreamSink);

					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_CHECK_STATE_DESCR(aIdentifier != mStreamID, MF_E_INVALIDSTREAMNUMBER);

					LOG_INVOKE_FUNCTION(checkShutdown);
					
					LOG_INVOKE_QUERY_INTERFACE_METHOD(mStream, aPtrPtrStreamSink);

				} while (false);

				return lresult;				
			}

			HRESULT EVRMediaSink::GetStreamSinkByIndex(
				DWORD aIndex,
				IMFStreamSink** aPtrPtrStreamSink)
			{
				HRESULT lresult;

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrStreamSink);

					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);

					LOG_CHECK_STATE_DESCR(aIndex != mStreamID, MF_E_INVALIDSTREAMNUMBER);

					LOG_INVOKE_FUNCTION(checkShutdown);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mStream, aPtrPtrStreamSink);

				} while (false);

				return lresult;
			}

			HRESULT EVRMediaSink::GetStreamSinkCount(
				DWORD* aPtrStreamSinkCount)
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_CHECK_PTR_MEMORY(aPtrStreamSinkCount);

					LOG_INVOKE_FUNCTION(checkShutdown);

					*aPtrStreamSinkCount = 1;

				} while (false);

				return lresult;
			}

			HRESULT EVRMediaSink::RemoveStreamSink(
				DWORD aStreamSinkIdentifier)
			{				
				return MF_E_STREAMSINKS_FIXED;
			}

			HRESULT EVRMediaSink::SetPresentationClock(
				IMFPresentationClock* aPtrPresentationClock)
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_CHECK_PTR_MEMORY(aPtrPresentationClock);

					LOG_INVOKE_FUNCTION(checkShutdown);

					CComPtrCustom<IMFClockStateSink> lClockStateSink;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mStream, &lClockStateSink);

					LOG_CHECK_PTR_MEMORY(lClockStateSink);

					if (mClock)
					{
						LOG_INVOKE_MF_METHOD(RemoveClockStateSink, mClock, lClockStateSink);
					}

					LOG_INVOKE_MF_METHOD(AddClockStateSink, aPtrPresentationClock, lClockStateSink);

					mClock = aPtrPresentationClock;
										
				} while (false);

				return lresult;
			}

			HRESULT EVRMediaSink::Shutdown()
			{
				HRESULT lresult;

				do
				{
					std::lock_guard<std::mutex> lLock(mAccessMutex);

					LOG_INVOKE_FUNCTION(checkShutdown);
					
					mIsShutdown = true;

					if (mStream)
					{
						mStream->shutdown();
					}
					
				} while (false);

				return lresult;
			}


			HRESULT EVRMediaSink::GetService(
				REFGUID aRefGUIDService,
				REFIID aRefIID,
				LPVOID* aPtrPtrObject)
			{
				HRESULT lresult(MF_E_UNSUPPORTED_SERVICE);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrObject);

					if (aRefGUIDService == MR_VIDEO_MIXER_SERVICE)
					{
						if (aRefIID == __uuidof(IMFTransform))
						{
							if (mMixer)
							{
								LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(mMixer,
									aRefIID,
									aPtrPtrObject);
							}
							else
							{
								lresult = E_NOTIMPL;
							}
						}

					}

				} while (false);

				return lresult;
			}
			
			HRESULT EVRMediaSink::createEVRMediaSink(
				IPresenter* aPtrPresenter,
				IMFTransform* aPtrMixer,
				DWORD aMixerStreamID,
				bool aIsSingleStream,
				IMFMediaSink** aPtrPtrMediaSink)
			{
				HRESULT lresult(E_FAIL);

				do
				{

					LOG_CHECK_PTR_MEMORY(aPtrPresenter);

					LOG_CHECK_PTR_MEMORY(aPtrMixer);

					LOG_CHECK_PTR_MEMORY(aPtrPtrMediaSink);

					CComPtrCustom<EVRMediaSink> lEVRMediaSink(new (std::nothrow) EVRMediaSink);

					LOG_CHECK_PTR_MEMORY(lEVRMediaSink);
										
					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrMixer, &lEVRMediaSink->mMixer);							
					
					LOG_INVOKE_FUNCTION(EVRStreamSink::createEVRStreamSink,
						lEVRMediaSink->mStreamID,
						lEVRMediaSink,
						aPtrPresenter,
						aMixerStreamID,
						aIsSingleStream,
						&lEVRMediaSink->mStream);					
					
					LOG_CHECK_PTR_MEMORY(lEVRMediaSink->mMixer);
					
					LOG_INVOKE_QUERY_INTERFACE_METHOD(lEVRMediaSink, aPtrPtrMediaSink);
					
				} while (false);

				return lresult;
			}
			
			HRESULT EVRMediaSink::checkShutdown() const
			{
				if (mIsShutdown)
				{
					return MF_E_SHUTDOWN;
				}
				else
				{
					return S_OK;
				}
			}
		}
	}
}