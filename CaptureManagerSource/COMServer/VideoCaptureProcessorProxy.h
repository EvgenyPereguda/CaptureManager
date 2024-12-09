#pragma once
#include <thread>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include "../Common/BaseUnknown.h"
#include "../Common/IInnerCaptureProcessor.h"
#include "../Common/SourceState.h"
#include "../Common/ComPtrCustom.h"
#include "../Common/IInnerGetService.h"
#include "../CaptureInvoker/CaptureInvoker.h"
#include "CaptureManagerTypeInfo.h"
#include "InitilaizeCaptureSource.h"


namespace CaptureManager
{
	namespace COMServer
	{
		class VideoCaptureProcessorProxy :
			public BaseUnknown<
			IInnerCaptureProcessor,
			IInnerGetService>, public Core::CaptureInvoker
		{
			enum CaptureProcessorState
			{
				Started,
				Stopped,
				Paused,
				Uninitalized,
				Shutdown
			};

			CComPtrCustom<ICaptureProcessor> mICaptureProcessor;

			CComPtrCustom<InitilaizeCaptureSource> mInitilaizeCaptureSource;

			CComPtrCustom<ISourceRequestResult> mSourceRequestResult;

			UINT32 mDiscontinuity;

			MFTIME mTickSampleTime;

			MFTIME mSampleTime;

			DWORD mStreamIndex;

			INT64 mVideoFrameDuration;

			MFTIME mSleepDuration;

			MFTIME mDeltaTimeDuration;

			MFTIME mPrevTime;

			DWORD mHeight;

			DWORD mWidth;

			CaptureProcessorState mCaptureProcessorState;

			bool mIsHardware;

			bool mIsDirectX11;

			bool mIsBlocked;

			CComPtrCustom<IMFDXGIDeviceManager> mDeviceManager;




			//CComPtrCustom<IMF2DBuffer> mCopySrcTexture2D;
			//BYTE* mDstData;
			//DWORD mDstDataLength;
			//std::mutex mAccessCopyMutex;
			//std::mutex mAccessCopyQueueMutex;
			//std::condition_variable mCopyCondition;
		public:
			VideoCaptureProcessorProxy();

			HRESULT init(ICaptureProcessor* aPtrICaptureProcessor, InitilaizeCaptureSource* aPtrInitilaizeCaptureSource);

			// IInnerCaptureProcessor methods

			// get friendly name of Processor
			virtual HRESULT getFrendlyName(
				BSTR* aPtrString)override;

			// get synbolicLink of Processor
			virtual HRESULT getSymbolicLink(
				BSTR* aPtrString)override;

			// get supported amount of streams
			virtual HRESULT getStreamAmount(
				UINT32* aPtrStreamAmount)override;

			// get supported media types of stream
			virtual HRESULT getMediaTypes(
				UINT32 aStreamIndex,
				BSTR* aPtrStreamNameString,
				IUnknown*** aPtrPtrPtrMediaType,
				UINT32* aPtrMediaTypeCount)override;

			// set supported media type to stream
			virtual HRESULT setCurrentMediaType(
				UINT32 aStreamIndex,
				IUnknown* aPtrMediaType)override;


			std::mutex mAccessBufferQueueMutex;

			std::mutex mNewFrameMutex;

			std::condition_variable mNewFrameCondition;

			// get new sample of stream
			virtual HRESULT getNewSample(
				DWORD aStreamIdentifier,
				IUnknown** ppSample)override;

			// start processor
			virtual HRESULT start()override;

			// stop processor
			virtual HRESULT stop()override;

			// pause processor
			virtual HRESULT pause()override;

			// restart processor
			virtual HRESULT restart()override;

			// shutdown processor
			virtual HRESULT shutdown()override;

			// register Media Event interface
			virtual HRESULT registerMediaEvent(IUnknown* pUnkIMediaEvent)override;




			// IInnerGetService
			virtual HRESULT STDMETHODCALLTYPE GetService(
				REFGUID aRefGUIDService,
				REFIID aRefIID,
				LPVOID* aPtrPtrObject)override;

					   
		protected:

			// CaptureInvoker implementation

			virtual HRESULT STDMETHODCALLTYPE invoke() override;

		private:
			virtual ~VideoCaptureProcessorProxy();

			void update();
			
			HRESULT execute();
		};
	}
}