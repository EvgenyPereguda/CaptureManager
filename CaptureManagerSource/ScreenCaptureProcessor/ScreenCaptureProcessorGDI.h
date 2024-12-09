#pragma once
#include "../Common/IInnerCaptureProcessor.h"
//#include "BoxMode.h"

#include "../Common/BaseUnknown.h"

#include <thread>
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "ScreenCaptureProcessorFactory.h"
#include "../Common/ICheck.h"
#include "ScreenCaptureProcessor.h"


struct IMFMediaType;
struct IMFSample;
struct IMFMediaBuffer;

namespace CaptureManager
{
	namespace Sources
	{
		namespace ScreenCapture
		{
			class ScreenCaptureProcessorGDI :
				public ScreenCaptureProcessor
				//,protected ScreenCaptureProcessor
			{
			public:
				ScreenCaptureProcessorGDI();

				ScreenCaptureProcessorGDI(RECT aWorkArea);
				

				HRESULT init(
					const std::wstring& aRefDeviceName);

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


				virtual std::wstring getSymbolicLink() override;

				virtual std::wstring getFrendlyName();


				// ScreenCaptureProcessorGDI interface

				HWND mDesktopHWND;

				HDC mDesktopHDC;

				HDC mDesktopCompatibleHDC;

				BYTE* mPtrBits;



				HBITMAP mDesktopCompatibleHBitmap;

				HDC mSecondDesktopCompatibleHDC;

				HDC mDestCompatibleHDC;

				HBITMAP mSecondDesktopCompatibleHBitmap;

				HBITMAP mDestCompatibleHBitmap;

				BYTE* mSecondPtrBits;
							
				RECT mDesktopCoordinates;

				virtual ~ScreenCaptureProcessorGDI();


			protected:

				virtual HRESULT fillVectorScreenCaptureConfigs(
					std::vector<ScreenCaptureConfig>& aRefVectorScreenCaptureConfigs) override;

			private:
				SIZE mDesktopSize;

				std::wstring mOutputName;

				std::wstring mDeviceName;

				RECT mWorkArea;

				POINT mMultiDisplayOffset;

				BOOL mIsMultiDisplay;
			};
		}
	}
}