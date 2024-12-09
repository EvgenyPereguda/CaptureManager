#pragma once

#include <Unknwnbase.h>
#include <string>
#include <map>

#include "IWebCamKernelStreamingControl.h"
#include "SinkCommon.h"
#include "StreamControlCommon.h"
#include "EncodingSettings.h"
#include "ISessionCallbackInner.h"
#include "../PugiXML/pugixml.hpp"
#include "../Common/ComPtrCustom.h"

struct IInnerCaptureProcessor;
struct IDirect3DTexture9;

namespace CaptureManager
{
	struct ISampleGrabberCallInner;

	class CaptureManagerBroker
	{
	public:
		HRESULT getXMLDocumentStringListOfSources(
			std::wstring& aRefXMLDocumentString);

		HRESULT getXMLDocumentStringOfEncoders(
			std::wstring& aRefXMLstring);

		HRESULT getXMLDocumentStringEncoderMediaTypes(
			IUnknown* aPtrInputMediaType,
			REFGUID aEncoderCLSID,
			std::wstring& aRefXMLstring);


		HRESULT getSourceOutputMediaType(
			std::wstring aSymbolicLink,
			DWORD aIndexStream,
			DWORD aIndexMediaType,
			IUnknown** aPtrPtrOutputMediaType);

		HRESULT getSourceOutputMediaType(
			IUnknown* aPtrMediaSource,
			DWORD aIndexStream,
			DWORD aIndexMediaType,
			IUnknown** aPtrPtrOutputMediaType);

		HRESULT createSource(
			std::wstring& aSymbolicLink,
			IUnknown** aPtrPtrMediaSource);

		HRESULT createSourceNode(
			std::wstring& aSymbolicLink,
			DWORD aIndexStream,
			DWORD aIndexMediaType,
			IUnknown** aPtrPtrTopologyNode);

		HRESULT createSourceNode(
			IUnknown* aPtrMediaSource,
			DWORD aIndexStream,
			DWORD aIndexMediaType,
			IUnknown** aPtrPtrTopologyNode);

		HRESULT connectOutputNode(
			IUnknown *aPtrUpStreamTopologyNode,
			DWORD aUpStreamOutputIndex,
			IUnknown *aPtrDownStreamTopologyNode,
			DWORD aDownStreamInputIndex);

		HRESULT createSource(
			IUnknown* aPtrCaptureProcessor,
			IUnknown** aPtrPtrMediaSource);
		
		HRESULT createWebCamControl(
			std::wstring& aSymbolicLink,
			Controls::WebCamControls::IWebCamKernelStreamingControl** aPtrPtrWebCamControl);

		HRESULT getEncodingSettingsMode(
			REFGUID aRefEncodingModeGUID,
			EncodingSettings::EncodingMode& aRefEncodingMode);

		HRESULT createCompressedMediaType(
			IUnknown* aPtrUncompressedMediaType,
			EncodingSettings aEncodingSettings,
			REFGUID aEncoderCLSID,
			DWORD lIndexCompressedMediaType,
			IUnknown** aPtrPtrCompressedMediaType);

		HRESULT createEncoderNode(
			IUnknown *aPtrUncompressedMediaType,
			EncodingSettings aEncodingSettings,
			REFGUID aEncoderCLSID,
			DWORD lIndexOutputMediaType,
			IUnknown* aPtrOutputNode,
			IUnknown** aPtrPtrEncoderNode);

		HRESULT createSampleGrabberOutputNode(
			REFGUID aRefMajorType,
			REFGUID aRefSubType,
			std::wstring& aReadMode,
			LONG aSampleByteSize,
			ISampleGrabberCallInner** aPtrPtrISampleGrabber);

		HRESULT createSampleGrabberOutputNode(
			REFGUID aRefMajorType,
			REFGUID aRefSubType,
			IUnknown* aPtrISampleGrabberCallback,
			IUnknown** aPtrPtrTopologyNode);

		HRESULT GetOutputFactoryCollection(
			SinkType aSinkType,
			std::vector<GUIDToNamePair>& mContainers);

