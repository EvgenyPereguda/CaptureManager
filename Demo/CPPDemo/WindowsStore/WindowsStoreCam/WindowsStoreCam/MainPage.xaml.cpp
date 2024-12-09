//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace WindowsStoreCam;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Core;
using namespace Windows::UI;
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
using namespace Windows::Storage;
using namespace Windows::UI::Core;
using namespace concurrency;
using namespace std;

using namespace Windows::UI::Xaml::Media::Animation;


using namespace CaptureManagerToWinRTProxy;
using namespace CameraSession;


int lDeviceCount = 0;

IMap<String^, String^>^ lDeviceCollection = ref new Platform::Collections::Map<String^, String^>;

IMap<String^, String^>^ lAccessableDeviceCollection = ref new Platform::Collections::Map<String^, String^>;




MainPage::MainPage()
{
	InitializeComponent();
		
	Windows::ApplicationModel::Core::CoreApplication::Exiting += ref new EventHandler<Object^>(this, &MainPage::exiting);

	Application::Current->Suspending += ref new SuspendingEventHandler (this, &MainPage::suspending);

	Application::Current->Resuming += ref new EventHandler<Object^>(this, &MainPage::resuming);

	mPhotoDisplayPanel = ref new PhotoDisplayComponent::PhotoDisplayPanel;

	mFrontContentPresenter->Content = mPhotoDisplayPanel;

	mState = State::Video_Back_Photo_Front;
}

void MainPage::exiting(Platform::Object^ sender, Platform::Object^ e)
{

	if (mICameraSession != nullptr)
	{
		mICameraSession->stop();

		mICameraSession->close();
	}

}

void MainPage::suspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e)
{
	auto deferral = e->SuspendingOperation->GetDeferral();

	if (mICameraSession != nullptr)
	{
		mICameraSession->stop();

		mICameraSession->close();
	}

	deferral->Complete();
}

void MainPage::resuming(Platform::Object^ sender, Platform::Object^ e)
{
	if (mICameraSession != nullptr)
	{
		mICameraSession->stop();

		mICameraSession->close();
	}

	mCameraListBox->SelectedIndex = -1;


	this->Dispatcher->RunAsync(
		Windows::UI::Core::CoreDispatcherPriority::Normal,
		ref new Windows::UI::Core::DispatchedHandler([this]() {

		if (mCameraListBox->Items->Size > 0)
			mCameraListBox->SelectedIndex = 0;

	}));
	
}

void WindowsStoreCam::MainPage::Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	initLogPrintOut();

	initAccessVideoCapture();
}

