#pragma once
#include <guiddef.h>
#include <Unknwn.h>
#include <map>
#include <string>

//#include "Result.h"
//#include "pugixml.hpp"

namespace CaptureManager
{
	namespace Core
	{
		class IMaker;
	}

	namespace Transform
	{
		namespace Encoder
		{
			class EncoderInfoData
			{
			public:

				GUID mGUID;

				GUID mMediaSubType;
				
				BOOL mIsStreaming;

				UINT32 mMaxBitRate;

				UINT32 mMinBitRate;

			};

			struct IEncoderManager;

			struct GUIDComparer
			{
				bool operator()(const EncoderInfoData & Left, const EncoderInfoData & Right) const
				{
					return memcmp(&Left.mGUID, &Right.mGUID, sizeof(Right.mGUID)) < 0;
				}
			};

			class EncoderManagerFactory
			{
			public:
				
				HRESULT registerInstanceMaker(
					EncoderInfoData aEncoderInfoData,
					const CaptureManager::Core::IMaker* aPtrMaker);

				HRESULT createEncoderManager(
					REFGUID aRefEncoderCLSID,
					IEncoderManager** aPtrPtrIEncoderManager);

				HRESULT getMaxMinBitRate(
					REFGUID aRefEncoderCLSID,
					UINT32& aRefMaxBitRate,
					UINT32& aRefMinBitRate);

			protected:

				EncoderManagerFactory();
				~EncoderManagerFactory();

			private:

				std::map<EncoderInfoData, const CaptureManager::Core::IMaker*, GUIDComparer> mEncoderManagerMakers;
								
				EncoderManagerFactory(const EncoderManagerFactory&) = delete;
				EncoderManagerFactory& operator=(const EncoderManagerFactory&) = delete;
			};
		}
	}
}