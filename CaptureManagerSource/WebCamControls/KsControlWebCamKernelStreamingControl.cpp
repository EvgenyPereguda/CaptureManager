/*
MIT License

Copyright(c) 2021 Evgeny Pereguda

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "KsControlWebCamKernelStreamingControl.h"
#include "../LogPrintOut/LogPrintOut.h"

#include <ks.h>
#include <ksmedia.h>
#include <ksproxy.h>


namespace CaptureManager
{
	namespace Controls
	{
		namespace WebCamControls
		{
			namespace CustomisedWebCamControl
			{
				KsControlWebCamKernelStreamingControl::KsControlWebCamKernelStreamingControl(IUnknown* aUnkKsControl)
				{
					mUnkKsControl = aUnkKsControl;
				}

				KsControlWebCamKernelStreamingControl::~KsControlWebCamKernelStreamingControl()
				{

				}

				HRESULT STDMETHODCALLTYPE KsControlWebCamKernelStreamingControl::processProperty(
					LPVOID aInputPtrBuffer, LONGLONG aInputLengthBuffer,
					LPVOID aOutputPtrBuffer, LONGLONG aOutputLengthBuffer)
				{
					HRESULT lresult(E_FAIL);

					do
					{
						LOG_CHECK_PTR_MEMORY(mUnkKsControl);

						CComPtrCustom<IKsControl> lIKsControl;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(mUnkKsControl, &lIKsControl);

						LOG_CHECK_PTR_MEMORY(lIKsControl);


						ULONG bytesReturned = 0;

						lresult = lIKsControl->KsProperty((PKSPROPERTY)aInputPtrBuffer, aInputLengthBuffer, aOutputPtrBuffer, aOutputLengthBuffer, &bytesReturned);
						
					} while (false);

					return lresult;
				}
			}
		}
	}
}