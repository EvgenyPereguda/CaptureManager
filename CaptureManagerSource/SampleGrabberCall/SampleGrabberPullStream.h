#pragma once

#include <atomic>
#include <mutex>

#include "../Common/MFHeaders.h"
#include "../Common/BaseUnknown.h"
#include "../Common/ComPtrCustom.h"
#include "ReadWriteBufferRegularSync.h"
#include "ISampleGrabberPullStream.h"

namespace CaptureManager
{
	namespace Sinks
	{
		namespace SampleGrabberPull
		{
			class SampleGrabberPullSink;


			class SampleGrabberPullStream :
				public BaseUnknown <
				IMFShutdown,
				IMFStreamSink,
				IMFMediaTypeHandler,
				ISampleGrabberPullStream,
				SampleGrabberCall::RegularSampleGrabberCall::ReadWriteBufferRegularSync
				>
			{

			public:
				SampleGrabberPullStream();

				virtual ~SampleGrabberPullStream();		

				// IMFShutdown interface implmenentation

				virtual HRESULT STDMETHODCALLTYPE Shutdown()override;

				virtual HRESULT STDMETHODCALLTYPE GetShutdownStatus(MFSHUTDOWN_STATUS * aPtrStatus)override;


				// IMFStreamSink interface implementation
				STDMETHODIMP GetMediaSink(
					IMFMediaSink** aPtrPtrMediaSink);
				STDMETHODIMP GetIdentifier(
					DWORD* aPtrIdentifier);
				STDMETHODIMP GetMediaTypeHandler(
					IMFMediaTypeHandler** aPtrPtrHandler);
				STDMETHODIMP ProcessSample(
					IMFSample* aPtrSample);
				STDMETHODIMP PlaceMarker(
					MFSTREAMSINK_MARKER_TYPE aMarkerType, 
					const PROPVARIANT* aPtrVarMarkerValue, 
					const PROPVARIANT* aPtrVarContextValue);
				STDMETHODIMP Flush();

				// IMFMediaEventGenerator interface implementation
				STDMETHODIMP BeginGetEvent(
					IMFAsyncCallback* aPtrAsyncCallback, 
					IUnknown* aPtrUnkState);
				STDMETHODIMP EndGetEvent(
					IMFAsyncResult* aPtrAsyncResult, 
					IMFMediaEvent** aPtrPtrEvent);
				STDMETHODIMP GetEvent(
					DWORD aFlags, 
					IMFMediaEvent** aPtrPtrEvent);
				STDMETHODIMP QueueEvent(
					MediaEventType aMediaEventType, 
					REFGUID aRefGUIDExtendedType, 
					HRESULT aStatus, 
					const PROPVARIANT* aPtrVarValue);

				// IMFMediaTypeHandler interface implementation
				STDMETHODIMP IsMediaTypeSupported(
					IMFMediaType* aPtrMediaType, 
					IMFMediaType** aPtrPtrMediaType);
				STDMETHODIMP GetMediaTypeCount(
					DWORD* aPtrTypeCount);
				STDMETHODIMP GetMediaTypeByIndex(
					DWORD aIndex, 
					IMFMediaType** aPtrPtrType);
				STDMETHODIMP SetCurrentMediaType(
					IMFMediaType* aPtrMediaType);
				STDMETHODIMP GetCurrentMediaType(
					IMFMediaType** aPtrPtrMediaType);
				STDMETHODIMP GetMajorType(
					GUID* aPtrGUIDMajorType);

				// ReadWriteBufferRegularSync interface implementation 

				virtual HRESULT readData(
					unsigned char* aPtrData,
					DWORD* aPtrSampleSize);

				// SampleGrabberPullStream interface implementation

				virtual STDMETHODIMP init(
					SampleGrabberPullSink* aPtrSink,
					IMFMediaType* aPtrMediaType);

				// ISampleGrabberPullStream interface implementation

				virtual STDMETHODIMP start();

				virtual STDMETHODIMP stop();


			protected:


				virtual bool findInterface(
					REFIID aRefIID,
					void** aPtrPtrVoidObject)
				{
					if (aRefIID == __uuidof(IMFMediaEventGenerator))
					{
						return castInterfaces(
							aRefIID,
							aPtrPtrVoidObject,
							static_cast<IMFMediaEventGenerator*>(this));
					}
					else if (aRefIID == __uuidof(IRead))
					{
						return castInterfaces(
							aRefIID,
							aPtrPtrVoidObject,
							static_cast<IRead*>(this));
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

				std::mutex mReadLockMutex;

				std::condition_variable mReadLockCondition;

				SampleGrabberPullSink* mPtrSink;

				CComPtrCustom<IMFMediaType> mMediaType;

				CComPtrCustom<IMFMediaEventQueue> mEventQueue;
			};
		}
	}
}