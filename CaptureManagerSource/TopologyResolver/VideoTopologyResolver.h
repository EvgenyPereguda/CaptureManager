#pragma once
#include "BaseTopologyResolver.h"
#include "../Common/BoxMode.h"

struct IMFMediaTypeHandler;
struct IDirect3DDeviceManager9;
struct IMFVideoSampleAllocator;
struct IMFVideoSampleAllocatorEx;

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			class VideoTopologyResolver :
				public BaseTopologyResolver
			{
			protected:

				// BaseTopologyResolver interface

				virtual HRESULT getInputMediaTypeOfMediaSink(
					IMFTopology* aPtrTopology,
					IMFTopologyNode* aPtrUpStreamNode,
					IMFTopologyNode* aPtrDownStreamNode,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType** aPtrPtrDownStreamMediaType,
					UINT32 aOutputStreamIndex) override;

				virtual HRESULT resolveConnection(
					IMFTopology* aPtrTopology,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType,
					IMFTopologyNode** aPtrPtrHeadTopologyNode,
					IMFTopologyNode** aPtrPtrTailTopologyNode) override;

				virtual HRESULT resolveInputType(
					IMFTransform* aPtrTransform,
					IMFMediaType* aPtrUpStreamMediaType) override;
				
				CAPTUREMANAGER_BOX;

				HRESULT resolveConnectionDirect3DDeviceManager9MediaSink(
					IMFTopology* aPtrTopology,
					IMFMediaTypeHandler* aPtrMediaTypeHandler,
					IDirect3DDeviceManager9*  aPtrIDirect3DDeviceManager9,
					IMFTopologyNode* aPtrUpStreamNode,
					DWORD aOutputStreamIndex,
					IMFMediaType* aPtrUpStreamMediaType);

				HRESULT resolveConnectionVideoSampleAllocatorExMediaSink(
					IMFTopology* aPtrTopology,
					IMFMediaTypeHandler* aPtrMediaTypeHandler,
					IMFVideoSampleAllocatorEx*  aPtrIMFVideoSampleAllocatorEx,
					IMFTopologyNode* aPtrUpStreamNode,
					DWORD aOutputStreamIndex,
					IMFMediaType* aPtrUpStreamMediaType);

				HRESULT resolveConnectionVideoSampleAllocatorMediaSink(
					IMFTopology* aPtrTopology,
					IMFMediaTypeHandler* aPtrMediaTypeHandler,
					IMFVideoSampleAllocator*  aPtrIMFVideoSampleAllocator,
					IMFTopologyNode* aPtrUpStreamNode,
					DWORD aOutputStreamIndex,
					IMFMediaType* aPtrUpStreamMediaType);
				
				HRESULT createMediaType(
					IMFMediaType* aPtrUpStreamMediaType,
					GUID aMFVideoFormat,
					IMFMediaType** aPtrPtrMediaSinkMediaType);

				HRESULT resolveSubTypeVideoConnection(
					IMFTopology* aPtrTopology,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType,
					IMFTopologyNode** aPtrPtrHeadTopologyNode,
					IMFTopologyNode** aPtrPtrTailTopologyNode);

				HRESULT resolveResizeVideoConnection(
					IMFTopology* aPtrTopology,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType,
					IMFTopologyNode** aPtrPtrHeadTopologyNode,
					IMFTopologyNode** aPtrPtrTailTopologyNode);

				HRESULT resolveFrameRateVideoConnection(
					IMFTopology* aPtrTopology,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType,
					IMFTopologyNode** aPtrPtrHeadTopologyNode,
					IMFTopologyNode** aPtrPtrTailTopologyNode);

				HRESULT resolveFrameRateAndResizeVideoConnection(
					IMFTopology* aPtrTopology,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType,
					IMFTopologyNode** aPtrPtrHeadTopologyNode,
					IMFTopologyNode** aPtrPtrTailTopologyNode);

				HRESULT resolveColorConvertionVideoConnection(
					IMFTopology* aPtrTopology,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType,
					IMFTopologyNode** aPtrPtrHeadTopologyNode,
					IMFTopologyNode** aPtrPtrTailTopologyNode);

				HRESULT resolveColorConvertionInputType(
					IMFTransform* aPtrTransform,
					IMFMediaType* aPtrUpStreamMediaType);
				
				HRESULT resolveDecompressionVideoInputType(
					IMFTransform* aPtrTransform,
					IMFMediaType* aPtrUpStreamMediaType);

				HRESULT resolveDecompressionVideoConnection(
					IMFTopology* aPtrTopology,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType,
					IMFTopologyNode** aPtrPtrHeadTopologyNode,
					IMFTopologyNode** aPtrPtrTailTopologyNode);
				
				HRESULT checkDemandForVideoResizer(
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType);

				HRESULT checkDemandForFrameRateConvertor(
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType);

				HRESULT resolveFrameConvertorConnection(
					IMFTopology* aPtrTopology,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType,
					IMFTopologyNode** aPtrPtrHeadTopologyNode,
					IMFTopologyNode** aPtrPtrTailTopologyNode);

				HRESULT addHorizontMirroringConvertorConnection(
					IMFTopology* aPtrTopology,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFTopologyNode* aPtrUpStreamNode,
					DWORD aOutputStreamIndex,
					IMFMediaTypeHandler* aPtrMediaTypeHandler);
				
				HRESULT findSuitableDecoderMediaType(
					IMFMediaType* aPtrUpStreamMediaType,
					IMFVideoSampleAllocator*  aPtrIMFVideoSampleAllocator,
					IMFMediaType** aPtrPtrFirstOutputMediaType,
					IMFMediaTypeHandler* aPtrMediaTypeHandler);

				HRESULT findSuitableColorMediaType(
					IMFMediaType* aPtrUpStreamMediaType,
					IMFVideoSampleAllocator*  aPtrIMFVideoSampleAllocator,
					IMFMediaTypeHandler* aPtrMediaTypeHandler);
				
				HRESULT checkDemandForDirectX11Convertor(
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType);

				HRESULT resolveDirectX11ConvertorConnection(
					IMFTopology* aPtrTopology,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType,
					IMFTopologyNode** aPtrPtrHeadTopologyNode,
					IMFTopologyNode** aPtrPtrTailTopologyNode);


			};
		}
	}
}