void MainPage::initLogPrintOut()
{
	auto lILogPrintOutControl = CaptureManager::getInstance()->getILogPrintOutControl();

	do
	{

#if _DEBUG

		StorageFolder^ localFolder = ApplicationData::Current->LocalFolder;

		auto lLogFilePath = localFolder->Path + "\\Log.txt";

		lILogPrintOutControl->addPrintOutDestination(0, lLogFilePath);

		lILogPrintOutControl->addPrintOutDestination(1, lLogFilePath);
#endif

	} while (false);

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

				
				lInitializeTask->Completed = ref new AsyncActionCompletedHandler([this, lSymbolicLink, mediaCapture](
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
							
							auto lresult = CaptureManager::getInstance()->isInitialized();

							if (lresult)
							{
								fillCameraStack();
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

void MainPage::fillCameraStack()
{
	do
	{
		mCameraListBox->Items->Clear();

		mCameraListBox->BorderThickness = Thickness(2);

		mCameraListBox->BorderBrush = ref new SolidColorBrush(ColorHelper::FromArgb(255, 200, 10, 10));

		if (!CaptureManager::getInstance()->isInitialized())
			break;

		CaptureManagerToWinRTProxy::ISourceControl^ lISourceControl = CaptureManager::getInstance()->getISourceControl();

		if (lISourceControl == nullptr)
			break;

		String^ lXmlSourceCollection = lISourceControl->getCollectionOfSources();

		if (lXmlSourceCollection == nullptr)
			break;
		
		XmlDocument^ doc = ref new XmlDocument();

		doc->LoadXml(lXmlSourceCollection);

		auto lSourceXmlNodeList = doc->SelectNodes("Sources/Source");
		
		for (int lIndex = 0; lIndex < lSourceXmlNodeList->Length; lIndex++)
		{
			ListBoxItem ^ lItem = ref new ListBoxItem;

			auto lSourceXmlNode = lSourceXmlNodeList->Item(lIndex);

			auto lpart = lSourceXmlNode->SelectSingleNode("Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_MEDIA_TYPE']/Value.ValueParts/ValuePart[@Value='MFMediaType_Video']");

			if (lpart != nullptr)
			{
				auto lattr = lSourceXmlNode->SelectSingleNode("Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME']/SingleValue/@Value");
				
				lItem->Content = lattr->NodeValue;

				lItem->DataContext = lSourceXmlNode;

				mCameraListBox->Items->Append(lItem);
			}
		}

		if (mCameraListBox->Items->Size > 0)
			mCameraListBox->SelectedIndex = 0;

	} while (false);

}

void WindowsStoreCam::MainPage::mCameraListBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e)
{
	do
	{
		if (mCameraListBox->SelectedItem == nullptr)
			break;

		if (mICameraSession != nullptr)
		{
			mICameraSession->stop();

			mICameraSession->close();
		}

		ListBoxItem ^ lSelectedItem = safe_cast<ListBoxItem^>(mCameraListBox->SelectedItem);

		if (lSelectedItem == nullptr)
			break;

		auto lSourceXmlNode = safe_cast<IXmlNode^>(lSelectedItem->DataContext);

		if (lSourceXmlNode == nullptr)
			break;

		auto lattr = lSourceXmlNode->SelectSingleNode("Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK']/SingleValue/@Value");
		
		if (lattr == nullptr)
			break;
		
		mCurrentCameraSymbolicLink = lattr->NodeValue->ToString();

		auto lICameraSession = CameraSessionFactory::getInstance()->createICameraSession(
			mCurrentCameraSymbolicLink,
			mPhotoDisplayPanel);
			
		mICameraSession = lICameraSession;
		
		mBackContentPresenter->Content = lICameraSession->getDisplayPage();
		
		this->Dispatcher->RunAsync(
			Windows::UI::Core::CoreDispatcherPriority::Normal,
			ref new Windows::UI::Core::DispatchedHandler([this]() {

			if (mICameraSession != nullptr)
				mICameraSession->start();

		}));
		
	} while (false);
	
}


void WindowsStoreCam::MainPage::mFrontContentPresenter_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e)
{
	if (mState == State::Video_Back_Photo_Front)
	{
		mState = State::Photo_Back_Video_Front;



		mAnimContentPresenter->Width = mFrontContentPresenter->ActualWidth;

		mAnimContentPresenter->Height = mFrontContentPresenter->ActualHeight;
		

		Canvas::SetLeft(mAnimContentPresenter, mCanvas->ActualWidth - mAnimContentPresenter->Width - 7);

		Canvas::SetTop(mAnimContentPresenter, mCanvas->ActualHeight - mAnimContentPresenter->Height - 7);

				

		mBackContentPresenter->Content = nullptr;

		mFrontContentPresenter->Content = nullptr;


		mAnimContentPresenter->Content = mPhotoDisplayPanel;

		mFrontContentPresenter->Content = mICameraSession->getDisplayPage();


		mPhotoDisplayPanel->showElements();

		mICameraSession->hideElements();

		mCameraStack->Visibility = Xaml::Visibility::Collapsed;

		mForgrownGrid->Visibility = Xaml::Visibility::Collapsed;
		

		((Storyboard^)mCanvas->Resources->Lookup("storyboard"))->Begin();

	}
	else
	{
		mState = State::Video_Back_Photo_Front;

		mBackContentPresenter->Content = nullptr;

		mFrontContentPresenter->Content = nullptr;


		mBackContentPresenter->Content = mICameraSession->getDisplayPage(); 

		mFrontContentPresenter->Content = mPhotoDisplayPanel;



		mPhotoDisplayPanel->hideElements();

		mICameraSession->showElements();

		mCameraStack->Visibility = Xaml::Visibility::Visible;
	}

}


void WindowsStoreCam::MainPage::Storyboard_Completed(Platform::Object^ sender, Platform::Object^ e)
{

	mAnimContentPresenter->Content = nullptr;	
	
	mBackContentPresenter->Content = mPhotoDisplayPanel;

	mForgrownGrid->Visibility = Xaml::Visibility::Visible;

}


void WindowsStoreCam::MainPage::mCheckBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{

}


void WindowsStoreCam::MainPage::mCheckBtn_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	mForgrownGrid->IsTapEnabled = false;

	auto ld = mFrontContentPresenter->ActualHeight;

	mRiseAnim->To = ref new Platform::Box<double>(-(ld + 47));

	((Storyboard^)mForgrownGrid->Resources->Lookup("riseBtn"))->Begin();	
}


void WindowsStoreCam::MainPage::Storyboard_Completed_1(Platform::Object^ sender, Platform::Object^ e)
{
	mForgrownGrid->IsTapEnabled = true;
}


void WindowsStoreCam::MainPage::mCheckBtn_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	mForgrownGrid->IsTapEnabled = false;

	((Storyboard^)mForgrownGrid->Resources->Lookup("downBtn"))->Begin();	
}
