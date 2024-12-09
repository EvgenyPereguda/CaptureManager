#pragma once

#include "InnerScreenCaptureProcessor.h"

namespace CaptureManager
{
	namespace Sources
	{
		namespace ScreenCapture
		{
			class InnerScreenCaptureProcessorLandscape:
				public InnerScreenCaptureProcessor
			{
			public:
				InnerScreenCaptureProcessorLandscape();
				virtual ~InnerScreenCaptureProcessorLandscape();

				// IInnerScreenCaptureProcessor

				virtual HRESULT initResources(
					IUnknown* aPtrUnkDevice,
					ID3D11DeviceContext* aPtrDeviceContext,
					UINT aVideoFrameDuration,
					int aStride) override;

				virtual HRESULT readFromTexture(BYTE* aPtrData) override;

				virtual HRESULT readFromTexture(ID3D11Texture2D* aPtrTexture2D)override;

			private:
				CComPtrCustom<ID3D11Texture2D> mTempRenderImage;

				HRESULT redraw();
			};
		}
	}
}