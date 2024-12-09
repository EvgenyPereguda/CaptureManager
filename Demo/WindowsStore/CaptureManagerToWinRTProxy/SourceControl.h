#pragma once

#include "Interfaces/ISourceControl.h"
#include "CaptureManagerTypeInfo.h"
#include "ComPtrCustom.h"


namespace CaptureManagerToWinRTProxy
{
	ref class SourceControl sealed : 
		public CaptureManagerToWinRTProxy::ISourceControl
	{
	private:

		CComPtrCustom<::ISourceControl> mISourceControl;

	public:
		SourceControl(IntPtr aIntPtr);

		virtual String^ getCollectionOfSources();

		virtual IMediaType^ getSourceOutputMediaType(
			String^ aSymbolicLink,
			uint32 aIndexStream,
			uint32 aIndexMediaType);

		virtual ITopologyNode^ createSourceNode(
			String^ aSymbolicLink,
			uint32 aIndexStream,
			uint32 aIndexMediaType); 

		virtual ITopologyNode^ createSourceNodeWithDownStreamConnection(
			String^ aSymbolicLink,
			uint32 aIndexStream,
			uint32 aIndexMediaType,
			ITopologyNode^ aPtrDownStreamTopologyNode);

		virtual IMediaSource^ createMediaSource(String^ aSymbolicLink);

		virtual IMediaSource^ createMediaSource(ICaptureProcessor^ aICaptureProcessor);

		virtual IMediaType^ getSourceOutputMediaTypeFromMediaSource(
			IMediaSource^ aPtrMediaSource,
			uint32 aIndexStream,
			uint32 aIndexMediaType);

		virtual ITopologyNode^ createSourceNodeFromExternalSource(
			IMediaSource^ aPtrMediaSource,
			uint32 aIndexStream,
			uint32 aIndexMediaType);

		virtual ITopologyNode^ createSourceNodeFromExternalSourceWithDownStreamConnection(
			IMediaSource^ aPtrMediaSource,
			uint32 aIndexStream,
			uint32 aIndexMediaType,
			ITopologyNode^ aPtrDownStreamTopologyNode);

		virtual IWebCamControl^ createWebCamControl(String^ aSymbolicLink);
	};
}

