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

#include "InnerScreenCaptureProcessorLandscape.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/Singleton.h"
#include "../DirectXManager/DXGIManager.h"
#include "../DirectXManager/Direct3D11Manager.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"
#include <DirectXMath.h>



namespace CaptureManager
{
	namespace Sources
	{
		namespace ScreenCapture
		{
			using namespace Core;
			using namespace Core::DXGI;
			using namespace Core::Direct3D11;

			typedef struct _VERTEX2
			{
				DirectX::XMFLOAT3 Pos;
				DirectX::XMFLOAT2 TexCoord;
			} VERTEX;

#define NUMVERTICES 6

			_VERTEX2 g_LandscapeVertexes[NUMVERTICES];

			InnerScreenCaptureProcessorLandscape::InnerScreenCaptureProcessorLandscape()
			{
			}

			InnerScreenCaptureProcessorLandscape::~InnerScreenCaptureProcessorLandscape()
			{

			}

			HRESULT InnerScreenCaptureProcessorLandscape::initResources(
				IUnknown* aPtrUnkDevice,
				ID3D11DeviceContext* aPtrDeviceContext,
				UINT aVideoFrameDuration,
				int aStride)
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(aPtrUnkDevice);

					LOG_INVOKE_FUNCTION(InnerScreenCaptureProcessor::initResources, 
						aPtrUnkDevice, 
						aPtrDeviceContext, 
						aVideoFrameDuration, 
						aStride);
										
					LOG_CHECK_PTR_MEMORY(mDevice);


					D3D11_TEXTURE2D_DESC desc;

					desc.Width = mSize.cx;

					desc.Height = mSize.cy;

					desc.Format = mDesc.ModeDesc.Format;

					desc.ArraySize = 1;

					desc.BindFlags = D3D11_BIND_RENDER_TARGET;

					desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

					desc.SampleDesc.Count = 1;

					desc.SampleDesc.Quality = 0;

					desc.MipLevels = 1;

					desc.CPUAccessFlags = 0;// D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
					desc.Usage = D3D11_USAGE_DEFAULT;

					lresult = mDevice->CreateTexture2D(&desc, NULL, &mTempRenderImage);																

					if (FAILED(lresult))
						break;

					FLOAT lScale = 1.0f;

					if (mRotation == DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE90 ||
						mRotation == DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE270)
					{
						auto lprop = (FLOAT)mSize.cy / (FLOAT)mSize.cx;
						
						lScale = lprop * (FLOAT)mSize.cy / (FLOAT)mSize.cx;
					}

					g_LandscapeVertexes[0].Pos.x = -lScale;
					g_LandscapeVertexes[0].Pos.y = -1;
					g_LandscapeVertexes[0].Pos.z = 0;

					g_LandscapeVertexes[1].Pos.x = -lScale;
					g_LandscapeVertexes[1].Pos.y = 1;
					g_LandscapeVertexes[1].Pos.z = 0;

					g_LandscapeVertexes[2].Pos.x = lScale;
					g_LandscapeVertexes[2].Pos.y = -1;
					g_LandscapeVertexes[2].Pos.z = 0;

					g_LandscapeVertexes[3].Pos.x = lScale;
					g_LandscapeVertexes[3].Pos.y = -1;
					g_LandscapeVertexes[3].Pos.z = 0;

					g_LandscapeVertexes[4].Pos.x = -lScale;
					g_LandscapeVertexes[4].Pos.y = 1;
					g_LandscapeVertexes[4].Pos.z = 0;

					g_LandscapeVertexes[5].Pos.x = lScale;
					g_LandscapeVertexes[5].Pos.y = 1;
					g_LandscapeVertexes[5].Pos.z = 0;



					g_LandscapeVertexes[0].TexCoord.x = 0;
					g_LandscapeVertexes[0].TexCoord.y = 1;

					g_LandscapeVertexes[1].TexCoord.x = 0;
					g_LandscapeVertexes[1].TexCoord.y = 0;

					g_LandscapeVertexes[2].TexCoord.x = 1;
					g_LandscapeVertexes[2].TexCoord.y = 1;

					g_LandscapeVertexes[3].TexCoord.x = 1;
					g_LandscapeVertexes[3].TexCoord.y = 1;

					g_LandscapeVertexes[4].TexCoord.x = 0;
					g_LandscapeVertexes[4].TexCoord.y = 0;

					g_LandscapeVertexes[5].TexCoord.x = 1;
					g_LandscapeVertexes[5].TexCoord.y = 0;

