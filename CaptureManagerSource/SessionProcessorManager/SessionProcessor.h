#pragma once

#include <condition_variable>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../CaptureManagerBroker/ISessionCallbackInner.h"

namespace CaptureManager
{
	namespace Core
	{
		MIDL_INTERFACE("CF51D9BD-F45A-403C-AD51-81D6CA019032")
		ISessionProcessor : public IUnknown 
		{
			virtual HRESULT close()=0;

			virtual HRESULT startCapture(
				REFGUID aGUIDTimeFormat,
				const PROPVARIANT *aPtrVarStartPosition) = 0;

			virtual HRESULT pauseCapture() = 0;

			virtual HRESULT stopCapture() = 0;

			virtual HRESULT pauseSwitchers() = 0;

			virtual HRESULT resumeSwitchers() = 0;

			virtual HRESULT detachSwitchers() = 0;

			virtual HRESULT checkToplogyNode(IMFTopologyNode* aPtrTopologyNode) = 0;

			virtual HRESULT attachSwitcher(
				/* [in] */ IMFTopologyNode *aPtrSwitcherNode,
				/* [in] */ IMFTopologyNode *aPtrDownStreamNode) = 0;
		};

		class SessionProcessor : 
			public BaseUnknown<IMFAsyncCallback, ISessionProcessor>
		{
		public:
			SessionProcessor(
				IMFMediaSession* aPtrMediaSession,
				SessionDescriptor lSessionDescriptor,
				CComPtrCustom<ISessionCallbackInner>& aRefISessionCallback,
				CComPtrCustom<IMFAsyncCallback>& aRefSessionAsyncCallback);

			virtual ~SessionProcessor();


			STDMETHODIMP GetParameters(
				DWORD* aPtrFlags, 
				DWORD* aPtrQueue);

			STDMETHODIMP Invoke(
				IMFAsyncResult* aPtrAsyncResult);
			

			HRESULT close()override;

			HRESULT startCapture(
				REFGUID aGUIDTimeFormat,
				const PROPVARIANT *aPtrVarStartPosition)override;

			HRESULT pauseCapture()override;

			HRESULT stopCapture()override;

			HRESULT pauseSwitchers()override;

			HRESULT resumeSwitchers()override;

			HRESULT detachSwitchers()override;

			HRESULT checkToplogyNode(IMFTopologyNode* aPtrTopologyNode)override;

			HRESULT attachSwitcher(
				/* [in] */ IMFTopologyNode *aPtrSwitcherNode,
				/* [in] */ IMFTopologyNode *aPtrDownStreamNode)override;
			
			HRESULT setTopology(
				IMFTopology* aPtrTopology);

			HRESULT fastEncodingPersistence();

		private:

			enum MediaEvent
			{
				NULLPTR = 0,
				CLOSED = 1,
				OK = 2,
				STATUS_ERROR = 3,
				EXECUTION_ERROR = 4,
				STOPPED = 5,
				PAUSED = 6,
				ENDED = 7,
				STARTED = 8
			};
			
			std::condition_variable mConditionVariable;

			std::mutex mCritSec;

			std::mutex mMutex;

			bool mIsStarted = false;

			CComPtrCustom<IMFMediaSession> mLocalSession;
			
			CComPtrCustom<ISessionCallbackInner> mISessionCallback;

			CComPtrCustom<IMFAsyncCallback> mSessionAsyncCallback;

			DWORD mSyncWorkerQueue;
			
			SessionDescriptor mSessionDescriptor;

			std::tuple<MediaEvent, HRESULT> processMediaEvent(
				CComPtrCustom<IMFMediaEvent>& aMediaEvent);

			HRESULT postEncodingUpdate(
				IMFTopology* aPtrTopology);
			
			HRESULT postEncodingUpdate(
				IMFStreamSink* aPtrStreamSink,
				IMFTopologyNode* aPtrTopologyNode);

		};
	}
}