		HRESULT getSampleAccumulatorNodeFactoryCollection(
			pugi::xml_node& aRefRootXMLElement,
			std::vector<StreamControlInfo>& aCollection);

		HRESULT getMixerNodeFactoryCollection(
			pugi::xml_node& aRefRootXMLElement,
			std::vector<StreamControlInfo>& aCollection);

		HRESULT createOutputNodeFactory(
			REFGUID aRefContainerTypeGUID,
			IUnknown** aPtrPtrOutputNodeFactory);

		HRESULT createSessionDescriptor(
			std::vector<IUnknown*> lSourceNodes,
			IUnknown* aPtrISessionCallback,
			SessionDescriptor& aRefSessionDescriptor);

		HRESULT getXMLDocumentStringOfMediaType(
			IUnknown* aPtrMediaType,
			std::wstring& aRefXMLDocumentString);

		HRESULT getStrideForBitmapInfoHeader(
			const GUID aMFVideoFormat,
			DWORD aWidthInPixels,
			LONG& aRefStride);

		HRESULT createSpreaderNode(
			std::vector<IUnknown*> aArrayDownStreamTopologyNodes,
			IUnknown** aPtrPtrTopologySpreaderNode);

		HRESULT createSampleAccumulatorNode(
			REFIID aREFIID,
			std::vector<IUnknown*> aArrayDownStreamTopologyNodes,
			IUnknown** aPtrPtrTopologySpreaderNode);

		HRESULT startSession(
			SessionDescriptor& aRefSessionDescriptor,
			LONGLONG aStartPositionInHundredNanosecondUnits,
			const GUID aGUIDTimeFormat);

		HRESULT stopSession(
			SessionDescriptor& aRefSessionDescriptor);

		HRESULT pauseSession(
			SessionDescriptor& aRefSessionDescriptor);

		HRESULT closeSession(
			SessionDescriptor& aRefSessionDescriptor);

		void getVideoRendererModes(
			BOOL aMultiOutputNodeVideoRenderer,
			std::vector<GUIDToNamePair>& aRefReadModes);
				
		HRESULT createVideoRendererOutputNodes(
			HANDLE aHandle,
			IUnknown *aPtrUnkTarget,
			DWORD aOutputNodeAmount,
			std::vector<CComPtrCustom<IUnknown>>& aRefOutputNodes);

		HRESULT createMixerNodes(
			IUnknown *aPtrDownStreamTopologyNode,
			DWORD aInputNodeAmount,
			std::vector<CComPtrCustom<IUnknown>>& aRefInputNodes);

		HRESULT createEVRMultiSinkOutputNodes(
			HANDLE aHandle,
			IUnknown *aPtrUnkTarget,
			DWORD aOutputNodeAmount,
			std::vector<CComPtrCustom<IUnknown>>& aRefOutputNodes);

		void getEVRMultiSinkModes(
			std::vector<GUIDToNamePair>& aRefReadModes);

		HRESULT setEVRStreamPosition(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ FLOAT aLeft,
			/* [in] */ FLOAT aRight,
			/* [in] */ FLOAT aTop,
			/* [in] */ FLOAT aBottom);

		HRESULT setEVRStreamSrcPosition(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ FLOAT aLeft,
			/* [in] */ FLOAT aRight,
			/* [in] */ FLOAT aTop,
			/* [in] */ FLOAT aBottom);

		HRESULT setEVRStreamOpacity(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ FLOAT aOpacity);

		HRESULT setEVRStreamZOrder(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ DWORD aZOrder);




		HRESULT getEVRStreamPosition(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ FLOAT* aPtrLeft,
			/* [in] */ FLOAT* aPtrRight,
			/* [in] */ FLOAT* aPtrTop,
			/* [in] */ FLOAT* aPtrBottom);

		HRESULT getEVRStreamSrcPosition(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ FLOAT* aPtrLeft,
			/* [in] */ FLOAT* aPtrRight,
			/* [in] */ FLOAT* aPtrTop,
			/* [in] */ FLOAT* aPtrBottom);

