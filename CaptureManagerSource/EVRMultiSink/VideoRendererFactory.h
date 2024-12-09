#pragma once

#include <Unknwn.h>
#include <vector>


#include "../CaptureManagerBroker/SinkCommon.h"
#include "../Common/ComPtrCustom.h"


namespace EVRMultiSink
{
	namespace Sinks
	{
		class VideoRendererFactory
		{
		public:

			void getReadModes(
				std::vector<CaptureManager::GUIDToNamePair>& aRefReadModes);
									
			static HRESULT createRendererOutputNodes(
				HANDLE aHandle,
				IUnknown *aPtrUnkTarget,
				DWORD aOutputNodeAmount,
				std::vector<CaptureManager::CComPtrCustom<IUnknown>>& aRefOutputNodes);

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