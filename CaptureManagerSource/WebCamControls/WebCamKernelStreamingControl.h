#pragma once

#include "../CaptureManagerBroker/IWebCamKernelStreamingControl.h"
#include "./IProcessWebCapProperty.h"
#include "../Common/BaseUnknown.h"
#include "../Common/ComPtrCustom.h"

namespace CaptureManager
{
	namespace Controls
	{
		namespace WebCamControls
		{
			namespace CustomisedWebCamControl
			{
				class WebCamKernelStreamingControl :
					public BaseUnknown<IWebCamKernelStreamingControl, IProcessWebCapProperty>
				{
				public:

					// IWebCamKernelStreamingControl interface implemnetation

					virtual ~WebCamKernelStreamingControl();


					virtual HRESULT STDMETHODCALLTYPE getCamParametrs(
						BSTR *aXMLstring);

					virtual HRESULT STDMETHODCALLTYPE getCamParametr(
						DWORD aParametrIndex,
						LONG *aCurrentValue,
						LONG *aMin,
						LONG *aMax,
						LONG *aStep,
						LONG *aDefault,
						LONG *aFlag);

					virtual HRESULT STDMETHODCALLTYPE setCamParametr(
						DWORD aParametrIndex,
						LONG aNewValue,
						LONG aFlag);
				};
			}
		}
	}
}