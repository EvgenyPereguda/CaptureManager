#pragma once

#include <mutex>
#include <condition_variable>
#include <list>
#include <map>
#include <queue>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/BoxMode.h"
#include "../Common/ComPtrCustom.h"
#include "IMediaPipeline.h"
#include "../TopologyResolver/ITopologyResolver.h"
#include "../TopologyResolver/CollectionOfIDs.h"
#include "ISessionSwitcherControl.h"

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			struct ICustomisedRequest;
			
			class CustomisedMediaSession :
				public BaseUnknown<IMFMediaSession, ISessionSwitcherControl>
			{
				enum class MediaSessionSate: int
				{
					None = 0,
					SessionStarted = None + 1,
					SessionPaused = SessionStarted + 1,
					SessionStopped = SessionPaused + 1,
					SessionClosed = SessionStopped + 1,
					SessionShutdowned = SessionClosed + 1
					
				};

			public:
				CustomisedMediaSession();
				virtual ~CustomisedMediaSession();

				// ISessionSwitcherControl interface

				virtual HRESULT STDMETHODCALLTYPE pauseSwitchers();

				virtual HRESULT STDMETHODCALLTYPE resumeSwitchers();

				virtual HRESULT STDMETHODCALLTYPE detachSwitchers();

				virtual HRESULT STDMETHODCALLTYPE attachSwitcher(
					/* [in] */ IMFTopologyNode *aPtrSwitcherNode,
					/* [in] */ IMFTopologyNode *aPtrDownStreamNode);

			

				// IMFMediaEventGenerator implementation

				virtual HRESULT STDMETHODCALLTYPE GetEvent(
					DWORD dwFlags,
					IMFMediaEvent **ppEvent);

				virtual HRESULT STDMETHODCALLTYPE BeginGetEvent(
					IMFAsyncCallback* aPtrCallback,
					IUnknown* aPtrUnkState);

				virtual HRESULT STDMETHODCALLTYPE EndGetEvent(
					IMFAsyncResult* aPtrResult,
					IMFMediaEvent** aPtrPtrEvent);

				virtual HRESULT STDMETHODCALLTYPE QueueEvent(
					MediaEventType aMediaEventType,
					REFGUID aRefGUIDExtendedType,
					HRESULT aHRESULTStatus,
					const PROPVARIANT* aPtrPropVariantValue);

				// IMFMediaSession implementation
			
				virtual HRESULT STDMETHODCALLTYPE SetTopology(
					DWORD aSetTopologyFlags,
					IMFTopology* aPtrTopology);

				virtual HRESULT STDMETHODCALLTYPE ClearTopologies();

				virtual HRESULT STDMETHODCALLTYPE Start(
					const GUID* aConstPtrGUIDTimeFormat,
					const PROPVARIANT* aConstPtrVarStartPosition);

				virtual HRESULT STDMETHODCALLTYPE Pause();

				virtual HRESULT STDMETHODCALLTYPE Stop();

				virtual HRESULT STDMETHODCALLTYPE Close();

				virtual HRESULT STDMETHODCALLTYPE Shutdown();

				virtual HRESULT STDMETHODCALLTYPE GetClock(
					IMFClock **ppClock);

				virtual HRESULT STDMETHODCALLTYPE GetSessionCapabilities(
					DWORD *pdwCaps);

				virtual HRESULT STDMETHODCALLTYPE GetFullTopology(
					DWORD dwGetFullTopologyFlags,
					TOPOID TopoId,
					IMFTopology **ppFullTopology);




				virtual HRESULT checkInitBarier(IMFMediaStream* aPtrMediaStream);

				virtual HRESULT checkFinishBarier();


				HRESULT resetTargetNode(
					IMFTopologyNode* aPtrUpStreamTopologyNode,
					IMFMediaType* aPtrUpStreamMediaType,
					ITopologyResolver* aPtrTopologyResolver);

				LONG getInitBarierCount();

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
				
			CAPTUREMANAGER_BOX

				std::mutex mMutex;

				std::mutex mAccesseMutex;

				bool mIsSwitchersDetached;

				std::map<TOPOID, CollectionOfIDs> mSwitcherCollection;

				std::list<CollectionOfIDs> mListOfStreamsCollectionOfID;

				CComPtrCustom<IMFMediaEventQueue>  mEventQueue;

				CComPtrCustom<IMFPresentationClock>  mClock;

				MediaSessionSate mMediaSessionSate;

				LONGLONG mClockStartOffset;

				std::list<CComPtrCustom<IMediaPipeline>>  mListOfMediaPipelineProcessors;

				CComPtrCustom<ITopologyResolver> mVideoTopologyResolver;

				CComPtrCustom<ITopologyResolver> mAudioTopologyResolver;
				


				std::condition_variable mInitBarierCondition;

				std::mutex mInitBarrierMutex;

				std::mutex mFinishBarierMutex;

				std::condition_variable mInitBarrierCV;

				std::atomic<LONG> mInitBarrierCount;
				
				std::list<IMFMediaStream*> mMediaStreamList;

				bool mFinishBarierState;
							
				bool mSessionIsInitialised;

				std::queue<CComPtrCustom<IMFTopology>> mTopologyQueue;

				CComPtrCustom<IMFTopology> mCurrentTopology;

				HRESULT resolveTopology(
					IMFTopology* aPtrTopology);

				HRESULT resolveStreamTopology(
					IMFTopology* aPtrTopology,
					std::list<TOPOID>& aRefListOfUsedSources,
					IMFTopologyNode* aPtrUpStreamTopologyNode,
					IMFMediaType* aPtrUpStreamMediaType);
				
				HRESULT queueTopologyReadyEvent(
					IMFTopology* aPtrTopology,
					HRESULT aResult);

				HRESULT notifyToBeginStreaming(
					IMFTopology* aPtrTopology);

				HRESULT notifyToEndStreaming(
					IMFTopology* aPtrTopology);

				HRESULT notifyToEndStreaming(
					IMFTopologyNode* aPtrTopologyNode);


				HRESULT findOutputNodes(
					std::vector<CComPtrCustom<IMFTopologyNode>>& aOutputTopologyNodes,
					IMFTopologyNode* aPtrDownStreamNode);


				HRESULT releaseNode(IMFTopologyNode* aPtrDownStreamNode);

				HRESULT enumAndAddOutputTopologyNode(
					IMFTopology* aPtrTopology,
					IMFTopologyNode* aPtrTopologyNode);

				HRESULT STDMETHODCALLTYPE notifyToBeginSwitcherStreaming(
					/* [in] */ IMFTopologyNode *aPtrTopologyNode);
				
				HRESULT getTargetNode(IMFTopologyNode* aPtrDownStreamNode, IMFTopologyNode** aPtrPtrTargetStreamNode);
			};
		}
	}
}