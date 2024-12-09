#pragma once

#include "pch.h"

using namespace Platform;


namespace CaptureManagerToWinRTProxy
{
	
	public interface class IWriteableBitmapCallback
	{

		Windows::UI::Core::CoreDispatcher^ getDispatcher();
		
		void invoke(
			Guid aGUIDMajorMediaType,
			uint32 aSampleFlags,
			int64 aSampleTime,
			int64 aSampleDuration,
			Windows::UI::Xaml::Media::Imaging::WriteableBitmap^ aWriteableBitmap);
		
	};
}