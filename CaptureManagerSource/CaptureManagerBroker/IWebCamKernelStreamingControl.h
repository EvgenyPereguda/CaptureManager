#pragma once

#include <Unknwnbase.h>


namespace CaptureManager
{
	namespace Controls
	{
		namespace WebCamControls
		{
			MIDL_INTERFACE("72934FD4-8A00-44B4-AD03-0D9B2038D60E")
			IWebCamKernelStreamingControl : public IUnknown
			{
			public:
				virtual HRESULT STDMETHODCALLTYPE getCamParametrs(
					BSTR *aXMLstring) = 0;

				virtual HRESULT STDMETHODCALLTYPE getCamParametr(
					DWORD aParametrIndex,
					LONG *aCurrentValue,
					LONG *aMin,
					LONG *aMax,
					LONG *aStep,
					LONG *aDefault,
					LONG *aFlag) = 0;

				virtual HRESULT STDMETHODCALLTYPE setCamParametr(
					DWORD aParametrIndex,
					LONG aNewValue,
					LONG aFlag) = 0;
			};
		}
	}
}