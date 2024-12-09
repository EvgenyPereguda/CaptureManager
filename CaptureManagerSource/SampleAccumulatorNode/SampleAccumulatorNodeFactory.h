#pragma once


#include "../CaptureManagerBroker/StreamControlCommon.h"
#include "../PugiXML/pugixml.hpp"
#include <vector>
#include <Unknwnbase.h>


namespace CaptureManager
{
	namespace Transform
	{
		class SampleAccumulatorNodeFactory
		{
			struct SampleAccumulatorInfo
			{
				StreamControlInfo mStreamControlInfo;

				int mSampleSize;
			};

		public:

			HRESULT getSampleAccumulatorNodeFactoryCollection(
				pugi::xml_node& aRefRootXMLElement,
				std::vector<StreamControlInfo>& aCollection);

			HRESULT createSampleAccumulatorNode(
				REFIID aREFIID,
				std::vector<IUnknown*> aArrayDownStreamTopologyNodes,
				IUnknown** aPtrPtrTopologySampleAccumulatorNode);

		protected:
			SampleAccumulatorNodeFactory();
			~SampleAccumulatorNodeFactory();

		private:

			std::vector<SampleAccumulatorInfo> mSampleAccumulatorInfo;
		};
	}
}