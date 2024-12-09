#pragma once

#include "pch.h"
#include "IMediaType.h"
#include "ITopologyNode.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class IEncoderNodeFactory
	{
		IMediaType^ createCompressedMediaType(
			IMediaType^ aPtrUncompressedMediaType,
			Guid aRefEncodingModeGUID,
			uint32 aEncodingModeValue,
			uint32 aIndexCompressedMediaType);

		ITopologyNode^ createEncoderNode(
			IMediaType^ aPtrUncompressedMediaType,
			Guid aRefEncodingModeGUID,
			uint32 aEncodingModeValue,
			uint32 aIndexCompressedMediaType,
			ITopologyNode^ aPtrDownStreamNode);
	};
}