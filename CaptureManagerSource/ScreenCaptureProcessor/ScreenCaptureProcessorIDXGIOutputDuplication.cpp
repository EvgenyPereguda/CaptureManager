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

#include "ScreenCaptureProcessorIDXGIOutputDuplication.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/Common.h"
#include "../Common/GUIDs.h"
#include "../DataParser/DataParser.h"
#include "ScreenCaptureProcessorFactory.h"
#include "../Common/InstanceMaker.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Singleton.h"
#include "../DirectXManager/DXGIManager.h"
#include "../DirectXManager/Direct3D11Manager.h"
#include "../VideoRendererManager/Direct3D11VideoProcessor.h"
#include "ScreenCaptureProcessorGDI.h"
#include <VersionHelpers.h>
#include <dxgi1_3.h>
#include "InnerScreenCaptureProcessorFactory.h"
#include "PixelShader.h"
#include "VertexShader.h"
#include "../MemoryManager/MemorySampleManager.h"
#include "../ConfigManager/ConfigManager.h"


static CaptureManager::Core::InstanceMaker<CaptureManager::Sources::ScreenCapture::ScreenCaptureProcessorIDXGIOutputDuplication,
	CaptureManager::Sources::ScreenCaptureProcessorFactory> staticInstanceMaker(2);




namespace CaptureManager
{
	namespace Sources
	{
		namespace ScreenCapture
		{

			HRESULT createUncompressedVideoType(
				GUID				 subtype,  // FOURCC or D3DFORMAT value.     
				UINT32               width,
				UINT32               height,
				MFVideoInterlaceMode interlaceMode,
				const MFRatio&       frameRate,
				const MFRatio&       par,
				IMFMediaType         **ppType
				)
			{
				if (ppType == NULL)
				{
					return E_POINTER;
				}

				LONG    lStride = 0;
				UINT    cbImage = 0;

				CComPtrCustom<IMFMediaType> pType;

				HRESULT hr = MediaFoundation::MediaFoundationManager::MFCreateMediaType(&pType);
				if (FAILED(hr))
				{
					goto done;
				}

				hr = pType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
				if (FAILED(hr))
				{
					goto done;
				}

				hr = pType->SetGUID(MF_MT_SUBTYPE, subtype);
				if (FAILED(hr))
				{
					goto done;
				}

				hr = pType->SetUINT32(MF_MT_INTERLACE_MODE, interlaceMode);
				if (FAILED(hr))
				{
					goto done;
				}

				hr = MFSetAttributeSize(pType, MF_MT_FRAME_SIZE, width, height);
				if (FAILED(hr))
				{
					goto done;
				}

				hr = MediaFoundation::MediaFoundationManager::MFGetStrideForBitmapInfoHeader(subtype.Data1, width, &lStride);

				if (SUCCEEDED(hr))
				{
					lStride = -lStride;

					// Calculate the default stride value.
					hr = pType->SetUINT32(MF_MT_DEFAULT_STRIDE, UINT32(lStride));
					if (FAILED(hr))
					{
						goto done;
					}
				}

				// Calculate the image size in bytes.
				hr = MediaFoundation::MediaFoundationManager::MFCalculateImageSize(subtype, width, height, &cbImage);
				if (SUCCEEDED(hr))
				{
					hr = pType->SetUINT32(MF_MT_SAMPLE_SIZE, cbImage);
					if (FAILED(hr))
					{
						goto done;
					}
				}

				hr = pType->SetUINT32(MF_MT_FIXED_SIZE_SAMPLES, TRUE);
				if (FAILED(hr))
				{
					goto done;
				}

				hr = pType->SetUINT32(MF_MT_ALL_SAMPLES_INDEPENDENT, TRUE);
				if (FAILED(hr))
				{
					goto done;
				}

				// Frame rate
				hr = MFSetAttributeRatio(pType, MF_MT_FRAME_RATE, frameRate.Numerator,
					frameRate.Denominator);
				if (FAILED(hr))
				{
					goto done;
				}

				// Pixel aspect ratio
				hr = MFSetAttributeRatio(pType, MF_MT_PIXEL_ASPECT_RATIO, par.Numerator,
					par.Denominator);
				if (FAILED(hr))
				{
					goto done;
				}

				hr = pType->SetUINT32(CM_FLIPPED, TRUE);
				if (FAILED(hr))
				{
					goto done;
				}							   				 			

				// Return the pointer to the caller.

				if (pType)
				{
					hr = pType->QueryInterface(IID_PPV_ARGS(ppType));
				}

			done:
				return hr;
			}



			using namespace Core;
			using namespace Core::DXGI;
			using namespace Core::Direct3D11;
			
			
			// Feature levels supported
			D3D_FEATURE_LEVEL gFeatureLevels[] =
			{
				D3D_FEATURE_LEVEL_11_0
			};

			UINT gNumFeatureLevels = ARRAYSIZE(gFeatureLevels);

			UINT g_Flags = 0;
			
			ScreenCaptureProcessorIDXGIOutputDuplication::ScreenCaptureProcessorIDXGIOutputDuplication()
			{
				if (Singleton<ConfigManager>::getInstance().isWindows10_Or_Greater())
					mIsDirectX11 = true;
				else
					mIsDirectX11 = false;

				mIsFlipStride = TRUE;
			}
			
			ScreenCaptureProcessorIDXGIOutputDuplication::~ScreenCaptureProcessorIDXGIOutputDuplication()
			{
			}

			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::getOutputs(
				std::vector<CComPtrCustom<IDXGIOutput1>> &Outputs)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_INVOKE_FUNCTION(Singleton<DXGIManager>::getInstance().getState);

					CComPtrCustom<IDXGIFactory1> lFactory;

					LOG_INVOKE_DXGI_FUNCTION(CreateDXGIFactory1,
						IID_PPV_ARGS(&lFactory));

					LOG_CHECK_PTR_MEMORY(lFactory);
									
					// find primary display

					CComPtrCustom<IDXGIAdapter1> lAdapter;	

					std::wstring lPrimaryDisplay;

					for (DWORD iDevNum = 0; iDevNum < 24; iDevNum++)
					{
						DISPLAY_DEVICE lDisplayDevice;

						ZeroMemory(&lDisplayDevice, sizeof(DISPLAY_DEVICE));

						lDisplayDevice.cb = sizeof(DISPLAY_DEVICE);

						auto lbRes = EnumDisplayDevices(
							nullptr,
							iDevNum,
							&lDisplayDevice,
							0);
												
						if (lbRes != FALSE && (lDisplayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) != 0)
						{
							lPrimaryDisplay = lDisplayDevice.DeviceName;

							break;
						}
					}

					
					for (UINT lAdapterIndex = 0;
						lFactory->EnumAdapters1(lAdapterIndex, &lAdapter) != DXGI_ERROR_NOT_FOUND;
						++lAdapterIndex)
					{
						LOG_CHECK_PTR_MEMORY(lAdapter);

						DXGI_ADAPTER_DESC1 lDesc;

						lAdapter->GetDesc1(&lDesc);

						CComPtrCustom<IDXGIOutput> lOutput;

						for (UINT lOutputIndex = 0;
							lAdapter->EnumOutputs(lOutputIndex, &lOutput) != DXGI_ERROR_NOT_FOUND;
							++lOutputIndex)
						{
							LOG_CHECK_PTR_MEMORY(lOutput);

							CComPtrCustom<IDXGIOutput1> lOutput1;

							LOG_INVOKE_QUERY_INTERFACE_METHOD(lOutput, &lOutput1);

							DXGI_OUTPUT_DESC lDesc;

							ZeroMemory(&lDesc, sizeof(lDesc));

							lresult = lOutput1->GetDesc(&lDesc);

							if (SUCCEEDED(lresult))
							{
								if (lPrimaryDisplay == lDesc.DeviceName)
								{
									Outputs.push_back(lOutput1);

									break;
								}
							}
							
							lOutput.Release();
						}
						
						lAdapter.Release();

						if (!Outputs.empty())
						{
							break;
						}
					}		
					
