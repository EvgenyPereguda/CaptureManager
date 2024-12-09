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

#include "InnerScreenCaptureProcessor.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/Singleton.h"
#include "../DirectXManager/DXGIManager.h"
#include "../DirectXManager/Direct3D11Manager.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"
#include <DirectXMath.h>
#include "PixelShader.h"
#include "VertexShader.h"

namespace CaptureManager
{
	namespace Sources
	{
		namespace ScreenCapture
		{
			using namespace Core;
			using namespace Core::DXGI;
			using namespace Core::Direct3D11;


			// Supported formats
			extern DXGI_FORMAT gSupportedFormats[];
			extern UINT gNumSupportedFormats;

			// Driver types supported
			extern D3D_DRIVER_TYPE gDriverTypes[];
			extern UINT gNumDriverTypes;

			// Feature levels supported
			extern D3D_FEATURE_LEVEL gFeatureLevels[];

			extern UINT gNumFeatureLevels;

			typedef struct _VERTEX
			{
				DirectX::XMFLOAT3 Pos;
				DirectX::XMFLOAT2 TexCoord;
			} VERTEX;

#define NUMVERTICES 6

			VERTEX g_vertexes[NUMVERTICES];

			InnerScreenCaptureProcessor::InnerScreenCaptureProcessor()
			{
				ZeroMemory(&mSize, sizeof(mSize));
				mIsPortrait = false;
			}
			
			InnerScreenCaptureProcessor::~InnerScreenCaptureProcessor()
			{
			}

			HRESULT InnerScreenCaptureProcessor::init(IDXGIOutput1* aPtrIDXGIOutput1)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrIDXGIOutput1);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrIDXGIOutput1, &mOutput);

					LOG_CHECK_PTR_MEMORY(mOutput);
					
					LOG_INVOKE_POINTER_METHOD(mOutput, GetDesc,
						&mOutputDesc);

					/*
					Portrate: IDENTITY Width: 800 Height: 1280 FullDesc Width: 800 FullDesc Height: 1280

					Flipped Portrate: ROTATE180 Width: 800 Height: 1280 FullDesc Width: 800 FullDesc Height: 1280

					Flipped landscape: ROTATE90 Width: 1280 Height: 800 FullDesc Width: 800 FullDesc Height: 1280

					Landscape: ROTATE270 Width: 1280 Height: 800 FullDesc Width: 800 FullDesc Height: 1280
					*/
					
					int lWidth = mOutputDesc.DesktopCoordinates.right - mOutputDesc.DesktopCoordinates.left;

					int lHeight = mOutputDesc.DesktopCoordinates.bottom - mOutputDesc.DesktopCoordinates.top;
					
					mIsPortrait =
						(lWidth < lHeight && mOutputDesc.Rotation == DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_IDENTITY) ||
						(lWidth < lHeight && mOutputDesc.Rotation == DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE180) ||
						(lWidth > lHeight && mOutputDesc.Rotation == DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE90) ||
						(lWidth > lHeight && mOutputDesc.Rotation == DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE270);
					
					if (mIsPortrait)
					{
						switch (mOutputDesc.Rotation)
						{
						case DXGI_MODE_ROTATION_ROTATE90:
							mRotation = DXGI_MODE_ROTATION_ROTATE180;
							break;
						case DXGI_MODE_ROTATION_ROTATE180:
							mRotation = DXGI_MODE_ROTATION_ROTATE270;
							break;
						case DXGI_MODE_ROTATION_ROTATE270:
							mRotation = DXGI_MODE_ROTATION_IDENTITY;
							break;
						case DXGI_MODE_ROTATION_IDENTITY:
						case DXGI_MODE_ROTATION_UNSPECIFIED:
						default:
							mRotation = DXGI_MODE_ROTATION_ROTATE90;
							break;
						}

						mDisplayRotation = mRotation;
					}
					else
					{
						mRotation = mOutputDesc.Rotation;

						mDisplayRotation = mOutputDesc.Rotation;
					}

