#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/SourceState.h"
#include "../Common/ComPtrCustom.h"
#include "IStreamControl.h"


namespace CaptureManager
{
	namespace Sources
	{
		class CaptureSource;
			
		class CaptureStream : 
			public BaseUnknown<IMFMediaStream, IStreamControl>
		{
			class RequestContainer :
			public BaseUnknown<IUnknown>
			{
			public:
				RequestContainer(IUnknown* aPtrToken)
				{
					mToken = aPtrToken;
				}

				HRESULT getToken(IUnknown** aPtrPtrToken)
				{
					HRESULT lresult(S_OK);

					if (mToken)
						lresult = mToken->QueryInterface(IID_PPV_ARGS(aPtrPtrToken));

					return lresult;
				}

			private:
				CComPtrCustom<IUnknown> mToken;
			};

		public:

			CaptureStream(
				CaptureSource* aPtrCaptureSource,
				IMFStreamDescriptor* aStreamDescriptor);
			virtual ~CaptureStream();

			STDMETHODIMP BeginGetEvent(
				IMFAsyncCallback* aPtrCallback, 
				IUnknown* aPtrUnkState);

			STDMETHODIMP EndGetEvent(
				IMFAsyncResult* aPtrResult, 
				IMFMediaEvent** aPtrPtrEvent);
			
			STDMETHODIMP GetEvent(
				DWORD aFlags, 
				IMFMediaEvent** aPtrPtrEvent);
			
			STDMETHODIMP QueueEvent(
				MediaEventType aMediaEventType, 
				REFGUID aGUIDExtendedType,
				HRESULT aHRStatus, 
				const PROPVARIANT* aPtrvValue = nullptr);

			STDMETHODIMP GetMediaSource(
				IMFMediaSource** aPtrPtrMediaSource);
			
			STDMETHODIMP GetStreamDescriptor(
				IMFStreamDescriptor** aPtrPtrStreamDescriptor);
			
			STDMETHODIMP RequestSample(
				IUnknown* aPtrToken);



			virtual HRESULT start();

			virtual HRESULT pause();

			virtual HRESULT stop();

			virtual HRESULT shutdown();

			virtual HRESULT isActive(BOOL* aPtrState);

			virtual HRESULT activate(BOOL aActivate);

			virtual HRESULT checkShutdown();

			virtual HRESULT QueueEvent(
				MediaEventType aMediaEventType, 
				REFGUID aGUIDExtendedType,
				HRESULT aHRStatus, 
				IUnknown* aPtrUnk);

			virtual HRESULT queueRequestToken(
				IUnknown* aPtrToken);


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

			std::mutex mMutex;

			std::mutex mOperationMutex;

			CaptureSource* mPtrCaptureSource;

			CComPtrCustom<IMFStreamDescriptor> mStreamDescriptor;

			CComPtrCustom<IMFMediaEventQueue>  mEventQueue;

			CComPtrCustom<RequestContainer> mPostponedRequestContainer;

			bool mIsActive;

			DWORD mStreamIdentifier;

			SourceState mState;
		};
		
	}
}