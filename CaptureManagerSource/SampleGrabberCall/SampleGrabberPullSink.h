#pragma once

#include <atomic>
#include <mutex>

#include "SampleGrabberPullStream.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/BaseUnknown.h"

namespace CaptureManager
{
	namespace Sinks
	{
		namespace SampleGrabberPull
		{
			class SampleGrabberPullSink :
				public BaseUnknown<
				IMFFinalizableMediaSink,
				IMFClockStateSink,
				IMFAsyncCallback>
			{
			public:
				SampleGrabberPullSink();
				

				// IMFMediaSink interface
				STDMETHODIMP GetCharacteristics(
					DWORD* aPtrCharacteristics);
				STDMETHODIMP AddStreamSink(
					DWORD aStreamSinkIdentifier, 
					IMFMediaType* aPtrMediaType, 
					IMFStreamSink** aPtrPtrStreamSink);
				STDMETHODIMP RemoveStreamSink(
					DWORD aStreamSinkIdentifier);
				STDMETHODIMP GetStreamSinkCount(
					DWORD* aPtrStreamSinkCount);
				STDMETHODIMP GetStreamSinkByIndex(
					DWORD aIndex, 
					IMFStreamSink** aPtrPtrStreamSink);
				STDMETHODIMP GetStreamSinkById(
					DWORD aStreamSinkIdentifier, 
					IMFStreamSink** aPtrPtrStreamSink);
				STDMETHODIMP SetPresentationClock(
					IMFPresentationClock* aPtrPresentationClock);
				STDMETHODIMP GetPresentationClock(
					IMFPresentationClock** aPtrPtrPresentationClock);
				STDMETHODIMP Shutdown();


				// IMFClockStateSink interface
				STDMETHODIMP OnClockStart(
					MFTIME aSystemTime, 
					LONGLONG aClockStartOffset);
				STDMETHODIMP OnClockStop(
					MFTIME aSystemTime);
				STDMETHODIMP OnClockPause(
					MFTIME aSystemTime);
				STDMETHODIMP OnClockRestart(
					MFTIME aSystemTime);
				STDMETHODIMP OnClockSetRate(
					MFTIME aSystemTime, 
					float aRate);


				// IMFFinalizableMediaSink interface
				STDMETHODIMP BeginFinalize(
					IMFAsyncCallback* aPtrAsyncCallback, 
					IUnknown* aPtrUnkState);
				STDMETHODIMP EndFinalize(
					IMFAsyncResult* aPtrAsyncResult);


				// IMFAsyncCallback interface implementation
				STDMETHODIMP GetParameters(
					DWORD* aPtrFlags, 
					DWORD* aPtrQueue);

				STDMETHODIMP Invoke(
					IMFAsyncResult* aPtrAsyncResult);


				// SampleGrabberPullSink interface implementation

				STDMETHODIMP setStream(
					IUnknown* aPtrUnkSampleGrabberPullStream);

				virtual ~SampleGrabberPullSink();
				
			protected:				

				virtual bool findInterface(
					REFIID aRefIID,
					void** aPtrPtrVoidObject)
				{
					if (aRefIID == __uuidof(IMFMediaSink))
					{
						return castInterfaces(
							aRefIID,
							aPtrPtrVoidObject,
							static_cast<IMFMediaSink*>(this));
					}
					else
					{
						return BaseUnknown::findInterface(
							aRefIID,
							aPtrPtrVoidObject);
					}
				}

			private:

				std::mutex mMutex;

				CComPtrCustom<IMFStreamSink> mStreamSink;

				CComPtrCustom<IMFPresentationClock> mClock;

			};
		}
	}
}