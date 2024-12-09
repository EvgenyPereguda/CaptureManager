#pragma once

#include <string>
#include <map>
#include <vector>
#include <Unknwnbase.h>
#include <list>

#include "../CaptureManagerBroker/EncodingSettings.h"
#include "../Common/ComPtrCustom.h"

struct IMFMediaType;
struct IPropertyStore;
struct IMFTransform;
struct IMFTopologyNode;
struct ICodecAPI;

namespace CaptureManager
{
	namespace Transform
	{
		namespace Encoder
		{
			class EncoderInfo
			{
			public:

				std::wstring mName;

				GUID mGUID;

				BOOL mIsStreaming = FALSE;

			};

			using namespace CaptureManager;

			class EncoderManager
			{
			public:

				static HRESULT fixVBRQUALITY(
					EncodingSettings& aRefEncodingSettings);

				static GUID getModeGUID(
					EncodingSettings::EncodingMode aEncodingMode);

				static std::wstring getModeTitle(
					EncodingSettings::EncodingMode aEncodingMode);

				static EncodingSettings::EncodingMode getEncodingMode(
					REFGUID aRefModeGUID);

				static HRESULT getDictionaryOfVideoEncoders(
					std::list<EncoderInfo>& aRefVideoEncoderDic);

				static HRESULT getDictionaryOfAudioEncoders(
					std::list<EncoderInfo>& aRefAudioEncoderDic);

				static HRESULT enumEncoderMediaTypes(
					IMFMediaType* aPtrMediaType,
					REFGUID aEncoderCLSID,
					std::vector<CComPtrCustom<IUnknown>> &aRefListOfMediaTypes,
					EncodingSettings aEncodingSettings);
				
				static HRESULT EncoderManager::getCompressedMediaType(
					IMFMediaType* aPtrUncompressedMediaType,
					EncodingSettings aEncodingSettings,
					REFGUID aRefGUIDuidCategory,
					REFGUID aRefEncoderCLSID,
					DWORD lIndexCompressedMediaType,
					IMFMediaType** aPtrPtrCompressedMediaType);
								
				static HRESULT createEncoderTopologyNode(
					IMFMediaType* aPtrUncompressedMediaType,
					EncodingSettings aEncodingSettings,
					REFGUID aRefEncoderCLSID,
					DWORD lIndexCompressedMediaType,
					IMFTopologyNode** aPtrPtrEncoderTopologyNode);

				static HRESULT getMaxMinBitRate(
					REFGUID aRefEncoderCLSID,
					UINT32& aRefMaxBitRate,
					UINT32& aRefMinBitRate);
												
			private:
				EncoderManager() = delete;
				~EncoderManager() = delete;
				EncoderManager(const EncoderManager&) = delete;
				EncoderManager& operator=(const EncoderManager&) = delete;


				struct GUIDComparer
				{
					bool operator()(const GUID & Left, const GUID & Right) const
					{
						return memcmp(&Left, &Right, sizeof(Right)) < 0;
					}
				};


				static std::map<GUID, EncodingSettings::EncodingMode, GUIDComparer> getModeGUIDCollection();


			};
		}
	}
}