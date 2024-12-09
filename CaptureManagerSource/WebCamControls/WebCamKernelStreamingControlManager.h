#pragma once

#include <string>
#include "../CaptureManagerBroker/IWebCamKernelStreamingControl.h"


namespace CaptureManager
{
	namespace Controls
	{
		namespace WebCamControls
		{
			class WebCamKernelStreamingControlManager
			{
			public:

				WebCamKernelStreamingControlManager() = delete;

				~WebCamKernelStreamingControlManager() = delete;


				static HRESULT createIWebCamKernelStreamingControlFromFile(
					std::wstring aSymbolicLink,
					IWebCamKernelStreamingControl** aPtrPtrIWebCamKernelStreamingControl);

				static HRESULT createIWebCamKernelStreamingControlFromKsControl(
					IUnknown* aUnkIKsControl,
					IWebCamKernelStreamingControl** aPtrPtrIWebCamKernelStreamingControl);
			};
		}
	}
}