					lAdapter.Release();

					for (UINT lAdapterIndex = 0;
						lFactory->EnumAdapters1(lAdapterIndex, &lAdapter) != DXGI_ERROR_NOT_FOUND;
						++lAdapterIndex)
					{
						LOG_CHECK_PTR_MEMORY(lAdapter);

						DXGI_ADAPTER_DESC1 lDesc;

						lAdapter->GetDesc1(&lDesc);

						CComPtrCustom<IDXGIOutput> lOutput;

						for (UINT lOutputIndex = 0;
							lAdapter->EnumOutputs(lOutputIndex, &lOutput) != DXGI_ERROR_NOT_FOUND;
							++lOutputIndex)
						{
							LOG_CHECK_PTR_MEMORY(lOutput);

							CComPtrCustom<IDXGIOutput1> lOutput1;

							LOG_INVOKE_QUERY_INTERFACE_METHOD(lOutput, &lOutput1);
							
							DXGI_OUTPUT_DESC lDesc;

							ZeroMemory(&lDesc, sizeof(lDesc));

							lresult = lOutput1->GetDesc(&lDesc);

							if (SUCCEEDED(lresult))
							{
								if (lPrimaryDisplay != lDesc.DeviceName)
								{
									Outputs.push_back(lOutput1);
								}
							}

							lOutput.Release();
						}

						lAdapter.Release();
					}

				} while (false);