		HRESULT getEVRStreamOpacity(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ FLOAT* aPtrOpacity);

		HRESULT getEVRStreamZOrder(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ DWORD* aPtrZOrder);


		HRESULT setEVRStreamFlush(
			/* [in] */ IUnknown *aPtrEVROutputNode);
		
		HRESULT getCollectionOfFilters(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [out] */ BSTR *aPtrPtrXMLstring);

		HRESULT setFilterParametr(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ DWORD aParametrIndex,
			/* [in] */ LONG aNewValue,
			/* [in] */ BOOL aIsEnabled);

		HRESULT getCollectionOfOutputFeatures(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [out] */ BSTR *aPtrPtrXMLstring);

		HRESULT setOutputFeatureParametr(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ DWORD aParametrIndex,
			/* [in] */ LONG aNewValue);

		
		HRESULT STDMETHODCALLTYPE enableInnerRendering(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ BOOL aIsInnerRendering);

		HRESULT STDMETHODCALLTYPE renderToTarget(
			/* [in] */ IUnknown *aPtrEVROutputNode,
			/* [in] */ IUnknown *aPtrRenderTarget,
			/* [in] */ BOOL aCopyMode);
		
		HRESULT STDMETHODCALLTYPE createSwitcherNode(
			IUnknown *aPtrDownStreamTopologyNode,
			IUnknown **aPtrPtrTopologySwitcherNode);

		HRESULT pauseSwitchers(
			SessionDescriptor& aRefSessionDescriptor);

		HRESULT resumeSwitchers(
			SessionDescriptor& aRefSessionDescriptor);

		HRESULT detachSwitchers(
			SessionDescriptor& aRefSessionDescriptor);

		HRESULT attachSwitcher(
			/* [in] */ IUnknown *aPtrSwitcherNode,
			/* [in] */ IUnknown *aPtrDownStreamNode);

		HRESULT setVideoMixerPosition(
			/* [in] */ IUnknown *aPtrVideoMixerNode,
			/* [in] */ FLOAT aLeft,
			/* [in] */ FLOAT aRight,
			/* [in] */ FLOAT aTop,
			/* [in] */ FLOAT aBottom);

		HRESULT setVideoMixerSrcPosition(
			/* [in] */ IUnknown *aPtrVideoMixerNode,
			/* [in] */ FLOAT aLeft,
			/* [in] */ FLOAT aRight,
			/* [in] */ FLOAT aTop,
			/* [in] */ FLOAT aBottom);

		HRESULT setVideoMixerZOrder(
			/* [in] */ IUnknown *aPtrVideoMixerNode,
			/* [in] */ DWORD aZOrder);
		
		HRESULT setVideoMixerOpacity(
			/* [in] */ IUnknown *aPtrVideoMixerNode,
			/* [in] */ FLOAT aOpacity);

		HRESULT setVideoMixerFlush(
			/* [in] */ IUnknown *aPtrVideoMixerNode);

		HRESULT setAudioMixerRelativeVolume(
			/* [in] */ IUnknown *aPtrAudioMixerNode,
			/* [in] */ FLOAT aRelativeVolume);

		HRESULT createSARSinkOutputNode(
			CComPtrCustom<IUnknown>& aRefOutputNode);

		HRESULT getSARChannelCount(
			/* [in] */ IUnknown *aPtrSARNode,
			/* [out] */ UINT32 *aPtrCount);

		HRESULT getSARChannelVolume(
			/* [in] */ IUnknown *aPtrSARNode,
			/* [in] */ UINT32 aIndex,
			/* [out] */ float *aPtrLevel);

		HRESULT setSARChannelVolume(
			/* [in] */ IUnknown *aPtrSARNode,
			/* [in] */ UINT32 aIndex,
			/* [out] */ float aLevel);

		
		
	protected:
		CaptureManagerBroker();
		virtual ~CaptureManagerBroker();

	private:

		bool mDestroyedFlag;
	};
}
