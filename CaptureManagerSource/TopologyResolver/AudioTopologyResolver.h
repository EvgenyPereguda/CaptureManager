#pragma once
#include "BaseTopologyResolver.h"
#include "../Common/BoxMode.h"

namespace CaptureManager
{
	namespace MediaSession
	{
		namespace CustomisedMediaSession
		{
			class AudioTopologyResolver :
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
					IMFTopologyNode** aPtrPtrTailTopologyNode);

				virtual HRESULT resolveInputType(
					IMFTransform* aPtrTransform,
					IMFMediaType* aPtrUpStreamMediaType) override;

				CAPTUREMANAGER_BOX

					HRESULT checkUncompressionAudioSubTypes(
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType);

				HRESULT resolveUnCompressionAudioConnection(
					IMFTopology* aPtrTopology,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType,
					IMFTopologyNode** aPtrPtrHeadTopologyNode,
					IMFTopologyNode** aPtrPtrTailTopologyNode);

				HRESULT checkDemandForResolvingOfUncompressionAudioSubTypes(
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType);
				
				HRESULT checkDemandForAudioResampler(
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType);

				HRESULT resolveDecompressionAudioConnection(
					IMFTopology* aPtrTopology,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType,
					IMFTopologyNode** aPtrPtrHeadTopologyNode,
					IMFTopologyNode** aPtrPtrTailTopologyNode);

				HRESULT createAudioResamplerConnection(
					IMFTopology* aPtrTopology,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType,
					IMFTopologyNode** aPtrPtrHeadTopologyNode,
					IMFTopologyNode** aPtrPtrTailTopologyNode);

				HRESULT resolveSubTypeAudioConnection(
					IMFTopology* aPtrTopology,
					IMFMediaType* aPtrUpStreamMediaType,
					IMFMediaType* aPtrDownStreamMediaType,
					IMFTopologyNode** aPtrPtrHeadTopologyNode,
					IMFTopologyNode** aPtrPtrTailTopologyNode);

				HRESULT createUncompressedMediaType(
					UINT32 aSamplePerSecond,
					UINT32 aNumChannels,
					UINT32 aBitsPerSample,
					IMFMediaType** aPtrPtrStubUncompressedMediaType);
			};
		}
	}
}