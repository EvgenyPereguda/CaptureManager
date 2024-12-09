#pragma once
#include <guiddef.h>
#include <map>
#include <string>
#include <vector>

#include "../CaptureManagerBroker/SinkCommon.h"

namespace CaptureManager
{
	namespace Core
	{
		class IMaker;
	}
	namespace COMServer
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

		typedef GUIDToNamePair(*GetGUIDToNamePair)();
		
		class SinkCollection
		{
		public:
									
			HRESULT registerInstanceMaker(
				GetGUIDToNamePair aGetGUIDToNamePair,
				const CaptureManager::Core::IMaker* aPtrMaker);

			HRESULT createSink(
				REFGUID aRefSinkTypeGUID,
				IUnknown** aPtrPtrIUnknown);

			HRESULT getGUIDToNamePairs(
				std::vector<GUIDToNamePair> & aRefGUIDToNamePairs);

			HRESULT getXMLDocumentStringListOfSinkFactories(
				std::wstringstream &aRefwstringstream);

		protected:

			SinkCollection();

		private:

			std::map<GetGUIDToNamePair, const CaptureManager::Core::IMaker*> mSinkMakers;

			
			SinkCollection(const SinkCollection&) = delete;
			
			SinkCollection& operator=(const SinkCollection&) = delete;
		};
	}
}