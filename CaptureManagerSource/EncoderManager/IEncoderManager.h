#pragma once
#include <Unknwnbase.h>

#include "../Common/ComPtrCustom.h"
#include "../CaptureManagerBroker/EncodingSettings.h"

struct IMFMediaType;
struct IMFTransform;

namespace CaptureManager
{
	namespace Transform
	{
		namespace Encoder
		{			
			MIDL_INTERFACE("380AF4E6-AE79-4D70-960D-2319C9948313")
			IEncoderManager :
				public IUnknown
			{
			public:

				virtual HRESULT enumEncoderMediaTypes(
					IMFMediaType* aPtrUncompressedMediaType,
					EncodingSettings aEncodingSettings,
					REFGUID aRefEncoderCLSID,
					std::vector<CComPtrCustom<IUnknown>> &aRefListOfMediaTypes) = 0;

				virtual HRESULT getCompressedMediaType(
					IMFMediaType* aPtrUncompressedMediaType,
					EncodingSettings aEncodingSettings,
					REFGUID aRefEncoderCLSID,
					DWORD lIndexCompressedMediaType,
					IMFMediaType** aPtrPtrCompressedMediaType) = 0;

				virtual HRESULT getEncoder(
					IMFMediaType* aPtrUncompressedMediaType,
					EncodingSettings aEncodingSettings,
					REFGUID aRefEncoderCLSID,
					DWORD lIndexCompressedMediaType,
					IMFTransform** aPtrPtrEncoderTransform) = 0;
				
				virtual ~IEncoderManager()
				{
				}
			};
		}
	}
}