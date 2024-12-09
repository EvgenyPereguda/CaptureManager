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

#include "MediaFoundationManager.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../LogPrintOut/LogPrintOut.h"
#include <VersionHelpers.h>

namespace CaptureManager
{
	namespace Core
	{
		namespace MediaFoundation
		{

#ifndef CASTFUNCTION
#define CASTFUNCTION(Function){\
		auto lPtrFunc = GetProcAddress(aModule, #Function);\
 if (lPtrFunc != nullptr){\
auto lFunc = (CaptureManager::Core::MediaFoundation::Function)(lPtrFunc);\
if (lFunc != nullptr)Function = lFunc;\
 }\
	}\
		
#endif

#ifndef BINDFUNCTION
#define BINDFUNCTION(Function)Function MediaFoundationManager::Function = MediaFoundationManager::stub##Function
#endif
		
		std::vector<HMODULE> MediaFoundationManager::mModules;

		HRESULT MediaFoundationManager::mResult = E_FAIL;
		
		BINDFUNCTION(MFStartup);

		BINDFUNCTION(MFCreateMediaType);
		
		BINDFUNCTION(MFGetStrideForBitmapInfoHeader);

		BINDFUNCTION(MFCalculateImageSize);

		BINDFUNCTION(MFCreateVideoMediaTypeFromBitMapInfoHeaderEx);

		BINDFUNCTION(MFInitAMMediaTypeFromMFMediaType);

		BINDFUNCTION(MFPutWorkItem);

		BINDFUNCTION(MFInitMediaTypeFromWaveFormatEx);

		BINDFUNCTION(MFCreateTopologyNode);

		BINDFUNCTION(MFTEnumEx);

		BINDFUNCTION(MFTGetInfo);

		BINDFUNCTION(MFCreateAttributes);

		BINDFUNCTION(MFCreateDeviceSourceActivate);

		BINDFUNCTION(MFCreateTopology);

		BINDFUNCTION(MFCreateVideoSampleFromSurface);

		BINDFUNCTION(MFCreateDXSurfaceBuffer);

		BINDFUNCTION(MFCreateEventQueue);

		BINDFUNCTION(MFCreatePresentationClock);

		BINDFUNCTION(MFCreateSystemTimeSource);

		BINDFUNCTION(MFCreateMediaEvent);

		BINDFUNCTION(MFAllocateWorkQueueEx);

		BINDFUNCTION(MFUnlockWorkQueue);

		BINDFUNCTION(MFCreateStreamDescriptor);

		BINDFUNCTION(MFEnumDeviceSources);

		BINDFUNCTION(MFCreatePresentationDescriptor);

		BINDFUNCTION(MFCreateSampleGrabberSinkActivate);

		BINDFUNCTION(MFCreateVideoRendererActivate);

		BINDFUNCTION(MFCreateAudioRendererActivate);

		BINDFUNCTION(MFCreateASFContentInfo);

		BINDFUNCTION(MFCreateASFProfile);

		BINDFUNCTION(MFCreateASFMediaSinkActivate);

		BINDFUNCTION(MFCreateASFStreamingMediaSinkActivate);

		BINDFUNCTION(MFCreateWaveFormatExFromMFMediaType);

		BINDFUNCTION(MFCreateSample);

		BINDFUNCTION(MFCreateMemoryBuffer);

		BINDFUNCTION(MFShutdown);

		BINDFUNCTION(MFCreateAudioMediaType);

		BINDFUNCTION(MFCopyImage);

		BINDFUNCTION(MFCreateMediaSession);

		BINDFUNCTION(MFCreateMPEG4MediaSink);

		BINDFUNCTION(MFCreateFMPEG4MediaSink);

		BINDFUNCTION(MFCreateFile);

		BINDFUNCTION(MFLockWorkQueue);

		BINDFUNCTION(MFLockSharedWorkQueue);

		BINDFUNCTION(MFCreateSampleCopierMFT);

		BINDFUNCTION(MFAllocateSerialWorkQueue);

		BINDFUNCTION(MFGetSystemTime);

		BINDFUNCTION(MFInitMediaTypeFromAMMediaType);

		BINDFUNCTION(MFFrameRateToAverageTimePerFrame);

		BINDFUNCTION(MFCreateAsyncResult);

		BINDFUNCTION(MFPutWaitingWorkItem);

		BINDFUNCTION(MFCancelWorkItem);

		BINDFUNCTION(MFCreateVideoSampleAllocatorEx);

		BINDFUNCTION(MFCreateVideoSampleAllocator);

		BINDFUNCTION(MFCreateDXGIDeviceManager);

		BINDFUNCTION(MFInitVideoFormat_RGB);

		BINDFUNCTION(MFCreateVideoMediaType);

		BINDFUNCTION(MFCreateDXGISurfaceBuffer);		

		BINDFUNCTION(MFMapDX9FormatToDXGIFormat);

		BINDFUNCTION(MFMapDXGIFormatToDX9Format);

		BINDFUNCTION(MFGetService);

		BINDFUNCTION(MFScheduleWorkItemEx);

		BINDFUNCTION(MFRegisterPlatformWithMMCSS);

		BINDFUNCTION(MFUnregisterPlatformFromMMCSS);

		
		
						
		MediaFoundationManager::MediaFoundationManager()
		{
			
		}
		
		MediaFoundationManager::~MediaFoundationManager()
		{
		}

		// Initialize Media Foundation support
		HRESULT MediaFoundationManager::initialize()
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (SUCCEEDED(mResult))
				{
					lresult = mResult;

					break;
				}

				
				lresult = loadLibraries();

				if (FAILED(lresult))
					break;
				
				lresult = fillPtrFuncCollection();

				if (FAILED(lresult))
					break;
				
				LOG_INVOKE_MF_FUNCTION(MFStartup, MF_VERSION, MFSTARTUP_FULL);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(LogPrintOut::ERROR_LEVEL, L"MediaFoundationManager: Media Foundation cannot be initialized!!!");
			}
			else
			{
				LogPrintOut::getInstance().printOutln(LogPrintOut::INFO_LEVEL, L"MediaFoundationManager: Media Foundation is initialized!!!");
			}

			return mResult = lresult;
		}

		// load needed libraries dynamically;
		HRESULT MediaFoundationManager::loadLibraries()
		{

			HRESULT lresult = E_FAIL;

			do
			{

				auto lModule = LoadLibrary(L"Mfplat.dll");

				if (lModule == nullptr)
				{
					lresult = E_HANDLE;
				}

				mModules.push_back(lModule);

				lModule = LoadLibrary(L"MFPlay.dll");

				if (lModule == nullptr)
				{
					lresult = E_HANDLE;
				}

				mModules.push_back(lModule);

				lModule = LoadLibrary(L"MF.dll");

				if (lModule == nullptr)
				{
					lresult = E_HANDLE;
				}

				mModules.push_back(lModule);

				lModule = LoadLibrary(L"MFreadwrite.dll");

				if (lModule == nullptr)
				{
					lresult = E_HANDLE;
				}

				mModules.push_back(lModule);
				
				lModule = LoadLibrary(L"evr.dll");

				if (lModule == nullptr)
				{
					lresult = E_HANDLE;
				}

				mModules.push_back(lModule);

				lModule = LoadLibrary(L"msdmo.dll");

				if (lModule == nullptr)
				{
					lresult = E_HANDLE;
				}

				mModules.push_back(lModule);
				
				lresult = S_OK;

			} while (false);

			return lresult;
		}

