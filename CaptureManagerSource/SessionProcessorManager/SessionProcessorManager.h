#pragma once

#include <map>
#include <vector>
#include <Unknwn.h>


#include "../Common/ComPtrCustom.h"
#include "../CaptureManagerBroker/ISessionCallbackInner.h"


struct IMFTopologyNode;
struct IMFTopology;

namespace CaptureManager
{
	namespace Core
	{

		class SessionProcessor;

		class SessionProcessorManager
		{
		public:

			typedef struct SourceToOutputPair_
			{
				IMFTopologyNode* mPtrSourceTopologyNode;

				IMFTopologyNode* mPtrOutputTopologyNode;

			} SourceToOutputPair;

		public:

			HRESULT createSession(
				std::vector<IMFTopologyNode*>& aSourceNodes,
				IUnknown* aPtrISessionCallback,
				SessionDescriptor& aRefSessionDescriptor);

			HRESULT closeAllSessions();

			HRESULT closeSession(
				SessionDescriptor& aRefSessionDescriptor);

			HRESULT startSession(
				SessionDescriptor& aRefSessionDescriptor, 
				LONGLONG aStartPosition = 0, 
				const GUID aGUIDTimeFormat = GUID_NULL);

			HRESULT pauseSession(
				SessionDescriptor& aRefSessionDescriptor);

			HRESULT stopSession(
				SessionDescriptor& aRefSessionDescriptor);

			HRESULT pauseSwitchers(
				SessionDescriptor& aRefSessionDescriptor);

			HRESULT resumeSwitchers(
				SessionDescriptor& aRefSessionDescriptor);

			HRESULT detachSwitchers(
				SessionDescriptor& aRefSessionDescriptor);
			
			HRESULT attachSwitcher(
				/* [in] */ IUnknown *aPtrSwitcherNode,
				/* [in] */ IUnknown *aPtrDownStreamNode);
			
		protected:
			SessionProcessorManager();
			~SessionProcessorManager();
		private:
			SessionProcessorManager(const SessionProcessorManager&) = delete;
			SessionProcessorManager& operator=(const SessionProcessorManager&) = delete;

			HRESULT enumAndAddOutputTopologyNode(
				IMFTopology* aPtrTopology, 
				IMFTopologyNode* aPtrTopologyNode);
			

			SessionDescriptor mNextSessionDescriptor;

			std::map<SessionDescriptor, CComPtrCustom<IUnknown>> mSessionProcessorDictionary;

			CComPtrCustom<IUnknown> mSessionAsyncCallback;
		};
	}
}