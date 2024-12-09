#pragma once

#include <Unknwn.h>
#include <vector>


#include "../CaptureManagerBroker/SinkCommon.h"
#include "../Common/ComPtrCustom.h"


namespace CaptureManager
{
	namespace Sinks
	{
		class AudioRendererFactory
		{
		public:

			void getReadModes(
				BOOL aMultiOutputNodeVideoRenderer,
				std::vector<GUIDToNamePair>& aRefReadModes);

			HRESULT createRendererOutputNode(
				CComPtrCustom<IUnknown>& aRefOutputNode);

		protected:
			AudioRendererFactory();
			~AudioRendererFactory();

		private:
			AudioRendererFactory(
				const AudioRendererFactory&) = delete;
			AudioRendererFactory& operator=(
				const AudioRendererFactory&) = delete;
		};
	}
}