		// init collection of pointers on Media Foundation functions
		HRESULT MediaFoundationManager::initFunctions(HMODULE aModule)
		{

			HRESULT lresult = E_FAIL;

			do
			{				
				CASTFUNCTION(MFStartup);

				CASTFUNCTION(MFCreateMediaType);			
				
				CASTFUNCTION(MFGetStrideForBitmapInfoHeader);

				CASTFUNCTION(MFCalculateImageSize);

				CASTFUNCTION(MFCreateVideoMediaTypeFromBitMapInfoHeaderEx);

				CASTFUNCTION(MFInitAMMediaTypeFromMFMediaType);

				CASTFUNCTION(MFPutWorkItem);

				CASTFUNCTION(MFInitMediaTypeFromWaveFormatEx);

				CASTFUNCTION(MFCreateTopologyNode);

				CASTFUNCTION(MFTEnumEx);

				CASTFUNCTION(MFTGetInfo);

				CASTFUNCTION(MFCreateAttributes);

				CASTFUNCTION(MFEnumDeviceSources);

				CASTFUNCTION(MFCreateDeviceSourceActivate);

				CASTFUNCTION(MFCreateTopology);

				CASTFUNCTION(MFCreateVideoSampleFromSurface);

				CASTFUNCTION(MFCreateDXSurfaceBuffer);

				CASTFUNCTION(MFCreateEventQueue);

				CASTFUNCTION(MFCreatePresentationClock);

				CASTFUNCTION(MFCreateSystemTimeSource);

				CASTFUNCTION(MFCreateMediaEvent);

				CASTFUNCTION(MFAllocateWorkQueueEx);

				CASTFUNCTION(MFUnlockWorkQueue);

				CASTFUNCTION(MFCreateStreamDescriptor);

				CASTFUNCTION(MFCreatePresentationDescriptor);

				CASTFUNCTION(MFCreateSampleGrabberSinkActivate);

				CASTFUNCTION(MFCreateVideoRendererActivate);

				CASTFUNCTION(MFCreateAudioRendererActivate);				

				CASTFUNCTION(MFCreateASFContentInfo);

				CASTFUNCTION(MFCreateASFProfile);

				CASTFUNCTION(MFCreateASFMediaSinkActivate);

				CASTFUNCTION(MFCreateASFStreamingMediaSinkActivate);

				CASTFUNCTION(MFCreateWaveFormatExFromMFMediaType);

				CASTFUNCTION(MFCreateSample);

				CASTFUNCTION(MFCreateMemoryBuffer);

				CASTFUNCTION(MFShutdown);

				CASTFUNCTION(MFCreateAudioMediaType);

				CASTFUNCTION(MFCopyImage);

				CASTFUNCTION(MFCreateMediaSession);

				CASTFUNCTION(MFCreateMPEG4MediaSink);

				CASTFUNCTION(MFCreateFMPEG4MediaSink);							

				CASTFUNCTION(MFCreateFile);

				CASTFUNCTION(MFLockWorkQueue);

				CASTFUNCTION(MFLockSharedWorkQueue);

				CASTFUNCTION(MFCreateSampleCopierMFT);

				CASTFUNCTION(MFAllocateSerialWorkQueue);

				CASTFUNCTION(MFGetSystemTime);

				CASTFUNCTION(MFInitMediaTypeFromAMMediaType);

				CASTFUNCTION(MFFrameRateToAverageTimePerFrame);

				CASTFUNCTION(MFCreateAsyncResult);

				CASTFUNCTION(MFPutWaitingWorkItem);

				CASTFUNCTION(MFCancelWorkItem);

				CASTFUNCTION(MFCreateVideoSampleAllocatorEx);

				CASTFUNCTION(MFCreateVideoSampleAllocator);

				CASTFUNCTION(MFCreateDXGIDeviceManager);

				CASTFUNCTION(MFInitVideoFormat_RGB);

				CASTFUNCTION(MFCreateVideoMediaType);

				CASTFUNCTION(MFCreateDXGISurfaceBuffer);

				CASTFUNCTION(MFMapDX9FormatToDXGIFormat);

				CASTFUNCTION(MFMapDXGIFormatToDX9Format);

				CASTFUNCTION(MFGetService);

				CASTFUNCTION(MFScheduleWorkItemEx);

				CASTFUNCTION(MFRegisterPlatformWithMMCSS);

				CASTFUNCTION(MFUnregisterPlatformFromMMCSS);

				
				
				lresult = S_OK;

			} while (false);
			
			return lresult;			
		}

		// fill collection of pointers on Media Foundation functions
		HRESULT MediaFoundationManager::fillPtrFuncCollection()
		{
			HRESULT lresult = E_FAIL;

			do
			{
				for (auto& lModile : mModules)
				{
					initFunctions(lModile);
				}
				
				lresult = S_OK;

			} while (false);

			return lresult;
		}

		// Uninitialize Media Foundation support
		HRESULT MediaFoundationManager::unInitialize()
		{

			HRESULT lresult(S_OK);

			do
			{
			//	LOG_INVOKE_MF_FUNCTION(MFShutdown);

			} while (false);

			LogPrintOut::getInstance().printOutlnUnlock(LogPrintOut::INFO_LEVEL, L"MediaFoundationManager: Media Foundation is shutdowned!!!\n");

			//if (FAILED(lresult))
			//{
			//	LogPrintOut::getInstance().printOutln(LogPrintOut::ERROR_LEVEL, L"MediaFoundationManager: Media Foundation cannot be shutdowned!!!");
			//}
			//else
			//{
			//	LogPrintOut::getInstance().printOutln(LogPrintOut::INFO_LEVEL, L"MediaFoundationManager: Media Foundation is shutdowned!!!");
			//}

			mResult = MF_E_SHUTDOWN;

			return lresult;
		}

		HRESULT MediaFoundationManager::GetStatus()
		{
			return mResult;
		}
		
