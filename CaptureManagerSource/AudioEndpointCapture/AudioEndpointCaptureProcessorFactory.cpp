/*
MIT License

Copyright(c) 2020 Evgeny Pereguda

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

#include "AudioEndpointCaptureProcessorFactory.h"
#include "AudioEndpointCaptureProcessor.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "../Common/MFHeaders.h"
#include <mmreg.h>
#include <windows.h>
#include <commctrl.h>
#include <Mmdeviceapi.h> 

namespace CaptureManager
{
	namespace Sources
	{
		namespace AudioEndpointCapture
		{
			HRESULT AudioEndpointCaptureProcessorFactory::createAudioEndpointCaptureProcessors(
				std::vector<CComPtrCustom<IInnerCaptureProcessor>>& aVectorAudioEndpointCaptureProcessors)
			{
				HRESULT lresult;
				
				do
				{

					CComPtrCustom<AudioEndpointCaptureProcessor> lAudioEndpointCaptureProcessor = new(std::nothrow) AudioEndpointCaptureProcessor();

					LOG_CHECK_COM_PTR_OBJECT_MEMORY(lAudioEndpointCaptureProcessor);
					
					CComPtrCustom<IMMDeviceEnumerator> lMMDeviceEnumerator;

					LOG_INVOKE_OBJECT_METHOD(lMMDeviceEnumerator, 
						CoCreateInstance, 
						__uuidof(MMDeviceEnumerator));
					
					LOG_CHECK_PTR_MEMORY(lMMDeviceEnumerator);
					
					CComPtrCustom<IMMDevice> lMMDevice;

					LOG_INVOKE_POINTER_METHOD(lMMDeviceEnumerator,
						GetDefaultAudioEndpoint, 
						eRender, 
						eConsole, 
						&lMMDevice);

					LOG_CHECK_PTR_MEMORY(lMMDevice);

					LOG_INVOKE_POINTER_METHOD(lAudioEndpointCaptureProcessor,
						init,
						lMMDevice, 
						L"AudioLoopBack");
						
					CComPtrCustom<IInnerCaptureProcessor> lIInnerCaptureProcessor;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lAudioEndpointCaptureProcessor,
						&lIInnerCaptureProcessor);
					
					aVectorAudioEndpointCaptureProcessors.push_back(lIInnerCaptureProcessor);

				} while (false);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						(HRESULT)lresult);
				}

				return lresult;
			}

			HRESULT AudioEndpointCaptureProcessorFactory::createAudioEndpointCaptureProcessor(
				std::wstring aSymbolicLink,
				IInnerCaptureProcessor** aPtrPtrIInnerCaptureProcessor)
			{
				HRESULT lresult;


				do
				{

					CComPtrCustom<AudioEndpointCaptureProcessor> lAudioEndpointCaptureProcessor = new(std::nothrow) AudioEndpointCaptureProcessor();

					LOG_CHECK_COM_PTR_OBJECT_MEMORY(lAudioEndpointCaptureProcessor);

					CComPtrCustom<IMMDeviceEnumerator> lMMDeviceEnumerator;

					LOG_INVOKE_OBJECT_METHOD(lMMDeviceEnumerator,
						CoCreateInstance,
						__uuidof(MMDeviceEnumerator));

					LOG_CHECK_PTR_MEMORY(lMMDeviceEnumerator);

					if (aSymbolicLink == L"AudioEndpointCapture///AudioLoopBack")
					{
						CComPtrCustom<IMMDevice> lMMDevice;

						LOG_INVOKE_POINTER_METHOD(lMMDeviceEnumerator,
							GetDefaultAudioEndpoint,
							eRender,
							eConsole,
							&lMMDevice);

						LOG_CHECK_PTR_MEMORY(lMMDevice);

						LOG_INVOKE_POINTER_METHOD(lAudioEndpointCaptureProcessor,
							init,
							lMMDevice,
							L"AudioLoopBack");
					}
					else
					{
						{
							CComPtrCustom<IMMDevice> lMMDevice;

							LOG_INVOKE_POINTER_METHOD(lMMDeviceEnumerator,
								GetDefaultAudioEndpoint,
								eRender,
								eConsole,
								&lMMDevice);

							LOG_CHECK_PTR_MEMORY(lMMDevice);

							LPWSTR lPtrStrId;

							LOG_INVOKE_POINTER_METHOD(lMMDevice,
								GetId,
								&lPtrStrId);

							LOG_CHECK_PTR_MEMORY(lPtrStrId);
								
							std::wstring lSymbolicLink = std::wstring(lPtrStrId);

							CoTaskMemFree(lPtrStrId);

							auto lSymbolicLinkPos = aSymbolicLink.find(lSymbolicLink);

							LOG_CHECK_STATE(lSymbolicLinkPos == std::wstring::npos);	

							LOG_INVOKE_POINTER_METHOD(lAudioEndpointCaptureProcessor,
								init,
								lMMDevice,
								L"AudioLoopBack");
						}

					}

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lAudioEndpointCaptureProcessor,
						aPtrPtrIInnerCaptureProcessor);

				} while (false);

				return lresult;
			}
		}
	}
}