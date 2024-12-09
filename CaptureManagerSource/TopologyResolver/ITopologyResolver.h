#pragma once
#include "Unknwnbase.h"
#include "CollectionOfIDs.h"

struct IMFTopology;
struct IMFTopologyNode;
struct IMFMediaType;
struct IMFTopologyNode;

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{			
			MIDL_INTERFACE("9E2ACDDD-DB17-48C9-9D74-BF15A8695E59")
			ITopologyResolver:public IUnknown
			{
			public:

				virtual HRESULT resolveDownStreamConnect(
					IMFTopology* aPtrTopology, 
					CollectionOfIDs& aRefCollectionOfIDs,
					IMFTopologyNode* aPtrUpStreamNode,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFTopologyNode* aPtrDownStreamNode,
					DWORD aOutputStreamIndex) = 0;

			};
		}
	}
}