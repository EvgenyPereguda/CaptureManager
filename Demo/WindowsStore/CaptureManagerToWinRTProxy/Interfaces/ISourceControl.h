#pragma once

#include "pch.h"
#include "ICaptureProcessor.h"
#include "IWebCamControl.h"
#include "IMediaType.h"
#include "ITopologyNode.h"
#include "IMediaSource.h"


using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class ISourceControl
	{
		String^ getCollectionOfSources();

		IMediaType^ getSourceOutputMediaType(
			String^ aSymbolicLink,
			uint32 aIndexStream,
			uint32 aIndexMediaType);

		ITopologyNode^ createSourceNode(
			String^ aSymbolicLink,
			uint32 aIndexStream,
			uint32 aIndexMediaType);
		
		ITopologyNode^ createSourceNodeWithDownStreamConnection(
			String^ aSymbolicLink,
			uint32 aIndexStream,
			uint32 aIndexMediaType,
			ITopologyNode^ aPtrDownStreamTopologyNode);

		IMediaSource^ createMediaSource(String^ aSymbolicLink);

		IMediaSource^ createMediaSource(ICaptureProcessor^ aICaptureProcessor);
		
		IMediaType^ getSourceOutputMediaTypeFromMediaSource(
			IMediaSource^ aPtrMediaSource,
			uint32 aIndexStream,
			uint32 aIndexMediaType);

		ITopologyNode^ createSourceNodeFromExternalSource(
			IMediaSource^ aPtrMediaSource,
			uint32 aIndexStream,
			uint32 aIndexMediaType);

		ITopologyNode^ createSourceNodeFromExternalSourceWithDownStreamConnection(
			IMediaSource^ aPtrMediaSource,
			uint32 aIndexStream,
			uint32 aIndexMediaType,
			ITopologyNode^ aPtrDownStreamTopologyNode);

		IWebCamControl^ createWebCamControl(String^ aSymbolicLink);
	};
}
