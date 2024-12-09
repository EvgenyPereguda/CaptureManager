#pragma once
#include "WebCamKernelStreamingControl.h"



namespace CaptureManager
{
	namespace Controls
	{
		namespace WebCamControls
		{
			namespace CustomisedWebCamControl
			{
				class KsControlWebCamKernelStreamingControl :
					public WebCamKernelStreamingControl
				{
					friend class WebCamKernelStreamingControlManager;

				public:

					// IProcessWebCapProperty implementation

					virtual HRESULT STDMETHODCALLTYPE processProperty(
						LPVOID aInputPtrBuffer, LONGLONG aInputLengthBuffer,
						LPVOID aOutputPtrBuffer, LONGLONG aOutputLengthBuffer) override;

					virtual ~KsControlWebCamKernelStreamingControl();
				private:

					KsControlWebCamKernelStreamingControl(IUnknown* aUnkKsControl);

					CComPtrCustom<IUnknown> mUnkKsControl;
				};
			}
		}
	}
}
