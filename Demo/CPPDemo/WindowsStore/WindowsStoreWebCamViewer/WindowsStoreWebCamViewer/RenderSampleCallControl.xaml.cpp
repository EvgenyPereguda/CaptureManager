//
// RenderSampleCallControl.xaml.cpp
// Implementation of the RenderSampleCallControl class
//

#include "pch.h"
#include "RenderSampleCallControl.xaml.h"

using namespace WindowsStoreWebCamViewer;

using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Media::Imaging;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Storage::Streams;
using namespace std;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238


const GUID MFMediaType_Video = { 0x73646976, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 } };

const GUID MFVideoFormat_RGB32 = { 22, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71 } };

byte* lData;


BitmapSource^ fromArray(byte* data, unsigned long w, unsigned long h, int ch)
{

	WriteableBitmap^ wbm = ref new WriteableBitmap((int)w, (int)h);



	IUnknown* pUnk = reinterpret_cast<IUnknown*>(wbm->PixelBuffer);

	CComPtrCustom<IBufferByteAccess> lIBufferByteAccess;
	HRESULT hr = pUnk->QueryInterface(IID_PPV_ARGS(&lIBufferByteAccess));
	byte *pbytes = nullptr;
	hr = lIBufferByteAccess->Buffer(&pbytes);

	memcpy(pbytes, data, wbm->PixelBuffer->Length);

	return wbm;
}


RenderSampleCallControl::RenderSampleCallControl()
{
	InitializeComponent();
}

