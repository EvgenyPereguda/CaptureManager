#pragma once

#include <vector>

#include "../Common/BaseUnknown.h"
#include "../Common/ComPtrCustom.h"
#include "IEncoderManager.h"

struct IPropertyStore;
struct IPropertyBag;

namespace CaptureManager
{
	namespace Transform
	{
		namespace Encoder
		{
			class WMAudioVoiceEncoderManager :
				public BaseUnknown<IEncoderManager>
			{
			public:
				// IEncoderManager interface

				virtual HRESULT enumEncoderMediaTypes(
					IMFMediaType* aPtrInputMediaType,
					EncodingSettings aEncodingSettings,
					REFGUID aRefEncoderCLSID,
					std::vector<CComPtrCustom<IUnknown>> &aRefListOfMediaTypes);

				virtual HRESULT getCompressedMediaType(
					IMFMediaType* aPtrUncompressedMediaType,
					EncodingSettings aEncodingSettings,
					REFGUID aRefEncoderCLSID,
					DWORD lIndexCompressedMediaType,
					IMFMediaType** aPtrPtrCompressedMediaType);

				virtual HRESULT getEncoder(
					IMFMediaType* aPtrUncompressedMediaType,
					EncodingSettings aEncodingSettings,
					REFGUID aRefEncoderCLSID,
					DWORD lIndexCompressedMediaType,
					IMFTransform** aPtrPtrEncoderTransform);

				// WMAudioVoiceEncoderManager interface

				static GUID getMediaSubType();

			private:
				
				HRESULT setEncodingProperties(
					IPropertyStore* aPtrPropertyStore,
					EncodingSettings aEncodingSettings);

				HRESULT setEncodingProperties(
					IPropertyBag* aPtrIPropertyBag,
					EncodingSettings aEncodingSettings);

				HRESULT copyAndModifyInputMediaType(
					IMFMediaType* aPtrOriginalInputMediaType,
					IMFMediaType** aPtrPtrCopyAndModifyInputMediaType);

			};
		}
	}
}