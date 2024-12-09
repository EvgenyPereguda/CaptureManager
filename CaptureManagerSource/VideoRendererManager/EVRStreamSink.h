#pragma once

#include <mutex>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/IStreamSink.h"
#include "IPresenter.h"


namespace CaptureManager
{
	namespace Sinks
	{
		namespace EVR
		{
			class EVRStreamSink:
				public BaseUnknown<
				IMFStreamSink,
				IStreamSink,
				IMFMediaTypeHandler,
				IMFGetService,
				IMFAsyncCallback,
				IMFClockStateSink
				>
			{

				enum StreamEnum
				{
					Uninitialized,
					Ready,
					Started,
					Paused,
					Stoped
				};

			public:
				static HRESULT createEVRStreamSink(
					DWORD aStreamID,
					IMFMediaSink* aPtrMediaSink,
					IPresenter* aPtrPresenter,
					DWORD aMixerStreamID,
					bool aIsSingleStream,
					IStreamSink** aPtrPtrStreamSink);

				HRESULT initialize(
					IMFMediaSink* aPtrMediaSink,
					bool aIsSingleStream,
					IPresenter* aPtrPresenter);



				// IMFClockStateSink methods
				virtual HRESULT STDMETHODCALLTYPE OnClockStart(
					MFTIME aHNSSystemTime,
					LONGLONG aClockStartOffset);
				virtual HRESULT STDMETHODCALLTYPE OnClockStop(
					MFTIME aHNSSystemTime);
				virtual HRESULT STDMETHODCALLTYPE OnClockPause(
					MFTIME aHNSSystemTime);
				virtual HRESULT STDMETHODCALLTYPE OnClockRestart(
					MFTIME aHNSSystemTime);
				virtual HRESULT STDMETHODCALLTYPE OnClockSetRate(
					MFTIME aHNSSystemTime,
					float aRate);



				// IMFMediaTypeHandler implementation
				virtual HRESULT STDMETHODCALLTYPE IsMediaTypeSupported(
					IMFMediaType* aPtrMediaType,
					IMFMediaType** aPtrPtrMediaType);
				virtual HRESULT STDMETHODCALLTYPE GetMediaTypeCount(
					DWORD* aPtrTypeCount);
				virtual HRESULT STDMETHODCALLTYPE GetMediaTypeByIndex(
					DWORD aIndex,
					IMFMediaType** aPtrPtrType);
				virtual HRESULT STDMETHODCALLTYPE SetCurrentMediaType(
					IMFMediaType* aPtrMediaType);
				virtual HRESULT STDMETHODCALLTYPE GetCurrentMediaType(
					IMFMediaType** aPtrPtrMediaType);
				virtual HRESULT STDMETHODCALLTYPE GetMajorType(
					GUID* PtrGUIDMajorType);



				// IMFStreamSink implementation
				virtual HRESULT STDMETHODCALLTYPE Flush();
				virtual HRESULT STDMETHODCALLTYPE GetIdentifier(
					DWORD* aPtrIdentifier);
				virtual HRESULT STDMETHODCALLTYPE GetMediaSink(
					IMFMediaSink** aPtrPtrMediaSink);
				virtual HRESULT STDMETHODCALLTYPE GetMediaTypeHandler(
					IMFMediaTypeHandler** aPtrPtrHandler);
				virtual HRESULT STDMETHODCALLTYPE PlaceMarker(
					MFSTREAMSINK_MARKER_TYPE aMarkerType, 
					const PROPVARIANT* aPtrVarMarkerValue, 
					const PROPVARIANT* aPtrVarContextValue);
				virtual HRESULT STDMETHODCALLTYPE ProcessSample(
					IMFSample* aPtrSample);


				// IMFMediaEventGenerator implementation
				virtual HRESULT STDMETHODCALLTYPE BeginGetEvent(
					IMFAsyncCallback* aPtrCallback, 
					IUnknown* aPtrUnkState);
				virtual HRESULT STDMETHODCALLTYPE EndGetEvent(
					IMFAsyncResult* aPtrResult, 
					IMFMediaEvent** aPtrPtrEvent);
				virtual HRESULT STDMETHODCALLTYPE GetEvent(
					DWORD aFlags, 
					IMFMediaEvent** aPtrPtrEvent);
				virtual HRESULT STDMETHODCALLTYPE QueueEvent(
					MediaEventType aMediaEventType,
					REFGUID aRefGUIDExtendedType,
					HRESULT aHRStatus, 
					const PROPVARIANT* aPtrValue);


				// IStreamSink implementation
				virtual HRESULT STDMETHODCALLTYPE getMaxRate(
					BOOL aThin, 
					float* aPtrRate);
				virtual HRESULT STDMETHODCALLTYPE preroll();
				virtual HRESULT STDMETHODCALLTYPE shutdown();


				// IMFGetService
				virtual STDMETHODIMP GetService(REFGUID guidService, REFIID riid, LPVOID* ppvObject) override;


				// IMFAsyncCallback methods
				virtual HRESULT STDMETHODCALLTYPE GetParameters(
					DWORD* aPtrFlags, 
					DWORD* aPtrQueue);
				virtual HRESULT STDMETHODCALLTYPE Invoke(
					IMFAsyncResult* aPtrAsyncResult);

			protected:
				
				virtual bool findInterface(
					REFIID aRefIID,
					void** aPtrPtrVoidObject)
				{
					auto lresult = castInterfaces(
						aRefIID,
						aPtrPtrVoidObject,
						static_cast<IMFMediaEventGenerator*>(this));

					if (!lresult)
					{
						return BaseUnknown::findInterface(
							aRefIID,
							aPtrPtrVoidObject);
					}

					return lresult;
				}

			private:


				const DWORD mStreamID;
				const DWORD mMixerStreamID;
				DWORD mWorkQueueId;
				bool mIsShutdown;
				bool mIsSingleStream;
				CComPtrCustom<IMFMediaType> mCurrentMediaType;

				CComPtrCustom<IMFMediaSink> mMediaSink;
				CComPtrCustom<IMFMediaEventQueue> mEventQueue;  
				CComPtrCustom<IPresenter> mPresenter;
				CComPtrCustom<IMFMediaType> mCurrentType;
				
				CComPtrCustom<IMFTransform> mMixer;

				std::mutex mAccessMutex;
				std::mutex mClockStateAccessMutex;
				std::mutex mProcessMutex;


				StreamEnum mStreamState;


				EVRStreamSink(
					DWORD aStreamID,
					DWORD aMixerStreamID);
				virtual ~EVRStreamSink();
				HRESULT checkShutdown() const;
				HRESULT createVideoAllocator(
					IMFVideoSampleAllocator** aPtrPtrVideoSampleAllocator);
			};
		}
	}
}