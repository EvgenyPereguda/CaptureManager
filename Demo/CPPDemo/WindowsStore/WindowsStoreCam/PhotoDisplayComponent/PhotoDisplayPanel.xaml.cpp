//
// PhotoDisplayPanel.xaml.cpp
// Implementation of the PhotoDisplayPanel class
//

#include "pch.h"
#include "PhotoDisplayPanel.xaml.h"

using namespace PhotoDisplayComponent;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::Storage;

PhotoDisplayPanel::PhotoDisplayPanel()
{
	InitializeComponent();
}

void PhotoDisplayPanel::addPhoto(IRandomAccessStream^ aStreamSource, bool aIsJPEG)
{		
	if (aIsJPEG)
	{		
		
		BitmapImage^ image = ref new BitmapImage();
		
		image->SetSource(aStreamSource);

		mPhotoImage->Source = image;

		savePictureStreamIntoFile(aStreamSource);

		aStreamSource->Seek(0);

	}
	else
	{
		//using (InMemoryRandomAccessStream stream = new InMemoryRandomAccessStream())
		//{
		//	using (DataWriter writer = new DataWriter(stream.GetOutputStreamAt(0)))
		//	{
		//		writer.WriteBytes(lData);
		//		writer.StoreAsync().GetResults();

		//		BitmapImage image = new BitmapImage();
		//		image.SetSource(stream);

		//		mPhotoImage.Source = image;
		//	}
		//}

		//using (IRandomAccessStream lEncoderStream = new InMemoryRandomAccessStream())
		//{
		//	var encoder = BitmapEncoder.CreateAsync(BitmapEncoder.JpegEncoderId, lEncoderStream).GetResults();

		//	encoder.SetPixelData(BitmapPixelFormat.Bgra8,
		//		BitmapAlphaMode.Ignore,
		//		takePhotoSampleGrabberCall.mImageWidth,
		//		takePhotoSampleGrabberCall.mImageHeight,
		//		96, 96, lData);

		//	encoder.FlushAsync().GetResults();

		//	savePictureStreamIntoFile(lEncoderStream.GetInputStreamAt(0));
		//}

	}
	

	

}

void PhotoDisplayPanel::savePictureStreamIntoFile(IRandomAccessStream^ aSourceStream)
{
	using namespace Concurrency;

	try
	{		
		auto destFolderTask = create_task(KnownFolders::PicturesLibrary->CreateFolderAsync("CameraTest", CreationCollisionOption::OpenIfExists));
		
		destFolderTask.then([this, aSourceStream](StorageFolder^ aStorageFolder)
		{
			auto formatter = ref new Windows::Globalization::DateTimeFormatting::DateTimeFormatter("{month.abbreviated}_{day.integer(1)}_{year.full}_at_{hour.integer(1)}_{minute.integer(2)}_{second.integer(2)}");
			
			Windows::Globalization::Calendar^ cal = ref new Windows::Globalization::Calendar();
			Windows::Foundation::DateTime dateToFormat = cal->GetDateTime();


			auto k = formatter->Format(dateToFormat);

			auto createFileTask = create_task(aStorageFolder->CreateFileAsync("Image_" + k + ".jpg", CreationCollisionOption::ReplaceExisting));

			createFileTask.then(
				[this, aSourceStream](StorageFile^ aStorageFile)
			{
				auto openFileTask = create_task(aStorageFile->OpenAsync(FileAccessMode::ReadWrite));

				openFileTask.then(
					[this, aSourceStream](IRandomAccessStream^ aIRandomAccessStream)
				{
					auto copyAndCloseAsyncTask = create_task(RandomAccessStream::CopyAndCloseAsync(aSourceStream, aIRandomAccessStream));
					
					copyAndCloseAsyncTask.then(
						[this, aSourceStream](unsigned long long aResult)
					{
					
					}
					);
				}
				);

			}				
				);


		});

	}
	catch (Exception^ ex)
	{

	}
}

void PhotoDisplayPanel::hideElements()
{

}

void PhotoDisplayPanel::showElements()
{

}