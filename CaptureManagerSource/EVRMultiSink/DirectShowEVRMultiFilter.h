#pragma once

#include "BaseClasses.h"
#include "IPresenter.h"
#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"

namespace EVRMultiSink
{
	struct IPresenter;

	namespace Filters
	{
		namespace EVR
		{
			class DirectShowEVRMultiFilter;

			class CVideoInputPin : public CRendererInputPin
			{
				DirectShowEVRMultiFilter *m_pRenderer;        // The renderer that owns us
				CCritSec *m_pInterfaceLock;         // Main filter critical section

			public:
				// Constructor
				CVideoInputPin(
					TCHAR *pObjectName,             // Object string description
					DirectShowEVRMultiFilter *pRenderer,      // Used to delegate locking
					CCritSec *pInterfaceLock,       // Main critical section
					HRESULT *phr,                   // OLE failure return code
					LPCWSTR pPinName);              // This pins identification
			};

			class DirectShowEVRMultiFilter :
				public CBaseVideoRenderer
			{
			public:
				DirectShowEVRMultiFilter(DWORD aMixerStreamID);
				virtual ~DirectShowEVRMultiFilter();

				static HRESULT createFilter(
					IPresenter* aPtrPresenter,
					IMFTransform* aPtrMixer,
					DWORD aMixerStreamID,
					IBaseFilter** aPtrPrtBaseFilter);

				virtual CBasePin *GetPin(int n);

				HRESULT BreakConnect();
				HRESULT CompleteConnect(IPin *pReceivePin);

				//CBaseRenderer implements
				virtual HRESULT DoRenderSample(IMediaSample *pMediaSample)override;
				virtual HRESULT CheckMediaType(const CMediaType* aPtrCMediaType)override;
				virtual HRESULT SetMediaType(const CMediaType *aPtrCMediaType)override;
				
				virtual HRESULT OnStartStreaming()override;
				virtual HRESULT OnStopStreaming()override;
											

				DECLARE_IUNKNOWN
				STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, __deref_out VOID **ppv);
			private:

				HRESULT mOLECode = 0;
				const DWORD mMixerStreamID;
				BOOL mInnerAllocatorState;

				CaptureManager::CComPtrCustom<IMFTransform> mMixer;
				CaptureManager::CComPtrCustom<IPresenter> mPresenter; 
				CaptureManager::CComPtrCustom<IMFTransform> mAllocatorTransform;

				CaptureManager::CComPtrCustom<IMFSample> mCurrentSample;

				CaptureManager::CComPtrCustom<IMFMediaBuffer> mCurrentMediaBuffer;
				
				HRESULT createVideoAllocator(
					IMFVideoSampleAllocator** aPtrPtrVideoSampleAllocator);

			public:

				CMediaType      m_mtIn;            // Source connection media type

				SIZE mVideo;
				SIZE mARVideo;
			};
		}
	}
}