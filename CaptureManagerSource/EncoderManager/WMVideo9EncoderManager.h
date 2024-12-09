#pragma once

#include <vector>

#include "../Common/BaseUnknown.h"
#include "../Common/ComPtrCustom.h"
#include "BaseEncoderManager.h"
#include "IEncoderManager.h"
struct IPropertyStore;

namespace CaptureManager
{
	namespace Transform
	{
		namespace Encoder
		{
			class WMVideo9EncoderManager :
				public BaseUnknown<IEncoderManager>,
				public BaseEncoderManager
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

				// WMVideo9EncoderManager interface

				static GUID getMediaSubType();

			private:

				HRESULT copyInputMediaType(
					IMFMediaType* aPtrOriginalInputMediaType,
					IMFMediaType* aPtrCopyInputMediaType);

				HRESULT setEncodingProperties(
					IPropertyStore* aPtrPropertyStore,
					EncodingSettings aEncodingSettings);

				HRESULT addPrivateData(
					IMFTransform* aPtrEncoderTransform,
					IMFMediaType* aPtrMediaType);
			};
		}
	}
}

