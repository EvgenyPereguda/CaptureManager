//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include "IRenderControl.h"
#include "CaptureManagerProxy.h"
#include "RenderSampleCallControl.xaml.h"

using namespace WindowsStoreWebCamViewer;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Data::Xml::Dom;
using namespace Windows::Devices::Enumeration;
using namespace Windows::Media::Capture;
using namespace Windows::Storage::Streams;
using namespace concurrency;
using namespace std;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238


IRenderControl^ gIRenderControl = nullptr;

int lDeviceCount = 0;

IMap<String^, String^>^ lDeviceCollection = ref new Platform::Collections::Map<String^, String^>;

IMap<String^, String^>^ lAccessableDeviceCollection = ref new Platform::Collections::Map<String^, String^>;

MainPage::MainPage()
{
	InitializeComponent();
	mPlayState = PlayState::None;
}



struct Collect
{
	Windows::Media::Capture::MediaCapture^ mediaCapture;

	Windows::Storage::Streams::InMemoryRandomAccessStream^ photoStream;
};

void WindowsStoreWebCamViewer::MainPage::Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

	initAccessVideoCapture();

}

void MainPage::initAccessVideoCapture()
{
	do
	{
		auto lFindAllTask = create_task(DeviceInformation::FindAllAsync(Windows::Devices::Enumeration::DeviceClass::VideoCapture));

		lFindAllTask.then([this](DeviceInformationCollection^ op) {


			String^ lIdString;

			for (int lIndex = 0;
				lIndex < op->Size;
				lIndex++)
			{
				auto litem = op->GetAt(lIndex);

				auto lname = litem->Name;

				auto kk = litem->Properties;

				lIdString = litem->Id;

				lDeviceCollection->Insert(lIdString, lname);
			}

			for (auto& litem : lDeviceCollection)
			{
				auto mediaCapture = ref new MediaCapture();

				auto settings = ref new MediaCaptureInitializationSettings();

				settings->VideoDeviceId = litem->Key;

				auto lSymbolicLink = litem->Key;

				auto lInitializeTask = mediaCapture->InitializeAsync(settings);

				lInitializeTask->Completed = ref new AsyncActionCompletedHandler([this, lSymbolicLink](
					IAsyncAction^ asyncInfo,
					AsyncStatus asyncStatus)
				{
					if (asyncStatus == AsyncStatus::Completed)
					{
						lAccessableDeviceCollection->Insert(lSymbolicLink, lDeviceCollection->Lookup(lSymbolicLink));

					}
					else
					{

					}

					if (++lDeviceCount == lDeviceCollection->Size)
					{
						Dispatcher->RunAsync(CoreDispatcherPriority::Normal,
							ref new DispatchedHandler([this]() {


							auto lresult = CaptureManagerProxy::getInstance().isInitialized();

							if (lresult)
							{
								fillSourceComboBox();

								fillRendererComboBox();
							}
							else
							{

							}
						}));

					}

				});
			}

		});

	} while (false);
}

void MainPage::fillSourceComboBox()
{
	do
	{
		CComPtrCustom<ISourceControl> lISourceControl;

		CaptureManagerProxy::getInstance().getISourceControl(&lISourceControl);

		BSTR lXMLstring = nullptr;

		auto lhresult = lISourceControl->getCollectionOfSources(&lXMLstring);

		if (lhresult != 0)
			break;

		auto lXMLString = ref new String(lXMLstring);

		XmlDocument^ doc = ref new XmlDocument();

		doc->LoadXml(lXMLString);

		auto lSourceXmlNodeList = doc->SelectNodes("Sources/Source");

		//auto k = doc->SelectNodes("Sources/Source/Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_MEDIA_TYPE']/Value.ValueParts/ValuePart[@Value='MFMediaType_Video']");

		for (int lIndex = 0; lIndex < lSourceXmlNodeList->Length; lIndex++)
		{
			ComboBoxItem^ lItem = ref new ComboBoxItem;

			auto lSourceXmlNode = lSourceXmlNodeList->Item(lIndex);

			auto lpart = lSourceXmlNode->SelectSingleNode("Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_MEDIA_TYPE']/Value.ValueParts/ValuePart[@Value='MFMediaType_Video']");

			if (lpart != nullptr)
			{
				auto lattr = lSourceXmlNode->SelectSingleNode("Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME']/SingleValue/@Value");

				lItem->Content = lattr->NodeValue;

				lItem->DataContext = lSourceXmlNode;

				mSourceComboBox->Items->Append(lItem);
			}
		}

	} while (false);


}

