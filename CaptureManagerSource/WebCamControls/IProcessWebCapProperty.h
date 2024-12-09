#pragma once

#include <Unknwnbase.h>

namespace CaptureManager
{
	namespace Controls
	{
		namespace WebCamControls
		{
			namespace CustomisedWebCamControl
			{
				MIDL_INTERFACE("71A29DFE-22BC-40D2-8A45-A22A00D760BC")
				IProcessWebCapProperty : public IUnknown
				{
				public:
					virtual HRESULT STDMETHODCALLTYPE processProperty(
						LPVOID aInputPtrBuffer, LONGLONG aInputLengthBuffer,
						LPVOID aOutputPtrBuffer, LONGLONG aOutputLengthBuffer) = 0;

				};
			}
		}
	}
}
