//
// PhotoDisplayPanel.xaml.h
// Declaration of the PhotoDisplayPanel class
//

#pragma once

#include "PhotoDisplayPanel.g.h"
#include "IPhotoDisplayControl.h"

using namespace Windows::Storage::Streams;

namespace PhotoDisplayComponent
{
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class PhotoDisplayPanel sealed:
		public IPhotoDisplayControl
	{
	public:
		PhotoDisplayPanel();

		virtual void addPhoto(IRandomAccessStream^ aStreamSource, bool aIsJPEG);

		virtual void hideElements();

		virtual void showElements();

	private:
		void savePictureStreamIntoFile(IRandomAccessStream^ aSourceStream);

	};
}
