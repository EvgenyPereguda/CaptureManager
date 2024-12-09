#pragma once

#include <string>
#include <mutex>
#include <condition_variable>

#include "./WebCamKernelStreamingControl.h"


namespace CaptureManager
{
	namespace Controls
	{
		namespace WebCamControls
		{
			namespace CustomisedWebCamControl
			{
				class FileWebCamKernelStreamingControl :
					public WebCamKernelStreamingControl
				{
					friend class WebCamKernelStreamingControlManager;

				public:

					// IProcessWebCapProperty implementation

					virtual HRESULT STDMETHODCALLTYPE processProperty(
						LPVOID aInputPtrBuffer, LONGLONG aInputLengthBuffer,
						LPVOID aOutputPtrBuffer, LONGLONG aOutputLengthBuffer) override;
					
					virtual ~FileWebCamKernelStreamingControl();
					
				private:
					
					HANDLE mDevice;

					FileWebCamKernelStreamingControl(HANDLE aDevice);
				};
			}
		}
	}
}