		HRESULT MediaFoundationManager::DetachObject(
			IMFActivate* aPtrActivate)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrActivate == nullptr)
				{
					lresult = E_POINTER;

					break;
				}
				
				lresult = aPtrActivate->DetachObject();

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;

		}

		HRESULT MediaFoundationManager::Shutdown(
			IMFMediaSource* aPtrMediaSource)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaSource == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaSource->Shutdown();

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;

		}

		HRESULT MediaFoundationManager::Shutdown(
			IMFMediaSink* aPtrMediaSink)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaSink == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaSink->Shutdown();

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;

		}

		HRESULT MediaFoundationManager::AddBuffer(
			IMFSample* aPtrSample,
			IMFMediaBuffer* aPtrlMediaBuffer)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrSample == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrlMediaBuffer == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrSample->AddBuffer(aPtrlMediaBuffer);
				
			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;

		}

		HRESULT MediaFoundationManager::GetInputAvailableType(
			IMFTransform* aPtrTransform,
			DWORD aInputStreamID,
			DWORD aTypeIndex,
			IMFMediaType** aPtrPtrType)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTransform == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTransform->GetInputAvailableType(
					aInputStreamID,
					aTypeIndex,
					aPtrPtrType);
				
			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::SetSampleDuration(
			IMFSample* aPtrSample,
			LONGLONG aSampleDuration)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrSample == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrSample->SetSampleDuration(aSampleDuration);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::SetSampleTime(
			IMFSample* aPtrSample,
			LONGLONG aSampleTime)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrSample == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrSample->SetSampleTime(aSampleTime);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}
		
		HRESULT MediaFoundationManager::SetUINT64(
			IMFSample* aPtrSample,
			REFGUID aRefGUIDKey,
			UINT64 aValue)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrSample == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrSample->SetUINT64(
					aRefGUIDKey,
					aValue);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::Clone(
			IMFPresentationDescriptor* aPtrOriginalPresentationDescriptor,
			IMFPresentationDescriptor** aPtrPtrPresentationDescriptor)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrOriginalPresentationDescriptor == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrPresentationDescriptor == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrOriginalPresentationDescriptor->Clone(
					aPtrPtrPresentationDescriptor);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::SetGUID(
			IMFAttributes* aPtrAttributes, 
			REFGUID aGUIDKey, 
			REFGUID aGUIDValue)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->SetGUID(
					aGUIDKey, 
					aGUIDValue);

			} while (false);
						
			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::SetCurrentLength(
			IMFMediaBuffer* aPtrMediaBuffer,
			DWORD aValue)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaBuffer == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaBuffer->SetCurrentLength(aValue);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::Lock(
			IMFMediaBuffer* aPtrMediaBuffer,
			BYTE** aPtrPtrBuffer,
			DWORD* aPtrMaxLength,
			DWORD* aPtrCurrentLength)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaBuffer == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrBuffer == nullptr)
				{
					lresult = E_POINTER;

					break;
				}
				
				lresult = aPtrMediaBuffer->Lock(
					aPtrPtrBuffer,
					aPtrMaxLength,
					aPtrCurrentLength);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::Unlock(
			IMFMediaBuffer* aPtrMediaBuffer)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaBuffer == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaBuffer->Unlock();

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

		
		HRESULT MediaFoundationManager::GetGUID(
			IMFAttributes *aPtrAttributes, 
			REFGUID aGUIDKey, 
			GUID* aPtrGUIDValue)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrGUIDValue == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->GetGUID(
					aGUIDKey, 
					aPtrGUIDValue);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);					
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::CopyAllItems(
			IMFAttributes* aPtrSourceAttributes,
			IMFAttributes* aPtrDestinationAttributes)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrSourceAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrDestinationAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrSourceAttributes->CopyAllItems(aPtrDestinationAttributes);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::DeleteItem(
			IMFAttributes *aPtrSourceAttributes,
			REFGUID aGUIDKey)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrSourceAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrSourceAttributes->DeleteItem(aGUIDKey);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::SetBlob(
			IMFAttributes* aPtrAttributes,
			REFGUID aGUIDKey,
			UINT8* aPtrBlob,
			UINT32 aBlobSize)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->SetBlob(
					aGUIDKey,
					aPtrBlob,
					aBlobSize);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::SetUINT32(
			IMFAttributes* aPtrAttributes, 
			REFGUID aGUIDKey, 
			UINT32 aValue)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->SetUINT32(
					aGUIDKey, 
					aValue);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}
		
		HRESULT MediaFoundationManager::SetAttributeRatio(
			IMFAttributes* aPtrAttributes,
			REFGUID aGUIDKey,
			UINT32 aNumerator,
			UINT32 aDenominator)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = MFSetAttributeRatio(
					aPtrAttributes, 
					aGUIDKey, 
					aNumerator, 
					aDenominator);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::SetAttributeSize(
			IMFAttributes* aPtrAttributes,
			REFGUID aGUIDKey,
			UINT32 aWidth,
			UINT32 aHeight)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = MFSetAttributeSize(
					aPtrAttributes, 
					aGUIDKey, 
					aWidth, 
					aHeight);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}
		
		HRESULT MediaFoundationManager::GetAttributeSize(
			IMFAttributes* aPtrAttributes,
			REFGUID aGUIDKey,
			UINT32& aRefWidth,
			UINT32& aRefHeight)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = MFGetAttributeSize(
					aPtrAttributes,
					aGUIDKey,
					&aRefWidth,
					&aRefHeight);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::GetOutputStreamInfo(
			IMFTransform* aPtrIMFTransform,
			DWORD aOutputStreamID,
			MFT_OUTPUT_STREAM_INFO* aPtrStreamInfo)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrIMFTransform == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrStreamInfo == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrIMFTransform->GetOutputStreamInfo(
					aOutputStreamID,
					aPtrStreamInfo);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}


		HRESULT MediaFoundationManager::GetItem(
			IMFAttributes* aPtrAttributes,
			REFGUID aGUIDKey,
			PROPVARIANT* aPtrVarItem)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrVarItem == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->GetItem(
					aGUIDKey, 
					aPtrVarItem);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::SetItem(
			IMFAttributes* aPtrAttributes,
			REFGUID aGUIDKey,
			PROPVARIANT& lVarItem)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->SetItem(
					aGUIDKey, 
					lVarItem);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::CompareItem(
			IMFAttributes* aPtrAttributes,
			REFGUID aGUIDKey,
			REFPROPVARIANT aRefValue,
			BOOL* aPtrResult)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrResult == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->CompareItem(
					aGUIDKey,
					aRefValue,
					aPtrResult);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::isCompressedFormat(
			IMFMediaType* aPtrAttributes,
			BOOL& aRefResult)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->IsCompressedFormat(&aRefResult);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::GetUINT32(
			IMFAttributes* aPtrAttributes, 
			REFGUID aGUIDKey, 
			UINT32* aPtrValue)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrValue == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->GetUINT32(
					aGUIDKey, 
					aPtrValue);

			} while (false);
			
			return lresult;
		}
		
		HRESULT MediaFoundationManager::SetUINT64(
			IMFAttributes* aPtrAttributes,
			REFGUID aGUIDKey,
			UINT64 aValue)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}
				
				lresult = aPtrAttributes->SetUINT64(
					aGUIDKey,
					aValue);

				if (FAILED(lresult))
				{
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL,
						__FUNCTIONW__,
						L" Error code: ",
						lresult);
				}

			} while (false);

			return lresult;
		}

		HRESULT MediaFoundationManager::GetUINT64(
			IMFAttributes* aPtrAttributes,
			REFGUID aGUIDKey,
			UINT64* aPtrValue)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrValue == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->GetUINT64(
					aGUIDKey,
					aPtrValue);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}
		

		HRESULT MediaFoundationManager::SetUnknown(
			IMFAttributes* aPtrAttributes,
			REFGUID aRefGUIDKey,
			IUnknown* aPtrUnknown)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrUnknown == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->SetUnknown(aRefGUIDKey, aPtrUnknown);
				
			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::GetUnknown(
			IMFAttributes* aPtrAttributes,
			REFGUID aRefGUIDKey,
			REFIID aRefIID,
			void** aPtrPtrUnknown)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrUnknown == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->GetUnknown(
					aRefGUIDKey, 
					aRefIID, 
					(void**)aPtrPtrUnknown);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::SetString(
			IMFAttributes* aPtrAttributes,
			REFGUID aGUIDKey,
			LPCWSTR aPtrStringValue)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrStringValue == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->SetString(
					aGUIDKey, 
					aPtrStringValue);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::GetAllocatedString(
			IMFAttributes* aPtrAttributes, 
			REFGUID aGUIDKey,
			std::wstring& aStringValue)
		{
			HRESULT lresult = E_FAIL;

			wchar_t *lPtrStringValue = nullptr;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				UINT32  lStringhLength = 0;

				lresult = aPtrAttributes->GetAllocatedString(
					aGUIDKey,
					&lPtrStringValue,
					&lStringhLength);

				if (FAILED(lresult))
				{
					break;
				}

				aStringValue = std::wstring(lPtrStringValue);

				if (lPtrStringValue != nullptr)
					CoTaskMemFree(lPtrStringValue);
				
			} while (false);

			return lresult;
		}
		
		HRESULT MediaFoundationManager::GetSource(
			IMFActivate* aPtrSourceActivate, 
			IMFMediaSource** aPtrPtrMediaSource)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrSourceActivate == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrMediaSource == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				LOG_INVOKE_MF_METHOD(ActivateObject,
					aPtrSourceActivate,
					__uuidof(IMFMediaSource),
					(void**)aPtrPtrMediaSource
					);

				LOG_INVOKE_MF_METHOD(DetachObject,
					aPtrSourceActivate);
					
			} while (false);
			
			return lresult;
		}

		HRESULT MediaFoundationManager::ActivateObject(
			IMFActivate* aPtrActivate,
			REFIID aRefIID,
			void** aPtrPtrObject)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrActivate == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrObject == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrActivate->ActivateObject(
					aRefIID,
					aPtrPtrObject);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::SetInputType(
			IMFTransform* aPtrTransform,
			DWORD aInputStreamID,
			IMFMediaType* aPtrType,
			DWORD aFlags)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTransform == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTransform->SetInputType(
					aInputStreamID,
					aPtrType,
					aFlags);
				
				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::GetStreamSinkById(
			IMFMediaSink* aPtrPtrMediaSink,
			DWORD aStreamSinkIdentifier,
			IMFStreamSink** aPtrPtrStreamSink)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrPtrMediaSink == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrStreamSink == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrPtrMediaSink->GetStreamSinkById(
					aStreamSinkIdentifier,
					aPtrPtrStreamSink);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::GetStreamSinkByIndex(
			IMFMediaSink* aPtrPtrMediaSink,
			DWORD aIndex,
			IMFStreamSink** aPtrPtrStreamSink)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrPtrMediaSink == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrStreamSink == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrPtrMediaSink->GetStreamSinkByIndex(
					aIndex,
					aPtrPtrStreamSink);
				
			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::AddStreamSink(
			IMFMediaSink* aPtrPtrMediaSink,
			DWORD aStreamSinkIdentifier,
			IMFMediaType* aPtrMediaType,
			IMFStreamSink** aPtrPtrStreamSink)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrPtrMediaSink == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrStreamSink == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrPtrMediaSink->AddStreamSink(
					aStreamSinkIdentifier,
					aPtrMediaType,
					aPtrPtrStreamSink);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::CreatePresentationDescriptor(
			IMFMediaSource* aPtrMediaSource,
			IMFPresentationDescriptor** aPtrPtrPresentationDescriptor)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaSource == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrPresentationDescriptor == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaSource->CreatePresentationDescriptor(aPtrPtrPresentationDescriptor);
				
			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}
						
		HRESULT MediaFoundationManager::GetStreamDescriptorCount(
			IMFPresentationDescriptor* aPtrPresentationDescriptor,
			DWORD* aPtrStreamCount)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrPresentationDescriptor == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrStreamCount == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrPresentationDescriptor->GetStreamDescriptorCount(aPtrStreamCount);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetStreamDescriptorByIndex(
			IMFPresentationDescriptor* aPtrPD,
			DWORD aIndex,
			BOOL* aPtrIsSelected,
			IMFStreamDescriptor** aPtrPtrSD)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrPD == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrIsSelected == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrSD == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrPD->GetStreamDescriptorByIndex(
					aIndex, 
					aPtrIsSelected,
					aPtrPtrSD);
				
			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}
  
  HRESULT MediaFoundationManager::GetStreamDescriptor(
	  IMFMediaStream* aPtrIMFMediaStream,
	  IMFStreamDescriptor** aPtrPtrSD)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrIMFMediaStream == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrPtrSD == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrIMFMediaStream->GetStreamDescriptor(aPtrPtrSD);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetStreamIdentifier(
	  IMFStreamDescriptor* aPtrIMFStreamDescriptor,
	  DWORD* aPtrStreamIdentifier)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrIMFStreamDescriptor == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrStreamIdentifier == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrIMFStreamDescriptor->GetStreamIdentifier(
			  aPtrStreamIdentifier);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::SelectStream(
			IMFPresentationDescriptor* aPtrPD,
			int mIndexStream)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrPD == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (mIndexStream < 0)
				{
					lresult = MF_E_INVALIDSTREAMNUMBER;
					
					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL, 
						__FUNCTIONW__,
						L" - stream with index: ",
						mIndexStream, 
						L" is wrong.");

					break;
				}

				lresult = aPtrPD->SelectStream(mIndexStream);

				
			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::SetCurrentMediaType(
			IMFStreamDescriptor* aPtrSD,
			int aIndexMediaType)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrSD == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				CComPtrCustom<IMFMediaTypeHandler> lHandler;

				LOG_INVOKE_MF_METHOD(GetMediaTypeHandler,
					aPtrSD, &lHandler);
				
				if (aIndexMediaType < 0)
				{
					lresult = E_INVALIDARG;

					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL, 
						__FUNCTIONW__,
						L" - MediaType with index: ",
						aIndexMediaType, 
						L" is less zero.");

					break;
				}

				DWORD lMediaTypeCount = 0;

				LOG_INVOKE_MF_METHOD(GetMediaTypeCount,
					lHandler, 
					&lMediaTypeCount);
				
				if ((DWORD)aIndexMediaType >= lMediaTypeCount)
				{
					lresult = E_INVALIDARG;

					LogPrintOut::getInstance().printOutln(
						LogPrintOut::ERROR_LEVEL, 
						__FUNCTIONW__,
						L" - MediaType with index: ",
						aIndexMediaType, 
						L" is out of range.");

					break;
				}

				CComPtrCustom<IMFMediaType> lMediaType;

				LOG_INVOKE_MF_METHOD(GetMediaTypeByIndex,
					lHandler, (DWORD)aIndexMediaType, &lMediaType);
				
				LOG_INVOKE_MF_METHOD(SetCurrentMediaType,
					lHandler,
					lMediaType);
				
			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::IsEqual(
	  IMFMediaType* aPtrThisMediaType,
	  IMFMediaType* aPtrIMediaType,
	  DWORD* aPtrFlags)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrThisMediaType == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrIMediaType == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrFlags == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrThisMediaType->IsEqual(aPtrIMediaType,
			  aPtrFlags);
		  
	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }
		
  HRESULT MediaFoundationManager::GetCurrentLength(
	  IMFMediaBuffer* aPtrMediaBuffer,
	  DWORD* aPtrCurrentLength)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrMediaBuffer == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrCurrentLength == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }
		  
		  lresult = aPtrMediaBuffer->GetCurrentLength(aPtrCurrentLength);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::SetCurrentMediaType(
			IMFMediaTypeHandler* aMediaTypeHandler,
			IMFMediaType* aPtrMediaType)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aMediaTypeHandler == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrMediaType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}
				
				lresult = aMediaTypeHandler->SetCurrentMediaType(aPtrMediaType);

				if (FAILED(lresult))
				{
					break;
				}
				
			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetMediaTypeHandler(
			IMFStreamDescriptor* aPtrSD, 
			IMFMediaTypeHandler** aPtrPtrMediaTypeHandler)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrSD == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrMediaTypeHandler == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrSD->GetMediaTypeHandler(aPtrPtrMediaTypeHandler);
				
			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetMediaTypeHandler(
			IMFStreamSink* aPtrStreamSink,
			IMFMediaTypeHandler** aPtrPtrMediaTypeHandler)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrStreamSink == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrMediaTypeHandler == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrStreamSink->GetMediaTypeHandler(aPtrPtrMediaTypeHandler);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetMajorType(
			IMFMediaTypeHandler* aPtrMediaTypeHandler,
			GUID* aPtrGUIDMajorType)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaTypeHandler == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrGUIDMajorType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}
				
				lresult = aPtrMediaTypeHandler->GetMajorType(aPtrGUIDMajorType);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetMajorType(
			IMFMediaType* aPtrMediaType,
			GUID* aPtrGUIDMajorType)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrGUIDMajorType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaType->GetMajorType(aPtrGUIDMajorType);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetMediaTypeCount(
			IMFMediaTypeHandler* aPtrMediaTypeHandler,
			DWORD* aPtrMediaTypeCount)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaTypeHandler == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrMediaTypeCount == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaTypeHandler->GetMediaTypeCount(aPtrMediaTypeCount);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetMediaTypeByIndex(
			IMFMediaTypeHandler* aPtrMediaTypeHandler,
			DWORD aMediaTypeIndex, 
			IMFMediaType** aPtrPtrMediaType)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaTypeHandler == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrMediaType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaTypeHandler->GetMediaTypeByIndex(aMediaTypeIndex, aPtrPtrMediaType);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}
		
  HRESULT MediaFoundationManager::LockStore(
			IMFAttributes* aPtrAttributes)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->LockStore();

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::UnlockStore(
			IMFAttributes* aPtrAttributes)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->UnlockStore();

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetCount(
			IMFAttributes* aPtrAttributes, 
			UINT32* aPtrItemCount)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrItemCount == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->GetCount(aPtrItemCount);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetItemByIndex(
			IMFAttributes* aPtrAttributes, 
			UINT32 aIndex,
			GUID* aPtrGUIDKey,
			PROPVARIANT* aPtrValue)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrValue == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrGUIDKey == nullptr)
				{
					lresult = E_POINTER;

					break;
				}
				
				lresult = aPtrAttributes->GetItemByIndex(aIndex, aPtrGUIDKey, aPtrValue);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetAttributes(
			IMFTransform* aPtrTransform,
			IMFAttributes** aPtrPtrAttributes)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTransform == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTransform->GetAttributes(aPtrPtrAttributes);

			} while (false);

			if (FAILED(lresult) && lresult != 0x80004001)
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::enumMediaTypes(
			IMFStreamDescriptor* aPtrSD,
			pugi::xml_node& aRefStreamDescriptorNode)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrSD == nullptr)
				{
					lresult = E_POINTER;

					break;
				}
				
				CComPtrCustom<IMFMediaTypeHandler> lHandler;

				LOG_INVOKE_MF_FUNCTION(GetMediaTypeHandler, aPtrSD, &lHandler);
				
				GUID lMajor_type;

				LOG_INVOKE_MF_FUNCTION(GetMajorType, lHandler.get(), &lMajor_type);
								
				std::wstring lNameGUID;

				lresult = DataParser::GetGUIDName(lMajor_type, lNameGUID);

				if (FAILED(lresult))
					break;
				
				aRefStreamDescriptorNode.append_attribute(L"MajorType") = lNameGUID.c_str();


				WCHAR *lptrName = nullptr;

				lresult = StringFromCLSID(lMajor_type, &lptrName);

				if (SUCCEEDED(lresult))
				{
					aRefStreamDescriptorNode.append_attribute(L"MajorTypeGUID") = lptrName;

					CoTaskMemFree(lptrName);
				}

				struct MediaTypeItem
				{
					CComPtrCustom<IMFMediaType> m_MediaType;

					DWORD m_Index;
				};

				
				DWORD lTypeCount = 0;

				LOG_INVOKE_MF_FUNCTION(GetMediaTypeCount, lHandler, &lTypeCount);

				std::vector<MediaTypeItem> lMediaTypes;

				for (DWORD lTypeIndex = 0; lTypeIndex < lTypeCount; lTypeIndex++)
				{
					CComPtrCustom<IMFMediaType> lMediaType;

					LOG_INVOKE_MF_FUNCTION(GetMediaTypeByIndex, lHandler, lTypeIndex, &lMediaType);

					if (lMediaType != nullptr)
					{
						BOOL l_skipFlag = FALSE;

						for (size_t i = 0; i < lMediaTypes.size(); i++)
						{
							if (lMediaTypes[i].m_MediaType != nullptr)
							{

								UINT32 lBaseItemsCount;

								lMediaTypes[i].m_MediaType->GetCount(&lBaseItemsCount);

								UINT32 lCurrentItemsCount;

								lMediaType->GetCount(&lCurrentItemsCount);

								UINT32 lmin = lBaseItemsCount;

								if (lmin > lCurrentItemsCount)
									lmin = lCurrentItemsCount;

								l_skipFlag = TRUE;
								
								for (size_t i1 = 0; i1 < lmin; i1++)
								{
									GUID lguid = { 0 };

									PROPVARIANT lvar;
									PropVariantInit(&lvar);

									lMediaTypes[i].m_MediaType->GetItemByIndex(i1,
										&lguid,
										&lvar);

									if (lguid == MF_MT_AM_FORMAT_TYPE)
										continue;
									
									lMediaType->CompareItem(lguid, lvar, &l_skipFlag);

									PropVariantClear(&lvar);
									
									if (l_skipFlag == FALSE)
										break;
								}							
							}
							
							if (l_skipFlag != FALSE)
								break;
						}

						if (l_skipFlag != FALSE)
							continue;

						MediaTypeItem l_MediaTypeItem;

						l_MediaTypeItem.m_MediaType = lMediaType;

						l_MediaTypeItem.m_Index = lTypeIndex;

						lMediaTypes.push_back(l_MediaTypeItem);
					}
				}
				
				auto lMediaTypesNode = aRefStreamDescriptorNode.append_child(L"MediaTypes");

				DWORD lTypeIndex = 0;
				
				for (auto& lMediaType: lMediaTypes)
				{
					auto lMediaTypeNode = lMediaTypesNode.append_child(L"MediaType");
					
					lresult = DataParser::readMediaType(lMediaType.m_MediaType, lMediaTypeNode);

					lMediaTypeNode.append_attribute(L"Index") = (unsigned long long)lMediaType.m_Index;

					if (FAILED(lresult))
					{
						lMediaTypesNode.remove_child(lMediaTypeNode);

						lresult = S_OK;

						continue;
					}

					lTypeIndex++;
				}

				lMediaTypesNode.append_attribute(L"TypeCount") = (unsigned long long)lTypeIndex;

			} while (false);

			return lresult;
		}  	

  HRESULT MediaFoundationManager::SetObject(
			IMFTopologyNode* aPtrTopologyNode, 
			IUnknown* aPtrObject)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTopologyNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrObject == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTopologyNode->SetObject(aPtrObject);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::ProcessMessage(
	  IMFTransform* aPtrIMFTransform,
	  MFT_MESSAGE_TYPE aMessage,
	  ULONG_PTR aParam)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  lresult = aPtrIMFTransform->ProcessMessage(aMessage,
			  aParam);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }
  HRESULT MediaFoundationManager::GetObject(
			IMFTopologyNode* aPtrTopologyNode, 
			IUnknown** aPtrPtrObject)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTopologyNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrObject == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTopologyNode->GetObject(aPtrPtrObject);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}
  		
  HRESULT MediaFoundationManager::GetEvent(
	  IMFMediaEventQueue* aPtrIMFMediaEventQueue,
	  DWORD aFlags,
	  IMFMediaEvent ** aPtrPtrEvent)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrIMFMediaEventQueue == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrPtrEvent == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  
		  lresult = aPtrIMFMediaEventQueue->GetEvent(aFlags, aPtrPtrEvent);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::BeginGetEvent(
	  IMFMediaEventQueue* aPtrIMFMediaEventQueue,
	  IMFAsyncCallback *aPtrCallback,
	  IUnknown *aPtrUnkState)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrIMFMediaEventQueue == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrCallback == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }
		  		 
		  lresult = aPtrIMFMediaEventQueue->BeginGetEvent(aPtrCallback,
			  aPtrUnkState);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::EndGetEvent(
	  IMFMediaEventQueue* aPtrIMFMediaEventQueue,
	  IMFAsyncResult *aPtrResult,
	  IMFMediaEvent ** aPtrPtrEvent)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrIMFMediaEventQueue == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrResult == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrPtrEvent == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }
		  
		  lresult = aPtrIMFMediaEventQueue->EndGetEvent(aPtrResult,
			  aPtrPtrEvent);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::QueueEventParamVar(
	  IMFMediaEventQueue* aPtrIMFMediaEventQueue,
	  MediaEventType aMediaEventType,
	  REFGUID aGUIDExtendedType,
	  HRESULT aHRStatus,
	  const PROPVARIANT * aPtrValue)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrIMFMediaEventQueue == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }
		  
		  lresult = aPtrIMFMediaEventQueue->QueueEventParamVar(
			  aMediaEventType,
			  aGUIDExtendedType,
			  aHRStatus,
			  aPtrValue);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::QueueEventParamUnk(
	  IMFMediaEventQueue* aPtrIMFMediaEventQueue,
	  MediaEventType aMediaEventType,
	  REFGUID aRefGUIDExtendedType,
	  HRESULT aHRStatus,
	  IUnknown* aPtrUnk)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrIMFMediaEventQueue == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrIMFMediaEventQueue->QueueEventParamUnk(
			  aMediaEventType,
			  aRefGUIDExtendedType,
			  aHRStatus,
			  aPtrUnk);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetState(
	  IMFAsyncResult* aPtrIMFAsyncResult,
	  IUnknown ** aPtrPtrUnkState)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrIMFAsyncResult == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrPtrUnkState == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrIMFAsyncResult->GetState(
			  aPtrPtrUnkState);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::EndGetEvent(
			IMFMediaEventGenerator* aPtrMediaEventGenerator,
			IMFAsyncResult* aPtrAsyncResult,
			IMFMediaEvent** aPtrPtrMediaEvent)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaEventGenerator == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrAsyncResult == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrMediaEvent == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaEventGenerator->EndGetEvent(aPtrAsyncResult, aPtrPtrMediaEvent);
				
			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}
		
  HRESULT MediaFoundationManager::BeginGetEvent(
			IMFMediaEventGenerator* aPtrMediaEventGenerator,
			IMFAsyncCallback* aPtrAsyncCallback,
			IUnknown* aPtrUnkState)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaEventGenerator == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrAsyncCallback == nullptr)
				{
					lresult = E_POINTER;

					break;
				}
				
				lresult = aPtrMediaEventGenerator->BeginGetEvent(aPtrAsyncCallback, aPtrUnkState);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetType(
	  IMFMediaEvent* aPtrMediaEvent,
	  MediaEventType* aPtrMediaEventType)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaEvent == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrMediaEventType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}				

				lresult = aPtrMediaEvent->GetType(aPtrMediaEventType);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetExtendedType(
			IMFMediaEvent* aPtrMediaEvent,
			GUID& aRefGUIDExtendedType)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaEvent == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaEvent->GetExtendedType(&aRefGUIDExtendedType);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetStatus(
			IMFMediaEvent* aPtrMediaEvent,
			HRESULT* aPtrStatus)
		{
			  HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaEvent == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrStatus == nullptr)
				{
					lresult = E_POINTER;

					break;
				}				

				lresult = aPtrMediaEvent->GetStatus(aPtrStatus);

				if (FAILED(lresult))
				{
					*aPtrStatus = E_FAIL;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}
    
  HRESULT MediaFoundationManager::GetStatus(
	  IMFAsyncResult* aPtrAsyncResult)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrAsyncResult == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }
		  
		  lresult = aPtrAsyncResult->GetStatus();


	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }
  
  HRESULT MediaFoundationManager::GetValue(
			IMFMediaEvent* aPtrMediaEvent,
			PROPVARIANT* aPtrValue)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaEvent == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrValue == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaEvent->GetValue(aPtrValue);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

		HRESULT MediaFoundationManager::AddNode(
			IMFTopology* aPtrTopology,
			IMFTopologyNode* aPtrTopologyNode)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTopology == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrTopologyNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTopology->AddNode(aPtrTopologyNode);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

	HRESULT MediaFoundationManager::ConnectOutput(
			IMFTopologyNode* aPtrSourceTopologyNode,
			DWORD aOutputIndex,
			IMFTopologyNode* aPtrOutputTopologyNode,
			DWORD aIntputIndex)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrSourceTopologyNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrOutputTopologyNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrSourceTopologyNode->ConnectOutput(
					aOutputIndex,
					aPtrOutputTopologyNode,
					aIntputIndex);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}
			
  HRESULT MediaFoundationManager::SetTopology(
			  IMFMediaSession* aPtrMediaSession,
			  DWORD aSetTopologyFlags,
		      IMFTopology* aPtrTopology)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaSession == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrTopology == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaSession->SetTopology(
					aSetTopologyFlags, 
					aPtrTopology);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::Start(
			IMFMediaSession* aPtrMediaSession,
			const GUID* aPtrGUIDTimeFormat,
			const PROPVARIANT *aPtrVarStartPosition)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaSession == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrGUIDTimeFormat == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaSession->Start(
					aPtrGUIDTimeFormat,
					aPtrVarStartPosition);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}
  
  HRESULT MediaFoundationManager::Start(
	  IMFMediaSource* aPtrMediaSource,
	  IMFPresentationDescriptor* aPtrPresentationDescriptor,
	  const GUID* aPtrguidTimeFormat,
	  const PROPVARIANT* aPtrvarStartPosition)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrMediaSource == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrPresentationDescriptor == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrguidTimeFormat == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrvarStartPosition == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrMediaSource->Start(
			  aPtrPresentationDescriptor,
			  aPtrguidTimeFormat,
			  aPtrvarStartPosition);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::Pause(
			IMFMediaSession* aPtrMediaSession)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaSession == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaSession->Pause();

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
  }

  HRESULT MediaFoundationManager::Pause(
	  IMFMediaSource* aPtrMediaSource)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrMediaSource == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrMediaSource->Pause();

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::Stop(
	  IMFMediaSource* aPtrMediaSource)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrMediaSource == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrMediaSource->Stop();

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::Stop(
			IMFMediaSession* aPtrMediaSession)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaSession == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaSession->Stop();

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetOutputCount(
			IMFTopologyNode* aPtrTopologyNode,
			DWORD* aPtrOutputNodeCount)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTopologyNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrOutputNodeCount == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTopologyNode->GetOutputCount(aPtrOutputNodeCount);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}
			
			return lresult;
		}

  HRESULT MediaFoundationManager::GetOutput(
			IMFTopologyNode* aPtrUpstreamNode,
			DWORD aOutputNodeIndex,
			IMFTopologyNode** aPtrPtrDownstreamNode,
			DWORD* aPtrInputIndexOnDownStreamNode)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrUpstreamNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrDownstreamNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}
				
				lresult = aPtrUpstreamNode->GetOutput(
					aOutputNodeIndex,
					aPtrPtrDownstreamNode,
					aPtrInputIndexOnDownStreamNode);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::SetOutputPrefType(
	  IMFTopologyNode* aPtrUpstreamNode,
	  /* [in] */ DWORD dwOutputIndex,
		/* [in] */ IMFMediaType *pType)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrUpstreamNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (pType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrUpstreamNode->SetOutputPrefType(
					dwOutputIndex,
					pType);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetOutputPrefType(
	  IMFTopologyNode* aPtrUpstreamNode,
	  DWORD dwOutputIndex,
	  IMFMediaType **ppType)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrUpstreamNode == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (ppType == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrUpstreamNode->GetOutputPrefType(
			  dwOutputIndex,
			  ppType);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::SetInputPrefType(
	  IMFTopologyNode* aPtrUpstreamNode,
	  DWORD dwInputIndex,
	  IMFMediaType *pType)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrUpstreamNode == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (pType == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrUpstreamNode->SetInputPrefType(
			  dwInputIndex,
			  pType);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetInputPrefType(
	  IMFTopologyNode* aPtrUpstreamNode,
	  DWORD dwInputIndex,
	  IMFMediaType **ppType)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrUpstreamNode == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (ppType == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrUpstreamNode->GetInputPrefType(
			  dwInputIndex,
			  ppType);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::CloneFrom(
	  IMFTopologyNode* aPtrUpstreamNode,
	  IMFTopologyNode *pNode)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrUpstreamNode == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (pNode == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrUpstreamNode->CloneFrom(
			  pNode);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }
		  
  HRESULT MediaFoundationManager::CreateStream(
			IMFASFProfile* aPtrASFProfile,
			IMFMediaType* aPtrMediaType,
			IMFASFStreamConfig** aPtrPtrASFContentInfo)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrASFProfile == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrMediaType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrASFContentInfo == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrASFProfile->CreateStream(aPtrMediaType, aPtrPtrASFContentInfo);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}
			return lresult;
		}

  HRESULT MediaFoundationManager::SetStreamNumber(
			IMFASFStreamConfig* aPtrASFContentInfo,
			WORD aStreamNumber)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrASFContentInfo == nullptr)
				{
					lresult = E_POINTER;

					break;
				}
				
				lresult = aPtrASFContentInfo->SetStreamNumber(aStreamNumber);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}


			return lresult;
		}

  HRESULT MediaFoundationManager::SetStream(
			IMFASFProfile* aPtrASFProfile,
			IMFASFStreamConfig* aPtrASFContentInfo)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrASFProfile == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrASFContentInfo == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrASFProfile->SetStream(aPtrASFContentInfo);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}


			return lresult;
		}

  HRESULT MediaFoundationManager::GetEncodingConfigurationPropertyStore(
			IMFASFContentInfo* aPtrASFContentInfo,
			WORD aStreamNumber,
			IPropertyStore** aPtrPtrASFPropertyStore)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrPtrASFPropertyStore == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrASFContentInfo == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrASFContentInfo->GetEncodingConfigurationPropertyStore(
					aStreamNumber,
					aPtrPtrASFPropertyStore);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::SetValue(
			IPropertyStore* aPtrPropertyStore,
			REFPROPERTYKEY aKey,
			REFPROPVARIANT aValue)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrPropertyStore == nullptr)
				{
					lresult = E_POINTER;

					break;
				}
				
				lresult = aPtrPropertyStore->SetValue(aKey, aValue);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}
		
  HRESULT MediaFoundationManager::SetProfile(
			IMFASFContentInfo* aPtrASFContentInfo,
			IMFASFProfile* aPtrASFProfile)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrASFContentInfo == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrASFProfile == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrASFContentInfo->SetProfile(aPtrASFProfile);

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  
  
  HRESULT MediaFoundationManager::GetLockWorkQueue(
			DWORD* aPtrID)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (aPtrID == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (IsWindows8OrGreater())
				{
					lresult = LOG_INVOKE_MF_FUNCTION(MFAllocateSerialWorkQueue,
						MFASYNC_CALLBACK_QUEUE_MULTITHREADED,
						aPtrID);
				}

				if (FAILED(lresult))
				{
					lresult = LOG_INVOKE_MF_FUNCTION(MFAllocateWorkQueueEx,
						_MF_STANDARD_WORKQUEUE,
						aPtrID);

					if (FAILED(lresult))
					{
						*aPtrID = MFASYNC_CALLBACK_QUEUE_LONG_FUNCTION;

						lresult = S_OK;
					}
				}

				lresult = S_OK;

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}
  				
  HRESULT MediaFoundationManager::GetSourceNodeCollection(
			IMFTopology* aPtrTopology, 
			IMFCollection** aPtrPtrNodeCollection)
		{

			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTopology == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrNodeCollection == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTopology->GetSourceNodeCollection(aPtrPtrNodeCollection);


			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}
		
  HRESULT MediaFoundationManager::GetOutputNodeCollection(
			IMFTopology* aPtrTopology, 
			IMFCollection** aPtrPtrNodeCollection)
		{

			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTopology == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrNodeCollection == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTopology->GetOutputNodeCollection(aPtrPtrNodeCollection);


			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetElementCount(
			IMFCollection* aPtrElementCollection, 
			DWORD* aPtrCount)
		{

			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrElementCollection == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrCount == nullptr)
				{
					lresult = E_POINTER;

					break;
				}
				
				lresult = aPtrElementCollection->GetElementCount(aPtrCount);

				if (FAILED(lresult))
				{
					break;
				}

				if (*aPtrCount == 0)
				{
					lresult = MF_E_OUT_OF_RANGE;

					break;
				}


			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}
			
			return lresult;
		}

  HRESULT MediaFoundationManager::GetElement(
			IMFCollection* aPtrElementCollection, 
			DWORD aElementIndex, 
			IUnknown** aPtrPtrElement)
		{

			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrElementCollection == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrElement == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrElementCollection->GetElement(aElementIndex, aPtrPtrElement);
				
			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__,
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetCurrentMediaType(
			IMFMediaTypeHandler* aPtrMediaTypeHandler, 
			IMFMediaType** aPtrPtrMediaType)
		{

			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaTypeHandler == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrMediaType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaTypeHandler->GetCurrentMediaType(aPtrPtrMediaType);
				
			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL, 
					__FUNCTIONW__, 
					L" Error code: ", 
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetNodeType(
			IMFTopologyNode* aPtrTopologyNode, 
			MF_TOPOLOGY_TYPE* aPtrTopologyType)
		{

			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTopologyNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrTopologyType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}
				
				lresult = aPtrTopologyNode->GetNodeType(aPtrTopologyType);
				
			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetInputStreamInfo(
	  IMFTransform* aPtrTransform,
	  DWORD aInputStreamID,
	  MFT_INPUT_STREAM_INFO* aPtrStreamInfo)		
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrTransform == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrStreamInfo == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrTransform->GetInputStreamInfo(
			  aInputStreamID,
			  aPtrStreamInfo);
		  
	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetOutputAvailableType(
			IMFTransform* aPtrTransform,
			DWORD aOutputStreamID,
			DWORD aTypeIndex,
			IMFMediaType** aPtrPtrType)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTransform == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTransform->GetOutputAvailableType(
					aOutputStreamID,
					aTypeIndex,
					aPtrPtrType);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetInputCurrentType(
			IMFTransform* aPtrTransform,
			DWORD aOutputStreamID,
			IMFMediaType** aPtrPtrType)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTransform == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTransform->GetInputCurrentType(
					aOutputStreamID,
					aPtrPtrType);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetOutputCurrentType(
			IMFTransform* aPtrTransform,
			DWORD aOutputStreamID,
			IMFMediaType** aPtrPtrType)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTransform == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTransform->GetOutputCurrentType(
					aOutputStreamID,
					aPtrPtrType);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::SetOutputType(
			IMFTransform* aPtrTransform,
			DWORD aOutputStreamID,
			IMFMediaType* aPtrType,
			DWORD aFlag)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTransform == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrType == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTransform->SetOutputType(
					aOutputStreamID,
					aPtrType,
					aFlag);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::ProcessInput(
	  IMFTransform* aPtrTransform,
	  DWORD aInputStreamID,
	  IMFSample* aPtrSample,
	  DWORD aFlags)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrTransform == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrSample == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrTransform->ProcessInput(
			  aInputStreamID,
			  aPtrSample,
			  aFlags);
		  
	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::ProcessOutput(
	  IMFTransform* aPtrTransform,
	  DWORD aFlags,
	  DWORD aOutputBufferCount,
	  MFT_OUTPUT_DATA_BUFFER* aPtrOutputSamples,
	  DWORD* aPtrStatus)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrTransform == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrOutputSamples == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrStatus == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrTransform->ProcessOutput(
			  aFlags,
			  aOutputBufferCount,
			  aPtrOutputSamples,
			  aPtrStatus);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }
  
  HRESULT MediaFoundationManager::Compare(
			IMFAttributes* aPtrAttributes,
			IMFAttributes* aPtrTheirs,
			MF_ATTRIBUTES_MATCH_TYPE aMatchType,
			BOOL* aPtrResult)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrAttributes == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrTheirs == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrAttributes->Compare(
					aPtrTheirs,
					aMatchType,
					aPtrResult);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::ConvertToContiguousBuffer(
	  IMFSample* aPtrSample,
	  IMFMediaBuffer**  aPtrPtrBuffer)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrSample == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrPtrBuffer == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrSample->ConvertToContiguousBuffer(aPtrPtrBuffer);
		  
	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::CopyToBuffer(
	  IMFSample* aPtrSample,
	  IMFMediaBuffer* aPtrBuffer)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrSample == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrBuffer == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrSample->CopyToBuffer(aPtrBuffer);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetTotalLength(
	  IMFSample* aPtrSample,
	  DWORD* aPtrTotalLength)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrSample == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrTotalLength == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrSample->GetTotalLength(aPtrTotalLength);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetSampleTime(
	  IMFSample* aPtrSample,
	  LONGLONG* aPtrPtrSampleTime)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrSample == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrPtrSampleTime == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrSample->GetSampleTime(aPtrPtrSampleTime);

		  if (FAILED(lresult))
		  {
			  break;
		  }

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetSampleDuration(
	  IMFSample* aPtrSample,
	  LONGLONG* aPtrPtrSampleDuration)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrSample == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrPtrSampleDuration == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrSample->GetSampleDuration(aPtrPtrSampleDuration);

		  if (FAILED(lresult))
		  {
			  break;
		  }

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetBufferByIndex(
	  IMFSample* aPtrSample,
	  DWORD aIndex,
	  IMFMediaBuffer** aPtrPtrBuffer)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrSample == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrPtrBuffer == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrSample->GetBufferByIndex(
			  aIndex,
			  aPtrPtrBuffer);

		  if (FAILED(lresult))
		  {
			  break;
		  }

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetContiguousLength(
	  IMF2DBuffer* aPtr2DBuffer,
	  DWORD* aPtrLength)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtr2DBuffer == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrLength == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtr2DBuffer->GetContiguousLength(
			  aPtrLength);

		  if (FAILED(lresult))
		  {
			  break;
		  }

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::ContiguousCopyFrom(
	  IMF2DBuffer* aPtr2DBuffer,
	  const BYTE * aPtrSrcBuffer,
	  DWORD aSrcBufferLength)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtr2DBuffer == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrSrcBuffer == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtr2DBuffer->ContiguousCopyFrom(
			  aPtrSrcBuffer,
			  aSrcBufferLength);

		  if (FAILED(lresult))
		  {
			  break;
		  }

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }
  
  HRESULT MediaFoundationManager::ContiguousCopyTo(
	  IMF2DBuffer* aPtr2DBuffer,
	  BYTE * aPtrDestBuffer,
	  DWORD aDestBuffer)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtr2DBuffer == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrDestBuffer == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtr2DBuffer->ContiguousCopyTo(
			  aPtrDestBuffer,
			  aDestBuffer);

		  if (FAILED(lresult))
		  {
			  break;
		  }

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::SetTimeSource(
			IMFPresentationClock* aPtrPresentationClock,
			IMFPresentationTimeSource* aPtrTimeSource)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrPresentationClock == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrTimeSource == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrPresentationClock->SetTimeSource(aPtrTimeSource);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::SetPresentationClock(
			IMFMediaSink* aPtrMediaSink,
			IMFPresentationClock* aPtrPresentationClock)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaSink == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPresentationClock == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaSink->SetPresentationClock(aPtrPresentationClock);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::Start(
			IMFPresentationClock* aPtrPresentationClock,
			LONGLONG aClockStartOffset)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrPresentationClock == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrPresentationClock->Start(aClockStartOffset);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}
				
  HRESULT MediaFoundationManager::GetInput(
			IMFTopologyNode* aPtrDownStreamTopologyNode,
			DWORD aInputIndex,
			IMFTopologyNode** aPtrPtrUpStreamTopologyNode,
			DWORD* aPtrOutputIndexOnUpStreamNode)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrDownStreamTopologyNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrUpStreamTopologyNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrOutputIndexOnUpStreamNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrDownStreamTopologyNode->GetInput(
					aInputIndex,
					aPtrPtrUpStreamTopologyNode,
					aPtrOutputIndexOnUpStreamNode);
				
			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetIdentifier(
	  IMFStreamSink* aPtrStreamSink,
	  DWORD* aPtrIdentifier)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrStreamSink == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrIdentifier == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrStreamSink->GetIdentifier(aPtrIdentifier);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }
  
  HRESULT MediaFoundationManager::ProcessSample(
	  IMFStreamSink* aPtrStreamSink,
	  IMFSample* aPtrSample)		
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrStreamSink == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrSample == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrStreamSink->ProcessSample(aPtrSample);
		  
	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::AddClockStateSink(
	  IMFPresentationClock* aPtrPresentationClock,
	  IMFClockStateSink* aPtrPtrStateSink)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrPresentationClock == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrPtrStateSink == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrPresentationClock->AddClockStateSink(aPtrPtrStateSink);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::RemoveClockStateSink(
	  IMFPresentationClock* aPtrPresentationClock,
	  IMFClockStateSink* aPtrPtrStateSink)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrPresentationClock == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrPtrStateSink == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrPresentationClock->RemoveClockStateSink(aPtrPtrStateSink);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetService(
	  IMFGetService* aPtrGetService,
	  REFGUID aRefGUIDService,
	  REFIID aRefIID,
	  LPVOID* aPtrPtrObject)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrGetService == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrPtrObject == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrGetService->GetService(aRefGUIDService,
			  aRefIID,
			  aPtrPtrObject);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::SetVideoWindow(
	  IMFVideoDisplayControl* aPtrVideoDisplayControl,
	  HWND aHWNDVideo)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrVideoDisplayControl == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aHWNDVideo == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrVideoDisplayControl->SetVideoWindow(aHWNDVideo);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetCharacteristics(
	  IMFMediaSource* aPtrMediaSource,
	  DWORD* aPtrCharacteristics)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrMediaSource == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrCharacteristics == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrMediaSource->GetCharacteristics(aPtrCharacteristics);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::InitializeSampleAllocatorEx(
	  IMFVideoSampleAllocatorEx*  aPtrIMFVideoSampleAllocatorEx,
	  DWORD aInitialSamples,
	  DWORD aMaximumSamples,
	  IMFAttributes* aPtrAttributes,
	  IMFMediaType* aPtrMediaType)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrIMFVideoSampleAllocatorEx == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrAttributes == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrMediaType == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrIMFVideoSampleAllocatorEx->InitializeSampleAllocatorEx(
			  aInitialSamples,
			  aMaximumSamples,
			  aPtrAttributes,
			  aPtrMediaType);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetInputCount(
	  IMFTopologyNode* aPtrDownStreamTopologyNode,
	  DWORD* aPtrInputs)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrDownStreamTopologyNode == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrInputs == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrDownStreamTopologyNode->GetInputCount(
			  aPtrInputs);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }
  
  HRESULT MediaFoundationManager::GetStreamSinkCount(
	  IMFMediaSink* aPtrMediaSink,
	  DWORD* aPtrStreamSinkCount)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrMediaSink == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrStreamSinkCount == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrMediaSink->GetStreamSinkCount(aPtrStreamSinkCount);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetBufferCount(
	  IMFSample* aPtrSample,
	  DWORD *pdwBufferCount)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrSample == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (pdwBufferCount == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrSample->GetBufferCount(
			  pdwBufferCount);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetPresentationClock(
	  IMFMediaSink* aPtrIMFMediaSink,
	  IMFPresentationClock ** aPtrptrPresentationClock)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrIMFMediaSink == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrptrPresentationClock == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrIMFMediaSink->GetPresentationClock(
			  aPtrptrPresentationClock);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetCorrelatedTime(
	  IMFPresentationClock* aPtrPresentationClock,
	  DWORD aReserved,
	  LONGLONG* aPtrClockTime,
	  MFTIME* aPtrSystemTime)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrPresentationClock == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrClockTime == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrSystemTime == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrPresentationClock->GetCorrelatedTime(
			  aReserved,
			  aPtrClockTime,
			  aPtrSystemTime);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetBlobSize(
	  IMFAttributes* aPtrIMFAttributes,
	  REFGUID aGUIDKey,
	  UINT32* aPtrBlobSize)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrIMFAttributes == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrBlobSize == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrIMFAttributes->GetBlobSize(
			  aGUIDKey,
			  aPtrBlobSize);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetBlob(
	  IMFAttributes* aPtrIMFAttributes,
	  REFGUID aGUIDKey,
	  UINT8* aPtrBuffer,
	  UINT32 aBufferSize,
	  UINT32* aPtrBlobSize)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrIMFAttributes == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrBlobSize == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrBlobSize == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrIMFAttributes->GetBlob(
			  aGUIDKey,
			  aPtrBuffer,
			  aBufferSize,
			  aPtrBlobSize);

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }


  HRESULT MediaFoundationManager::DisconnectOutput(
			IMFTopologyNode* aPtrUpStreamTopologyNode,
			DWORD aOutputIndex)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;
			
				if (aPtrUpStreamTopologyNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrUpStreamTopologyNode->DisconnectOutput(aOutputIndex);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}
  		
  HRESULT MediaFoundationManager::Shutdown(
			IMFMediaSession* aPtrMediaSession)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaSession == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaSession->Shutdown();

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
  }

  HRESULT MediaFoundationManager::Shutdown(
	  IMFMediaEventQueue*  aPtrEventQueue)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrEventQueue == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrEventQueue->Shutdown();
		  
	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::QueueEvent(
			IMFMediaEventGenerator* aPtrMediaEventGenerator,
			MediaEventType aMediaEventType,
			REFGUID aRefGUID,
			HRESULT aHRESULTResult,
			const PROPVARIANT* aPtrPropVariantValue)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaEventGenerator == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaEventGenerator->QueueEvent(
					aMediaEventType,
					aRefGUID,
					aHRESULTResult,
					aPtrPropVariantValue);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::QueueEvent(
	  IMFMediaEventQueue* aPtrMediaEventQueue,
	  IMFMediaEvent* aPtrEvent)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrMediaEventQueue == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrEvent == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrMediaEventQueue->QueueEvent(
			  aPtrEvent);
		  
	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }
		
  HRESULT MediaFoundationManager::GetTopoNodeID(
			IMFTopologyNode* aPtrTopologyNode,
			TOPOID* aPtrTOPOID)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTopologyNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrTOPOID == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTopologyNode->GetTopoNodeID(aPtrTOPOID);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::SetTopoNodeID(
	  IMFTopologyNode* aPtrTopologyNode,
	  TOPOID ullTopoID)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrTopologyNode == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }
		  
		  lresult = aPtrTopologyNode->SetTopoNodeID(ullTopoID);

		  if (FAILED(lresult))
		  {
			  break;
		  }

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::GetNodeByID(
			IMFTopology* aPtrTopology,
			TOPOID aTopoNodeID,
			IMFTopologyNode** aPtrPtrTopologyNode)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTopology == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrTopologyNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTopology->GetNodeByID(
					aTopoNodeID,
					aPtrPtrTopologyNode);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetNode(
				IMFTopology* aPtrTopology,
				WORD aTopoNodeIndex,
				IMFTopologyNode** aPtrPtrTopologyNode)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTopology == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrPtrTopologyNode == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrTopology->GetNode(
					aTopoNodeIndex,
					aPtrPtrTopologyNode);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::GetNodeCount(
			IMFTopology* aPtrTopology,
			WORD* aPtrTopoNodeCount)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrTopology == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (aPtrTopoNodeCount == nullptr)
				{
					lresult = E_POINTER;

					break;
				}
							

				lresult = aPtrTopology->GetNodeCount(aPtrTopoNodeCount);

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}

  HRESULT MediaFoundationManager::pause(IMFMediaSource* aPtrMediaSource)
		{
			HRESULT lresult = E_FAIL;

			do
			{
				if (FAILED(mResult))
					break;

				if (aPtrMediaSource == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				lresult = aPtrMediaSource->Pause();

				if (FAILED(lresult))
				{
					break;
				}

			} while (false);

			if (FAILED(lresult))
			{
				LogPrintOut::getInstance().printOutln(
					LogPrintOut::ERROR_LEVEL,
					__FUNCTIONW__,
					L" Error code: ",
					lresult);
			}

			return lresult;
		}
			
  HRESULT MediaFoundationManager::BeginFinalize(
	  IMFFinalizableMediaSink* aPtrFinalizableMediaSink,
	  IMFAsyncCallback* aPtrCallback,
	  IUnknown* aPtrUnkState)
  {
	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrFinalizableMediaSink == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrCallback == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrFinalizableMediaSink->BeginFinalize(aPtrCallback, aPtrUnkState);

		  if (FAILED(lresult))
		  {
			  break;
		  }

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

  HRESULT MediaFoundationManager::EndFinalize(
	  IMFFinalizableMediaSink* aPtrFinalizableMediaSink,
	  IMFAsyncResult* aPtrResult)
  {

	  HRESULT lresult = E_FAIL;

	  do
	  {
		  if (FAILED(mResult))
			  break;

		  if (aPtrFinalizableMediaSink == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  if (aPtrResult == nullptr)
		  {
			  lresult = E_POINTER;

			  break;
		  }

		  lresult = aPtrFinalizableMediaSink->EndFinalize(aPtrResult);

		  if (FAILED(lresult))
		  {
			  break;
		  }

	  } while (false);

	  if (FAILED(lresult))
	  {
		  LogPrintOut::getInstance().printOutln(
			  LogPrintOut::ERROR_LEVEL,
			  __FUNCTIONW__,
			  L" Error code: ",
			  lresult);
	  }

	  return lresult;
  }

		}
	}
}