				return lresult;
			}
			
			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::releaseResources()
			{
				HRESULT lresult(S_OK);

				do
				{
					LOG_CHECK_PTR_MEMORY(mIInnerScreenCaptureProcessor);
									   
					if (mUnkVideoSampleAllocator)
					{
						CComQIPtrCustom<IMFVideoSampleAllocatorEx> lVideoSampleAllocatorEx;

						lVideoSampleAllocatorEx = mUnkVideoSampleAllocator;

						if (lVideoSampleAllocatorEx)
							lVideoSampleAllocatorEx->UninitializeSampleAllocator();
					}
					
					mUnkVideoSampleAllocator.Release();
										
					mDevice.Release();
					
					mImmediateContext.Release();

					lresult = mIInnerScreenCaptureProcessor->releaseResources();

				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::initResources(
				ScreenCaptureConfig& aScreenCaptureConfig, 
				IMFMediaType* aPtrOutputMediaType)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(mIInnerScreenCaptureProcessor);

					LOG_CHECK_PTR_MEMORY(mDevice);

					LOG_CHECK_PTR_MEMORY(mImmediateContext);

					mVideoFrameDuration = 1000 / aScreenCaptureConfig.mVideoFPS;

					mIsBlocked = true;

					LOG_INVOKE_FUNCTION(Singleton<DXGIManager>::getInstance().getState);

					LOG_INVOKE_FUNCTION(Singleton<Direct3D11Manager>::getInstance().getState);
					
					LOG_INVOKE_POINTER_METHOD(mIInnerScreenCaptureProcessor, releaseResources);
					
					LOG_INVOKE_POINTER_METHOD(mIInnerScreenCaptureProcessor, initResources,
						mDevice,
						mImmediateContext,
						mIsBlocked ? mVideoFrameDuration : 0,
						mStride);
					
					mScreenRect.top = 0;

					mScreenRect.left = 0;

					mScreenRect.bottom = mDesktopSize.cy;

					mScreenRect.right = mDesktopSize.cx;

					CComPtrCustom<IUnknown> lUnkGDITexture;

					LOG_INVOKE_POINTER_METHOD(mIInnerScreenCaptureProcessor, getGDITexture, &lUnkGDITexture);

					LOG_CHECK_PTR_MEMORY(lUnkGDITexture);

					HDC  lHDC;

					CComPtrCustom<IDXGISurface1> lIDXGISurface1;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkGDITexture, &lIDXGISurface1);

					LOG_INVOKE_POINTER_METHOD(lIDXGISurface1, GetDC, FALSE, &lHDC);

					ScreenCaptureProcessor::initResources(lHDC);

					LOG_INVOKE_POINTER_METHOD(lIDXGISurface1, ReleaseDC, nullptr);


					DXGI_SURFACE_DESC desc;

					LOG_INVOKE_POINTER_METHOD(lIDXGISurface1, GetDesc,
						&desc);


					D3D11_TEXTURE2D_DESC lTempTextureDesc;

					lTempTextureDesc.Width = desc.Width;

					lTempTextureDesc.Height = desc.Height;
					
					if (mClipResource.misEnable)
					{
						mDestImage.Release();

						lTempTextureDesc.Width = mClipResource.mWidth;

						lTempTextureDesc.Height = mClipResource.mHeight;
												
						D3D11_TEXTURE2D_DESC lTextureDesc;

						lTextureDesc.Width = mClipResource.mWidth;

						lTextureDesc.Height = mClipResource.mHeight;

						lTextureDesc.Format = desc.Format;

						lTextureDesc.ArraySize = 1;

						lTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET;

						lTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

						lTextureDesc.SampleDesc.Count = 1;

						lTextureDesc.SampleDesc.Quality = 0;

						lTextureDesc.MipLevels = 1;

						lTextureDesc.CPUAccessFlags = 0;// D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

						lTextureDesc.Usage = D3D11_USAGE_DEFAULT;

						LOG_INVOKE_POINTER_METHOD(mDevice, CreateTexture2D,
							&lTextureDesc, NULL, &mClipImage);
						
						lTextureDesc.BindFlags = 0;

						lTextureDesc.MiscFlags = 0;

						lTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
						lTextureDesc.Usage = D3D11_USAGE_STAGING;

						LOG_INVOKE_POINTER_METHOD(mDevice, CreateTexture2D,
							&lTextureDesc, NULL, &mDestImage);


						// VERTEX shader
						UINT Size = ARRAYSIZE(g_VS);
						lresult = mDevice->CreateVertexShader(g_VS, Size, nullptr, &mVertexShader);

						if (FAILED(lresult))
							break;

						// Input layout
						D3D11_INPUT_ELEMENT_DESC Layout[] =
						{
							{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
							{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
						};
						UINT NumElements = ARRAYSIZE(Layout);

						lresult = mDevice->CreateInputLayout(Layout, NumElements, g_VS, Size, &mInputLayout);

						if (FAILED(lresult))
							break;

						mImmediateContext->IASetInputLayout(mInputLayout);

						// Pixel shader
						Size = ARRAYSIZE(g_PS);

						lresult = mDevice->CreatePixelShader(g_PS, Size, nullptr, &mPixelShader);

						if (FAILED(lresult))
							break;


						// Set up sampler
						D3D11_SAMPLER_DESC SampDesc;
						RtlZeroMemory(&SampDesc, sizeof(SampDesc));
						SampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
						SampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
						SampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
						SampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
						SampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
						SampDesc.MinLOD = 0;
						SampDesc.MaxLOD = D3D11_FLOAT32_MAX;

						lresult = mDevice->CreateSamplerState(&SampDesc, &mSamplerState);

						if (FAILED(lresult))
							break;

						mVertexes[0].Pos.x = -1;
						mVertexes[0].Pos.y = -1;
						mVertexes[0].Pos.z = 0;

						mVertexes[1].Pos.x = -1;
						mVertexes[1].Pos.y = 1;
						mVertexes[1].Pos.z = 0;

						mVertexes[2].Pos.x = 1;
						mVertexes[2].Pos.y = -1;
						mVertexes[2].Pos.z = 0;

						mVertexes[3].Pos.x = 1;
						mVertexes[3].Pos.y = -1;
						mVertexes[3].Pos.z = 0;

						mVertexes[4].Pos.x = -1;
						mVertexes[4].Pos.y = 1;
						mVertexes[4].Pos.z = 0;

						mVertexes[5].Pos.x = 1;
						mVertexes[5].Pos.y = 1;
						mVertexes[5].Pos.z = 0;


						RECT lDesktopCoordinates;

						DXGI_MODE_ROTATION lRotation;

						LOG_INVOKE_POINTER_METHOD(mIInnerScreenCaptureProcessor, getCursorInfo,
							lDesktopCoordinates, 
							lRotation);

						FLOAT lLeftOffset = 0;
						FLOAT lTopOffset = 0;
						FLOAT lRightOffset = 1;
						FLOAT lBottomOffset = 1;

						switch (lRotation)
						{
						case DXGI_MODE_ROTATION_IDENTITY:
						case DXGI_MODE_ROTATION_UNSPECIFIED:
						default:
						{
							// indentity

							lLeftOffset = (FLOAT)mClipResource.mLeft / (FLOAT)mDesktopSize.cx;
							lTopOffset = (FLOAT)mClipResource.mTop / (FLOAT)mDesktopSize.cy;
							lRightOffset = (FLOAT)(mClipResource.mLeft + mClipResource.mWidth) / (FLOAT)mDesktopSize.cx;
							lBottomOffset = (FLOAT)(mClipResource.mTop + mClipResource.mHeight) / (FLOAT)mDesktopSize.cy;



							mVertexes[0].TexCoord.x = lLeftOffset;
							mVertexes[0].TexCoord.y = lBottomOffset;

							mVertexes[1].TexCoord.x = lLeftOffset;
							mVertexes[1].TexCoord.y = lTopOffset;

							mVertexes[2].TexCoord.x = lRightOffset;
							mVertexes[2].TexCoord.y = lBottomOffset;

							mVertexes[3].TexCoord.x = lRightOffset;
							mVertexes[3].TexCoord.y = lBottomOffset;

							mVertexes[4].TexCoord.x = lLeftOffset;
							mVertexes[4].TexCoord.y = lTopOffset;

							mVertexes[5].TexCoord.x = lRightOffset;
							mVertexes[5].TexCoord.y = lTopOffset;
						}
							break;
						case DXGI_MODE_ROTATION_ROTATE90:
						{

							lLeftOffset = (FLOAT)mClipResource.mTop / (FLOAT)mDesktopSize.cx;
							lTopOffset = (FLOAT)(mDesktopSize.cy - mClipResource.mLeft) / (FLOAT)mDesktopSize.cy;
							lRightOffset = lLeftOffset + (FLOAT)(mClipResource.mHeight) / (FLOAT)mDesktopSize.cx;
							lBottomOffset = lTopOffset - (FLOAT)(mClipResource.mWidth) / (FLOAT)mDesktopSize.cy;



							mVertexes[0].TexCoord.x = lRightOffset;
							mVertexes[0].TexCoord.y = lTopOffset;

							mVertexes[1].TexCoord.x = lLeftOffset;
							mVertexes[1].TexCoord.y = lTopOffset;

							mVertexes[2].TexCoord.x = lRightOffset;
							mVertexes[2].TexCoord.y = lBottomOffset;

							mVertexes[3].TexCoord.x = lRightOffset;
							mVertexes[3].TexCoord.y = lBottomOffset;

							mVertexes[4].TexCoord.x = lLeftOffset;
							mVertexes[4].TexCoord.y = lTopOffset;

							mVertexes[5].TexCoord.x = lLeftOffset;
							mVertexes[5].TexCoord.y = lBottomOffset;
							
						}
							break;
						case DXGI_MODE_ROTATION_ROTATE180:
						{
							// indentity

							lLeftOffset = (FLOAT)(mDesktopSize.cx - mClipResource.mLeft) / (FLOAT)mDesktopSize.cx;
							lTopOffset = (FLOAT)(mDesktopSize.cy - mClipResource.mTop) / (FLOAT)mDesktopSize.cy;
							lRightOffset = lLeftOffset - (FLOAT)(mClipResource.mWidth) / (FLOAT)mDesktopSize.cx;
							lBottomOffset = lTopOffset - (FLOAT)(mClipResource.mHeight) / (FLOAT)mDesktopSize.cy;



							mVertexes[0].TexCoord.x = lLeftOffset;
							mVertexes[0].TexCoord.y = lBottomOffset;

							mVertexes[1].TexCoord.x = lLeftOffset;
							mVertexes[1].TexCoord.y = lTopOffset;

							mVertexes[2].TexCoord.x = lRightOffset;
							mVertexes[2].TexCoord.y = lBottomOffset;

							mVertexes[3].TexCoord.x = lRightOffset;
							mVertexes[3].TexCoord.y = lBottomOffset;

							mVertexes[4].TexCoord.x = lLeftOffset;
							mVertexes[4].TexCoord.y = lTopOffset;

							mVertexes[5].TexCoord.x = lRightOffset;
							mVertexes[5].TexCoord.y = lTopOffset;
						}
							break;
						case DXGI_MODE_ROTATION_ROTATE270:
						{

							lLeftOffset = (FLOAT)(mDesktopSize.cx - mClipResource.mTop) / (FLOAT)mDesktopSize.cx;
							lTopOffset = (FLOAT)(mClipResource.mLeft) / (FLOAT)mDesktopSize.cy;
							lRightOffset = lLeftOffset - (FLOAT)(mClipResource.mHeight) / (FLOAT)mDesktopSize.cx;
							lBottomOffset = lTopOffset + (FLOAT)(mClipResource.mWidth) / (FLOAT)mDesktopSize.cy;



							mVertexes[0].TexCoord.x = lRightOffset;
							mVertexes[0].TexCoord.y = lTopOffset;

							mVertexes[1].TexCoord.x = lLeftOffset;
							mVertexes[1].TexCoord.y = lTopOffset;

							mVertexes[2].TexCoord.x = lRightOffset;
							mVertexes[2].TexCoord.y = lBottomOffset;

							mVertexes[3].TexCoord.x = lRightOffset;
							mVertexes[3].TexCoord.y = lBottomOffset;

							mVertexes[4].TexCoord.x = lLeftOffset;
							mVertexes[4].TexCoord.y = lTopOffset;

							mVertexes[5].TexCoord.x = lLeftOffset;
							mVertexes[5].TexCoord.y = lBottomOffset;

						}
							break;
						}
					}
					
					CComPtrCustom<IMFDXGIDeviceManager> lDeviceManager;

					LOG_INVOKE_FUNCTION(createOrGetDeviceManager, IID_PPV_ARGS(&lDeviceManager));

					LOG_CHECK_PTR_MEMORY(lDeviceManager);
					
					if (mUnkVideoSampleAllocator)
					{
						CComQIPtrCustom<IMFVideoSampleAllocatorEx> lVideoSampleAllocatorEx;

						lVideoSampleAllocatorEx = mUnkVideoSampleAllocator;

						if (lVideoSampleAllocatorEx)
							lVideoSampleAllocatorEx->UninitializeSampleAllocator();
					}
										
					if (lDeviceManager)
					{
						CComPtrCustom<IMFVideoSampleAllocatorEx> lVideoSampleAllocatorEx;

						LOG_INVOKE_MF_FUNCTION(MFCreateVideoSampleAllocatorEx,
							IID_PPV_ARGS(&lVideoSampleAllocatorEx));

						if (lVideoSampleAllocatorEx)
						{
							if (lDeviceManager)
								lVideoSampleAllocatorEx->SetDirectXManager(lDeviceManager);

							mUnkVideoSampleAllocator.Release();


							CComPtrCustom<IMFAttributes> inputAttr;

							LOG_INVOKE_MF_FUNCTION(MFCreateAttributes,
								&inputAttr, 4);

							LOG_INVOKE_MF_METHOD(SetUINT32,
								inputAttr,
								MF_SA_BUFFERS_PER_SAMPLE, 1);
							LOG_INVOKE_MF_METHOD(SetUINT32,
								inputAttr,
								MF_SA_D3D11_USAGE,
								D3D11_USAGE_DEFAULT);
							LOG_INVOKE_MF_METHOD(SetUINT32,
								inputAttr,
								MF_SA_D3D11_BINDFLAGS,
								D3D11_BIND_RENDER_TARGET
								);
														

							MFRatio lFrameRate;

							lFrameRate.Numerator = 30;
							lFrameRate.Denominator = 1;

							MFRatio lAspectRatio;

							lAspectRatio.Numerator = 1;

							lAspectRatio.Denominator = 1;


							CComPtrCustom<IMFMediaType> lOutputMediaType;

							lOutputMediaType = aPtrOutputMediaType;


							GUID lSubType;

							LOG_INVOKE_MF_METHOD(GetGUID,
								aPtrOutputMediaType,
								MF_MT_SUBTYPE,
								&lSubType);
							
							LOG_INVOKE_FUNCTION(createUncompressedVideoType,
								lSubType,
								aScreenCaptureConfig.mBitMapInfo.bmiHeader.biWidth,
								aScreenCaptureConfig.mBitMapInfo.bmiHeader.biHeight,
								MFVideoInterlaceMode::MFVideoInterlace_Progressive,
								lFrameRate,
								lAspectRatio,
								&lOutputMediaType);
													   
							LOG_INVOKE_MF_METHOD(InitializeSampleAllocatorEx,
								lVideoSampleAllocatorEx,
								30,
								30,
								inputAttr.get(),
								lOutputMediaType
								);

							LOG_INVOKE_QUERY_INTERFACE_METHOD(lVideoSampleAllocatorEx, &mUnkVideoSampleAllocator);
						}

						LOG_CHECK_PTR_MEMORY(mUnkVideoSampleAllocator);
					}	

				} while (false);

				return lresult;
			}
			
			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::drawClip(ID3D11Texture2D* aPtrID3D11Texture2D, BYTE* aPtrData)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrID3D11Texture2D);

