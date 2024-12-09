#pragma once
#include "../Common/BaseUnknown.h"
#include "ITopologyResolver.h"

struct IMFTransform;

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			class BaseTopologyResolver :
				public BaseUnknown<ITopologyResolver>
			{
			public:
				
				virtual HRESULT resolveDownStreamConnect(
					IMFTopology* aPtrTopology,
					CollectionOfIDs& aRefCollectionOfIDs,
					IMFTopologyNode* aPtrUpStreamNode,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFTopologyNode* aPtrDownStreamNode,
					DWORD aOutputStreamIndex);

			protected:

				virtual HRESULT getInputMediaTypeOfMediaSink(
					IMFTopology* aPtrTopology,
					IMFTopologyNode* aPtrUpStreamNode,
					IMFTopologyNode* aPtrDownStreamNode,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType** aPtrPtrDownStreamMediaType,
					UINT32 aOutputStreamIndex);

				virtual HRESULT resolveConnection(
					IMFTopology* aPtrTopology,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType,
					IMFTopologyNode** aPtrPtrHeadTopologyNode,
					IMFTopologyNode** aPtrPtrTailTopologyNode) = 0;

				virtual HRESULT resolveInputType(
					IMFTransform* aPtrTransform,
					IMFMediaType* aPtrUpStreamMediaType) = 0;

			private:

				HRESULT getCurrentMediaTypesOfTransform(
					IMFTopologyNode* aPtrDownStreamNode,
					IMFMediaType* aPtrOutputUpStreamMediaType,
					IMFMediaType** aPtrPtrInputDownStreamMediaType,
					IMFMediaType*** aPtrPtrPtrOutputDownStramMediaTypes,
					UINT32& aRefOutputDownStramMediaTypeCount);

				HRESULT getCurrentMediaTypesOfTeeNode(
					IMFTopologyNode* aPtrDownStreamNode,
					IMFMediaType* aPtrOutputUpStreamMediaType,
					IMFMediaType** aPtrPtrInputDownStreamMediaType,
					IMFMediaType*** aPtrPtrPtrOutputDownStramMediaTypes,
					UINT32& aRefOutputDownStramMediaTypeCount);
			};
		}
	}
}