bool RenderSampleCallControl::init(
	IXmlNode^ aSourceXmlNode,
	IXmlNode^ aMediaTypeXmlNode,
	IMap<String^, String^>^ aDataCollection)
{
	bool lresult = false;

	do
	{

		if (aSourceXmlNode == nullptr)
			break;



		auto lSymbolicLinkXmlNode = aSourceXmlNode->SelectSingleNode("Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK']/SingleValue/@Value");

		if (lSymbolicLinkXmlNode == nullptr)
			break;

		auto lSymbolicLink = lSymbolicLinkXmlNode->NodeValue->ToString();

		int lStreamIndex = 0;

		if (aMediaTypeXmlNode == nullptr)
			break;

		auto lStringValue = aMediaTypeXmlNode->Attributes->GetNamedItem("Index")->NodeValue->ToString();

		auto lMediaTypeIndex = std::stoul(lStringValue->Data());




		auto lFrameSizeXmlNode = aMediaTypeXmlNode->SelectSingleNode("MediaTypeItem[@Name='MF_MT_FRAME_SIZE']");

		auto lValuesXmlNodeList = lFrameSizeXmlNode->SelectNodes("Value.ValueParts/ValuePart");

		auto lWidthValueXmlNode = lValuesXmlNodeList->GetAt(0)->SelectSingleNode("@Value");

		auto lHeightValueXmlNode = lValuesXmlNodeList->GetAt(1)->SelectSingleNode("@Value");




		unsigned long lVideoWidth = std::stoul(lWidthValueXmlNode->NodeValue->ToString()->Data());

		unsigned long lVideoHeight = std::stoul(lHeightValueXmlNode->NodeValue->ToString()->Data());



		LONG lWidthInBytes;

		CComPtrCustom<IStrideForBitmap> lIStrideForBitmap;

		CaptureManagerProxy::getInstance().getIStrideForBitmap(&lIStrideForBitmap);

		lIStrideForBitmap->getStrideForBitmap(
			MFVideoFormat_RGB32,
			lVideoWidth,
			&lWidthInBytes);


		auto lsampleByteSize = (unsigned long)std::abs(lWidthInBytes) * lVideoHeight;


		if (aDataCollection == nullptr)
			break;

		if (aDataCollection->Lookup("lGUIDSinkFactory") != "{759D24FF-C5D6-4B65-8DDF-8A2B2BECDE39}")
			break;

		auto lModeSinkFactory = aDataCollection->Lookup("lModeSinkFactory");

		CComPtrCustom<ISinkControl> lISinkControl;

		CaptureManagerProxy::getInstance().getISinkControl(&lISinkControl);

		ISampleGrabberCallSinkFactory*lISinkFactory;

		IID lMode;

		IIDFromString(lModeSinkFactory->Data(), &lMode);

		lISinkControl->createSinkFactory(
			lMode,
			__uuidof(ISampleGrabberCallSinkFactory),
			(IUnknown**)&lISinkFactory);

		ISampleGrabberCall* lISampleGrabberCall;

		lISinkFactory->createOutputNode(
			MFMediaType_Video,
			MFVideoFormat_RGB32,
			lsampleByteSize,
			__uuidof(ISampleGrabberCall),
			(IUnknown**)&lISampleGrabberCall);

		if (lISampleGrabberCall != nullptr)
		{

			lData = new byte[lsampleByteSize];

			mTimer = ref new DispatcherTimer();

			mTimer->Tick += ref new EventHandler<Platform::Object^>(
				[this, lISampleGrabberCall, lVideoWidth, lVideoHeight](Platform::Object^ sender, Platform::Object^ aeg)
			{
				DWORD lByteSize = 0;

				auto kk = lISampleGrabberCall->readData(lData, &lByteSize);

				if (kk != S_OK)
				{
					kk = S_OK;
				}

				if (lByteSize != 0)
				{
					mDisplayImage->Source = fromArray(lData, lVideoWidth, lVideoHeight, 4);
				}
			});

			CComPtrCustom<IUnknown> lSampleGrabberCallNode;

			lISampleGrabberCall->QueryInterface(IID_PPV_ARGS(&lSampleGrabberCallNode));

			if (lSampleGrabberCallNode == nullptr)
				break;


			CComPtrCustom<ISourceControl> lISourceControl;

			CaptureManagerProxy::getInstance().getISourceControl(&lISourceControl);

			CComPtrCustom<IUnknown> lISourceTopologyNode;

			wstring lstringSymbolicLink(lSymbolicLink->Data());

			auto lSize = lstringSymbolicLink.size();

			std::unique_ptr<OLECHAR> lBSTRSymbolicLink(new OLECHAR[lSize + 1]);

			memcpy(lBSTRSymbolicLink.get(),
				lstringSymbolicLink.c_str(),
				(lSize + 1)*sizeof(OLECHAR));

			lBSTRSymbolicLink.get()[lSize] = '\0';

			lISourceControl->createSourceNodeWithDownStreamConnection(
				lBSTRSymbolicLink.get(),
				lStreamIndex,
				lMediaTypeIndex,
				lSampleGrabberCallNode,
				&lISourceTopologyNode);


			vector<IUnknown*> lSourcesList;

			lSourcesList.push_back(lISourceTopologyNode);

			lresult = CaptureManagerProxy::getInstance().getISession(lSourcesList, &mISession);
			//if ()
			//{
			//	//lISession->startSession(0, GUID_NULL);

			//	//mTimer->Start();
			//}

		}

	} while (false);

	return lresult;
}



bool RenderSampleCallControl::start()
{
	bool lresult = false;

	do
	{
		if (!mISession)
			break;

		mISession->startSession(0, GUID_NULL);

		mTimer->Start();

	} while (false);

	return lresult;
}

bool RenderSampleCallControl::pause()
{
	bool lresult = false;

	do
	{
		if (!mISession)
			break;

		mTimer->Stop();

		auto kk = mISession->pauseSession();

		if (SUCCEEDED(kk))
		{
			kk = S_OK;
		}

	} while (false);

	return lresult;
}

bool RenderSampleCallControl::stop()
{
	bool lresult = false;

	do
	{
		if (!mISession)
			break;

		mISession->stopSession();

		mTimer->Stop();

	} while (false);

	return lresult;
}

bool RenderSampleCallControl::close()
{
	bool lresult = false;

	do
	{
		if (!mISession)
			break;

		mISession->closeSession();

		mTimer->Stop();

	} while (false);

	return lresult;
}