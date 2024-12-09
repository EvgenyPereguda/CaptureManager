#pragma once
#include <Unknwnbase.h>
#include <dxgitype.h>

struct ID3D11Texture2D;
struct ID3D11DeviceContext;

namespace CaptureManager
{
	namespace Sources
	{
		namespace ScreenCapture
		{
			MIDL_INTERFACE("CF999140-05FD-48CB-9C57-1BA973B8E66F")
			IInnerScreenCaptureProcessor : public IUnknown
			{
				virtual HRESULT getSize(SIZE& aRefSize) = 0;

				virtual HRESULT releaseResources() = 0;

				virtual HRESULT initResources(
					IUnknown* aPtrUnkDevice,
					ID3D11DeviceContext* aPtrDeviceContext,
					UINT aVideoFrameDuration,
					int aStride) = 0;

				virtual HRESULT getGDITexture(IUnknown** aPtrPtrUnk) = 0;

				virtual HRESULT updateFrame() = 0;

				virtual HRESULT updateFrame(ID3D11Texture2D* aPtrTexture2D) = 0;				

				virtual HRESULT readFromTexture(BYTE* aPtrData) = 0;

				virtual HRESULT readFromTexture(ID3D11Texture2D* aPtrTexture2D) = 0;

				virtual HRESULT getCursorInfo(RECT& aRefDesktopCoordinates, DXGI_MODE_ROTATION& aRefRotation) = 0;
			};
		}
	}
}