void WindowsStoreWebCamViewer::MainPage::mSourceComboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	do
	{
		if (mSourceComboBox->SelectedIndex < 0)
			mSizeComboBox->IsEnabled = false;

		mSizeComboBox->IsEnabled = true;

		mSizeComboBox->Items->Clear();

		auto lSourceXmlNode = safe_cast<IXmlNode^>((safe_cast<ComboBoxItem^>(mSourceComboBox->SelectedItem))->DataContext);

		if (lSourceXmlNode == nullptr)
			break;



		auto lStreamDescriptorXmlNodeList = lSourceXmlNode->SelectNodes("PresentationDescriptor/StreamDescriptor");

		if (lStreamDescriptorXmlNodeList->Length == 0)
			break;

		auto lMediaTypeXmlNodeList = lStreamDescriptorXmlNodeList->GetAt(0)->SelectNodes("MediaTypes/MediaType");

		for (int lIndex = 0; lIndex < lMediaTypeXmlNodeList->Length; lIndex++)
		{
			auto lMediaTypeXmlNode = lMediaTypeXmlNodeList->Item(lIndex);


			auto lFrameSizeXmlNode = lMediaTypeXmlNode->SelectSingleNode("MediaTypeItem[@Name='MF_MT_FRAME_SIZE']");

			auto lValuesXmlNodeList = lFrameSizeXmlNode->SelectNodes("Value.ValueParts/ValuePart");

			auto lWidthValueXmlNode = lValuesXmlNodeList->GetAt(0)->SelectSingleNode("@Value");

			auto lHeightValueXmlNode = lValuesXmlNodeList->GetAt(1)->SelectSingleNode("@Value");




			lFrameSizeXmlNode = lMediaTypeXmlNode->SelectSingleNode("MediaTypeItem[@Name='MF_MT_FRAME_RATE']");

			lValuesXmlNodeList = lFrameSizeXmlNode->SelectNodes("RatioValue");

			auto lFrameRateValueXmlNode = lValuesXmlNodeList->GetAt(0)->SelectSingleNode("@Value");



			lFrameSizeXmlNode = lMediaTypeXmlNode->SelectSingleNode("MediaTypeItem[@Name='MF_MT_SUBTYPE']");

			lValuesXmlNodeList = lFrameSizeXmlNode->SelectNodes("SingleValue");

			auto lSubTypeValueXmlNode = lValuesXmlNodeList->GetAt(0)->SelectSingleNode("@Value");



			ComboBoxItem^ lItem = ref new ComboBoxItem;

			lItem->Content = lWidthValueXmlNode->NodeValue->ToString() + " x " +
				lHeightValueXmlNode->NodeValue->ToString() + ", " +
				lFrameRateValueXmlNode->NodeValue->ToString() + ", " +
				lSubTypeValueXmlNode->NodeValue->ToString();

			lItem->DataContext = lMediaTypeXmlNode;

			mSizeComboBox->Items->Append(lItem);

		}

	} while (false);



}

void WindowsStoreWebCamViewer::MainPage::mSizeComboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	do
	{
		if (mSizeComboBox->SelectedIndex < 0)
		{
			mRendererComboBox->IsEnabled = false;

			break;
		}

		mRendererComboBox->IsEnabled = true;

	} while (false);

}