					if (mRotation == DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE180)
					{
						auto lTempTexture = g_LandscapeVertexes[0].TexCoord;

						g_LandscapeVertexes[0].TexCoord = g_LandscapeVertexes[5].TexCoord;
						g_LandscapeVertexes[5].TexCoord = lTempTexture;

						lTempTexture = g_LandscapeVertexes[2].TexCoord;

						g_LandscapeVertexes[2].TexCoord = g_LandscapeVertexes[1].TexCoord;
						g_LandscapeVertexes[1].TexCoord = lTempTexture;

						g_LandscapeVertexes[3].TexCoord = g_LandscapeVertexes[2].TexCoord;
						g_LandscapeVertexes[4].TexCoord = g_LandscapeVertexes[1].TexCoord;
					}
					else if (mRotation == DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE90)
					{
						auto lTempTexture = g_LandscapeVertexes[0].TexCoord;

						g_LandscapeVertexes[0].TexCoord = g_LandscapeVertexes[2].TexCoord;

						g_LandscapeVertexes[2].TexCoord = g_LandscapeVertexes[5].TexCoord;

						g_LandscapeVertexes[5].TexCoord = g_LandscapeVertexes[1].TexCoord;

						g_LandscapeVertexes[1].TexCoord = lTempTexture;

						g_LandscapeVertexes[3].TexCoord = g_LandscapeVertexes[2].TexCoord;
						g_LandscapeVertexes[4].TexCoord = g_LandscapeVertexes[1].TexCoord;
					}
					else if (mRotation == DXGI_MODE_ROTATION::DXGI_MODE_ROTATION_ROTATE270)
					{
						auto lTempTexture = g_LandscapeVertexes[0].TexCoord;

						g_LandscapeVertexes[0].TexCoord = g_LandscapeVertexes[1].TexCoord;

						g_LandscapeVertexes[1].TexCoord = g_LandscapeVertexes[5].TexCoord;

						g_LandscapeVertexes[5].TexCoord = g_LandscapeVertexes[2].TexCoord;

						g_LandscapeVertexes[2].TexCoord = lTempTexture;

						g_LandscapeVertexes[3].TexCoord = g_LandscapeVertexes[2].TexCoord;
						g_LandscapeVertexes[4].TexCoord = g_LandscapeVertexes[1].TexCoord;
					}

				} while (false);

				return lresult;
			}
			
			HRESULT InnerScreenCaptureProcessorLandscape::readFromTexture(BYTE* aPtrData)
			{
				HRESULT lresult;

				do
				{
					LOG_INVOKE_FUNCTION(redraw);

					mImmediateContext->CopyResource(mDestImage, mTempRenderImage);

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

			HRESULT InnerScreenCaptureProcessorLandscape::readFromTexture(ID3D11Texture2D* aPtrTexture2D)
			{
				HRESULT lresult;

				do
				{
					LOG_INVOKE_FUNCTION(redraw);

					LOG_CHECK_PTR_MEMORY(aPtrTexture2D);

					mImmediateContext->CopyResource(aPtrTexture2D, mTempRenderImage);

				} while (false);

				return S_OK;
			}

			HRESULT InnerScreenCaptureProcessorLandscape::redraw()
			{
				HRESULT lresult(E_FAIL);

				do
				{
					LOG_CHECK_PTR_MEMORY(mTempRenderImage);

					D3D11_TEXTURE2D_DESC FullDesc;
					mGDI_COMPATIBLEImage->GetDesc(&FullDesc);

					CComPtrCustom<ID3D11RenderTargetView> m_RTV;

					lresult = mDevice->CreateRenderTargetView(mTempRenderImage, nullptr, &m_RTV);

					if (FAILED(lresult))
						break;


					D3D11_SHADER_RESOURCE_VIEW_DESC ShaderDesc;
					ShaderDesc.Format = FullDesc.Format;
					ShaderDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
					ShaderDesc.Texture2D.MostDetailedMip = FullDesc.MipLevels - 1;
					ShaderDesc.Texture2D.MipLevels = FullDesc.MipLevels;

					// Create new shader resource view
					CComPtrCustom<ID3D11ShaderResourceView> ShaderResource = nullptr;
					lresult = mDevice->CreateShaderResourceView(mGDI_COMPATIBLEImage, &ShaderDesc, &ShaderResource);

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
					InitData.pSysMem = g_LandscapeVertexes;

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

		}
	}
}