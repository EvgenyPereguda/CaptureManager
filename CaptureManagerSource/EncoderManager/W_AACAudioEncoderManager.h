#pragma once

#include <vector>


#include "../Common/BaseUnknown.h"
#include "IEncoderManager.h"

namespace CaptureManager
{
	namespace Transform
	{
		namespace Encoder
		{
			class W_AACAudioEncoderManager :
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

				// W_H264VideoEncoderManager interface

				static GUID getMediaSubType();

			private:

				HRESULT checkAndFixInputMediatype(
					IMFMediaType** aPtrPtrInputMediaType);

				HRESULT createOutputMediaType(
					IMFMediaType* aPtrOriginalInputMediaType,
					IMFMediaType* aPtrOutputMediaType);

			};
		}
	}
}