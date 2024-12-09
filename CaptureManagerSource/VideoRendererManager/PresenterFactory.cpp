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

#include "PresenterFactory.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/Singleton.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Common.h"
#include "Direct3D9Presenter.h"
#include "Direct3D11Presenter.h"
#include "Direct3D11and3D9Presenter.h"
#include "DXVAVideoProcessor.h"
#include "Direct3D11VideoProcessor.h"
#include "IPresenterInit.h"
#include "../ConfigManager/ConfigManager.h"
#include <Windows.ui.xaml.media.dxinterop.h>
#include <VersionHelpers.h>

namespace CaptureManager
{
	namespace Sinks
	{
		namespace EVR
		{
			HRESULT PresenterFactory::createPresenter(
				HANDLE aHandle,
				IUnknown* aPtrUnkTarget,
				DWORD aOutputNodeAmount,
				IPresenter** aPtrPresenter,
				IMFTransform** aPtrPtrMixer)
			{
				using namespace Core;

				using namespace Direct3D9;

				using namespace Direct3D11;

				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPresenter);

					LOG_CHECK_PTR_MEMORY(aPtrPtrMixer);									

					if (aHandle != nullptr && aPtrUnkTarget == nullptr)
					{
						do
						{
							if (Singleton<ConfigManager>::getInstance().isWindows8Point1_Or_Greater())
							{
								do
								{
									LOG_INVOKE_FUNCTION(createDirect3D11PresenterFromWindowHandler,
										aHandle,
										aOutputNodeAmount,
										aPtrPresenter,
										aPtrPtrMixer);

								} while (false);

								if (FAILED(lresult))
								{
									LOG_INVOKE_FUNCTION(createDirect3D9PresenterFromWindowHandler,
										aHandle,
										aOutputNodeAmount,
										aPtrPresenter,
										aPtrPtrMixer);
								}
							}
							else
							{
								LOG_INVOKE_FUNCTION(createDirect3D9PresenterFromWindowHandler,
									aHandle,
									aOutputNodeAmount,
									aPtrPresenter,
									aPtrPtrMixer);
							}

						} while (false);

						if (FAILED(lresult))
						{
							if (Singleton<ConfigManager>::getInstance().isWindows8Point1_Or_Greater())
							{
								do
								{
									LOG_INVOKE_FUNCTION(createDirect3D11Presenter,
										aHandle,
										nullptr,
										aOutputNodeAmount,
										aPtrPresenter,
										aPtrPtrMixer);

									lresult = S_FALSE;

								} while (false);
								
								LOG_INVOKE_FUNCTION(createFromSharedHandler,
									aHandle,
									aOutputNodeAmount,
									aPtrPresenter,
									aPtrPtrMixer);
							}
							else
							{
								LOG_INVOKE_FUNCTION(createFromSharedHandler,
									aHandle,
									aOutputNodeAmount,
									aPtrPresenter,
									aPtrPtrMixer);
							}
						}
					}
					else
					{
						LOG_CHECK_PTR_MEMORY(aPtrUnkTarget);

						CComQIPtrCustom<ISwapChainPanelNative> lSwapChainPanelNative = aPtrUnkTarget;

						if (lSwapChainPanelNative)
						{
							LOG_INVOKE_FUNCTION(createDirect3D11Presenter,
								aHandle,
								lSwapChainPanelNative,
								aOutputNodeAmount,
								aPtrPresenter,
								aPtrPtrMixer);

							lresult = S_FALSE;

							break;
						}

						CComQIPtrCustom<ID3D11Texture2D> lD3D11Texture2D = aPtrUnkTarget;

						if (lD3D11Texture2D)
						{
							LOG_INVOKE_FUNCTION(createDirect3D11Presenter,
								aHandle,
								lD3D11Texture2D,
								aOutputNodeAmount,
								aPtrPresenter,
								aPtrPtrMixer);

							lresult = S_FALSE;

							break;
						}

						CComQIPtrCustom<IDXGISwapChain> lDXGISwapChain = aPtrUnkTarget;

						if (lDXGISwapChain)
						{
							LOG_INVOKE_FUNCTION(createDirect3D11Presenter,
								aHandle,
								lDXGISwapChain,
								aOutputNodeAmount,
								aPtrPresenter,
								aPtrPtrMixer);

							lresult = S_FALSE;

							break;
						}
						
						CComPtrCustom<IDirect3DSurface9> lDirect3DSurface9;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrUnkTarget, &lDirect3DSurface9);