void WindowsStoreWebCamViewer::MainPage::fillRendererComboBox()
{
	do
	{
		CComPtrCustom<ISinkControl> lISinkControl;

		CaptureManagerProxy::getInstance().getISinkControl(&lISinkControl);

		BSTR lXMLstring = nullptr;

		auto lhresult = lISinkControl->getCollectionOfSinks(&lXMLstring);

		if (lhresult != 0)
			break;

		auto lXMLString = ref new String(lXMLstring);

		XmlDocument^ doc = ref new XmlDocument();

		doc->LoadXml(lXMLString);

		auto lSinkFactoryXmlNodeList = doc->SelectNodes("SinkFactories/SinkFactory");

		for (int lIndex = 0; lIndex < lSinkFactoryXmlNodeList->Length; lIndex++)
		{
			ComboBoxItem^ lItem = ref new ComboBoxItem;

			auto lSinkFactoryXmlNode = lSinkFactoryXmlNodeList->Item(lIndex);

			auto lGUIDSinkFactory = lSinkFactoryXmlNode->Attributes->GetNamedItem("GUID")->NodeValue->ToString();

			if (lGUIDSinkFactory == "{759D24FF-C5D6-4B65-8DDF-8A2B2BECDE39}" ||
				lGUIDSinkFactory == "{3D64C48E-EDA4-4EE1-8436-58B64DD7CF13}")
			{
				auto lTitle = lSinkFactoryXmlNode->Attributes->GetNamedItem("Title")->NodeValue->ToString();

				auto lModeXmlNodeList = lSinkFactoryXmlNode->SelectNodes("Value.ValueParts/ValuePart");

				auto lCount = lModeXmlNodeList->Length;

				auto lModeSinkFactory = lModeXmlNodeList->GetAt(0)->Attributes->GetNamedItem("GUID")->NodeValue->ToString();

				IMap<String^, String^>^ mDataCollection = ref new Platform::Collections::Map<String^, String^>;

				mDataCollection->Insert("lGUIDSinkFactory", lGUIDSinkFactory);

				mDataCollection->Insert("lModeSinkFactory", lModeSinkFactory);

				ComboBoxItem^ lItem = ref new ComboBoxItem;

				lItem->Content = lTitle;

				lItem->DataContext = mDataCollection;

				mRendererComboBox->Items->Append(lItem);
			}

			//auto lpart = lSourceXmlNode->SelectSingleNode("Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_MEDIA_TYPE']/Value.ValueParts/ValuePart[@Value='MFMediaType_Video']");

			//if (lpart != nullptr)
			//{
			//	auto lattr = lSourceXmlNode->SelectSingleNode("Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME']/SingleValue/@Value");

			//	lItem->Content = lattr->NodeValue;

			//	lItem->DataContext = lSourceXmlNode;

			//	mSourceComboBox->Items->Append(lItem);
			//}
		}

	} while (false);
}

void WindowsStoreWebCamViewer::MainPage::mRendererComboBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{

	do
	{
		if (mRendererComboBox->SelectedIndex < 0)
		{
			mPlayPauseButton->IsEnabled = false;

			break;
		}

		auto lDataCollection = safe_cast<IMap<String^, String^>^>(
			(safe_cast<ComboBoxItem^>(mRendererComboBox->SelectedItem))->DataContext);

		if (lDataCollection == nullptr)
			return;


		if (lDataCollection->Lookup("lGUIDSinkFactory") == "{759D24FF-C5D6-4B65-8DDF-8A2B2BECDE39}")
		{
			gIRenderControl = ref new RenderSampleCallControl;

			mRenderPresenter->Content = gIRenderControl;
		}
		else if (lDataCollection->Lookup("lGUIDSinkFactory") == "{3D64C48E-EDA4-4EE1-8436-58B64DD7CF13}")
		{
			//gIRenderControl = ref new RenderSampleCallbackControl;

			//mRenderPresenter->Content = gIRenderControl;
		}
		else
			break;


		if (gIRenderControl == nullptr)
			break;

		auto lSourceXmlNode = safe_cast<IXmlNode^>((safe_cast<ComboBoxItem^>(mSourceComboBox->SelectedItem))->DataContext);

		if (lSourceXmlNode == nullptr)
			break;

		auto lMediaTypeXmlNode = safe_cast<IXmlNode^>((safe_cast<ComboBoxItem^>(mSizeComboBox->SelectedItem))->DataContext);

		if (lMediaTypeXmlNode == nullptr)
			return;

		if (gIRenderControl->init(
			lSourceXmlNode,
			lMediaTypeXmlNode,
			lDataCollection))
		{
			changeState(PlayState::Ready);
		}

		//if (mDataCollection->Lookup("lGUIDSinkFactory") != "{759D24FF-C5D6-4B65-8DDF-8A2B2BECDE39}")
		//	break;


		//	RenderSampleCallControl

	} while (false);
}

