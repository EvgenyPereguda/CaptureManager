#pragma once

#include <Shlwapi.h>


#include "../Common/MFHeaders.h"
#include "../Common/BaseUnknown.h"


namespace CaptureManager
{
	namespace Sinks
	{
		namespace SampleGrabberCall
		{
			namespace RegularSampleGrabberCall
			{
				template<typename Callback>
				class SampleGrabberSink :
					public BaseUnknown < IMFSampleGrabberSinkCallback >
				{
				protected:

					virtual bool findInterface(
						REFIID aRefIID,
						void** aPtrPtrVoidObject)
					{
						if (aRefIID == __uuidof(IMFSampleGrabberSinkCallback))
						{
							return castInterfaces(
								aRefIID,
								aPtrPtrVoidObject,
								static_cast<IMFSampleGrabberSinkCallback*>(this));
						}
						else if (aRefIID == __uuidof(IMFClockStateSink))
						{
							return castInterfaces(
								aRefIID,
								aPtrPtrVoidObject,
								static_cast<IMFClockStateSink*>(this));
						}
						else
						{
							return BaseUnknown::findInterface(
								aRefIID,
								aPtrPtrVoidObject);
						}
					}

				public:

					SampleGrabberSink(Callback* aSampleGrabberCallback = nullptr)
					{
						mSampleGrabberCallback = aSampleGrabberCallback;
					}

					virtual ~SampleGrabberSink(){}
					
					STDMETHODIMP OnClockStart(
						MFTIME aHnsSystemTime,
						LONGLONG aClockStartOffset)
					{
						return S_OK;
					}

					STDMETHODIMP OnClockStop(
						MFTIME aHnsSystemTime)
					{
						return S_OK;
					}

					STDMETHODIMP OnClockPause(
						MFTIME aHnsSystemTime)
					{
						return S_OK;
					}

					STDMETHODIMP OnClockRestart(
						MFTIME aHnsSystemTime)
					{
						return S_OK;
					}

					STDMETHODIMP OnClockSetRate(
						MFTIME aHnsSystemTime,
						float aRate)
					{
						return S_OK;
					}

					STDMETHODIMP OnSetPresentationClock(
						IMFPresentationClock* aPtrClock)
					{
						return S_OK;
					}

					STDMETHODIMP OnProcessSample(
						REFGUID aGUIDMajorMediaType,
						DWORD aSampleFlags,
						LONGLONG aSampleTime,
						LONGLONG aSampleDuration,
						const BYTE* aPtrSampleBuffer,
						DWORD aSampleSize)
					{
						mSampleGrabberCallback->writeData(
							aPtrSampleBuffer,
							aSampleSize);

						return S_OK;
					}

					STDMETHODIMP OnShutdown()
					{
						return S_OK;
					}


				private:

					CComPtrCustom<Callback> mSampleGrabberCallback;
				};
								
			}
		}
	}
}