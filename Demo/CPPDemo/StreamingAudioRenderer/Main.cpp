// StreamingAudioRenderer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#define WIN32_LEAN_AND_MEAN    

#include <SDKDDKVer.h>

#include <stdio.h>
#include <tchar.h>


#include <windows.h>
#include <Commdlg.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <gl/gl.h>
#include <memory>
#include <vector>
#include <atomic>
#include <Unknwnbase.h>
#include <fstream>
#include <iostream>
#include <map>
#include "Common/ComPtrCustom.h"
#include "Common/pugixml.hpp"
#include "Common/CaptureManagerTypeInfo.h"
#include "Common/CaptureManagerLoader.h"

#define IID_PPV_ARGSIUnknown(ppType) __uuidof(**(ppType)), (IUnknown**)(ppType)

using namespace pugi;

BOOL bQuit = FALSE;

void runLoop();


struct SourceData
{
	std::wstring m_SymbolicLink;

	std::wstring m_FriendlyName;
};

std::vector<SourceData> g_AudioSources;



void collectSources(xml_document& lSourcesXmlDoc)
{
	// find symbolic link for Video Source
	auto lFindVideoSymbolicLink = [](const xml_node &node)
	{
		bool lresult = false;


		if (lstrcmpW(node.name(), L"Source.Attributes") == 0)
		{

			// name 'MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK' has only video source
			xml_node lVideoSymbolicLinkAttrNode = node.find_child_by_attribute(L"Name", L"MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_VIDCAP_SYMBOLIC_LINK");

			if (lVideoSymbolicLinkAttrNode.empty())
			{
				// name 'MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK' has only audio source
				lVideoSymbolicLinkAttrNode = node.find_child_by_attribute(L"Name", L"MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK");
				// CM_DEVICE_LINK
				if (!lVideoSymbolicLinkAttrNode.empty())
				{
					xml_node lFriendlyNameAttrNode = node.find_child_by_attribute(L"Name", L"MF_DEVSOURCE_ATTRIBUTE_FRIENDLY_NAME");

					if (!lFriendlyNameAttrNode.empty())
					{
						auto lFirstValueAttrNode = lVideoSymbolicLinkAttrNode.first_child().attribute(L"Value");

						auto lSecondValueAttrNode = lFriendlyNameAttrNode.first_child().attribute(L"Value");


						if (!lFirstValueAttrNode.empty() && !lSecondValueAttrNode.empty())
						{
							SourceData lSourceData;

							lSourceData.m_SymbolicLink = lFirstValueAttrNode.as_string();

							lSourceData.m_FriendlyName = lSecondValueAttrNode.as_string();

							g_AudioSources.push_back(lSourceData);
						}

						lresult = true;
					}
				}
			}
		}

		return lresult;
	};

	// find first Video Source
	auto lFindFirstVideoSource = [lFindVideoSymbolicLink](const xml_node &node)
	{
		bool lresult = false;

		if ((lstrcmpW(node.name(), L"Source") == 0))
		{
			xml_node lAttrNode = node.find_node(lFindVideoSymbolicLink);
		}

		return lresult;
	};

	xml_node lVideoSourceXMLNode = lSourcesXmlDoc.find_node(lFindFirstVideoSource);
}

