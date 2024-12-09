//
// RenderSampleCallControl.xaml.h
// Declaration of the RenderSampleCallControl class
//

#pragma once

#include "RenderSampleCallControl.g.h"
#include "ComPtrCustom.h"
#include "CaptureManagerProxy.h"
#include "IRenderControl.h"


using namespace Platform;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::Data::Xml::Dom;

namespace WindowsStoreWebCamViewer
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	[Windows::Foundation::Metadata::WebHostHidden]
	public ref class RenderSampleCallControl sealed : IRenderControl
	{
	public:
		RenderSampleCallControl();

		virtual bool init(
			IXmlNode^ aSourceXmlNode,
			IXmlNode^ aMediaTypeXmlNode,
			IMap<String^, String^>^ aDataCollection);



		virtual bool start();

		virtual bool pause();

		virtual bool stop();

		virtual bool close();

	private:

		DispatcherTimer^ mTimer;

		CComPtrCustom<ISession> mISession;
	};
}
