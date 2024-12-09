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

#include "InitilaizeCaptureSource.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "../VideoRendererManager/Direct3D11VideoProcessor.h"
#include "../DirectXManager/Direct3D11Manager.h"
#include "../DirectXManager/DXGIManager.h"
#include "../LogPrintOut/LogPrintOut.h"
#include "../Common/Singleton.h"
#include "../Common/Common.h"
#include "../Common/GUIDs.h"
#include "../ConfigManager/ConfigManager.h"


namespace CaptureManager
{
	namespace COMServer
	{
		using namespace pugi;

		using namespace Core;

		// Driver types supported
		static D3D_DRIVER_TYPE gDriverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE
		};
		static UINT gNumDriverTypes = ARRAYSIZE(gDriverTypes);

		// Feature levels supported
		static D3D_FEATURE_LEVEL gFeatureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0
			//D3D_FEATURE_LEVEL_10_1,
			//D3D_FEATURE_LEVEL_10_0,
			//D3D_FEATURE_LEVEL_9_1
		};

		static UINT gNumFeatureLevels = ARRAYSIZE(gFeatureLevels);


		static UINT mUseDebugLayer(D3D11_CREATE_DEVICE_VIDEO_SUPPORT);
		
		InitilaizeCaptureSource::InitilaizeCaptureSource()
		{
		}
		
		HRESULT InitilaizeCaptureSource::createDevice(ID3D11Device** aPtrPtrDevice)
		{
			using namespace Core;

			using namespace Core::Direct3D11;

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrPtrDevice);

				LOG_INVOKE_FUNCTION(Singleton<Direct3D11Manager>::getInstance().getState);

				D3D_FEATURE_LEVEL lfeatureLevel;

				LOG_INVOKE_DX11_FUNCTION(D3D11CreateDevice,
					NULL,
					D3D_DRIVER_TYPE_HARDWARE,
					NULL,
					mUseDebugLayer,
					gFeatureLevels,
					gNumFeatureLevels,
					D3D11_SDK_VERSION,
					aPtrPtrDevice,
					&lfeatureLevel,
					NULL);

				LOG_CHECK_PTR_MEMORY(aPtrPtrDevice);

			} while (false);

			return lresult;
		}

		HRESULT InitilaizeCaptureSource::createUncompressedVideoType(
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
				//if (subtype == MFVideoFormat_NV12)
				//	lStride = -lStride;

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

			// Return the pointer to the caller.

			if (pType)
			{
				hr = pType->QueryInterface(IID_PPV_ARGS(ppType));
			}

		done:
			return hr;
		}


		HRESULT InitilaizeCaptureSource::processVideoMediaType(
			xml_node& aMediaType,
			IMFMediaType* aPtrVideoMediaType)
		{
			using namespace Sinks::EVR::Mixer;

			HRESULT lresult(E_FAIL);

			do
			{
				if (aMediaType.empty())
					return lresult;

				auto lDirectX11_CaptureMediaTypeItemNode = aMediaType.select_node(L"MediaTypeItem[@Name = 'CM_DirectX11_Capture_Texture']");

				if (lDirectX11_CaptureMediaTypeItemNode.node().empty())
				{
					auto lMediaTypeItemNodes = aMediaType.select_nodes(L"MediaTypeItem");

					LOG_CHECK_PTR_MEMORY(aPtrVideoMediaType);

					LOG_INVOKE_MF_METHOD(SetGUID, aPtrVideoMediaType,
						MF_MT_MAJOR_TYPE,
						MFMediaType_Video);



					GUID lInputSubtype = GUID_NULL;

					GUID lSubType = GUID_NULL;

					UINT32 lWidthInPixels = 0;

					INT32 lStride = 0;

					for (auto& lItem : lMediaTypeItemNodes)
					{
						auto lAttribute = lItem.node().attribute(L"Name");

						if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_FRAME_SIZE")
						{
							auto lValueParts = lItem.node().select_nodes(L"Value.ValueParts/ValuePart");

							if (lValueParts.size() == 2)
							{

								UINT32 lWidth = 0;

								UINT32 lHeight = 0;

								for (auto& lValuePart : lValueParts)
								{
									lAttribute = lValuePart.node().attribute(L"Title");

									if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"Width")
									{
										lAttribute = lValuePart.node().attribute(L"Value");

										if (!lAttribute.empty())
										{
											lWidth = lAttribute.as_uint();

											lWidthInPixels = lWidth;
										}
									}
									else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"Height")
									{
										lAttribute = lValuePart.node().attribute(L"Value");

										if (!lAttribute.empty())
										{
											lHeight = lAttribute.as_uint();
										}
									}

								}

								LOG_INVOKE_FUNCTION(MFSetAttributeSize,
									aPtrVideoMediaType,
									MF_MT_FRAME_SIZE,
									lWidth,
									lHeight);
							}
						}
						else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_AVG_BITRATE")
						{
							auto lSingleValue = lItem.node().select_node(L"SingleValue");

							if (!lSingleValue.node().empty())
							{
								UINT32 lAverBitRate = lSingleValue.node().attribute(L"Value").as_uint();

								LOG_INVOKE_MF_METHOD(SetUINT32, aPtrVideoMediaType,
									MF_MT_AVG_BITRATE,
									lAverBitRate);
							}
						}
						else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_DEFAULT_STRIDE")
						{
							auto lSingleValue = lItem.node().select_node(L"SingleValue");

							if (!lSingleValue.node().empty())
							{
								lStride = lSingleValue.node().attribute(L"Value").as_int();

								if (lStride != 0)
									LOG_INVOKE_MF_METHOD(SetUINT32, aPtrVideoMediaType,
									CM_SourceStride,
									*((UINT32*)&lStride));

								if (lStride != 0)
									LOG_INVOKE_MF_METHOD(SetUINT32, aPtrVideoMediaType,
									MF_MT_DEFAULT_STRIDE,
									*((UINT32*)&lStride));
							}
						}
						else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_FIXED_SIZE_SAMPLES")
						{
							auto lSingleValue = lItem.node().select_node(L"SingleValue");

							if (!lSingleValue.node().empty())
							{
								BOOL lState = FALSE;

								if (std::wstring(lSingleValue.node().attribute(L"Value").value()) == L"True")
								{
									lState = TRUE;
								}

								LOG_INVOKE_MF_METHOD(SetUINT32, aPtrVideoMediaType,
									MF_MT_FIXED_SIZE_SAMPLES,
									lState);
							}
						}
						else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_FRAME_RATE")
						{
							auto lValueParts = lItem.node().select_nodes(L"RatioValue/Value.ValueParts/ValuePart");

							if (lValueParts.size() == 2)
							{

								UINT32 lNumerator = 0;

								UINT32 lDenominator = 0;

								for (auto& lValuePart : lValueParts)
								{
									lAttribute = lValuePart.node().attribute(L"Title");

									if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"Numerator")
									{
										lAttribute = lValuePart.node().attribute(L"Value");

										if (!lAttribute.empty())
										{
											lNumerator = lAttribute.as_uint();
										}
									}
									else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"Denominator")
									{
										lAttribute = lValuePart.node().attribute(L"Value");

										if (!lAttribute.empty())
										{
											lDenominator = lAttribute.as_uint();
										}
									}

								}

								LOG_INVOKE_FUNCTION(MFSetAttributeRatio,
									aPtrVideoMediaType,
									MF_MT_FRAME_RATE,
									lNumerator,
									lDenominator);
							}
						}
						else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_PIXEL_ASPECT_RATIO")
						{
							auto lValueParts = lItem.node().select_nodes(L"RatioValue/Value.ValueParts/ValuePart");

							if (lValueParts.size() == 2)
							{

								UINT32 lNumerator = 0;

								UINT32 lDenominator = 0;

								for (auto& lValuePart : lValueParts)
								{
									lAttribute = lValuePart.node().attribute(L"Title");

									if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"Numerator")
									{
										lAttribute = lValuePart.node().attribute(L"Value");

										if (!lAttribute.empty())
										{
											lNumerator = lAttribute.as_uint();
										}
									}
									else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"Denominator")
									{
										lAttribute = lValuePart.node().attribute(L"Value");

										if (!lAttribute.empty())
										{
											lDenominator = lAttribute.as_uint();
										}
									}

								}

								LOG_INVOKE_FUNCTION(MFSetAttributeRatio,
									aPtrVideoMediaType,
									MF_MT_PIXEL_ASPECT_RATIO,
									lNumerator,
									lDenominator);
							}
						}
						else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_ALL_SAMPLES_INDEPENDENT")
						{
							auto lSingleValue = lItem.node().select_node(L"SingleValue");

							if (!lSingleValue.node().empty())
							{
								BOOL lState = FALSE;

								if (std::wstring(lSingleValue.node().attribute(L"Value").value()) == L"True")
								{
									lState = TRUE;
								}

								LOG_INVOKE_MF_METHOD(SetUINT32, aPtrVideoMediaType,
									MF_MT_ALL_SAMPLES_INDEPENDENT,
									lState);
							}
						}
						else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_SAMPLE_SIZE")
						{
							auto lSingleValue = lItem.node().select_node(L"SingleValue");

							if (!lSingleValue.node().empty())
							{
								UINT32 lAverBitRate = lSingleValue.node().attribute(L"Value").as_uint();

								LOG_INVOKE_MF_METHOD(SetUINT32, aPtrVideoMediaType,
									MF_MT_SAMPLE_SIZE,
									lAverBitRate);
							}
						}
						else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_INTERLACE_MODE")
						{
							auto lSingleValue = lItem.node().select_node(L"SingleValue");

							if (!lSingleValue.node().empty())
							{
								if (std::wstring(lSingleValue.node().attribute(L"Value").value()) == L"MFVideoInterlace_Progressive")
								{
									LOG_INVOKE_MF_METHOD(SetUINT32, aPtrVideoMediaType,
										MF_MT_INTERLACE_MODE,
										MFVideoInterlace_Progressive);
								}
							}
						}
						else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_SUBTYPE")
						{
							auto lSingleValue = lItem.node().select_node(L"SingleValue");

							if (!lSingleValue.node().empty())
							{
								lresult = CLSIDFromString(lSingleValue.node().attribute(L"GUID").value(), &lSubType);

								if (SUCCEEDED(lresult))
								{
									LOG_INVOKE_MF_METHOD(SetGUID, aPtrVideoMediaType,
										MF_MT_SUBTYPE,
										lSubType);
								}
							}
						}
						else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"CM_DIRECT_CALL")
						{
							auto lSingleValue = lItem.node().select_node(L"SingleValue");

							if (!lSingleValue.node().empty())
							{
								BOOL lState = FALSE;

								if (std::wstring(lSingleValue.node().attribute(L"Value").value()) == L"True")
								{
									lState = TRUE;
								}

								LOG_INVOKE_MF_METHOD(SetUINT32, aPtrVideoMediaType,
									CM_DIRECT_CALL,
									lState);
							}
						}

						
					}

					do
					{
						LONG pStride(0);

						if (lWidthInPixels > 0 && lStride != 0)
						{
							LOG_INVOKE_MF_FUNCTION(MFGetStrideForBitmapInfoHeader,
								lSubType.Data1,
								lWidthInPixels,
								&pStride);

							if (pStride != 0)
								LOG_INVOKE_MF_METHOD(SetUINT32, aPtrVideoMediaType,
								MF_MT_DEFAULT_STRIDE,
								*((UINT32*)&pStride));
						}

					} while (false);
				}
				else
				{
					LOG_CHECK_STATE_DESCR(!Singleton<ConfigManager>::getInstance().isWindows10_Or_Greater(), E_INVALIDARG);

					auto lFrameRateMediaTypeItemNode = aMediaType.select_node(L"MediaTypeItem[@Name = 'MF_MT_FRAME_RATE']");

					LOG_CHECK_STATE_DESCR(lFrameRateMediaTypeItemNode.node().empty(), E_INVALIDARG);

					auto lValueParts = lFrameRateMediaTypeItemNode.node().select_nodes(L"RatioValue/Value.ValueParts/ValuePart");

					MFRatio lFrameRate;

					MFRatio lAspectRatio;

					lAspectRatio.Numerator = 1;

					lAspectRatio.Denominator = 1;

					if (lValueParts.size() == 2)
					{

						UINT32 lNumerator = 0;

						UINT32 lDenominator = 0;

						for (auto& lValuePart : lValueParts)
						{
							auto lAttribute = lValuePart.node().attribute(L"Title");

							if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"Numerator")
							{
								lAttribute = lValuePart.node().attribute(L"Value");

								if (!lAttribute.empty())
								{
									lNumerator = lAttribute.as_uint();
								}
							}
							else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"Denominator")
							{
								lAttribute = lValuePart.node().attribute(L"Value");

								if (!lAttribute.empty())
								{
									lDenominator = lAttribute.as_uint();
								}
							}

						}

						LOG_INVOKE_FUNCTION(MFSetAttributeRatio,
							aPtrVideoMediaType,
							MF_MT_FRAME_RATE,
							lNumerator,
							lDenominator);

						lFrameRate.Numerator = lNumerator;

						lFrameRate.Denominator = lDenominator;
					}

					auto lIs_FlippedMediaTypeItemNode = aMediaType.select_node(L"MediaTypeItem[@Name = 'CM_Is_Flipped']");

					UINT32 lIsFlipped = FALSE;

					if (!lIs_FlippedMediaTypeItemNode.node().empty())
					{
						auto lSingleValue = lIs_FlippedMediaTypeItemNode.node().select_node(L"SingleValue");

						if (!lSingleValue.node().empty())
						{
							auto lValue = std::wstring(lSingleValue.node().attribute(L"Value").as_string());

							if (lValue == L"True")
								lIsFlipped = TRUE;
						}
					}

					auto lSingleValue = lDirectX11_CaptureMediaTypeItemNode.node().select_node(L"SingleValue");

					LOG_CHECK_STATE_DESCR(lSingleValue.node().empty(), E_INVALIDARG);


					CComPtrCustom<IMFMediaType> lOutputMediaType;

					CComPtrCustom<IMFMediaType> lInputMediaType;

					auto l_IUnknown = (IUnknown*)lSingleValue.node().attribute(L"Value").as_ullong();

					bool l_convert = true;

					if (l_IUnknown != nullptr)
					{
						CComQIPtrCustom<ID3D11Texture2D> lID3D11Texture2D = l_IUnknown;

						if (lID3D11Texture2D)
						{

							D3D11_TEXTURE2D_DESC lDestDesc;

							lID3D11Texture2D->GetDesc(&lDestDesc);

							if (lDestDesc.Format == DXGI_FORMAT_R8G8B8A8_TYPELESS ||
								lDestDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM)
							{
								lDestDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

							}
							else
								if (lDestDesc.Format == DXGI_FORMAT_B8G8R8X8_TYPELESS ||
									lDestDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM)
								{
									lDestDesc.Format = DXGI_FORMAT_B8G8R8X8_UNORM;
								}
								else
									if (lDestDesc.Format == DXGI_FORMAT_B8G8R8A8_TYPELESS ||
										lDestDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM)
									{
										lDestDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;

										l_convert = false;
									}
									else
									{
										LOG_CHECK_STATE(true);
									}



							GUID lInputSubtype = GUID_NULL;

							LOG_INVOKE_FUNCTION(Direct3D11VideoProcessor::convertDXGIFormatToSubType, lDestDesc.Format, &lInputSubtype);

							CComPtrCustom<ID3D11Device> lD3D11Device;

							lID3D11Texture2D->GetDevice(&lD3D11Device);

							if (lD3D11Device)
							{
								auto lCreationFlags = lD3D11Device->GetCreationFlags();

								bool lSharedTexture = false;

								if ((lCreationFlags & D3D11_CREATE_DEVICE_SINGLETHREADED) != 0
									//|| (lCreationFlags & D3D11_CREATE_DEVICE_VIDEO_SUPPORT) == 0
									)
								{
									lSharedTexture = true;
								}

								if (lSharedTexture)
								{
									lDestDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

									lDestDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;

									LOG_INVOKE_POINTER_METHOD(lD3D11Device, CreateTexture2D,
										&lDestDesc, NULL, &mSharedID3D11Texture2D);

									LOG_CHECK_PTR_MEMORY(mSharedID3D11Texture2D);

									CComPtrCustom<IDXGIResource> lResource;

									LOG_INVOKE_QUERY_INTERFACE_METHOD(mSharedID3D11Texture2D, &lResource);

									HANDLE sharedHandle;

									LOG_INVOKE_POINTER_METHOD(lResource, GetSharedHandle, &sharedHandle);

									LOG_CHECK_PTR_MEMORY(sharedHandle);

									LOG_INVOKE_FUNCTION(createDevice, &mID3D11Device);

									LOG_CHECK_PTR_MEMORY(mID3D11Device);

									CComPtrCustom<ID3D11Resource> lTempResource11;

									LOG_INVOKE_POINTER_METHOD(mID3D11Device, OpenSharedResource,
										sharedHandle, IID_PPV_ARGS(&lTempResource11));

									LOG_CHECK_PTR_MEMORY(lTempResource11);

									LOG_INVOKE_QUERY_INTERFACE_METHOD(lTempResource11, &mID3D11Texture2D);
								}
								else
								{
									mID3D11Device = lD3D11Device;

									mID3D11Texture2D = lID3D11Texture2D;
								}

								LOG_CHECK_PTR_MEMORY(mID3D11Device);
								
								CComPtrCustom<ID3D11DeviceContext> lImmediateContext;

								mID3D11Device->GetImmediateContext(&lImmediateContext);

								LOG_CHECK_PTR_MEMORY(lImmediateContext);

								do
								{
									CComPtrCustom<ID3D10Multithread> lMultiThread;

									// Need to explitly set the multithreaded mode for this device
									LOG_INVOKE_QUERY_INTERFACE_METHOD(lImmediateContext, &lMultiThread);

									LOG_CHECK_PTR_MEMORY(lMultiThread);

									BOOL lbRrsult = lMultiThread->SetMultithreadProtected(TRUE);

								} while (false);



								mDeviceManager.Release();

								LOG_INVOKE_MF_FUNCTION(MFCreateDXGIDeviceManager,
									&mDeviceResetToken,
									&mDeviceManager);

								LOG_CHECK_PTR_MEMORY(mDeviceManager);

								LOG_INVOKE_POINTER_METHOD(mDeviceManager, ResetDevice,
									mID3D11Device,
									mDeviceResetToken);





								LOG_INVOKE_FUNCTION(createUncompressedVideoType,
									l_convert? MFVideoFormat_NV12: MFVideoFormat_ARGB32,
									//MFVideoFormat_NV12,
									lDestDesc.Width,
									lDestDesc.Height,
									MFVideoInterlaceMode::MFVideoInterlace_Progressive,
									lFrameRate,
									lAspectRatio,
									&lOutputMediaType);




								LOG_INVOKE_FUNCTION(createUncompressedVideoType,
									lInputSubtype,
									lDestDesc.Width,
									lDestDesc.Height,
									MFVideoInterlaceMode::MFVideoInterlace_Progressive,
									lFrameRate,
									lAspectRatio,
									&lInputMediaType);


							}
						}

						mCaptureID3D11Texture2D = lID3D11Texture2D;
					}

					if (mDeviceManager && l_convert)
					{


						LOG_INVOKE_FUNCTION(Direct3D11VideoProcessor::createProcessor, &mMixer, 1, lIsFlipped != FALSE);
						
						LOG_INVOKE_MF_METHOD(ProcessMessage, mMixer, MFT_MESSAGE_SET_D3D_MANAGER, 0);

						LOG_INVOKE_MF_METHOD(ProcessMessage, mMixer, MFT_MESSAGE_SET_D3D_MANAGER, (ULONG_PTR)mDeviceManager.get());

						LOG_INVOKE_MF_METHOD(SetOutputType, mMixer, 0, lOutputMediaType, MFT_SET_TYPE_TEST_ONLY);

						LOG_INVOKE_MF_METHOD(SetOutputType, mMixer, 0, lOutputMediaType, 0);

						LOG_INVOKE_MF_METHOD(SetInputType, mMixer, 0, lInputMediaType, MFT_SET_TYPE_TEST_ONLY);

						LOG_INVOKE_MF_METHOD(SetInputType, mMixer, 0, lInputMediaType, 0);

						LOG_INVOKE_MF_METHOD(ProcessMessage, mMixer, MFT_MESSAGE_NOTIFY_BEGIN_STREAMING, 0);

						LOG_INVOKE_MF_METHOD(CopyAllItems,
							lOutputMediaType,
							aPtrVideoMediaType);

						LOG_INVOKE_MF_METHOD(SetUINT32, aPtrVideoMediaType,
							CM_Is_Flipped,
							lIsFlipped);
					}
					else
					{

						LOG_INVOKE_MF_METHOD(CopyAllItems,
							lOutputMediaType,
							aPtrVideoMediaType);

						LOG_INVOKE_MF_METHOD(SetUINT32, aPtrVideoMediaType,
							CM_Is_Flipped,
							lIsFlipped);
					}
				}

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		HRESULT InitilaizeCaptureSource::processAudioMediaType(
			xml_node& aMediaType,
			IMFMediaType* aPtrAudioMediaType)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				if (aMediaType.empty())
					return lresult;

				auto lMediaTypeItemNodes = aMediaType.select_nodes(L"MediaTypeItem");

				LOG_CHECK_PTR_MEMORY(aPtrAudioMediaType);

				LOG_INVOKE_MF_METHOD(SetGUID, aPtrAudioMediaType,
					MF_MT_MAJOR_TYPE,
					MFMediaType_Audio);



				GUID lInputSubtype = GUID_NULL;

				GUID lSubType = GUID_NULL;

				GUID lMajorType = GUID_NULL;

				UINT32 lWidthInPixels = 0;

				INT32 lStride = 0;

				for (auto& lItem : lMediaTypeItemNodes)
				{
					auto lAttribute = lItem.node().attribute(L"Name");

					if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_MAJOR_TYPE")
					{
						auto lSingleValue = lItem.node().select_node(L"SingleValue");

						if (!lSingleValue.node().empty())
						{
							lresult = CLSIDFromString(lSingleValue.node().attribute(L"GUID").value(), &lMajorType);

							if (SUCCEEDED(lresult))
							{
								LOG_INVOKE_MF_METHOD(SetGUID, aPtrAudioMediaType,
									MF_MT_MAJOR_TYPE,
									lMajorType);
							}
						}
					}
					else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_SUBTYPE")
					{
						auto lSingleValue = lItem.node().select_node(L"SingleValue");

						if (!lSingleValue.node().empty())
						{
							lresult = CLSIDFromString(lSingleValue.node().attribute(L"GUID").value(), &lSubType);

							if (SUCCEEDED(lresult))
							{
								LOG_INVOKE_MF_METHOD(SetGUID, aPtrAudioMediaType,
									MF_MT_SUBTYPE,
									lSubType);
							}
						}
					}
					else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_AUDIO_NUM_CHANNELS")
					{
						auto lSingleValue = lItem.node().select_node(L"SingleValue");

						if (!lSingleValue.node().empty())
						{
							UINT32 lvalue = lSingleValue.node().attribute(L"Value").as_uint();

							LOG_INVOKE_MF_METHOD(SetUINT32, aPtrAudioMediaType,
								MF_MT_AUDIO_NUM_CHANNELS,
								lvalue);
						}
					}
					else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_AUDIO_SAMPLES_PER_SECOND")
					{
						auto lSingleValue = lItem.node().select_node(L"SingleValue");

						if (!lSingleValue.node().empty())
						{
							UINT32 lvalue = lSingleValue.node().attribute(L"Value").as_uint();

							LOG_INVOKE_MF_METHOD(SetUINT32, aPtrAudioMediaType,
								MF_MT_AUDIO_SAMPLES_PER_SECOND,
								lvalue);
						}
					}
					else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_AUDIO_BLOCK_ALIGNMENT")
					{
						auto lSingleValue = lItem.node().select_node(L"SingleValue");

						if (!lSingleValue.node().empty())
						{
							UINT32 lvalue = lSingleValue.node().attribute(L"Value").as_uint();

							LOG_INVOKE_MF_METHOD(SetUINT32, aPtrAudioMediaType,
								MF_MT_AUDIO_BLOCK_ALIGNMENT,
								lvalue);
						}
					}
					else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_AUDIO_AVG_BYTES_PER_SECOND")
					{
						auto lSingleValue = lItem.node().select_node(L"SingleValue");

						if (!lSingleValue.node().empty())
						{
							UINT32 lvalue = lSingleValue.node().attribute(L"Value").as_uint();

							LOG_INVOKE_MF_METHOD(SetUINT32, aPtrAudioMediaType,
								MF_MT_AUDIO_AVG_BYTES_PER_SECOND,
								lvalue);
						}
					}
					else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_AUDIO_BITS_PER_SAMPLE")
					{
						auto lSingleValue = lItem.node().select_node(L"SingleValue");

						if (!lSingleValue.node().empty())
						{
							UINT32 lvalue = lSingleValue.node().attribute(L"Value").as_uint();

							LOG_INVOKE_MF_METHOD(SetUINT32, aPtrAudioMediaType,
								MF_MT_AUDIO_BITS_PER_SAMPLE,
								lvalue);
						}
					}
					else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"MF_MT_ALL_SAMPLES_INDEPENDENT")
					{
						auto lSingleValue = lItem.node().select_node(L"SingleValue");

						if (!lSingleValue.node().empty())
						{
							BOOL lState = FALSE;

							if (std::wstring(lSingleValue.node().attribute(L"Value").value()) == L"True")
							{
								lState = TRUE;
							}

							LOG_INVOKE_MF_METHOD(SetUINT32, aPtrAudioMediaType,
								MF_MT_ALL_SAMPLES_INDEPENDENT,
								lState);
						}
					}
					else if (!lAttribute.empty() && std::wstring(lAttribute.value()) == L"CM_DURATION_PERIOD")
					{
						auto lSingleValue = lItem.node().select_node(L"SingleValue");

						if (!lSingleValue.node().empty())
						{
							UINT64 lvalue = lSingleValue.node().attribute(L"Value").as_ullong();
							
							LOG_INVOKE_MF_METHOD(SetUINT64, aPtrAudioMediaType,
								CM_DURATION_PERIOD,
								lvalue);
						}
					}

					
				}

			} while (false);

			return lresult;
		}

		HRESULT InitilaizeCaptureSource::processStreamDescriptor(xml_node& aStreamDescriptor,
			StreamMediaType& aStreamMediaType)
		{
			HRESULT lresult(E_FAIL);

			if (aStreamDescriptor.empty())
				return lresult;

			auto lAttribute = aStreamDescriptor.attribute(L"MajorType");

			if (lAttribute.empty())
				return lresult;		

			mStreamName = L"Unknown";

			if (std::wstring(lAttribute.value()) == L"MFMediaType_Video")
			{
				auto lMediaTypeNodes = aStreamDescriptor.select_nodes(L"MediaTypes/MediaType");

				for (auto& lItem : lMediaTypeNodes)
				{
					auto lAttribute = lItem.node().attribute(L"Index");

					if (!lAttribute.empty())
					{
						CComPtrCustom<IMFMediaType> lVideoMediaType;

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
							&lVideoMediaType);

						LOG_INVOKE_FUNCTION(processVideoMediaType,
							lItem.node(),
							lVideoMediaType);

						auto lIndex = lAttribute.as_uint();

						aStreamMediaType[lIndex] = lVideoMediaType;
					}
				}

				mStreamName = L"Video stream";

				mDeviceType = DeviceType_Video;
			}
			else if (std::wstring(lAttribute.value()) == L"MFMediaType_Audio")
			{
				auto lMediaTypeNodes = aStreamDescriptor.select_nodes(L"MediaTypes/MediaType");

				for (auto& lItem : lMediaTypeNodes)
				{
					auto lAttribute = lItem.node().attribute(L"Index");

					if (!lAttribute.empty())
					{
						CComPtrCustom<IMFMediaType> lAudioMediaType;

						LOG_INVOKE_MF_FUNCTION(MFCreateMediaType,
							&lAudioMediaType);

						LOG_INVOKE_FUNCTION(processAudioMediaType,
							lItem.node(),
							lAudioMediaType);

						auto lIndex = lAttribute.as_uint();

						aStreamMediaType[lIndex] = lAudioMediaType;
					}
				}

				mStreamName = L"Audio stream";

				mDeviceType = DeviceType_Audio;
			}
						
			auto lAttributes = aStreamDescriptor.select_nodes(L"Attribute");

			for (auto& lItem : lAttributes)
			{
				if (!lItem.node().empty())
				{
					auto lNameAttribute = lItem.node().attribute(L"Name");

					if (std::wstring(lNameAttribute.value()) == L"MF_SD_STREAM_NAME")
					{
						auto lSingleValue = lItem.node().select_node(L"SingleValue");

						if (!lSingleValue.node().empty())
						{
							if (!lSingleValue.node().attribute(L"Value").empty())
								mStreamName = std::wstring(lSingleValue.node().attribute(L"Value").value());
						}
					}
				}
			}

			return lresult;
		}

		//    IInitilaizeCaptureSource methods
		HRESULT STDMETHODCALLTYPE InitilaizeCaptureSource::setPresentationDescriptor(
			/* [in] */ BSTR aXMLPresentationDescriptor)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				UINT32 StreamCount = 0;

				xml_document lxmlDoc;

				auto lXMLRes = lxmlDoc.load_string(aXMLPresentationDescriptor);

				if (lXMLRes.status == xml_parse_status::status_ok)
				{
					auto ldocument = lxmlDoc.document_element();

					if (!ldocument.empty() && std::wstring(ldocument.name()) == L"PresentationDescriptor")
					{
						auto lAttribute = ldocument.attribute(L"StreamCount");

						if (!lAttribute.empty())
						{
							StreamCount = lAttribute.as_uint(1);
						}

						auto lNodes = ldocument.select_nodes(L"StreamDescriptor");

						for (auto& lItem : lNodes)
						{
							lAttribute = lItem.node().attribute(L"Index");

							if (!lAttribute.empty())
							{
								auto lIndex = lAttribute.as_uint(0);

								if (lIndex == 0)
								{
									StreamMediaType lStreamMediaType;

									LOG_INVOKE_FUNCTION(processStreamDescriptor,
										lItem.node(),
										lStreamMediaType);

									mStreams[lIndex] = lStreamMediaType;
								}
							}
						}
					}
				}

			} while (false);

			return lresult;

		}

		std::unordered_map<DWORD, StreamMediaType> mStreams;

		HRESULT InitilaizeCaptureSource::getOutputTexture(ID3D11Texture2D** aPtrPtrOutputTexture)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(mID3D11Texture2D);

				LOG_CHECK_PTR_MEMORY(aPtrPtrOutputTexture);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(mID3D11Texture2D, aPtrPtrOutputTexture);

			} while (false);

			return lresult;
		}

		HRESULT InitilaizeCaptureSource::getSharedTexture(ID3D11Texture2D** aPtrPtrSharedTexture)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				if (!mSharedID3D11Texture2D)
				{
					lresult = S_OK;

					break;
				}

				LOG_CHECK_PTR_MEMORY(aPtrPtrSharedTexture);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(mSharedID3D11Texture2D, aPtrPtrSharedTexture);

			} while (false);

			return lresult;
		}

		HRESULT InitilaizeCaptureSource::getCaptureTexture(ID3D11Texture2D** aPtrPtrCaptureTexture)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(mCaptureID3D11Texture2D);

				LOG_CHECK_PTR_MEMORY(aPtrPtrCaptureTexture);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(mCaptureID3D11Texture2D, aPtrPtrCaptureTexture);

			} while (false);

			return lresult;
		}

		HRESULT InitilaizeCaptureSource::getImmediateContext(ID3D11DeviceContext** aPtrPtrImmediateContext)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(mID3D11Device);

				LOG_CHECK_PTR_MEMORY(aPtrPtrImmediateContext);

				CComPtrCustom<ID3D11DeviceContext> lImmediateContext;

				mID3D11Device->GetImmediateContext(&lImmediateContext);

				LOG_CHECK_PTR_MEMORY(lImmediateContext);

				CComPtrCustom<ID3D10Multithread> lMultiThread;

				// Need to explitly set the multithreaded mode for this device
				LOG_INVOKE_QUERY_INTERFACE_METHOD(lImmediateContext, &lMultiThread);

				LOG_CHECK_PTR_MEMORY(lMultiThread);

				BOOL lbRrsult = lMultiThread->SetMultithreadProtected(TRUE);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(lImmediateContext, aPtrPtrImmediateContext);

			} while (false);

			return lresult;
		}

		HRESULT InitilaizeCaptureSource::getDeviceManager(IMFDXGIDeviceManager** aPtrPtrDeviceManager)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(mDeviceManager);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(mDeviceManager, aPtrPtrDeviceManager);

			} while (false);

			return lresult;
		}

		HRESULT InitilaizeCaptureSource::getMixer(IMFTransform** aPtrPtrMixer)
		{
			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(mMixer);

				LOG_INVOKE_QUERY_INTERFACE_METHOD(mMixer, aPtrPtrMixer);

			} while (false);

			return lresult;
		}

		INT InitilaizeCaptureSource::getDeviceType()
		{
			return mDeviceType;
		}


		InitilaizeCaptureSource::~InitilaizeCaptureSource()
		{
		}
	}
}