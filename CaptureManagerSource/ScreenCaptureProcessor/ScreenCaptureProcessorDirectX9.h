#pragma once

#include "ScreenCaptureProcessor.h"

#include <d3d9.h>

namespace CaptureManager
{
	namespace Sources
	{
		namespace ScreenCapture
		{
			class ScreenCaptureProcessorDirectX9 :
				public ScreenCaptureProcessor
			{
			public:
				ScreenCaptureProcessorDirectX9();
				

				// IScreenCaptureCheck interface

				virtual HRESULT STDMETHODCALLTYPE check();


				// IScreenCaptureProcessorEnum interface

				virtual HRESULT STDMETHODCALLTYPE enumIInnerCaptureProcessor(
					UINT aIndex,
					UINT aOrientation,
					IInnerCaptureProcessor** aPtrPtrIInnerCaptureProcessor);


				// ScreenCaptureProcessor interface

				virtual HRESULT initResources(ScreenCaptureConfig& aScreenCaptureConfig, IMFMediaType* aPtrOutputMediaType);

				virtual HRESULT releaseResources();

				virtual HRESULT grabImage(BYTE* aPtrData);

								
				CComPtrCustom<IDirect3D9Ex> mDirect3D;

				CComPtrCustom<IDirect3DDevice9Ex> mDirect3DDevice;

				CComPtrCustom<IDirect3DSurface9> mSurface;



			protected:

				virtual HRESULT fillVectorScreenCaptureConfigs(
					std::vector<ScreenCaptureConfig>& aRefVectorScreenCaptureConfigs) override;

			private:

				SIZE mDesktopSize;

				RECT mDesktopCoordinates;

				virtual ~ScreenCaptureProcessorDirectX9();
			};
		}
	}
}