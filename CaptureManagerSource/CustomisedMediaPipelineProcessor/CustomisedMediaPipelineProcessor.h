#pragma once

#include <mutex>
#include <condition_variable>
#include <vector>
#include <list>

#include "../Common/BaseUnknown.h"
#include "../Common/ComPtrCustom.h"
#include "../CustomisedMediaSession/IMediaPipeline.h"
#include "../TopologyResolver/CollectionOfIDs.h"
#include "IMediaPipelineProcessorControl.h"

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			struct ICustomisedRequest;

			class CustomisedMediaSession;

			class CustomisedMediaPipelineProcessor :
				public BaseUnknown<IMediaPipeline, IMediaPipelineProcessorControl>
			{
			public:
				CustomisedMediaPipelineProcessor(
					CustomisedMediaSession* aPtrCustomisedMediaSession);
				virtual ~CustomisedMediaPipelineProcessor();
				
				virtual HRESULT finishSourceStreamStarting();

				// IMFAsyncCallback implementation

				virtual HRESULT STDMETHODCALLTYPE GetParameters(
					DWORD* aPtrFlags,
					DWORD* aPtrQueue);

				virtual HRESULT STDMETHODCALLTYPE Invoke(
					IMFAsyncResult* aPtrAsyncResult);
				
				HRESULT init(
					CollectionOfIDs& aRefCollectionOfIDs,
					IMFTopology* aPtrTopology,
					IMFPresentationClock* aPtrPresentationClock);

				// IMediaPipeline interface
				
				virtual HRESULT startSources(
					const GUID* aConstPtrGUIDTimeFormat,
					const PROPVARIANT* aConstPtrVarStartPosition);

				virtual HRESULT pauseSources();

				virtual HRESULT stopSources();

				virtual HRESULT finishSourceStreamStopping();

				virtual HRESULT finishSourceStreamPausing();
				
				virtual HRESULT finishStreamSinkStopping();

				virtual HRESULT finishStreamSinkPausing();

				// IMediaPipelineProcessorControl interface

				virtual HRESULT pause()override;

				virtual HRESULT restart()override;

				virtual HRESULT removeStreamSink(IMFStreamSink* aPtrStreamSink)override;

				virtual HRESULT checkSwitcherNode(const TOPOID& aRefSwitcherNodeID)override;

				virtual HRESULT registerStreamSink(const TOPOID& aRefStreamSinkNodeID,
					IMFPresentationClock* aPtrPresentationClock)override;

			private:

				enum class State:byte
				{
					None = 0,
					Started = None + 1,
					Paused = Started + 1,
					Stopped = Paused + 1
				};

				std::atomic<LONG> mStreamSinkCount;

				std::atomic<LONG> mSourceStreamCount;

				std::atomic<LONG> mSourceStreamPausingCount;
				
				std::mutex mMediaPipelineMutex;

				std::mutex mSourceStartingPipelineMutex;

				int mStopWaitingTime;

				std::condition_variable mStreamSinkStoppingCondition;

				std::condition_variable mStreamSinkPausingCondition;

				std::condition_variable mSourceStreamStartingCondition;

				std::condition_variable mSourceStreamPausingCondition;

				std::condition_variable mSourceStreamStoppingCondition;

				DWORD mSyncWorkerQueue;

				GUID mGUIDMajorType;

				GUID mGUIDSubType;

				State mCurrentState;
				
				bool mIsDiscontinuity;

				bool m_processSampleAccess;
					 

				CustomisedMediaSession* mPtrCustomisedMediaSession;

				std::vector<CComPtrCustom<IMFMediaStream>> mMediaStreams;
				
				std::list<CComPtrCustom<IMFStreamSink>> mStreamSinks;

				std::vector<LONGLONG> mSampleTimes;

				std::vector<LONGLONG> mPrevSampleTimes;

				CollectionOfIDs mCollectionOfIDs;

				CComPtrCustom<IMFTopology> mCurrentTopology;





				// CustomisedMediaSession implementation

				HRESULT handleSourceRequest(
					IMFAsyncResult* aPtrResult,
					ICustomisedRequest* aPtrICustomisedRequest);

				HRESULT handleSourceStreamRequest(
					IMFAsyncResult* aPtrResult,
					ICustomisedRequest* aPtrICustomisedRequest);

				HRESULT handleSourceStreamPushNewSampleRequest(
					IMFAsyncResult* aPtrResult,
					ICustomisedRequest* aPtrICustomisedRequest);				

				HRESULT handleStreamSinkRequest(
					IMFAsyncResult* aPtrResult,
					ICustomisedRequest* aPtrICustomisedRequest);

				HRESULT handlePullNewSampleRequest(
					IMFAsyncResult* aPtrResult,
					ICustomisedRequest* aPtrICustomisedRequest);

				HRESULT handlePushNewSampleRequest(
					IMFAsyncResult* aPtrResult,
					ICustomisedRequest* aPtrICustomisedRequest);

				HRESULT pullNewSampleFromSourceStream(
					IMFTopologyNode* aPtrTopologyNode);

				HRESULT pullPushNewSampleFromToTransform(
					IMFSample* aPtrSample,
					IMFTopologyNode* aPtrTopologyNode);

				HRESULT pullPushNewSampleFromToTeeNode(
					IMFSample* aPtrSample,
					IMFTopologyNode* aPtrTopologyNode,
					DWORD aIndexUpperStreamNode = 0);

				HRESULT pushNewSampleToOutputNode(
					IMFSample* aPtrSample,
					IMFTopologyNode* aPtrTopologyNode);

				HRESULT initOutputDataBuffer(
					IMFTransform* aPtrTransform,
					MFT_OUTPUT_DATA_BUFFER& aRefOutputBuffer);

				HRESULT findAndRegisterSwitcherNode(const TOPOID& aRefSwitcherNodeID, IMFTopologyNode* aPtrTopologyNode);
			};
		}
	}
}