					if (lWidth < lHeight)
					{
						mSize.cx = lHeight;
						mSize.cy = lWidth;
					}
					else
					{
						mSize.cx = lWidth;
						mSize.cy = lHeight;
					}
					
				} while (false);

				return lresult;
			}

			HRESULT InnerScreenCaptureProcessor::getSize(SIZE& aRefSize)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_STATE(mSize.cx == 0 || mSize.cy == 0);

					aRefSize = mSize;

					lresult = S_OK;

				} while (false);

				return lresult;
			}
			
			HRESULT InnerScreenCaptureProcessor::releaseResources()
			{
				HRESULT lresult(E_FAIL);

				do
				{

					if (mDeskDupl)
					{
						mDeskDupl.Release();
					}

					if (mAcquiredDesktopImage)
					{
						mAcquiredDesktopImage.Release();
					}

					if (mVertexShader)
					{
						mVertexShader.Release();
					}

					if (mPixelShader)
					{
						mPixelShader.Release();
					}

					if (mInputLayout)
					{
						mInputLayout.Release();
					}

					if (mDevice)
					{
						mDevice.Release();
					}

					if (mImmediateContext)
					{
						mImmediateContext.Release();
					}

					lresult = S_OK;
					
				} while (false);

				return lresult;
			}

			HRESULT InnerScreenCaptureProcessor::initResources(
				IUnknown* aPtrUnkDevice,
				ID3D11DeviceContext* aPtrDeviceContext,
				UINT aVideoFrameDuration,
				int aStride)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUnkDevice);

					LOG_CHECK_PTR_MEMORY(aPtrDeviceContext);

					LOG_CHECK_PTR_MEMORY(mOutput);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(aPtrUnkDevice, &mDevice);

					LOG_CHECK_PTR_MEMORY(mDevice);

					mImmediateContext = aPtrDeviceContext;

					LOG_CHECK_PTR_MEMORY(mImmediateContext);
					
					// Create desktop duplication

					LOG_INVOKE_POINTER_METHOD(mOutput, DuplicateOutput,
						mDevice,
						&mDeskDupl);
										
					LOG_CHECK_PTR_MEMORY(mDeskDupl);

					mDeskDupl->GetDesc(&mDesc);
					
					D3D11_TEXTURE2D_DESC desc;

					desc.Width = mSize.cx;

					desc.Height = mSize.cy;

					desc.Format = mDesc.ModeDesc.Format;

					desc.ArraySize = 1;

					desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

					desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

					desc.SampleDesc.Count = 1;

					desc.SampleDesc.Quality = 0;

					desc.MipLevels = 1;

					desc.CPUAccessFlags = 0;// D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
					desc.Usage = D3D11_USAGE_DEFAULT;

					lresult = mDevice->CreateTexture2D(&desc, NULL, &mGDI_COMPATIBLEImage);

					desc.BindFlags = D3D11_BIND_RENDER_TARGET;
					
					lresult = mDevice->CreateTexture2D(&desc, NULL, &mTempDesktopImage);
					
					desc.BindFlags = 0;

					desc.MiscFlags = 0;
					
					desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
					desc.Usage = D3D11_USAGE_STAGING;

					lresult = mDevice->CreateTexture2D(&desc, NULL, &mDestImage);
					
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

					g_vertexes[0].Pos.x = -1;
					g_vertexes[0].Pos.y = -1;
					g_vertexes[0].Pos.z = 0;

					g_vertexes[1].Pos.x = -1;
					g_vertexes[1].Pos.y = 1;
					g_vertexes[1].Pos.z = 0;

					g_vertexes[2].Pos.x = 1;
					g_vertexes[2].Pos.y = -1;
					g_vertexes[2].Pos.z = 0;

					g_vertexes[3].Pos.x = 1;
					g_vertexes[3].Pos.y = -1;
					g_vertexes[3].Pos.z = 0;

					g_vertexes[4].Pos.x = -1;
					g_vertexes[4].Pos.y = 1;
					g_vertexes[4].Pos.z = 0;

					g_vertexes[5].Pos.x = 1;
					g_vertexes[5].Pos.y = 1;
					g_vertexes[5].Pos.z = 0;


					if (mIsPortrait)
					{
						g_vertexes[0].TexCoord.x = 0;
						g_vertexes[0].TexCoord.y = 0;

						g_vertexes[1].TexCoord.x = 1;
						g_vertexes[1].TexCoord.y = 0;

						g_vertexes[2].TexCoord.x = 0;
						g_vertexes[2].TexCoord.y = 1;

						g_vertexes[3].TexCoord.x = 0;
						g_vertexes[3].TexCoord.y = 1;

						g_vertexes[4].TexCoord.x = 1;
						g_vertexes[4].TexCoord.y = 0;

						g_vertexes[5].TexCoord.x = 1;
						g_vertexes[5].TexCoord.y = 1;
					}
					else
					{
						g_vertexes[0].TexCoord.x = 0;
						g_vertexes[0].TexCoord.y = 1;

						g_vertexes[1].TexCoord.x = 0;
						g_vertexes[1].TexCoord.y = 0;

						g_vertexes[2].TexCoord.x = 1;
						g_vertexes[2].TexCoord.y = 1;

						g_vertexes[3].TexCoord.x = 1;
						g_vertexes[3].TexCoord.y = 1;

						g_vertexes[4].TexCoord.x = 0;
						g_vertexes[4].TexCoord.y = 0;

						g_vertexes[5].TexCoord.x = 1;
						g_vertexes[5].TexCoord.y = 0;
					}
					
					mVideoFrameDuration = aVideoFrameDuration;

					mStride = aStride;

				} while (false);

				return lresult;
			}
			
			HRESULT InnerScreenCaptureProcessor::getGDITexture(IUnknown** aPtrPtrUnk)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrUnk);

					LOG_CHECK_PTR_MEMORY(mGDI_COMPATIBLEImage);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mGDI_COMPATIBLEImage, aPtrPtrUnk);

				} while (false);

				return lresult;
			}

			HRESULT InnerScreenCaptureProcessor::updateFrame()
			{
				HRESULT lresult(E_FAIL);

				do
				{
					bool lTimeout;

					LOG_INVOKE_FUNCTION(acquireNextFrame,
						&lTimeout);

					if (!lTimeout)
					{
						LOG_CHECK_PTR_MEMORY(mAcquiredDesktopImage);

						redraw();

						mAcquiredDesktopImage.Release();

						LOG_INVOKE_POINTER_METHOD(mDeskDupl, ReleaseFrame);
					}
					
					mImmediateContext->CopyResource(mGDI_COMPATIBLEImage, mTempDesktopImage);

				} while (false);

				return lresult;
			}

			HRESULT InnerScreenCaptureProcessor::updateFrame(ID3D11Texture2D* aPtrTexture2D)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					bool lTimeout;

					LOG_INVOKE_FUNCTION(acquireNextFrame,
						&lTimeout);

					if (!lTimeout)
					{
						LOG_CHECK_PTR_MEMORY(mAcquiredDesktopImage);

						//redraw();
						mImmediateContext->CopyResource(aPtrTexture2D, mAcquiredDesktopImage);

						mAcquiredDesktopImage.Release();

						LOG_INVOKE_POINTER_METHOD(mDeskDupl, ReleaseFrame);
					}

					//mImmediateContext->CopyResource(aPtrTexture2D, mTempDesktopImage);

				} while (false);

				return lresult;
			}

			HRESULT InnerScreenCaptureProcessor::acquireNextFrame(
				bool* aPtrTimeout)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					CComPtrCustom<IDXGIResource> lDesktopResource;
					DXGI_OUTDUPL_FRAME_INFO lFrameInfo;

					// If still holding old frame, destroy it
					if (mAcquiredDesktopImage)
					{
						mAcquiredDesktopImage.Release();
					}

					if (mVideoFrameDuration == 0)
						mOutput->WaitForVBlank();
																				
					// Get new frame
					LOG_INVOKE_POINTER_METHOD(mDeskDupl, AcquireNextFrame,
						//INFINITE,
						100,		
						&lFrameInfo,
						&lDesktopResource);
									
					LOG_INVOKE_QUERY_INTERFACE_METHOD(lDesktopResource, &mAcquiredDesktopImage);
					lDesktopResource.Release();


				} while (false);
				
				if (lresult == DXGI_ERROR_WAIT_TIMEOUT)
				{
					*aPtrTimeout = true;

					lresult = S_OK;
				}
				else
				{
					*aPtrTimeout = false;
				}

				return lresult;
			}
			
			HRESULT InnerScreenCaptureProcessor::redraw()
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(mAcquiredDesktopImage);

					D3D11_TEXTURE2D_DESC FullDesc;
					mAcquiredDesktopImage->GetDesc(&FullDesc);

					CComPtrCustom<ID3D11RenderTargetView> m_RTV;

					lresult = mDevice->CreateRenderTargetView(mTempDesktopImage, nullptr, &m_RTV);

					if (FAILED(lresult))
						break;


					D3D11_SHADER_RESOURCE_VIEW_DESC ShaderDesc;
					ShaderDesc.Format = FullDesc.Format;
					ShaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					ShaderDesc.Texture2D.MostDetailedMip = FullDesc.MipLevels - 1;
					ShaderDesc.Texture2D.MipLevels = FullDesc.MipLevels;

					// Create new shader resource view
					CComPtrCustom<ID3D11ShaderResourceView> ShaderResource = nullptr;
					lresult = mDevice->CreateShaderResourceView(mAcquiredDesktopImage, &ShaderDesc, &ShaderResource);

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
					InitData.pSysMem = g_vertexes;

					CComPtrCustom<ID3D11Buffer> VertBuf;
					lresult = mDevice->CreateBuffer(&BufferDesc, &InitData, &VertBuf);

					if (FAILED(lresult))
						break;

					UINT Stride = sizeof(VERTEX);
					UINT Offset = 0;
					mImmediateContext->IASetVertexBuffers(0, 1, &VertBuf, &Stride, &Offset);

					D3D11_VIEWPORT VP;
					
					VP.Width = static_cast<FLOAT>(mSize.cx);
					VP.Height = static_cast<FLOAT>(mSize.cy);

					VP.MinDepth = 0.0f;
					VP.MaxDepth = 1.0f;
					VP.TopLeftX = 0.0f;
					VP.TopLeftY = 0.0f;
					mImmediateContext->RSSetViewports(1, &VP);

					mImmediateContext->Draw(NUMVERTICES * DirtyCount, 0);


				} while (false);

				return lresult;
			}

			HRESULT InnerScreenCaptureProcessor::readFromTexture(BYTE* aPtrData)
			{
				HRESULT lresult;

				do
				{
					mImmediateContext->CopyResource(mDestImage, mGDI_COMPATIBLEImage);
					
					D3D11_MAPPED_SUBRESOURCE resource;
					UINT subresource = D3D11CalcSubresource(0, 0, 0);
					lresult = mImmediateContext->Map(mDestImage, subresource, D3D11_MAP_READ_WRITE, 0, &resource);

					// COPY from texture to bitmap buffer
					//uint8_t* sptr = reinterpret_cast<uint8_t*>(resource.pData);
					//uint8_t* dptr = aPtrData;

					//for (size_t h = 0; h < mDesc.ModeDesc.Height; ++h)
					//{
					//	size_t msize = std::min<size_t>(mDesc.ModeDesc.Width * 4, resource.RowPitch);
					//	memcpy_s(dptr, mDesc.ModeDesc.Width * 4, sptr, msize);
					//	sptr += resource.RowPitch;
					//	dptr += mDesc.ModeDesc.Width * 4;
					//}

					if (SUCCEEDED(lresult))
					{
						aPtrData += (mSize.cy - 1)*mStride;

						LOG_INVOKE_MF_FUNCTION(MFCopyImage,
							aPtrData,
							-mStride,
							(BYTE*)resource.pData,
							resource.RowPitch,
							mStride,
							mSize.cy);
					}

					mImmediateContext->Unmap(mDestImage, subresource);

				} while (false);

				return S_OK;
			}
						
			HRESULT InnerScreenCaptureProcessor::readFromTexture(ID3D11Texture2D* aPtrTexture2D)
			{
				HRESULT lresult(S_OK);

				do
				{					
					mImmediateContext->CopyResource(aPtrTexture2D, mGDI_COMPATIBLEImage);
					
				} while (false);

				return lresult;
			}
			
			HRESULT InnerScreenCaptureProcessor::getCursorInfo(RECT& aRefDesktopCoordinates, DXGI_MODE_ROTATION& aRefRotation)
			{
				aRefDesktopCoordinates = mOutputDesc.DesktopCoordinates;

				aRefRotation = mDisplayRotation;

				return S_OK;
			}
		}
	}
}