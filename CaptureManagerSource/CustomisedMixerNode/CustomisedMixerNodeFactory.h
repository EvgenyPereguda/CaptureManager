#pragma once

#include "../CaptureManagerBroker/StreamControlCommon.h"
#include "../PugiXML/pugixml.hpp"
#include <vector>
#include <Unknwnbase.h>
#include "../Common/ComPtrCustom.h"

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			class CustomisedMixerNodeFactory
			{
				struct MixerNodeInfo
				{
					StreamControlInfo mStreamControlInfo;
				};

			public:

				HRESULT getCustomisedMixerNodeFactoryCollection(
					pugi::xml_node& aRefRootXMLElement,
					std::vector<StreamControlInfo>& aCollection);

				HRESULT createCustomisedMixerNodes(
					IUnknown *aPtrDownStreamTopologyNode,
					DWORD aInputNodeAmount,
					std::vector<CComPtrCustom<IUnknown>>& aRefInputNodes);

			protected:
				CustomisedMixerNodeFactory();
				virtual ~CustomisedMixerNodeFactory();

			private:

				std::vector<MixerNodeInfo> mMixerNodeInfos;
			};
		}
	}
}