int _tmain(int argc, _TCHAR* argv[])
{

	HRESULT lhresult = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

	if (FAILED(lhresult))
		return lhresult;

	CComPtrCustom<IClassFactory> lCoLogPrintOut;

	lhresult = CaptureManagerLoader::getInstance().createCalssFactory(__uuidof(CoLogPrintOut), &lCoLogPrintOut);

	if (FAILED(lhresult))
		return lhresult;

	CComPtrCustom<ILogPrintOutControl> lLogPrintOutControl;

	lCoLogPrintOut->LockServer(true);

	lhresult = lCoLogPrintOut->CreateInstance(
		nullptr,
		IID_PPV_ARGS(&lLogPrintOutControl));

	if (FAILED(lhresult))
		return lhresult;

	// set log file for info
	lhresult = lLogPrintOutControl->addPrintOutDestination(
		(DWORD)INFO_LEVEL,
		L"Log.txt");

	// set log file for info
	lhresult = lLogPrintOutControl->addPrintOutDestination(
		(DWORD)ERROR_LEVEL,
		L"Log.txt");

	if (FAILED(lhresult))
		return lhresult;

	/* initialisation CaptureManager */

	CComPtrCustom<IClassFactory> lCoCaptureManager;

	lhresult = CaptureManagerLoader::getInstance().createCalssFactory(__uuidof(CoCaptureManager), &lCoCaptureManager);

	if (FAILED(lhresult))
		return lhresult;

	lCoCaptureManager->LockServer(true);

	CComPtrCustom<ICaptureManagerControl> lCaptureManagerControl;

	// get ICaptureManagerControl interfrace
	lhresult = lCoCaptureManager->CreateInstance(
		nullptr,
		IID_PPV_ARGS(&lCaptureManagerControl));

	if (FAILED(lhresult))
		return lhresult;



	// get ISourceControl inetrface
	CComPtrCustom<ISourceControl> lSourceControl;

	lhresult = lCaptureManagerControl->createControl(
		IID_PPV_ARGSIUnknown(&lSourceControl));

	if (FAILED(lhresult))
		return lhresult;

	BSTR lXMLString = nullptr;

	lhresult = lSourceControl->getCollectionOfSources(&lXMLString);

	if (FAILED(lhresult))
		return lhresult;


	xml_document lSourcesXmlDoc;

	lSourcesXmlDoc.load_string(lXMLString);


	// Get source collection.
	collectSources(lSourcesXmlDoc);

	SysFreeString(lXMLString);
	

	std::wcout << std::endl << L"Select Audio source:" << std::endl;

	int lIndex = 0;

	for (auto& lItem : g_AudioSources)
	{
		std::wcout << ++lIndex << L" " << lItem.m_FriendlyName << std::endl;
	}

	std::wcout << std::endl << L"Enter number: ";

	int g_AudioSourceIndex = 0;

	std::cin >> g_AudioSourceIndex;

	if (g_AudioSourceIndex == 0)
		return -1;

	BSTR lSymbolicLink = SysAllocString(
		g_AudioSources[g_AudioSourceIndex - 1].m_SymbolicLink.c_str());

	DWORD lIndexStream = 0; // Number of media stream

	DWORD lIndexMediaType = 0; // Number of Media Type




	// Create Audio renderer node
	// get ISinkControl inetrface
	CComPtrCustom<ISinkControl> lSinkControl;

	lhresult = lCaptureManagerControl->createControl(
		IID_PPV_ARGSIUnknown(&lSinkControl));

	if (FAILED(lhresult))
		return lhresult;


	BSTR lXMLstring = nullptr;


	lSinkControl->getCollectionOfSinks(&lXMLstring);

	xml_document lSinksXmlDoc;

	auto k = lSinksXmlDoc.load_string(lXMLstring);

	if (lXMLstring != nullptr)
		SysFreeString(lXMLstring);


	// find sample call sink by GUID - IID_ISampleGrabberCallSinkFactory
	auto lFindSink = [](const xml_node &node)
	{
		bool lresult = false;

		if ((lstrcmpW(node.name(), L"SinkFactory") == 0))
		{
			xml_attribute lAttrNode = node.attribute(L"GUID");

			if (!lAttrNode.empty())
			{
				if ((lstrcmpW(lAttrNode.as_string(), L"{62F99475-1754-4B2B-BFA5-277BDCE03C4F}") == 0))
					lresult = true;
			}
		}
		return lresult;
	};


	xml_node lSinkXMLNode = lSinksXmlDoc.find_node(lFindSink);

	if (lSinkXMLNode.empty())
		return -1;

	// get first read mode xml node (SYNC read mode)
	//                                  Value.ValueParts/ValuePart[0]
	xml_node lValuePartNode = lSinkXMLNode.first_child().first_child();

	if (lValuePartNode.empty())
		return -1;


	xml_attribute lReadModeXMLAttr = lValuePartNode.attribute(L"GUID");

	if (lReadModeXMLAttr.empty())
		return -1;

	// get read mode IID
	IID lReadMode;

	lhresult = IIDFromString(lReadModeXMLAttr.as_string(),
		&lReadMode);

	if (FAILED(lhresult))
		return lhresult;
	
	// create object with ISampleGrabberCallSinkFactory interface
	CComPtrCustom<ISARSinkFactory> lSARSinkFactory;

	lhresult = lSinkControl->createSinkFactory(
		lReadMode,
		IID_PPV_ARGSIUnknown(&lSARSinkFactory));

	if (FAILED(lhresult))
		return lhresult;

	CComPtrCustom<IUnknown> SARActivateTopologyNode;

	lhresult = lSARSinkFactory->createOutputNode(
		&SARActivateTopologyNode);

	if (FAILED(lhresult))
		return lhresult;

	// get video source node
	CComPtrCustom<IUnknown> lAudioSourceNode;

	lhresult = lSourceControl->createSourceNodeWithDownStreamConnection(
		lSymbolicLink,
		lIndexStream,
		lIndexMediaType,
		SARActivateTopologyNode,
		&lAudioSourceNode);

	if (FAILED(lhresult))
		return lhresult;

	SysFreeString(lSymbolicLink);




	// create session descriptor


	CComPtrCustom<ISessionControl> lSessionControl;

	lhresult = lCaptureManagerControl->createControl(
		IID_PPV_ARGSIUnknown(&lSessionControl));

	if (FAILED(lhresult))
		return lhresult;


	SAFEARRAY* pSA = NULL;
	SAFEARRAYBOUND bound[1];
	bound[0].lLbound = 0;
	bound[0].cElements = 1;
	pSA = SafeArrayCreate(VT_VARIANT, 1, bound);


	VARIANT lVar;

	VariantInit(&lVar);

	lVar.vt = VT_UNKNOWN;

	lVar.punkVal = lAudioSourceNode;

	long i = 0;

	lhresult = SafeArrayPutElement(pSA, &i, &lVar);

	if (FAILED(lhresult))
		return lhresult;

	VARIANT theArray;

	VariantInit(&theArray);

	theArray.vt = VT_SAFEARRAY | VT_UNKNOWN;

	theArray.parray = pSA;


	CComPtrCustom<ISession> lSession;

	lhresult = lSessionControl->createSession(
		theArray,
		IID_PPV_ARGSIUnknown(&lSession));

	SafeArrayDestroy(pSA);

	VariantClear(&theArray);

	if (FAILED(lhresult))
		return lhresult;

	class SessionCallback : public ISessionCallback
	{
	public:

		SessionCallback() :
			mRefCount(1)
		{}

		virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE invoke(
			/* [in] */ DWORD aCallbackEventCode,
			/* [in] */ DWORD aSessionDescriptor)
		{
			switch (aCallbackEventCode)
			{
			case SessionCallbackEventCode::Error:
			{}
			break;
			case SessionCallbackEventCode::Status_Error:
			{
				bQuit = TRUE;
			}
			break;
			case SessionCallbackEventCode::Execution_Error:
			{}
			break;
			case SessionCallbackEventCode::ItIsReadyToStart:
			{}
			break;
			case SessionCallbackEventCode::ItIsStarted:
			{}
			break;
			case SessionCallbackEventCode::ItIsPaused:
			{}
			break;
			case SessionCallbackEventCode::ItIsStopped:
			{}
			break;
			case SessionCallbackEventCode::ItIsEnded:
			{}
			break;
			case SessionCallbackEventCode::ItIsClosed:
			{}
			break;
			case SessionCallbackEventCode::VideoCaptureDeviceRemoved:
			{}
			break;

			case SessionCallbackEventCode::UnknownEvent:
			default:
			{}
			break;
			}

			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, _COM_Outptr_ void __RPC_FAR *__RPC_FAR *ppvObject)
		{
			HRESULT lhresult = E_NOINTERFACE;

			do
			{
				if (ppvObject == NULL)
				{
					lhresult = E_POINTER;

					break;
				}

				lhresult = S_OK;

				if (riid == IID_IUnknown)
				{
					*ppvObject = static_cast<IUnknown*>(this);

					break;
				}
				else if (riid == __uuidof(ISessionCallback))
				{
					*ppvObject = static_cast<ISessionCallback*>(this);

					break;
				}

				*ppvObject = NULL;

				lhresult = E_NOINTERFACE;

			} while (false);

			if (SUCCEEDED(lhresult))
				AddRef();

			return lhresult;
		}

		virtual ULONG STDMETHODCALLTYPE AddRef(void)
		{
			return ++mRefCount;
		}

		virtual ULONG STDMETHODCALLTYPE Release(void)
		{
			ULONG lCount = --mRefCount;

			if (lCount == 0)
			{
				delete this;
			}
			return lCount;
		}

	private:

		std::atomic<ULONG> mRefCount;

		virtual ~SessionCallback() {}
	};

	CComPtrCustom<IConnectionPointContainer> IConnectionPointContainer;

	lhresult = lSession->getIConnectionPointContainer(
		IID_PPV_ARGSIUnknown(&IConnectionPointContainer));

	if (FAILED(lhresult))
		return lhresult;

	CComPtrCustom<IConnectionPoint> lConnectionPoint;

	lhresult = IConnectionPointContainer->FindConnectionPoint(
		__uuidof(ISessionCallback),
		&lConnectionPoint);

	if (FAILED(lhresult))
		return lhresult;

	CComPtrCustom<ISessionCallback> lSessionCallback = new SessionCallback();

	DWORD lStreamID;

	lhresult = lConnectionPoint->Advise(
		lSessionCallback,
		&lStreamID);

	if (FAILED(lhresult))
		return lhresult;


	if (lSession != nullptr)
	{
		lSession->startSession(0, GUID_NULL);
	}

	runLoop();

	if (lSession != nullptr)
	{
		lSession->stopSession();

		lSession->closeSession();
	}

	lCoCaptureManager->LockServer(false);
}

void runLoop()
{
	MSG msg;

	/* program main loop */
	while (!bQuit)
	{
		/* check for messages */
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			/* handle or dispatch messages */
			if (msg.message == WM_QUIT)
			{
				bQuit = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		Sleep(200);
	}

}
