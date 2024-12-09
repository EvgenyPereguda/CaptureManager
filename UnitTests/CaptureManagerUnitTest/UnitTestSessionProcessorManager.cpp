#include "stdafx.h"
#include "CppUnitTest.h"
#include <basetsd.h>
#include <Unknwnbase.h>
//#include "Common.h"
//#include "CaptureManager.h"
//#include "ISampleGrabberCallbackInner.h"
//#include "SessionProcessorManager.h"
#include "../../CaptureManagerSource/Common/ComPtrCustom.h"
#include "../../CaptureManagerSource/Common/Common.h"
#include "../../CaptureManagerSource/SessionProcessorManager/SessionProcessorManager.h"
#include "../../CaptureManagerSource/MediaFoundationManager/MediaFoundationManager.h"
#include "../../CaptureManagerSource/Common/MFHeaders.h"
#include "../../CaptureManagerSource/LogPrintOut/LogPrintOut.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace CaptureManager;
using namespace CaptureManager::Core;


namespace CaptureManagerUnitTest
{
	TEST_CLASS(UnitTestSessionProcessorManager)
	{
	public:
		
		//TEST_METHOD(createSession_closeSession)
		//{
		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL, L"\n");

		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL,
		//		__FUNCTIONW__, L"\n");

		//	std::vector<Source> lListOfSources;

		//	auto lresult = CaptureManager::getInstance().getListOfSources(lListOfSources);

		//	Assert::IsFalse(lresult, L"CaptureManager: getListOfVideoSources() cannot be executed!!!");

		//	Assert::IsFalse(lListOfSources.size() == 0, L"lListOfSources is empty!!!");

		//	SourceSettings lSourceSettings;

		//	lSourceSettings.mSymbolicLink = lListOfSources[0].mSymbolicLink;

		//	lSourceSettings.mIndexStream = lListOfSources[0].mlistOfVideoStreams[0].mStreamDescriptorIndex;

		//	lSourceSettings.mIndexMediaType = 0;

		//	CComPtrCustom<IMFTopologyNode> lVideoSourceNode;

		//	lresult = CaptureManager::getInstance().createSourceNode(
		//		lSourceSettings,
		//		(IUnknown**)&lVideoSourceNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSourceNode() cannot be executed!!!");



		//	CComPtrCustom<IMFTopologyNode> lSampleGrabberOutputNode;


		//	// implement ISampleGrabberCallbackInner interface

		//	class SampleGrabberCallback :
		//		public BaseUnknown<ISampleGrabberCallbackInner>
		//	{
		//	public:

		//		SampleGrabberCallback() :
		//			mRefCount(1)
		//		{}

		//		// ISampleGrabberCallbackInner interface

		//		virtual void operator()(
		//			REFGUID aGUIDMajorMediaType,
		//			DWORD aSampleFlags,
		//			LONGLONG aSampleTime,
		//			LONGLONG aSampleDuration,
		//			const BYTE* aPtrSampleBuffer,
		//			DWORD aSampleSize)
		//		{
		//			return;
		//		}

		//	private:

		//		std::atomic<ULONG> mRefCount;

		//		virtual ~SampleGrabberCallback(){}
		//	};

		//	// create sample grabber output node

		//	CComPtrCustom<ISampleGrabberCallbackInner> lSampleCallback = new (std::nothrow) SampleGrabberCallback();

		//	lresult = CaptureManager::getInstance().createSampleGrabberOutputNode(
		//		MFMediaType_Video,
		//		MFVideoFormat_RGB24,
		//		lSampleCallback,
		//		(IUnknown**)&lSampleGrabberOutputNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSampleGrabberOutputNode() cannot be executed!!!");

		//	SessionDescriptor lSessionDescriptor;

		//	std::vector<SessionProcessorManager::SourceToOutputPair> lTopologyNodeSourceToOutputPairs;

		//	SessionProcessorManager::SourceToOutputPair lpaire{ lVideoSourceNode, lSampleGrabberOutputNode };