					D3D11_TEXTURE2D_DESC FullDesc;
					aPtrID3D11Texture2D->GetDesc(&FullDesc);

					CComPtrCustom<ID3D11RenderTargetView> m_RTV;

					lresult = mDevice->CreateRenderTargetView(mClipImage, nullptr, &m_RTV);

					if (FAILED(lresult))
						break;


					D3D11_SHADER_RESOURCE_VIEW_DESC ShaderDesc;
					ShaderDesc.Format = FullDesc.Format;
					ShaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					ShaderDesc.Texture2D.MostDetailedMip = FullDesc.MipLevels - 1;
					ShaderDesc.Texture2D.MipLevels = FullDesc.MipLevels;

					// Create new shader resource view
					CComPtrCustom<ID3D11ShaderResourceView> ShaderResource = nullptr;
					lresult = mDevice->CreateShaderResourceView(aPtrID3D11Texture2D, &ShaderDesc, &ShaderResource);

					if (FAILED(lresult))
						break;

					FLOAT BlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
					mImmediateContext->OMSetBlendState(nullptr, BlendFactor, 0xFFFFFFFF);
					mImmediateContext->OMSetRenderTargets(1, &m_RTV, nullptr);
					mImmediateContext->VSSetShader(mVertexShader, nullptr, 0);
					mImmediateContext->PSSetShader(mPixelShader, nullptr, 0);
					mImmediateContext->PSSetShaderResources(0, 1, &ShaderResource);
					mImmediateContext->PSSetSamplers(0, 1, &mSamplerState);
					mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

					UINT DirtyCount(1);

					// Create space for vertices for the dirty rects if the current space isn't large enough
					UINT BytesNeeded = sizeof(VERTEX) * NUMVERTICES * DirtyCount;


					// Create vertex buffer
					D3D11_BUFFER_DESC BufferDesc;
					RtlZeroMemory(&BufferDesc, sizeof(BufferDesc));
					BufferDesc.Usage = D3D11_USAGE_DEFAULT;
					BufferDesc.ByteWidth = BytesNeeded;
					BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					BufferDesc.CPUAccessFlags = 0;
					D3D11_SUBRESOURCE_DATA InitData;
					RtlZeroMemory(&InitData, sizeof(InitData));
					InitData.pSysMem = mVertexes;

					CComPtrCustom<ID3D11Buffer> VertBuf;
					lresult = mDevice->CreateBuffer(&BufferDesc, &InitData, &VertBuf);

					if (FAILED(lresult))
						break;

					UINT Stride = sizeof(VERTEX);
					UINT Offset = 0;
					mImmediateContext->IASetVertexBuffers(0, 1, &VertBuf, &Stride, &Offset);

					D3D11_VIEWPORT VP;

					VP.Width = static_cast<FLOAT>(mClipResource.mWidth);
					VP.Height = static_cast<FLOAT>(mClipResource.mHeight);

					VP.MinDepth = 0.0f;
					VP.MaxDepth = 1.0f;
					VP.TopLeftX = 0.0f;
					VP.TopLeftY = 0.0f;
					mImmediateContext->RSSetViewports(1, &VP);

					mImmediateContext->Draw(NUMVERTICES * DirtyCount, 0);



					mImmediateContext->CopyResource(mDestImage, mClipImage);

					D3D11_MAPPED_SUBRESOURCE resource;
					UINT subresource = D3D11CalcSubresource(0, 0, 0);
					lresult = mImmediateContext->Map(mDestImage, subresource, D3D11_MAP_READ_WRITE, 0, &resource);
					