						if (lDirect3DSurface9)
						{
							do
							{

								CComPtrCustom<Direct3D11and3D9Presenter> lDirect3D9Presenter(new (std::nothrow) Direct3D11and3D9Presenter);

								//CComPtrCustom<Direct3D11Presenter> lPresenter(new (std::nothrow) Direct3D11Presenter);

								LOG_CHECK_PTR_MEMORY(lDirect3D9Presenter);

								CComPtrCustom<IMFTransform> lMixer;

								LOG_INVOKE_FUNCTION(EVR::Mixer::Direct3D11VideoProcessor::createProcessor,
									&lMixer,
									aOutputNodeAmount);


								//CComPtrCustom<Direct3D9Presenter> lDirect3D9Presenter(new (std::nothrow) Direct3D9Presenter);

								//LOG_CHECK_PTR_MEMORY(lDirect3D9Presenter);

								//CComPtrCustom<IMFTransform> lMixer;

								//LOG_INVOKE_FUNCTION(EVR::Mixer::DXVAVideoProcessor::createDXVAVideoProcessor,
								//	&lMixer);

								LOG_INVOKE_POINTER_METHOD(lDirect3D9Presenter, initialize,
									1920,
									1080,
									30,
									1,
									lMixer);

								CComPtrCustom<IPresenterInit> lPresenterInit;

								LOG_INVOKE_QUERY_INTERFACE_METHOD(lDirect3D9Presenter, &lPresenterInit);

								LOG_CHECK_PTR_MEMORY(lPresenterInit);

								LOG_INVOKE_POINTER_METHOD(lPresenterInit, initializeSharedTarget,
									aHandle,
									aPtrUnkTarget);

								LOG_INVOKE_QUERY_INTERFACE_METHOD(lDirect3D9Presenter, aPtrPresenter);

								LOG_INVOKE_QUERY_INTERFACE_METHOD(lMixer, aPtrPtrMixer);

							} while (false);

							if (FAILED(lresult))
							{
								CComPtrCustom<Direct3D9Presenter> lDirect3D9Presenter(new (std::nothrow) Direct3D9Presenter);

								LOG_CHECK_PTR_MEMORY(lDirect3D9Presenter);

								CComPtrCustom<IMFTransform> lMixer;

								LOG_INVOKE_FUNCTION(EVR::Mixer::DXVAVideoProcessor::createDXVAVideoProcessor,
									&lMixer);

								LOG_INVOKE_POINTER_METHOD(lDirect3D9Presenter, initialize,
									1920,
									1080,
									30,
									1,
									lMixer);

								CComPtrCustom<IPresenterInit> lPresenterInit;

								LOG_INVOKE_QUERY_INTERFACE_METHOD(lDirect3D9Presenter, &lPresenterInit);

								LOG_CHECK_PTR_MEMORY(lPresenterInit);

								LOG_INVOKE_POINTER_METHOD(lPresenterInit, initializeSharedTarget,
									aHandle,
									aPtrUnkTarget);

								LOG_INVOKE_QUERY_INTERFACE_METHOD(lDirect3D9Presenter, aPtrPresenter);

								LOG_INVOKE_QUERY_INTERFACE_METHOD(lMixer, aPtrPtrMixer);
							}
						}				   
					}
				} while (false);