void MainPage::changeState(PlayState aPlayState)
{
	switch (aPlayState)
	{
	case WindowsStoreWebCamViewer::PlayState::None:
	{
		mPlayPauseButton->IsEnabled = false;

		mSourceComboBox->IsEnabled = true;

		mSizeComboBox->IsEnabled = true;

		mRendererComboBox->IsEnabled = true;

		mRendererComboBox->SelectedIndex = -1;

		mStopButton->IsEnabled = false;

		mCloseButton->IsEnabled = false;

		mPlayPauseButton->Content = L"Play";

	}
		break;
	case WindowsStoreWebCamViewer::PlayState::Ready:
	{
		mPlayPauseButton->IsEnabled = true;

		mSourceComboBox->IsEnabled = true;

		mSizeComboBox->IsEnabled = true;

		mRendererComboBox->IsEnabled = true;

		mStopButton->IsEnabled = true;

		mCloseButton->IsEnabled = true;

		mPlayPauseButton->Content = L"Play";
	}
		break;
	case WindowsStoreWebCamViewer::PlayState::Play:
	{
		mPlayPauseButton->IsEnabled = true;

		mSourceComboBox->IsEnabled = false;

		mSizeComboBox->IsEnabled = false;

		mRendererComboBox->IsEnabled = false;

		mStopButton->IsEnabled = true;

		mCloseButton->IsEnabled = true;

		mPlayPauseButton->Content = L"Pause";
	}
		break;
	case WindowsStoreWebCamViewer::PlayState::Pause:
	{
		mPlayPauseButton->IsEnabled = true;

		mSourceComboBox->IsEnabled = false;

		mSizeComboBox->IsEnabled = false;

		mRendererComboBox->IsEnabled = false;

		mStopButton->IsEnabled = true;

		mCloseButton->IsEnabled = true;

		mPlayPauseButton->Content = L"Play";
	}
		break;
	case WindowsStoreWebCamViewer::PlayState::Stop:
	{
		mPlayPauseButton->IsEnabled = true;

		mSourceComboBox->IsEnabled = false;

		mSizeComboBox->IsEnabled = false;

		mRendererComboBox->IsEnabled = false;

		mStopButton->IsEnabled = true;

		mCloseButton->IsEnabled = true;

		mPlayPauseButton->Content = L"Play";
	}
		break;
	default:
		break;
	}

	mPlayState = aPlayState;
}

void WindowsStoreWebCamViewer::MainPage::mPlayPauseButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	do
	{
		if (mPlayState != PlayState::Play)
		{
			changeState(PlayState::Play);

			gIRenderControl->start();
		}
		else
		{
			changeState(PlayState::Pause);

			gIRenderControl->pause();
		}

	} while (false);
}

void WindowsStoreWebCamViewer::MainPage::mStopButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	do
	{
		if (mPlayState == PlayState::Play ||
			mPlayState == PlayState::Pause)
		{
			changeState(PlayState::Stop);

			gIRenderControl->stop();
		}

	} while (false);
}


void WindowsStoreWebCamViewer::MainPage::mCloseButton_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	do
	{
		if (mPlayState != PlayState::None)
		{
			changeState(PlayState::None);

			gIRenderControl->close();
		}

	} while (false);
}