		//	lTopologyNodeSourceToOutputPairs.push_back(lpaire);

		//	lresult = SessionProcessorManager::getInstance().createSession(lTopologyNodeSourceToOutputPairs, nullptr, lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: createSession() cannot be executed!!!");

		//	Assert::IsFalse(lSessionDescriptor < 0, L"SessionProcessorManager: lSessionDescriptor is not created!!!");

		//	lresult = SessionProcessorManager::getInstance().closeSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: closeSession() cannot be executed!!!");

		//}

		//TEST_METHOD(startCaptureSession)
		//{
		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL, L"\n");

		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL,
		//		__FUNCTIONW__, L"\n");

		//	std::vector<Source> lListOfSources;

		//	auto lresult = CaptureManager::getInstance().getListOfSources(lListOfSources);

		//	Assert::IsFalse(lresult, L"CaptureManager: getListOfVideoSources() cannot be executed!!!");

		//	Assert::IsFalse(lListOfSources.size() == 0, L"lListOfSources is empty!!!");

		//	SourceSettings lSourceSettings;

		//	lSourceSettings.mSymbolicLink = lListOfSources[0].mSymbolicLink;

		//	lSourceSettings.mIndexStream = lListOfSources[0].mlistOfVideoStreams[0].mStreamDescriptorIndex;

		//	lSourceSettings.mIndexMediaType = 0;

		//	CComPtrCustom<IMFTopologyNode> lVideoSourceNode;

		//	lresult = CaptureManager::getInstance().createSourceNode(
		//		lSourceSettings,
		//		(IUnknown**)&lVideoSourceNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSourceNode() cannot be executed!!!");



		//	CComPtrCustom<IMFTopologyNode> lSampleGrabberOutputNode;



		//	// implement ISampleGrabberCallbackInner interface

		//	class SampleGrabberCallback :
		//		public BaseUnknown<ISampleGrabberCallbackInner>
		//	{
		//	public:

		//		SampleGrabberCallback() :
		//			mRefCount(1)
		//		{}

		//		// ISampleGrabberCallbackInner interface

		//		virtual void operator()(
		//			REFGUID aGUIDMajorMediaType,
		//			DWORD aSampleFlags,
		//			LONGLONG aSampleTime,
		//			LONGLONG aSampleDuration,
		//			const BYTE* aPtrSampleBuffer,
		//			DWORD aSampleSize)
		//		{
		//			return;
		//		}

		//	private:

		//		std::atomic<ULONG> mRefCount;

		//		virtual ~SampleGrabberCallback(){}
		//	};

		//	// create sample grabber output node

		//	CComPtrCustom<ISampleGrabberCallbackInner> lSampleCallback = new (std::nothrow) SampleGrabberCallback();

		//	lresult = CaptureManager::getInstance().createSampleGrabberOutputNode(
		//		MFMediaType_Video,
		//		MFVideoFormat_RGB24,
		//		lSampleCallback,
		//		(IUnknown**)&lSampleGrabberOutputNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSampleGrabberOutputNode() cannot be executed!!!");

		//	SessionDescriptor lSessionDescriptor;

		//	std::vector<SessionProcessorManager::SourceToOutputPair> lTopologyNodeSourceToOutputPairs;

		//	SessionProcessorManager::SourceToOutputPair lpaire{ lVideoSourceNode, lSampleGrabberOutputNode };

		//	lTopologyNodeSourceToOutputPairs.push_back(lpaire);

		//	lresult = SessionProcessorManager::getInstance().createSession(lTopologyNodeSourceToOutputPairs, nullptr, lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: createSession() cannot be executed!!!");

		//	Assert::IsFalse(lSessionDescriptor < 0, L"SessionProcessorManager: lSessionDescriptor is not created!!!");




		//	lresult = SessionProcessorManager::getInstance().startCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: startCaptureSession() cannot be executed!!!");



		//	lresult = SessionProcessorManager::getInstance().closeSession(lSessionDescriptor);
		//	//MessageBox(NULL, L"", L"", 0);
		//	Assert::IsFalse(lresult, L"SessionProcessorManager: closeSession() cannot be executed!!!");

		//}

		//TEST_METHOD(stopCaptureSession)
		//{
		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL, L"\n");

		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL,
		//		__FUNCTIONW__, L"\n");

		//	std::vector<Source> lListOfSources;

		//	auto lresult = CaptureManager::getInstance().getListOfSources(lListOfSources);

		//	Assert::IsFalse(lresult, L"CaptureManager: getListOfVideoSources() cannot be executed!!!");

		//	Assert::IsFalse(lListOfSources.size() == 0, L"lListOfSources is empty!!!");

		//	SourceSettings lSourceSettings;

		//	lSourceSettings.mSymbolicLink = lListOfSources[0].mSymbolicLink;

		//	lSourceSettings.mIndexStream = lListOfSources[0].mlistOfVideoStreams[0].mStreamDescriptorIndex;

		//	lSourceSettings.mIndexMediaType = 0;

		//	CComPtrCustom<IMFTopologyNode> lVideoSourceNode;

		//	lresult = CaptureManager::getInstance().createSourceNode(
		//		lSourceSettings,
		//		(IUnknown**)&lVideoSourceNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSourceNode() cannot be executed!!!");



		//	CComPtrCustom<IMFTopologyNode> lSampleGrabberOutputNode;



		//	// implement ISampleGrabberCallbackInner interface

		//	class SampleGrabberCallback :
		//		public BaseUnknown<ISampleGrabberCallbackInner>
		//	{
		//	public:

		//		SampleGrabberCallback() :
		//			mRefCount(1)
		//		{}

		//		// ISampleGrabberCallbackInner interface

		//		virtual void operator()(
		//			REFGUID aGUIDMajorMediaType,
		//			DWORD aSampleFlags,
		//			LONGLONG aSampleTime,
		//			LONGLONG aSampleDuration,
		//			const BYTE* aPtrSampleBuffer,
		//			DWORD aSampleSize)
		//		{
		//			return;
		//		}

		//	private:

		//		std::atomic<ULONG> mRefCount;

		//		virtual ~SampleGrabberCallback(){}
		//	};

		//	// create sample grabber output node

		//	CComPtrCustom<ISampleGrabberCallbackInner> lSampleCallback = new (std::nothrow) SampleGrabberCallback();

		//	lresult = CaptureManager::getInstance().createSampleGrabberOutputNode(
		//		MFMediaType_Video,
		//		MFVideoFormat_RGB24,
		//		lSampleCallback,
		//		(IUnknown**)&lSampleGrabberOutputNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSampleGrabberOutputNode() cannot be executed!!!");

		//	SessionDescriptor lSessionDescriptor;

		//	std::vector<SessionProcessorManager::SourceToOutputPair> lTopologyNodeSourceToOutputPairs;

		//	SessionProcessorManager::SourceToOutputPair lpaire{ lVideoSourceNode, lSampleGrabberOutputNode };

		//	lTopologyNodeSourceToOutputPairs.push_back(lpaire);

		//	lresult = SessionProcessorManager::getInstance().createSession(lTopologyNodeSourceToOutputPairs, nullptr, lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: createSession() cannot be executed!!!");

		//	Assert::IsFalse(lSessionDescriptor < 0, L"SessionProcessorManager: lSessionDescriptor is not created!!!");




		//	lresult = SessionProcessorManager::getInstance().startCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: startCaptureSession() cannot be executed!!!");



		//	lresult = SessionProcessorManager::getInstance().stopCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: stopCaptureSession() cannot be executed!!!");
		//	//MessageBox(NULL, L"", L"", 0);


		//	lresult = SessionProcessorManager::getInstance().closeSession(lSessionDescriptor);
		//	//MessageBox(NULL, L"1", L"1", 0);
		//	Assert::IsFalse(lresult, L"SessionProcessorManager: closeSession() cannot be executed!!!");

		//}

		//TEST_METHOD(start_startCaptureSession)
		//{
		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL, L"\n");

		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL,
		//		__FUNCTIONW__, L"\n");

		//	std::vector<Source> lListOfSources;

		//	auto lresult = CaptureManager::getInstance().getListOfSources(lListOfSources);

		//	Assert::IsFalse(lresult, L"CaptureManager: getListOfVideoSources() cannot be executed!!!");

		//	Assert::IsFalse(lListOfSources.size() == 0, L"lListOfSources is empty!!!");

		//	SourceSettings lSourceSettings;

		//	lSourceSettings.mSymbolicLink = lListOfSources[0].mSymbolicLink;

		//	lSourceSettings.mIndexStream = lListOfSources[0].mlistOfVideoStreams[0].mStreamDescriptorIndex;

		//	lSourceSettings.mIndexMediaType = 0;

		//	CComPtrCustom<IMFTopologyNode> lVideoSourceNode;

		//	lresult = CaptureManager::getInstance().createSourceNode(
		//		lSourceSettings,
		//		(IUnknown**)&lVideoSourceNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSourceNode() cannot be executed!!!");



		//	CComPtrCustom<IMFTopologyNode> lSampleGrabberOutputNode;



		//	// implement ISampleGrabberCallbackInner interface

		//	class SampleGrabberCallback :
		//		public BaseUnknown<ISampleGrabberCallbackInner>
		//	{
		//	public:

		//		SampleGrabberCallback() :
		//			mRefCount(1)
		//		{}

		//		// ISampleGrabberCallbackInner interface

		//		virtual void operator()(
		//			REFGUID aGUIDMajorMediaType,
		//			DWORD aSampleFlags,
		//			LONGLONG aSampleTime,
		//			LONGLONG aSampleDuration,
		//			const BYTE* aPtrSampleBuffer,
		//			DWORD aSampleSize)
		//		{
		//			return;
		//		}
		//		
		//	private:

		//		std::atomic<ULONG> mRefCount;

		//		virtual ~SampleGrabberCallback(){}
		//	};

		//	// create sample grabber output node

		//	CComPtrCustom<ISampleGrabberCallbackInner> lSampleCallback = new (std::nothrow) SampleGrabberCallback();

		//	lresult = CaptureManager::getInstance().createSampleGrabberOutputNode(
		//		MFMediaType_Video,
		//		MFVideoFormat_RGB24,
		//		lSampleCallback,
		//		(IUnknown**)&lSampleGrabberOutputNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSampleGrabberOutputNode() cannot be executed!!!");

		//	SessionDescriptor lSessionDescriptor;

		//	std::vector<SessionProcessorManager::SourceToOutputPair> lTopologyNodeSourceToOutputPairs;

		//	SessionProcessorManager::SourceToOutputPair lpaire{ lVideoSourceNode, lSampleGrabberOutputNode };

		//	lTopologyNodeSourceToOutputPairs.push_back(lpaire);

		//	lresult = SessionProcessorManager::getInstance().createSession(lTopologyNodeSourceToOutputPairs, nullptr, lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: createSession() cannot be executed!!!");

		//	Assert::IsFalse(lSessionDescriptor < 0, L"SessionProcessorManager: lSessionDescriptor is not created!!!");




		//	lresult = SessionProcessorManager::getInstance().startCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: startCaptureSession() cannot be executed!!!");


		//	lresult = SessionProcessorManager::getInstance().startCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: startCaptureSession() cannot be executed!!!");

		//	Sleep(2000);

		//	lresult = SessionProcessorManager::getInstance().stopCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: stopCaptureSession() cannot be executed!!!");



		//	lresult = SessionProcessorManager::getInstance().closeSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: closeSession() cannot be executed!!!");

		//}

		//TEST_METHOD(start_stop_startCaptureSession)
		//{
		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL, L"\n");

		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL,
		//		__FUNCTIONW__, L"\n");

		//	std::vector<Source> lListOfSources;

		//	auto lresult = CaptureManager::getInstance().getListOfSources(lListOfSources);

		//	Assert::IsFalse(lresult, L"CaptureManager: getListOfVideoSources() cannot be executed!!!");

		//	Assert::IsFalse(lListOfSources.size() == 0, L"lListOfSources is empty!!!");

		//	SourceSettings lSourceSettings;

		//	lSourceSettings.mSymbolicLink = lListOfSources[0].mSymbolicLink;

		//	lSourceSettings.mIndexStream = lListOfSources[0].mlistOfVideoStreams[0].mStreamDescriptorIndex;

		//	lSourceSettings.mIndexMediaType = 0;

		//	CComPtrCustom<IMFTopologyNode> lVideoSourceNode;

		//	lresult = CaptureManager::getInstance().createSourceNode(
		//		lSourceSettings,
		//		(IUnknown**)&lVideoSourceNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSourceNode() cannot be executed!!!");



		//	CComPtrCustom<IMFTopologyNode> lSampleGrabberOutputNode;



		//	// implement ISampleGrabberCallbackInner interface

		//	class SampleGrabberCallback :
		//		public BaseUnknown<ISampleGrabberCallbackInner>
		//	{
		//	public:

		//		SampleGrabberCallback() :
		//			mRefCount(1)
		//		{}

		//		// ISampleGrabberCallbackInner interface

		//		virtual void operator()(
		//			REFGUID aGUIDMajorMediaType,
		//			DWORD aSampleFlags,
		//			LONGLONG aSampleTime,
		//			LONGLONG aSampleDuration,
		//			const BYTE* aPtrSampleBuffer,
		//			DWORD aSampleSize)
		//		{
		//			return;
		//		}

		//	private:

		//		std::atomic<ULONG> mRefCount;

		//		virtual ~SampleGrabberCallback(){}
		//	};

		//	// create sample grabber output node

		//	CComPtrCustom<ISampleGrabberCallbackInner> lSampleCallback = new (std::nothrow) SampleGrabberCallback();

		//	lresult = CaptureManager::getInstance().createSampleGrabberOutputNode(
		//		MFMediaType_Video,
		//		MFVideoFormat_RGB24,
		//		lSampleCallback,
		//		(IUnknown**)&lSampleGrabberOutputNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSampleGrabberOutputNode() cannot be executed!!!");

		//	SessionDescriptor lSessionDescriptor;

		//	std::vector<SessionProcessorManager::SourceToOutputPair> lTopologyNodeSourceToOutputPairs;

		//	SessionProcessorManager::SourceToOutputPair lpaire{ lVideoSourceNode, lSampleGrabberOutputNode };

		//	lTopologyNodeSourceToOutputPairs.push_back(lpaire);

		//	lresult = SessionProcessorManager::getInstance().createSession(lTopologyNodeSourceToOutputPairs, nullptr, lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: createSession() cannot be executed!!!");

		//	Assert::IsFalse(lSessionDescriptor < 0, L"SessionProcessorManager: lSessionDescriptor is not created!!!");




		//	lresult = SessionProcessorManager::getInstance().startCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: startCaptureSession() cannot be executed!!!");


		//	lresult = SessionProcessorManager::getInstance().stopCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: stopCaptureSession() cannot be executed!!!");


		//	lresult = SessionProcessorManager::getInstance().startCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: startCaptureSession() cannot be executed!!!");

		//	Sleep(2000);

		//	lresult = SessionProcessorManager::getInstance().stopCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: stopCaptureSession() cannot be executed!!!");



		//	lresult = SessionProcessorManager::getInstance().closeSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: closeSession() cannot be executed!!!");

		//}
		//
		//TEST_METHOD(start_pause_startCaptureSession)
		//{
		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL, L"\n");

		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL,
		//		__FUNCTIONW__, L"\n");

		//	std::vector<Source> lListOfSources;

		//	auto lresult = CaptureManager::getInstance().getListOfSources(lListOfSources);

		//	Assert::IsFalse(lresult, L"CaptureManager: getListOfVideoSources() cannot be executed!!!");

		//	Assert::IsFalse(lListOfSources.size() == 0, L"lListOfSources is empty!!!");

		//	SourceSettings lSourceSettings;

		//	lSourceSettings.mSymbolicLink = lListOfSources[0].mSymbolicLink;

		//	lSourceSettings.mIndexStream = lListOfSources[0].mlistOfVideoStreams[0].mStreamDescriptorIndex;

		//	lSourceSettings.mIndexMediaType = 0;

		//	CComPtrCustom<IMFTopologyNode> lVideoSourceNode;

		//	lresult = CaptureManager::getInstance().createSourceNode(
		//		lSourceSettings,
		//		(IUnknown**)&lVideoSourceNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSourceNode() cannot be executed!!!");



		//	CComPtrCustom<IMFTopologyNode> lSampleGrabberOutputNode;



		//	// implement ISampleGrabberCallbackInner interface

		//	class SampleGrabberCallback :
		//		public BaseUnknown<ISampleGrabberCallbackInner>
		//	{
		//	public:

		//		SampleGrabberCallback() :
		//			mRefCount(1)
		//		{}

		//		// ISampleGrabberCallbackInner interface

		//		virtual void operator()(
		//			REFGUID aGUIDMajorMediaType,
		//			DWORD aSampleFlags,
		//			LONGLONG aSampleTime,
		//			LONGLONG aSampleDuration,
		//			const BYTE* aPtrSampleBuffer,
		//			DWORD aSampleSize)
		//		{
		//			return;
		//		}

		//	private:

		//		std::atomic<ULONG> mRefCount;

		//		virtual ~SampleGrabberCallback(){}
		//	};

		//	// create sample grabber output node

		//	CComPtrCustom<ISampleGrabberCallbackInner> lSampleCallback = new (std::nothrow) SampleGrabberCallback();

		//	lresult = CaptureManager::getInstance().createSampleGrabberOutputNode(
		//		MFMediaType_Video,
		//		MFVideoFormat_RGB24,
		//		lSampleCallback,
		//		(IUnknown**)&lSampleGrabberOutputNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSampleGrabberOutputNode() cannot be executed!!!");

		//	SessionDescriptor lSessionDescriptor;

		//	std::vector<SessionProcessorManager::SourceToOutputPair> lTopologyNodeSourceToOutputPairs;

		//	SessionProcessorManager::SourceToOutputPair lpaire{ lVideoSourceNode, lSampleGrabberOutputNode };

		//	lTopologyNodeSourceToOutputPairs.push_back(lpaire);

		//	lresult = SessionProcessorManager::getInstance().createSession(lTopologyNodeSourceToOutputPairs, nullptr, lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: createSession() cannot be executed!!!");

		//	Assert::IsFalse(lSessionDescriptor < 0, L"SessionProcessorManager: lSessionDescriptor is not created!!!");




		//	lresult = SessionProcessorManager::getInstance().startCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: startCaptureSession() cannot be executed!!!");


		//	lresult = SessionProcessorManager::getInstance().pauseSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: pauseCaptureSession() cannot be executed!!!");


		//	lresult = SessionProcessorManager::getInstance().startCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: startCaptureSession() cannot be executed!!!");

		//	Sleep(2000);

		//	lresult = SessionProcessorManager::getInstance().stopCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: stopCaptureSession() cannot be executed!!!");



		//	lresult = SessionProcessorManager::getInstance().closeSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: closeSession() cannot be executed!!!");

		//}

		//TEST_METHOD(start_pause_pause_startCaptureSession)
		//{
		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL, L"\n");

		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL,
		//		__FUNCTIONW__, L"\n");

		//	std::vector<Source> lListOfSources;

		//	auto lresult = CaptureManager::getInstance().getListOfSources(lListOfSources);

		//	Assert::IsFalse(lresult, L"CaptureManager: getListOfVideoSources() cannot be executed!!!");

		//	Assert::IsFalse(lListOfSources.size() == 0, L"lListOfSources is empty!!!");

		//	SourceSettings lSourceSettings;

		//	lSourceSettings.mSymbolicLink = lListOfSources[0].mSymbolicLink;

		//	lSourceSettings.mIndexStream = lListOfSources[0].mlistOfVideoStreams[0].mStreamDescriptorIndex;

		//	lSourceSettings.mIndexMediaType = 0;

		//	CComPtrCustom<IMFTopologyNode> lVideoSourceNode;

		//	lresult = CaptureManager::getInstance().createSourceNode(
		//		lSourceSettings,
		//		(IUnknown**)&lVideoSourceNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSourceNode() cannot be executed!!!");



		//	CComPtrCustom<IMFTopologyNode> lSampleGrabberOutputNode;



		//	// implement ISampleGrabberCallbackInner interface

		//	class SampleGrabberCallback :
		//		public BaseUnknown<ISampleGrabberCallbackInner>
		//	{
		//	public:

		//		SampleGrabberCallback() :
		//			mRefCount(1)
		//		{}

		//		// ISampleGrabberCallbackInner interface

		//		virtual void operator()(
		//			REFGUID aGUIDMajorMediaType,
		//			DWORD aSampleFlags,
		//			LONGLONG aSampleTime,
		//			LONGLONG aSampleDuration,
		//			const BYTE* aPtrSampleBuffer,
		//			DWORD aSampleSize)
		//		{
		//			return;
		//		}

		//	private:

		//		std::atomic<ULONG> mRefCount;

		//		virtual ~SampleGrabberCallback(){}
		//	};

		//	// create sample grabber output node

		//	CComPtrCustom<ISampleGrabberCallbackInner> lSampleCallback = new (std::nothrow) SampleGrabberCallback();

		//	lresult = CaptureManager::getInstance().createSampleGrabberOutputNode(
		//		MFMediaType_Video,
		//		MFVideoFormat_RGB24,
		//		lSampleCallback,
		//		(IUnknown**)&lSampleGrabberOutputNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSampleGrabberOutputNode() cannot be executed!!!");

		//	SessionDescriptor lSessionDescriptor;

		//	std::vector<SessionProcessorManager::SourceToOutputPair> lTopologyNodeSourceToOutputPairs;

		//	SessionProcessorManager::SourceToOutputPair lpaire{ lVideoSourceNode, lSampleGrabberOutputNode };

		//	lTopologyNodeSourceToOutputPairs.push_back(lpaire);

		//	lresult = SessionProcessorManager::getInstance().createSession(lTopologyNodeSourceToOutputPairs, nullptr, lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: createSession() cannot be executed!!!");

		//	Assert::IsFalse(lSessionDescriptor < 0, L"SessionProcessorManager: lSessionDescriptor is not created!!!");




		//	lresult = SessionProcessorManager::getInstance().startCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: startCaptureSession() cannot be executed!!!");


		//	lresult = SessionProcessorManager::getInstance().pauseSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: pauseCaptureSession() cannot be executed!!!");


		//	lresult = SessionProcessorManager::getInstance().pauseSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: pauseCaptureSession() cannot be executed!!!");


		//	lresult = SessionProcessorManager::getInstance().startCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: startCaptureSession() cannot be executed!!!");

		//	Sleep(2000);

		//	lresult = SessionProcessorManager::getInstance().stopCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: stopCaptureSession() cannot be executed!!!");



		//	lresult = SessionProcessorManager::getInstance().closeSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: closeSession() cannot be executed!!!");

		//}

		//TEST_METHOD(start_pause_stop_startCaptureSession)
		//{
		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL, L"\n");

		//	LogPrintOut::getInstance().printOutln(
		//		LogPrintOut::INFO_LEVEL,
		//		__FUNCTIONW__, L"\n");

		//	std::vector<Source> lListOfSources;

		//	auto lresult = CaptureManager::getInstance().getListOfSources(lListOfSources);

		//	Assert::IsFalse(lresult, L"CaptureManager: getListOfVideoSources() cannot be executed!!!");

		//	Assert::IsFalse(lListOfSources.size() == 0, L"lListOfSources is empty!!!");

		//	SourceSettings lSourceSettings;

		//	lSourceSettings.mSymbolicLink = lListOfSources[0].mSymbolicLink;

		//	lSourceSettings.mIndexStream = lListOfSources[0].mlistOfVideoStreams[0].mStreamDescriptorIndex;

		//	lSourceSettings.mIndexMediaType = 0;

		//	CComPtrCustom<IMFTopologyNode> lVideoSourceNode;

		//	lresult = CaptureManager::getInstance().createSourceNode(
		//		lSourceSettings,
		//		(IUnknown**)&lVideoSourceNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSourceNode() cannot be executed!!!");



		//	CComPtrCustom<IMFTopologyNode> lSampleGrabberOutputNode;



		//	// implement ISampleGrabberCallbackInner interface

		//	class SampleGrabberCallback :
		//		public BaseUnknown<ISampleGrabberCallbackInner>
		//	{
		//	public:

		//		SampleGrabberCallback() :
		//			mRefCount(1)
		//		{}

		//		// ISampleGrabberCallbackInner interface

		//		virtual void operator()(
		//			REFGUID aGUIDMajorMediaType,
		//			DWORD aSampleFlags,
		//			LONGLONG aSampleTime,
		//			LONGLONG aSampleDuration,
		//			const BYTE* aPtrSampleBuffer,
		//			DWORD aSampleSize)
		//		{
		//			return;
		//		}

		//	private:

		//		std::atomic<ULONG> mRefCount;

		//		virtual ~SampleGrabberCallback(){}
		//	};

		//	// create sample grabber output node

		//	CComPtrCustom<ISampleGrabberCallbackInner> lSampleCallback = new (std::nothrow) SampleGrabberCallback();

		//	lresult = CaptureManager::getInstance().createSampleGrabberOutputNode(
		//		MFMediaType_Video,
		//		MFVideoFormat_RGB24,
		//		lSampleCallback,
		//		(IUnknown**)&lSampleGrabberOutputNode);

		//	Assert::IsFalse(lresult, L"CaptureManager: createSampleGrabberOutputNode() cannot be executed!!!");

		//	SessionDescriptor lSessionDescriptor;

		//	std::vector<SessionProcessorManager::SourceToOutputPair> lTopologyNodeSourceToOutputPairs;

		//	SessionProcessorManager::SourceToOutputPair lpaire{ lVideoSourceNode, lSampleGrabberOutputNode };

		//	lTopologyNodeSourceToOutputPairs.push_back(lpaire);

		//	lresult = SessionProcessorManager::getInstance().createSession(lTopologyNodeSourceToOutputPairs, nullptr, lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: createSession() cannot be executed!!!");

		//	Assert::IsFalse(lSessionDescriptor < 0, L"SessionProcessorManager: lSessionDescriptor is not created!!!");




		//	lresult = SessionProcessorManager::getInstance().startCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: startCaptureSession() cannot be executed!!!");


		//	lresult = SessionProcessorManager::getInstance().pauseSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: pauseCaptureSession() cannot be executed!!!");


		//	lresult = SessionProcessorManager::getInstance().stopCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: stopCaptureSession() cannot be executed!!!");


		//	lresult = SessionProcessorManager::getInstance().startCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: startCaptureSession() cannot be executed!!!");

		//	Sleep(2000);

		//	lresult = SessionProcessorManager::getInstance().stopCaptureSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: stopCaptureSession() cannot be executed!!!");



		//	lresult = SessionProcessorManager::getInstance().closeSession(lSessionDescriptor);

		//	Assert::IsFalse(lresult, L"SessionProcessorManager: closeSession() cannot be executed!!!");

		//}
	};
}