#pragma once

#include <vector>

#include "../Common/BaseUnknown.h"
#include "../Common/ComPtrCustom.h"
#include "IEncoderManager.h"
#include "BaseEncoderManager.h"

struct IPropertyStore;

namespace CaptureManager
{
	namespace Transform
	{
		namespace Encoder
		{
			class WMVideo9ScreenEncoderManager :
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

				// WMVideo9ScreenEncoderManager interface

				static GUID getMediaSubType();

			private:

				HRESULT checkAndFixInputMediatype(
					IMFMediaType* aPtrInputMediaType);

				HRESULT copyInputMediaType(
					IMFMediaType* aPtrOriginalInputMediaType,
					IMFMediaType* aPtrCopyInputMediaType);

				HRESULT modifySubTypeOFMediaType(
					IMFMediaType* aPtrMediaType,
					REFGUID aNewSubType);
				
				HRESULT copyFromInputToOutputMediaType(
					IMFMediaType* aPtrInputMediaType,
					IMFMediaType* aPtrOutputMediaType);

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