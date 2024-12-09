#pragma once

#include <map>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "../Common/MFHeaders.h"
#include "../Common/SourceState.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/IInnerCaptureProcessor.h"
#include "../Common/BaseUnknown.h"
#include "../Common/IMediaEvent.h"
#include "IStreamControl.h"

namespace CaptureManager
{
	namespace Sources
	{
		struct ISourceOperation;
			
		class CaptureSource :
			public BaseUnknown<
			IMFMediaSource, 
			IMFGetService,
			IMFAsyncCallback,
			IMediaEvent>
		{
		public:

			CaptureSource();

			// IMFMediaSource interface

			virtual HRESULT STDMETHODCALLTYPE GetCharacteristics(
				__RPC__out DWORD *aPtrdwCharacteristics);
				
			virtual HRESULT STDMETHODCALLTYPE CreatePresentationDescriptor(
				IMFPresentationDescriptor **aPtrPtrPresentationDescriptor);

			virtual HRESULT STDMETHODCALLTYPE Start(
				IMFPresentationDescriptor * aPtrPresentationDescriptor,
				const GUID * aPtrGUIDTimeFormat,
				const PROPVARIANT * aPtrVarStartPosition);

			virtual HRESULT STDMETHODCALLTYPE Stop();

			virtual HRESULT STDMETHODCALLTYPE Pause();

			virtual HRESULT STDMETHODCALLTYPE Shutdown();

			virtual HRESULT STDMETHODCALLTYPE GetEvent(
				DWORD dwFlags,
				IMFMediaEvent **ppEvent);

			virtual HRESULT STDMETHODCALLTYPE BeginGetEvent(
				IMFAsyncCallback *aPtrCallback,
				IUnknown *aPtrUnkState);

			virtual HRESULT STDMETHODCALLTYPE EndGetEvent(
				IMFAsyncResult *aPtrResult,
				_Out_  IMFMediaEvent ** aPtrPtrEvent);

			virtual HRESULT STDMETHODCALLTYPE QueueEvent(
				MediaEventType met,
				__RPC__in REFGUID guidExtendedType,
				HRESULT hrStatus,
				const PROPVARIANT *pvValue);




			// IMFGetService
			virtual HRESULT STDMETHODCALLTYPE GetService(
				REFGUID aRefGUIDService,
				REFIID aRefIID,
				LPVOID* aPtrPtrObject)override;

			// IMFAsyncCallback interface

			virtual HRESULT STDMETHODCALLTYPE GetParameters(
				__RPC__out DWORD *pdwFlags,
				__RPC__out DWORD *pdwQueue);

			virtual HRESULT STDMETHODCALLTYPE Invoke(
				IMFAsyncResult *pAsyncResult);

			// IMediaEvent interface

			virtual HRESULT STDMETHODCALLTYPE PostEvent(
				DWORD aMediaEventType,
				REFGUID aGUIDExtendedType,
				HRESULT aHRStatus,
				IUnknown* aPtrUnk) override;

			// CaptureSource interface

			HRESULT init(
				CComPtrCustom<IInnerCaptureProcessor> aCaptureProcessor,
				BOOL aIsHardWare = FALSE);

			HRESULT createPresentationDescriptor(BOOL aIsHardWare);

			HRESULT requestNewSample(
				DWORD aStreamIdentifier, 
				IUnknown* aPtrToken);

			HRESULT startCapture();

			HRESULT stopCapture();

			HRESULT shutdownCapture();



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
				else
				{
					return BaseUnknown::findInterface(
						aRefIID,
						aPtrPtrVoidObject);
				}
			}



		private:

			DWORD mSyncWorkerQueue;

			std::condition_variable mConditionVariable;

			std::mutex mMutex;
				
			virtual ~CaptureSource();

			HRESULT STDMETHODCALLTYPE checkShutdown() const;

			HRESULT STDMETHODCALLTYPE isInitialized() const;

			HRESULT STDMETHODCALLTYPE initialized();

			HRESULT STDMETHODCALLTYPE isStarted() const;

			SourceState mState;

			CComPtrCustom<IInnerCaptureProcessor> mCaptureProcessor;

			std::map<DWORD, CComPtrCustom<IStreamControl>> mStreams;

			CComPtrCustom<IMFPresentationDescriptor> mPresentationDescriptor;

			CComPtrCustom<IMFMediaEventQueue>  mEventQueue;


			HRESULT createStreamDescriptor(
				DWORD aStreamIdentifier,
				IMFStreamDescriptor** aPtrPtrStreamDescriptor);

			HRESULT STDMETHODCALLTYPE internalStart(
				ISourceOperation* aPtrCommand);

			HRESULT STDMETHODCALLTYPE internalPause();

			HRESULT STDMETHODCALLTYPE internalStop();

			HRESULT STDMETHODCALLTYPE requestSample(
				ISourceOperation* aPtrCommand);

			HRESULT STDMETHODCALLTYPE selectStream(
				IMFPresentationDescriptor * aPtrPresentationDescriptor);
		};
		
	}
}