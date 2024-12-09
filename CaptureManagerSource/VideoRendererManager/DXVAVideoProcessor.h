#pragma once

#include <atomic>
#include <condition_variable>
#include <memory>
#include <unordered_map>

#include "../Common/BaseUnknown.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "IMixerStreamPositionControl.h"
#include "IStreamFilterControl.h"
#include "../PugiXML/pugixml.hpp"

namespace CaptureManager
{
	namespace Sinks
	{
		namespace EVR
		{
			namespace Mixer
			{				
				class DXVAVideoProcessor :
					public BaseUnknown<
					IMFTransform,
					IMixerStreamPositionControl,
					IStreamFilterControl
					>
				{
				private:

					enum StretchState
					{
						SOURCE = 0,
						FILL = SOURCE + 1,
						MAX = FILL + 1
					};

					std::vector<DWORD> m_dwInputIDs;

					std::list<DWORD> m_dwZOrders;

					BOOL mOneStreamOnly;

					GUID mProcessorGUID;

					DXVA2_VideoDesc mMainVideoDesc;

					struct StreamInfo
					{
						CComPtrCustom<IMFMediaType> mInputMediaType;

						CComPtrCustom<IMFSample>  mSample;

						CComPtrCustom<IDirect3DSurface9>  mSubStreamSurface;						

						MFVideoNormalizedRect mSrcVideoNormalizedRect;

						MFVideoNormalizedRect mDestVideoNormalizedRect;

						DXVA2_Fixed32 mDXVA2_Fixed32;

						StretchState mStretchState;

						StreamInfo()
						{
							ZeroMemory(&mDestVideoNormalizedRect, sizeof(mDestVideoNormalizedRect));

							mDestVideoNormalizedRect.bottom = 1.0f;

							mDestVideoNormalizedRect.right = 1.0f;


							ZeroMemory(&mSrcVideoNormalizedRect, sizeof(mSrcVideoNormalizedRect));

							mSrcVideoNormalizedRect.bottom = 1.0f;

							mSrcVideoNormalizedRect.right = 1.0f;


							mDXVA2_Fixed32 = DXVA2FloatToFixed(1.0f);

							mStretchState = StretchState::SOURCE;
						}
					};


					std::unordered_map<DWORD, StreamInfo > m_InputStreams;

					DXVAVideoProcessor();

					virtual ~DXVAVideoProcessor();
					
				public:

					static HRESULT createDXVAVideoProcessor(IMFTransform** aPtrPtrTrsnaform);

				public:

					// IMixerStreamPositionControl methods
			
					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setPosition(
						/* [in] */ DWORD aInputStreamID,
						/* [in] */ FLOAT aLeft,
						/* [in] */ FLOAT aRight,
						/* [in] */ FLOAT aTop,
						/* [in] */ FLOAT aBottom)override;

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setSrcPosition(
						/* [in] */ DWORD aStreamID,
						/* [in] */ FLOAT aLeft,
						/* [in] */ FLOAT aRight,
						/* [in] */ FLOAT aTop,
						/* [in] */ FLOAT aBottom)override;

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setOpacity(
						/* [in] */ DWORD aInputStreamID,
						/* [in] */ FLOAT aOpacity)override;

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setZOrder(
						/* [in] */ DWORD aInputStreamID,
						/* [in] */ DWORD aZOrder)override;

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getPosition(
						/* [in] */ DWORD aInputStreamID,
						/* [out] */ FLOAT *aPtrLeft,
						/* [out] */ FLOAT *aPtrRight,
						/* [out] */ FLOAT *aPtrTop,
						/* [out] */ FLOAT *aPtrBottom)override;

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getSrcPosition(
						/* [in] */ DWORD aStreamID,
						/* [out] */ FLOAT *aPtrLeft,
						/* [out] */ FLOAT *aPtrRight,
						/* [out] */ FLOAT *aPtrTop,
						/* [out] */ FLOAT *aPtrBottom)override;

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getOpacity(
						/* [in] */ DWORD aInputStreamID,
						/* [out] */ FLOAT *aPtrOpacity)override;

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getZOrder(
						/* [in] */ DWORD aInputStreamID,
						/* [out] */ DWORD *aPtrZOrder)override;

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE flush(
						/* [in] */ DWORD aInputStreamID)override;




					// IStreamFilterControl methods

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getCollectionOfFilters(
						/* [in] */ DWORD aMixerStreamID,
						/* [out] */ BSTR *aPtrPtrXMLstring)override;

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setFilterParametr(
						/* [in] */ DWORD aMixerStreamID,
						/* [in] */ DWORD aParametrIndex,
						/* [in] */ LONG aNewValue,
						/* [in] */ BOOL aIsEnabled)override;

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE getCollectionOfOutputFeatures(
						/* [out] */ BSTR *aPtrPtrXMLstring)override;

					virtual /* [id][helpstring] */ HRESULT STDMETHODCALLTYPE setOutputFeatureParametr(
						/* [in] */ DWORD aParametrIndex,
						/* [in] */ LONG aNewValue)override;

					// IMFTransform methods

					STDMETHODIMP GetStreamLimits(
						DWORD* aPtrInputMinimum,
						DWORD* aPtrInputMaximum,
						DWORD* aPtrOutputMinimum,
						DWORD* aPtrOutputMaximum);

					STDMETHODIMP GetStreamIDs(
						DWORD aInputIDArraySize,
						DWORD* aPtrInputIDs,
						DWORD aOutputIDArraySize,
						DWORD* aPtrOutputIDs);

					STDMETHODIMP GetStreamCount(
						DWORD* aPtrInputStreams,
						DWORD* aPtrOutputStreams);

					STDMETHODIMP GetInputStreamInfo(
						DWORD aInputStreamID,
						MFT_INPUT_STREAM_INFO* aPtrStreamInfo);

					STDMETHODIMP GetOutputStreamInfo(
						DWORD aOutputStreamID,
						MFT_OUTPUT_STREAM_INFO* aPtrStreamInfo);

					STDMETHODIMP GetInputStreamAttributes(
						DWORD aInputStreamID,
						IMFAttributes** aPtrPtrAttributes);

					STDMETHODIMP GetOutputStreamAttributes(
						DWORD aOutputStreamID,
						IMFAttributes** aPtrPtrAttributes);

					STDMETHODIMP DeleteInputStream(
						DWORD aInputStreamID);

					STDMETHODIMP AddInputStreams(
						DWORD aStreams,
						DWORD* aPtrStreamIDs);

					STDMETHODIMP GetInputAvailableType(
						DWORD aInputStreamID,
						DWORD aTypeIndex,
						IMFMediaType** aPtrPtrType);

					STDMETHODIMP GetOutputAvailableType(
						DWORD aOutputStreamID,
						DWORD aTypeIndex,
						IMFMediaType** aPtrPtrType);

					STDMETHODIMP SetInputType(
						DWORD aInputStreamID,
						IMFMediaType* aPtrType,
						DWORD aFlags);

					STDMETHODIMP SetOutputType(
						DWORD aOutputStreamID,
						IMFMediaType* aPtrType,
						DWORD aFlags);

					STDMETHODIMP GetInputCurrentType(
						DWORD aInputStreamID,
						IMFMediaType** aPtrPtrType);

					STDMETHODIMP GetOutputCurrentType(
						DWORD aOutputStreamID,
						IMFMediaType** aPtrPtrType);

					STDMETHODIMP GetInputStatus(
						DWORD aInputStreamID,
						DWORD* aPtrFlags);

					STDMETHODIMP GetOutputStatus(
						DWORD* aPtrFlags);

					STDMETHODIMP SetOutputBounds(
						LONGLONG aLowerBound,
						LONGLONG aUpperBound);

					STDMETHODIMP ProcessEvent(
						DWORD aInputStreamID,
						IMFMediaEvent* aPtrEvent);

					STDMETHODIMP GetAttributes(
						IMFAttributes** aPtrPtrAttributes);

					STDMETHODIMP ProcessMessage(
						MFT_MESSAGE_TYPE aMessage,
						ULONG_PTR aParam);

					STDMETHODIMP ProcessInput(
						DWORD aInputStreamID,
						IMFSample* aPtrSample,
						DWORD aFlags);

					STDMETHODIMP ProcessOutput(
						DWORD aFlags,
						DWORD aOutputBufferCount,
						MFT_OUTPUT_DATA_BUFFER* aPtrOutputSamples,
						DWORD* aPtrStatus);

				private:

					std::condition_variable mConditionVariable;

					std::mutex mZOrderMutex;

					std::mutex mMutex;

					UINT mSubStreamCount;

					UINT64 mAverageTimePerFrame;

					D3DCOLOR mBackgroundColor;
					
					RECT mOutputTargetRect;
					
					CComPtrCustom<IMFMediaType> mOutputMediaType;

					D3DFORMAT mVideoRenderTargetFormat;

					DXVA2_VideoProcessorCaps mVPCaps;


					
					CComPtrCustom<IDirect3DDeviceManager9> mDeviceManager;

					CComPtrCustom<IDirectXVideoProcessorService> mDXVAVPDService;

					CComPtrCustom<IDirectXVideoProcessor> mDXVAVPD;

					CComPtrCustom<IDirect3DSurface9> mBackSurface;


					HRESULT createVideoProcessor();

					HRESULT ConvertMFTypeToDXVAType(IMFMediaType *pType, DXVA2_VideoDesc *pDesc);

					HRESULT GetVideoProcessorService(
						IDirect3DDeviceManager9 *pDeviceManager,
						IDirectXVideoProcessorService **ppVPService);

					BOOL checkVideoProcessorGUID(
						REFGUID aGUID,
						IDirectXVideoProcessorService* aPtrVPService,
						DXVA2_VideoDesc *pDesc = nullptr);

					RECT ScaleRectangle(const RECT& input, const RECT& src, const RECT& dst);

					INT ComputeLongSteps(DXVA2_ValueRange &range);

					DWORD RGBtoYUV(const D3DCOLOR rgb);		

					DXVA2_AYUVSample16 GetBackgroundColor();

					HRESULT checkSupportedSubFormat(DXVA2_VideoDesc *pDesc);

					void GetDXVA2ExtendedFormatFromMFMediaType(
						IMFMediaType *pType,
						DXVA2_ExtendedFormat *pFormat
						);

					HRESULT blit(
						IMFSample* aPtrSample,
						REFERENCE_TIME aTargetFrame,
						REFERENCE_TIME aTargetEndFrame);

					HRESULT createMediaType(
						IMFMediaType* aPtrUpStreamMediaType,
						GUID aMFVideoFormat,
						IMFMediaType** aPtrPtrMediaSinkMediaType);

					HRESULT createUncompressedVideoType(
						DWORD                fccFormat,  // FOURCC or D3DFORMAT value.     
						UINT32               width,
						UINT32               height,
						MFVideoInterlaceMode interlaceMode,
						const MFRatio&       frameRate,
						const MFRatio&       par,
						IMFMediaType         **ppType);

					HRESULT createSurfaces(IDirect3DDeviceManager9 *pDeviceManager);
					
					HRESULT fillColorFeatureNode(
						pugi::xml_node& aFeatureNode,
						UINT32 aStartIndex,
						D3DCOLOR aColor);

					HRESULT createDXVA2VPDevice(
						REFGUID guid,
						IDirectXVideoProcessorService* aPtrVPService,
						DXVA2_VideoDesc *pDesc);

					HRESULT getStreamStretch(
						DWORD aInputStreamID,
						int aIndex,
						pugi::xml_node& aFilterNode);
				};
			}
		}
	}
}