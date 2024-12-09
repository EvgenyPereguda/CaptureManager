#pragma once
#include <guiddef.h>
#include <map>
#include <string>
#include <Unknwnbase.h>
#include "../CaptureManagerBroker/SinkCommon.h"


namespace CaptureManager
{
	namespace Core
	{
		class IMaker;
	}
	namespace Sinks
	{


		struct GUIDComparer
		{
			bool operator()(const GUIDToNamePair & Left, const GUIDToNamePair & Right) const
			{
				return memcmp(&(Left.mGUID), &(Right.mGUID), sizeof(Right.mGUID)) < 0;
			}

			bool operator()(const GUID & Left, const GUID & Right) const
			{
				return memcmp(&Left, &Right, sizeof(Right)) < 0;
			}
		};


		class OutputNodeFactoryCollection
		{
		public:
						
			HRESULT registerInstanceMaker(
				GUIDToNamePair aGUIDToNamePair,
				const Core::IMaker* aPtrMaker);

			HRESULT createOutputNodeFactory(
				REFGUID aRefOutputNodeFactoryCollectionGUID,
				IUnknown** aPtrPtrIUnknown);
			
			HRESULT getGUIDToNamePairs(
				SinkType aSinkType,
				std::vector<GUIDToNamePair> & aRefGUIDToNamePairs);
		protected:
			OutputNodeFactoryCollection();

		private:

			std::map<GUIDToNamePair, const Core::IMaker*, GUIDComparer> mSinkMakers;

			std::multimap<SinkType, GUIDToNamePair> mSinkTypes;

			
			OutputNodeFactoryCollection(const OutputNodeFactoryCollection&) = delete;
			
			OutputNodeFactoryCollection& operator=(const OutputNodeFactoryCollection&) = delete;
		};
	}
}