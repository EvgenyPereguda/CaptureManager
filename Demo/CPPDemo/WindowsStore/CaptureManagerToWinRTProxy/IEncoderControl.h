#pragma once

#include "pch.h"
#include "IEncoderNodeFactory.h"
#include "IMediaType.h"


using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	public interface class IEncoderControl
	{
		String^ getCollectionOfEncoders();

		String^ getMediaTypeCollectionOfEncoder(
			IMediaType^ aPtrUncompressedMediaType,
			Guid aRefEncoderCLSID);

		IEncoderNodeFactory^ createEncoderNodeFactory(
			Guid aRefEncoderCLSID);
	};
}