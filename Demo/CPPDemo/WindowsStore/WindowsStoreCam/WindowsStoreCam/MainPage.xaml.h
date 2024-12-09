//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"

namespace WindowsStoreCam
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
		enum class State
		{
			Video_Back_Photo_Front,
			Photo_Back_Video_Front
		};

	public:
		MainPage();

	private:

		State mState;

		Platform::String^ mCurrentCameraSymbolicLink;

		PhotoDisplayComponent::PhotoDisplayPanel^ mPhotoDisplayPanel;

		CameraSession::ICameraSession^ mICameraSession;

		void exiting(Platform::Object^ sender, Platform::Object^ e);

		void suspending(Platform::Object^ sender, Windows::ApplicationModel::SuspendingEventArgs^ e);

		void resuming(Platform::Object^ sender, Platform::Object^ e);

		void initLogPrintOut();

		void initAccessVideoCapture();

		void fillCameraStack();


	private:
		void Page_Loaded(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void mCameraListBox_SelectionChanged(Platform::Object^ sender, Windows::UI::Xaml::Controls::SelectionChangedEventArgs^ e);
		

		
		void mFrontContentPresenter_Tapped(Platform::Object^ sender, Windows::UI::Xaml::Input::TappedRoutedEventArgs^ e);
		void Storyboard_Completed(Platform::Object^ sender, Platform::Object^ e);
		void mCheckBtn_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void mCheckBtn_Checked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		void Storyboard_Completed_1(Platform::Object^ sender, Platform::Object^ e);
		void mCheckBtn_Unchecked(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
	};
}
