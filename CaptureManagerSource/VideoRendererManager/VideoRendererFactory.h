#pragma once

#include <Unknwn.h>
#include <vector>


#include "../CaptureManagerBroker/SinkCommon.h"
#include "../Common/ComPtrCustom.h"


namespace CaptureManager
{
	namespace Sinks
	{
		class VideoRendererFactory
		{
		public:

			void getReadModes(
				BOOL aMultiOutputNodeVideoRenderer,
				std::vector<GUIDToNamePair>& aRefReadModes);
						
			HRESULT createRendererOutputNodes(
				HANDLE aHandle,
				IUnknown *aPtrUnkTarget,
				DWORD aOutputNodeAmount,
				std::vector<CComPtrCustom<IUnknown>>& aRefOutputNodes);

		protected:
			VideoRendererFactory();
			~VideoRendererFactory();
						
		private:
			VideoRendererFactory(
				const VideoRendererFactory&) = delete;
			VideoRendererFactory& operator=(
				const VideoRendererFactory&) = delete;
		};
	}
}