				return lresult;
			}

			HRESULT PresenterFactory::createDirect3D9PresenterFromWindowHandler(
				HANDLE aHandle,
				DWORD aOutputNodeAmount,
				IPresenter** aPtrPresenter,
				IMFTransform** aPtrPtrMixer)
			{
				using namespace Core;

				using namespace Direct3D9;

				using namespace Direct3D11;

				HRESULT lresult(E_FAIL);

				do
				{

					DWORD lMaxInputStreamCount(0);

					LOG_INVOKE_FUNCTION(getMaxInputStreamCount,
						&lMaxInputStreamCount);

					LOG_CHECK_STATE_DESCR(aOutputNodeAmount > lMaxInputStreamCount, E_INVALIDARG);
					
					CComPtrCustom<IMFTransform> lMixer;

					CComPtrCustom<Direct3D9Presenter> lPresenter(new (std::nothrow) Direct3D9Presenter);

					LOG_CHECK_PTR_MEMORY(lPresenter);

					LOG_INVOKE_FUNCTION(EVR::Mixer::DXVAVideoProcessor::createDXVAVideoProcessor,
						&lMixer);

					LOG_CHECK_PTR_MEMORY(lMixer);

					LOG_INVOKE_POINTER_METHOD(lPresenter, initialize,
						1920,
						1080,
						30,
						1,
						lMixer);

					LOG_INVOKE_POINTER_METHOD(lPresenter,
						setVideoWindowHandle,
						(HWND)aHandle);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lPresenter, aPtrPresenter);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lMixer, aPtrPtrMixer);

				} while (false);

				return lresult;
			}

			HRESULT PresenterFactory::createDirect3D11PresenterFromWindowHandler(
				HANDLE aHandle,
				DWORD aOutputNodeAmount,
				IPresenter** aPtrPresenter,
				IMFTransform** aPtrPtrMixer)
			{
				using namespace Core;

				using namespace Direct3D9;

				using namespace Direct3D11;

				HRESULT lresult(E_FAIL);

				do
				{

					DWORD lMaxInputStreamCount(0);

					LOG_INVOKE_FUNCTION(getMaxInputStreamCount,
						&lMaxInputStreamCount);

					LOG_CHECK_STATE_DESCR(aOutputNodeAmount > lMaxInputStreamCount, E_INVALIDARG);

					CComPtrCustom<IMFTransform> lMixer;

					CComPtrCustom<Direct3D11Presenter> lPresenter(new (std::nothrow) Direct3D11Presenter);

					LOG_CHECK_PTR_MEMORY(lPresenter);

					LOG_INVOKE_FUNCTION(EVR::Mixer::Direct3D11VideoProcessor::createProcessor,
						&lMixer,
						aOutputNodeAmount);

					LOG_CHECK_PTR_MEMORY(lMixer);

					LOG_INVOKE_POINTER_METHOD(lPresenter, initialize,
						1920,
						1080,
						30,
						1,
						lMixer);

					LOG_INVOKE_POINTER_METHOD(lPresenter,
						setVideoWindowHandle,
						(HWND)aHandle);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lPresenter, aPtrPresenter);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lMixer, aPtrPtrMixer);

				} while (false);

				return lresult;
			}

			HRESULT PresenterFactory::createFromSharedHandler(
				HANDLE aHandle,
				DWORD aOutputNodeAmount,
				IPresenter** aPtrPresenter,
				IMFTransform** aPtrPtrMixer)
			{
				using namespace Core;

				using namespace Direct3D9;

				using namespace Direct3D11;

				HRESULT lresult(E_FAIL);

				do
				{

					DWORD lMaxInputStreamCount(0);

					LOG_INVOKE_FUNCTION(getMaxInputStreamCount,
						&lMaxInputStreamCount);

					LOG_CHECK_STATE_DESCR(aOutputNodeAmount > lMaxInputStreamCount, E_INVALIDARG);

					CComPtrCustom<IMFTransform> lMixer;

					CComPtrCustom<Direct3D9Presenter> lPresenter(new (std::nothrow) Direct3D9Presenter);

					LOG_CHECK_PTR_MEMORY(lPresenter);

					LOG_INVOKE_FUNCTION(EVR::Mixer::DXVAVideoProcessor::createDXVAVideoProcessor,
						&lMixer);

					LOG_CHECK_PTR_MEMORY(lMixer);

					LOG_INVOKE_POINTER_METHOD(lPresenter, initialize,
						1920,
						1080,
						30,
						1,
						lMixer);

					CComPtrCustom<IPresenterInit> lPresenterInit;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lPresenter, &lPresenterInit);

					LOG_CHECK_PTR_MEMORY(lPresenterInit);

					LOG_INVOKE_POINTER_METHOD(lPresenterInit, initializeSharedTarget,
						aHandle,
						nullptr);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lPresenter, aPtrPresenter);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lMixer, aPtrPtrMixer);

				} while (false);

				return lresult;
			}

			

			HRESULT PresenterFactory::createDirect3D11Presenter(
				HANDLE aHandle,
				IUnknown* aPtrUnkTarget,
				DWORD aOutputNodeAmount,
				IPresenter** aPtrPresenter,
				IMFTransform** aPtrPtrMixer)
			{
				using namespace Core;

				using namespace Direct3D9;

				using namespace Direct3D11;

				HRESULT lresult(E_FAIL);

				do
				{

					DWORD lMaxInputStreamCount(0);

					LOG_INVOKE_FUNCTION(getMaxInputStreamCount,
						&lMaxInputStreamCount);

					LOG_CHECK_STATE_DESCR(aOutputNodeAmount > lMaxInputStreamCount, E_INVALIDARG);

					CComPtrCustom<Direct3D11Presenter> lDirect3D11Presenter(new (std::nothrow) Direct3D11Presenter);

					LOG_CHECK_PTR_MEMORY(lDirect3D11Presenter);

					CComPtrCustom<IMFTransform> lMixer;

					LOG_INVOKE_FUNCTION(EVR::Mixer::Direct3D11VideoProcessor::createProcessor,
						&lMixer,
						aOutputNodeAmount);

					LOG_INVOKE_POINTER_METHOD(lDirect3D11Presenter, initialize,
						1920,
						1080,
						10,
						1,
						lMixer);

					CComPtrCustom<IPresenterInit> lPresenterInit;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lDirect3D11Presenter, &lPresenterInit);

					LOG_CHECK_PTR_MEMORY(lPresenterInit);

					LOG_INVOKE_POINTER_METHOD(lPresenterInit, initializeSharedTarget,
						aHandle,
						aPtrUnkTarget);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lDirect3D11Presenter, aPtrPresenter);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lMixer, aPtrPtrMixer);

				} while (false);

				return lresult;
			}
						
			HRESULT PresenterFactory::getMaxInputStreamCount(DWORD* aPtrMaxInputStreamCount)
			{
				using namespace Core;

				using namespace Direct3D9;

				using namespace Direct3D11;

				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrMaxInputStreamCount);
					
					*aPtrMaxInputStreamCount = Singleton<ConfigManager>::getInstance().getMaxVideoRenderStreamCount();

					lresult = S_OK;

				} while (false);

				return lresult;
			}
		}
	}
}