					if (SUCCEEDED(lresult))
					{
						aPtrData += (mClipResource.mHeight - 1)*mStride;

						LOG_INVOKE_MF_FUNCTION(MFCopyImage,
							aPtrData,
							-mStride,
							(BYTE*)resource.pData,
							resource.RowPitch,
							mStride,
							mClipResource.mHeight);
					}

					mImmediateContext->Unmap(mDestImage, subresource);

				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::drawClip(
				ID3D11Texture2D* aPtrID3D11Texture2D, 
				ID3D11Texture2D* aPtrDestID3D11Texture2D)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrID3D11Texture2D);

					LOG_CHECK_PTR_MEMORY(aPtrDestID3D11Texture2D);

					D3D11_TEXTURE2D_DESC FullDesc;
					aPtrID3D11Texture2D->GetDesc(&FullDesc);

					CComPtrCustom<ID3D11RenderTargetView> m_RTV;

					lresult = mDevice->CreateRenderTargetView(mClipImage, nullptr, &m_RTV);

					if (FAILED(lresult))
						break;


					D3D11_SHADER_RESOURCE_VIEW_DESC ShaderDesc;
					ShaderDesc.Format = FullDesc.Format;
					ShaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					ShaderDesc.Texture2D.MostDetailedMip = FullDesc.MipLevels - 1;
					ShaderDesc.Texture2D.MipLevels = FullDesc.MipLevels;

					// Create new shader resource view
					CComPtrCustom<ID3D11ShaderResourceView> ShaderResource = nullptr;
					lresult = mDevice->CreateShaderResourceView(aPtrID3D11Texture2D, &ShaderDesc, &ShaderResource);

					if (FAILED(lresult))
						break;

					FLOAT BlendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
					mImmediateContext->OMSetBlendState(nullptr, BlendFactor, 0xFFFFFFFF);
					mImmediateContext->OMSetRenderTargets(1, &m_RTV, nullptr);
					mImmediateContext->VSSetShader(mVertexShader, nullptr, 0);
					mImmediateContext->PSSetShader(mPixelShader, nullptr, 0);
					mImmediateContext->PSSetShaderResources(0, 1, &ShaderResource);
					mImmediateContext->PSSetSamplers(0, 1, &mSamplerState);
					mImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

					UINT DirtyCount(1);

					// Create space for vertices for the dirty rects if the current space isn't large enough
					UINT BytesNeeded = sizeof(VERTEX) * NUMVERTICES * DirtyCount;


					// Create vertex buffer
					D3D11_BUFFER_DESC BufferDesc;
					RtlZeroMemory(&BufferDesc, sizeof(BufferDesc));
					BufferDesc.Usage = D3D11_USAGE_DEFAULT;
					BufferDesc.ByteWidth = BytesNeeded;
					BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
					BufferDesc.CPUAccessFlags = 0;
					D3D11_SUBRESOURCE_DATA InitData;
					RtlZeroMemory(&InitData, sizeof(InitData));
					InitData.pSysMem = mVertexes;

					CComPtrCustom<ID3D11Buffer> VertBuf;
					lresult = mDevice->CreateBuffer(&BufferDesc, &InitData, &VertBuf);

					if (FAILED(lresult))
						break;

					UINT Stride = sizeof(VERTEX);
					UINT Offset = 0;
					mImmediateContext->IASetVertexBuffers(0, 1, &VertBuf, &Stride, &Offset);

					D3D11_VIEWPORT VP;

					VP.Width = static_cast<FLOAT>(mClipResource.mWidth);
					VP.Height = static_cast<FLOAT>(mClipResource.mHeight);

					VP.MinDepth = 0.0f;
					VP.MaxDepth = 1.0f;
					VP.TopLeftX = 0.0f;
					VP.TopLeftY = 0.0f;
					mImmediateContext->RSSetViewports(1, &VP);

					mImmediateContext->Draw(NUMVERTICES * DirtyCount, 0);
									   
					mImmediateContext->CopyResource(aPtrDestID3D11Texture2D, mClipImage);
					
				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::createOrGetDeviceManager(
				REFIID aRefIID,
				LPVOID* aPtrPtrObject)
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrObject);

					if (!mUnkDeviceManager)
					{
						CComPtrCustom<IMFDXGIDeviceManager> lDeviceManager;

						LOG_INVOKE_MF_FUNCTION(MFCreateDXGIDeviceManager,
							&mDeviceResetToken,
							&lDeviceManager);

						LOG_CHECK_PTR_MEMORY(lDeviceManager);

						LOG_INVOKE_POINTER_METHOD(lDeviceManager,
							ResetDevice,
							mDevice,
							mDeviceResetToken);

						LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(lDeviceManager,
							aRefIID,
							aPtrPtrObject);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lDeviceManager, &mUnkDeviceManager);
					}
					else
					{
						LOG_INVOKE_WIDE_QUERY_INTERFACE_METHOD(mUnkDeviceManager,
							aRefIID,
							aPtrPtrObject);
					}

				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::GetService(
				REFGUID aRefGUIDService,
				REFIID aRefIID,
				LPVOID* aPtrPtrObject)
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{
					if (!mIsDirectX11)
						break;

					LOG_CHECK_PTR_MEMORY(aPtrPtrObject);

					if (aRefGUIDService == CM_DeviceManager)
					{
						if (aRefIID == __uuidof(IMFDXGIDeviceManager))
						{
							LOG_INVOKE_FUNCTION(createOrGetDeviceManager,
								aRefIID,
								aPtrPtrObject);
						}
					}

				} while (false);

				return lresult;
			}
			
			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::isDefaultAdapter(LUID aAdapterLuid)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					D3D_FEATURE_LEVEL FeatureLevel;

					CComPtrCustom<ID3D11Device> lDevice;

					CComPtrCustom<ID3D11DeviceContext> lImmediateContext;

					LOG_INVOKE_DX11_FUNCTION(D3D11CreateDevice,
						nullptr,
						D3D_DRIVER_TYPE_HARDWARE,
						nullptr,
						0,
						gFeatureLevels,
						gNumFeatureLevels,
						D3D11_SDK_VERSION,
						&lDevice,
						&FeatureLevel,
						&lImmediateContext);

					LOG_CHECK_PTR_MEMORY(lDevice);

					CComPtrCustom<IDXGIDevice> lDXGIDevice;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lDevice, &lDXGIDevice);

					CComPtrCustom<IDXGIAdapter1> lDxgiAdapter;

					LOG_INVOKE_POINTER_METHOD(lDXGIDevice, GetParent, IID_PPV_ARGS(&lDxgiAdapter));

					DXGI_ADAPTER_DESC1 lDefaultDesc;

					LOG_INVOKE_POINTER_METHOD(lDxgiAdapter, GetDesc1, &lDefaultDesc);

					if (lDefaultDesc.AdapterLuid.HighPart == aAdapterLuid.HighPart &&
						lDefaultDesc.AdapterLuid.LowPart == aAdapterLuid.LowPart)
						lresult = S_OK;
					else
						lresult = S_FALSE;

				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::init(
				std::wstring aDeviceName,
				UINT aOrientation,
				IDXGIOutput1* aPtrOutput)
			{
				using namespace Sinks::EVR::Mixer;

				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrOutput);

					CComPtrCustom<IDXGIOutput1> lOutput;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrOutput, &lOutput);

					LOG_CHECK_PTR_MEMORY(lOutput);

					CComPtrCustom<IInnerScreenCaptureProcessor> lIInnerScreenCaptureProcessor;

					InnerScreenCaptureProcessorFactory::create(
						(InnerScreenCaptureProcessorFactory::Type) aOrientation,
						lOutput,
						&lIInnerScreenCaptureProcessor);

					LOG_CHECK_PTR_MEMORY(lIInnerScreenCaptureProcessor);

					mIInnerScreenCaptureProcessor = lIInnerScreenCaptureProcessor;

					LOG_CHECK_PTR_MEMORY(mIInnerScreenCaptureProcessor);

					mOutputName = aDeviceName;
					
					CComPtrCustom<IDXGIAdapter1> lDxgiAdapter;

					HRESULT lresultSecond = lOutput->GetParent(IID_PPV_ARGS(&lDxgiAdapter));

					if (SUCCEEDED(lresultSecond))
					{
						DXGI_ADAPTER_DESC1 lDesc2;

						if (lDxgiAdapter)
						{
							lresultSecond = lDxgiAdapter->GetDesc1(&lDesc2);

							if (SUCCEEDED(lresultSecond))
								mDeviceName = std::wstring(lDesc2.Description);
						}
					}
					
					fillVectorScreenCaptureConfigs(mVectorScreenCaptureConfigs);
					
					LOG_CHECK_PTR_MEMORY(lDxgiAdapter);

					D3D_FEATURE_LEVEL FeatureLevel;

					DXGI_ADAPTER_DESC1 lSourceDesc;

					LOG_INVOKE_POINTER_METHOD(lDxgiAdapter, GetDesc1, &lSourceDesc);

					lresult = isDefaultAdapter(lSourceDesc.AdapterLuid);

					// Create device

					if (lresult == S_OK)
					{
						do
						{
							LOG_INVOKE_DX11_FUNCTION(D3D11CreateDevice,
								nullptr,
								D3D_DRIVER_TYPE_HARDWARE,
								nullptr,
								g_Flags,
								gFeatureLevels,
								gNumFeatureLevels,
								D3D11_SDK_VERSION,
								&mDevice,
								&FeatureLevel,
								&mImmediateContext);

							LOG_CHECK_PTR_MEMORY(mDevice);						
							
						} while (false);

					}

					if (lresult != S_OK)
						LOG_INVOKE_DX11_FUNCTION(D3D11CreateDevice,
						lDxgiAdapter,
						D3D_DRIVER_TYPE_UNKNOWN,
						nullptr,
						g_Flags,
						gFeatureLevels,
						gNumFeatureLevels,
						D3D11_SDK_VERSION,
						&mDevice,
						&FeatureLevel,
						&mImmediateContext);

					LOG_CHECK_PTR_MEMORY(mDevice);

					LOG_CHECK_PTR_MEMORY(mImmediateContext);
					
					LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);
									   					 
					do
					{
						CComPtrCustom<ID3D10Multithread> lMultiThread;

						// Need to explitly set the multithreaded mode for this device
						LOG_INVOKE_QUERY_INTERFACE_METHOD(mImmediateContext, &lMultiThread);

						LOG_CHECK_PTR_MEMORY(lMultiThread);

						BOOL lbRrsult = lMultiThread->SetMultithreadProtected(TRUE);

					} while (false);
					
					CComPtrCustom<IDXGIOutputDuplication> lDeskDupl;

					LOG_INVOKE_POINTER_METHOD(lOutput, DuplicateOutput,
						mDevice,
						&lDeskDupl);

					LOG_CHECK_PTR_MEMORY(lDeskDupl);

					DXGI_OUTDUPL_DESC lDesc;

					lDeskDupl->GetDesc(&lDesc);

					lDeskDupl.Release();
										
				} while (false);

				return lresult;
			}

			std::wstring ScreenCaptureProcessorIDXGIOutputDuplication::getSymbolicLink()
			{
				if (mOutputName.empty())
					return ScreenCaptureProcessor::getSymbolicLink();
				else
					return ScreenCaptureProcessor::getSymbolicLink() + mOutputName;
			}

			std::wstring ScreenCaptureProcessorIDXGIOutputDuplication::getFrendlyName()
			{
				if (mOutputName.empty())
				{					
					return ScreenCaptureProcessor::getFrendlyName();
				}
				else
				{
					auto lpos = mOutputName.find(L"D");

					std::wstring llocalDeviceName = mOutputName;

					if (lpos != std::wstring::npos)
					{
						llocalDeviceName = mOutputName.substr(lpos, mOutputName.size());

					}

					if (!mDeviceName.empty())
					{
						llocalDeviceName += L" - " + mDeviceName;
					}

					return ScreenCaptureProcessor::getFrendlyName() + L" (" + llocalDeviceName + L")";
				}
			}

			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::grabImage(BYTE* aPtrData)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(mIInnerScreenCaptureProcessor);
										
					LOG_INVOKE_POINTER_METHOD(mIInnerScreenCaptureProcessor, updateFrame);

					CComPtrCustom<IUnknown> lUnkGDITexture;

					LOG_INVOKE_POINTER_METHOD(mIInnerScreenCaptureProcessor, getGDITexture, &lUnkGDITexture);

					RECT lDesktopCoordinates;
					
					DXGI_MODE_ROTATION lRotation;

					mIInnerScreenCaptureProcessor->getCursorInfo(lDesktopCoordinates, lRotation);

					CComPtrCustom<IDXGISurface1> lIDXGISurface1;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkGDITexture, &lIDXGISurface1);

					HDC  lHDC;

					LOG_INVOKE_POINTER_METHOD(lIDXGISurface1, GetDC, FALSE, &lHDC);

					drawOn(lDesktopCoordinates, lHDC, lRotation);

					LOG_INVOKE_POINTER_METHOD(lIDXGISurface1, ReleaseDC, nullptr);

					if (mClipResource.misEnable)
					{
						CComPtrCustom<ID3D11Texture2D> lFullImage;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lIDXGISurface1, &lFullImage);

						LOG_CHECK_PTR_MEMORY(lFullImage);

						drawClip(lFullImage, aPtrData);
					}
					else
					{
						LOG_INVOKE_POINTER_METHOD(mIInnerScreenCaptureProcessor, readFromTexture, aPtrData);
					}		

				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::grabImage(IUnknown** aPtrPtrUnkSample)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					CComPtrCustom<ID3D11Texture2D> lDestSurface;

					CComPtrCustom<IMFSample> lAllocatedSample;

					LOG_CHECK_PTR_MEMORY(mUnkVideoSampleAllocator);

					CComQIPtrCustom<IMFVideoSampleAllocatorEx> lVideoSampleAllocatorEx;

					lVideoSampleAllocatorEx = mUnkVideoSampleAllocator;

					LOG_CHECK_PTR_MEMORY(lVideoSampleAllocatorEx);

					LOG_INVOKE_POINTER_METHOD(lVideoSampleAllocatorEx, AllocateSample, &lAllocatedSample);

					LOG_CHECK_PTR_MEMORY(lAllocatedSample);
					

					CComPtrCustom<IMFMediaBuffer> lInputBuffer;

					LOG_INVOKE_MF_METHOD(GetBufferByIndex, lAllocatedSample, 0, &lInputBuffer);




					CComPtrCustom<IMFSample> lSample;

					LOG_INVOKE_MF_FUNCTION(MFCreateSample, &lSample);

					LOG_CHECK_PTR_MEMORY(lSample);

					LOG_INVOKE_MF_METHOD(AddBuffer, lSample, lInputBuffer);

					CComPtrCustom<IMFDXGIBuffer> lIMFDXGIBuffer;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lInputBuffer, &lIMFDXGIBuffer);

					LOG_CHECK_PTR_MEMORY(lIMFDXGIBuffer);

					LOG_INVOKE_DXGI_METHOD(GetResource, lIMFDXGIBuffer,
						IID_PPV_ARGS(&lDestSurface));

					LOG_CHECK_PTR_MEMORY(lDestSurface);					

					LOG_CHECK_PTR_MEMORY(mIInnerScreenCaptureProcessor);

					LOG_INVOKE_POINTER_METHOD(mIInnerScreenCaptureProcessor, updateFrame);

					CComPtrCustom<IUnknown> lUnkGDITexture;

					LOG_INVOKE_POINTER_METHOD(mIInnerScreenCaptureProcessor, getGDITexture, &lUnkGDITexture);

					RECT lDesktopCoordinates;

					DXGI_MODE_ROTATION lRotation;

					mIInnerScreenCaptureProcessor->getCursorInfo(lDesktopCoordinates, lRotation);

					CComPtrCustom<IDXGISurface1> lIDXGISurface1;

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lUnkGDITexture, &lIDXGISurface1);

					mDevice->GetExceptionMode();

					HDC  lHDC;

					LOG_INVOKE_POINTER_METHOD(lIDXGISurface1, GetDC, FALSE, &lHDC);

					drawOn(lDesktopCoordinates, lHDC, lRotation);

					LOG_INVOKE_POINTER_METHOD(lIDXGISurface1, ReleaseDC, nullptr);


					
					if (mClipResource.misEnable)
					{
						CComPtrCustom<ID3D11Texture2D> lFullImage;

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lIDXGISurface1, &lFullImage);

						LOG_CHECK_PTR_MEMORY(lFullImage);

						drawClip(lFullImage, lDestSurface);
					}
					else
					{
						LOG_INVOKE_POINTER_METHOD(mIInnerScreenCaptureProcessor, readFromTexture, lDestSurface);
					}

					LOG_INVOKE_QUERY_INTERFACE_METHOD(lSample, aPtrPtrUnkSample);
					
				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::execute()
			{
				HRESULT lresult(E_NOTIMPL);

				do
				{

				} while (false);

				return lresult;
			}
			
			
			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::check()
			{
				HRESULT lresult(E_FAIL);

				do
				{
				//	if (IsWindows8Point1OrGreater())
					{
						LOG_INVOKE_FUNCTION(Singleton<DXGIManager>::getInstance().getState);


						LOG_INVOKE_FUNCTION(Singleton<DXGIManager>::getInstance().getState);

						LOG_INVOKE_FUNCTION(Singleton<Direct3D11Manager>::getInstance().getState);
						
						std::vector<CComPtrCustom<IDXGIOutput1>> lOutputs;

						LOG_INVOKE_FUNCTION(getOutputs,
							lOutputs);
						
						LOG_CHECK_STATE(lOutputs.empty());

						for (auto& lOutputItem : lOutputs)
						{
							do
							{								
								CComPtrCustom<IDXGIAdapter1> lDxgiAdapter;

								LOG_INVOKE_POINTER_METHOD(lOutputItem, GetParent,
									IID_PPV_ARGS(&lDxgiAdapter));

								DXGI_OUTPUT_DESC kl;

								lOutputItem->GetDesc(&kl);

								DXGI_ADAPTER_DESC1 lDesc;

								lDxgiAdapter->GetDesc1(&lDesc);
								
								D3D_FEATURE_LEVEL FeatureLevel;

								CComPtrCustom<ID3D11Device> lDevice;

								CComPtrCustom<ID3D11DeviceContext> lImmediateContext;
																
								// Create device

								LOG_INVOKE_DX11_FUNCTION(D3D11CreateDevice,
									lDxgiAdapter,
									D3D_DRIVER_TYPE_UNKNOWN,
									nullptr,
									0,
									gFeatureLevels,
									gNumFeatureLevels,
									D3D11_SDK_VERSION,
									&lDevice,
									&FeatureLevel,
									&lImmediateContext);

								LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);

								LOG_CHECK_PTR_MEMORY(lDevice);
								
								CComPtrCustom<IDXGIOutputDuplication> lDeskDupl;
								
								// Create desktop duplication

								LOG_INVOKE_POINTER_METHOD(lOutputItem, DuplicateOutput,
									lDevice,
									&lDeskDupl);

							} while (false);

							lresult = S_OK;
						}											
					}

				} while (false);

				return lresult;
			}
			
			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::fillVectorScreenCaptureConfigs(
				std::vector<ScreenCaptureConfig>& aRefVectorScreenCaptureConfigs)
			{
				HRESULT lresult;

				UINT32 lSizeImage = 0;

				do
				{
					LOG_CHECK_PTR_MEMORY(mIInnerScreenCaptureProcessor);

					SIZE lSize;

					LOG_INVOKE_POINTER_METHOD(mIInnerScreenCaptureProcessor, getSize, lSize);

					mDesktopSize = lSize;

					int lWidth = lSize.cx;

					int lHeight = lSize.cy;

					aRefVectorScreenCaptureConfigs.clear();

					if (mClipResource.misEnable)
					{
						lWidth = mClipResource.mWidth;

						lHeight = mClipResource.mHeight;
					}

					if (mResizeResource.misEnable)
					{
						lWidth = (lWidth * mResizeResource.mWidth) / 100;

						lHeight = (lHeight * mResizeResource.mHeight) / 100;
					}

					BITMAPINFO	lBmpInfo;

					// BMP 32 bpp

					ZeroMemory(&lBmpInfo, sizeof(BITMAPINFO));

					lBmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

					lBmpInfo.bmiHeader.biBitCount = 32;

					lBmpInfo.bmiHeader.biCompression = BI_RGB;

					lBmpInfo.bmiHeader.biWidth = lWidth;

					lBmpInfo.bmiHeader.biHeight = lHeight;

					lBmpInfo.bmiHeader.biPlanes = 1;


					LOG_INVOKE_MF_FUNCTION(MFCalculateImageSize,
						MFVideoFormat_RGB32,
						lBmpInfo.bmiHeader.biWidth,
						lBmpInfo.bmiHeader.biHeight,
						&lSizeImage);

					lBmpInfo.bmiHeader.biSizeImage = lSizeImage;



					ScreenCaptureConfig lScreenCaptureConfig;

					lScreenCaptureConfig.mBitMapInfo = lBmpInfo;


					// 1 fps

					lScreenCaptureConfig.mVideoFPS = 1;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 5 fps

					lScreenCaptureConfig.mVideoFPS = 5;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 10 fps

					lScreenCaptureConfig.mVideoFPS = 10;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 15 fps

					lScreenCaptureConfig.mVideoFPS = 15;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 20 fps 

					lScreenCaptureConfig.mVideoFPS = 20;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 25 fps 

					lScreenCaptureConfig.mVideoFPS = 25;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


					// 30 fps 

					lScreenCaptureConfig.mVideoFPS = 30;

					aRefVectorScreenCaptureConfigs.push_back(lScreenCaptureConfig);


				} while (false);

				return lresult;
			}


			// ScreenCaptureProcessorIDXGIOutputDuplication implementations

			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::createVideoMediaType(
				ScreenCaptureConfig aScreenCaptureConfig,
				IMFMediaType** aPtrPtrMediaType)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					if (!mIsDirectX11)
						return ScreenCaptureProcessor::createVideoMediaType(aScreenCaptureConfig, aPtrPtrMediaType);


					MFRatio lFrameRate;

					MFRatio lAspectRatio;

					lAspectRatio.Numerator = 1;

					lAspectRatio.Denominator = 1;



					lFrameRate.Numerator = aScreenCaptureConfig.mVideoFPS;

					lFrameRate.Denominator = 1;



					CComPtrCustom<IMFMediaType> lOutputMediaType;

					CComPtrCustom<IMFMediaType> lInputMediaType;


					if (Singleton<ConfigManager>::getInstance().isWindows8Point1_Or_Greater())
					{
						LOG_INVOKE_FUNCTION(createUncompressedVideoType,
							MFVideoFormat_ARGB32,
							aScreenCaptureConfig.mBitMapInfo.bmiHeader.biWidth,
							aScreenCaptureConfig.mBitMapInfo.bmiHeader.biHeight,
							MFVideoInterlaceMode::MFVideoInterlace_Progressive,
							lFrameRate,
							lAspectRatio,
							&lInputMediaType);
					}
					else
					{
						LOG_INVOKE_FUNCTION(createUncompressedVideoType,
							MFVideoFormat_RGB32,
							aScreenCaptureConfig.mBitMapInfo.bmiHeader.biWidth,
							aScreenCaptureConfig.mBitMapInfo.bmiHeader.biHeight,
							MFVideoInterlaceMode::MFVideoInterlace_Progressive,
							lFrameRate,
							lAspectRatio,
							&lInputMediaType);
					}									

					lOutputMediaType = lInputMediaType;
					
					CComPtrCustom<IMFMediaType> lVideoMediaType;

					if (FAILED(lresult))
					{
						lVideoMediaType = lInputMediaType;
					}
					else
					{
						lVideoMediaType = lOutputMediaType;
					}

					
					// result - out

					LOG_INVOKE_FUNCTION(MFSetAttributeRatio,
						lVideoMediaType,
						MF_MT_FRAME_RATE_RANGE_MAX,
						aScreenCaptureConfig.mVideoFPS,
						1);

					LOG_INVOKE_FUNCTION(MFSetAttributeRatio,
						lVideoMediaType,
						MF_MT_FRAME_RATE_RANGE_MIN,
						aScreenCaptureConfig.mVideoFPS,
						1);

					*aPtrPtrMediaType = lVideoMediaType.detach();

				} while (false);

				return lresult;
			}

			HRESULT ScreenCaptureProcessorIDXGIOutputDuplication::enumIInnerCaptureProcessor(
				UINT aIndex,
				UINT aOrientation,
				IInnerCaptureProcessor** aPtrPtrIInnerCaptureProcessor)
			{
				HRESULT lresult = E_FAIL;

				do
				{
					LOG_INVOKE_FUNCTION(Singleton<DXGIManager>::getInstance().getState);

					std::vector<CComPtrCustom<IDXGIOutput1>> lOutputs;

					LOG_INVOKE_FUNCTION(getOutputs,
						lOutputs);

					LOG_CHECK_STATE(lOutputs.empty());

					LOG_CHECK_STATE(aIndex >= lOutputs.size());
					
					auto lSelectedOutput = lOutputs.at(aIndex);

					LOG_CHECK_PTR_MEMORY(lSelectedOutput);
					
					do
					{
						CComPtrCustom<IDXGIAdapter1> lDxgiAdapter;

						LOG_INVOKE_POINTER_METHOD(lSelectedOutput, GetParent,
							IID_PPV_ARGS(&lDxgiAdapter));

						DXGI_OUTPUT_DESC kl;

						lSelectedOutput->GetDesc(&kl);

						DXGI_ADAPTER_DESC1 lDesc;

						lDxgiAdapter->GetDesc1(&lDesc);

						D3D_FEATURE_LEVEL FeatureLevel;

						CComPtrCustom<ID3D11Device> lDevice;

						CComPtrCustom<ID3D11DeviceContext> lImmediateContext;

						// Create device

						LOG_INVOKE_DX11_FUNCTION(D3D11CreateDevice,
							lDxgiAdapter,
							D3D_DRIVER_TYPE_UNKNOWN,
							nullptr,
							0,
							gFeatureLevels,
							gNumFeatureLevels,
							D3D11_SDK_VERSION,
							&lDevice,
							&FeatureLevel,
							&lImmediateContext);

						LOG_CHECK_STATE_DESCR(FAILED(lresult), lresult);

						LOG_CHECK_PTR_MEMORY(lDevice);

						CComPtrCustom<IDXGIOutputDuplication> lDeskDupl;

						// Create desktop duplication

						LOG_INVOKE_POINTER_METHOD(lSelectedOutput, DuplicateOutput,
							lDevice,
							&lDeskDupl);

					} while (false);

					if (SUCCEEDED(lresult))
					{
						CComPtrCustom<ScreenCaptureProcessorIDXGIOutputDuplication> lIInnerCaptureProcessor(
							new (std::nothrow) ScreenCaptureProcessorIDXGIOutputDuplication);

						LOG_CHECK_PTR_MEMORY(lIInnerCaptureProcessor);

						DXGI_OUTPUT_DESC lDesc;

						LOG_INVOKE_POINTER_METHOD(lSelectedOutput, GetDesc,
							&lDesc);

						LOG_CHECK_STATE(lDesc.AttachedToDesktop == FALSE);

						std::wstring lDeviceName;

						if (aIndex > 0)
						{
							lDeviceName = std::wstring(lDesc.DeviceName);
						}

						LOG_INVOKE_POINTER_METHOD(lIInnerCaptureProcessor, init,
							lDeviceName,
							aOrientation,
							lSelectedOutput);

						LOG_INVOKE_QUERY_INTERFACE_METHOD(lIInnerCaptureProcessor, aPtrPtrIInnerCaptureProcessor);
					}
					else
					{
						DXGI_OUTPUT_DESC lDesc;

						lSelectedOutput->GetDesc(&lDesc);
						
						std::wstring lDeviceName;

						if (aIndex > 0)
						{
							lDeviceName = std::wstring(lDesc.DeviceName);
						}

						MONITORINFO lMONITORINFO;

						ZeroMemory(&lMONITORINFO, sizeof(lMONITORINFO));

						lMONITORINFO.cbSize = sizeof(lMONITORINFO);

						BOOL lr = GetMonitorInfo(lDesc.Monitor,
							&lMONITORINFO);

						CComPtrCustom<ScreenCaptureProcessorGDI> lIInnerCaptureProcessor(
							new (std::nothrow) ScreenCaptureProcessorGDI(lMONITORINFO.rcWork));

						lIInnerCaptureProcessor->init(lDeviceName);
						
						LOG_INVOKE_QUERY_INTERFACE_METHOD(lIInnerCaptureProcessor, aPtrPtrIInnerCaptureProcessor);
					}

				} while (false);

				return lresult;
			}
		}
	}
}