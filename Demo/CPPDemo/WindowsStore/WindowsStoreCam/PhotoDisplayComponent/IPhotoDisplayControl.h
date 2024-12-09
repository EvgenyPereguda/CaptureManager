#pragma once

#include "pch.h"

using namespace Windows::Storage::Streams;

namespace PhotoDisplayComponent
{
	public interface class IPhotoDisplayControl
	{
		void addPhoto(IRandomAccessStream^ aStreamSource, bool aIsJPEG);

		void hideElements();

		void showElements();
	};
}
