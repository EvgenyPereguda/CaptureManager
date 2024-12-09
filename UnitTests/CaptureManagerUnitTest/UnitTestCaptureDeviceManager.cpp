#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../CaptureManagerSource/MediaFoundationManager/MediaFoundationManager.h"
#include "../../CaptureManagerSource/CaptureDeviceManager/CaptureDeviceManager.h"
#include "../../CaptureManagerSource/Common/ComPtrCustom.h"
#include "../../CaptureManagerSource/Common/MFHeaders.h"
#include "../../CaptureManagerSource/Common/Singleton.h"
#include "../../CaptureManagerSource/PugiXML/pugixml.hpp"
#include "../../CaptureManagerSource/LogPrintOut/LogPrintOut.h"
#include "../../CaptureManagerSource/CaptureManager/Libraries.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace CaptureManager;
using namespace CaptureManager::Core;
using namespace CaptureManager::Core::MediaFoundation;
using namespace pugi;

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace CaptureManagerUnitTest
{
	TEST_MODULE_INITIALIZE(initialize)
	{
		LogPrintOut::getInstance().addFileLogPrintOutStream(
			LogPrintOut::INFO_LEVEL,
			L"UnitTestLog.txt");

		LogPrintOut::getInstance().addFileLogPrintOutStream(
			LogPrintOut::ERROR_LEVEL,
			L"UnitTestLog.txt");

		Singleton<MediaFoundationManager>::getInstance().initialize();
	}

	TEST_MODULE_CLEANUP(unInitialize)
	{
		Singleton<MediaFoundationManager>::getInstance().unInitialize();
	}

	TEST_CLASS(UnitTestCaptureDeviceManager)
	{
	public:
		
		TEST_METHOD(getXMLDocOfSources)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");


			xml_document lxmlDoc;

			auto ldeclNode = lxmlDoc.append_child(node_declaration);

			ldeclNode.append_attribute(L"version") = L"1.0";

			xml_node lcommentNode = lxmlDoc.append_child(node_comment);

			lcommentNode.set_value(L"XML Document of sources");

			auto lRootXMLElement = lxmlDoc.append_child(L"Sources");

			auto lresult = CaptureDeviceManager::getXMLDocOfSources(lRootXMLElement);

			Assert::IsFalse(lresult, L"CaptureDeviceManager: getXMLDocOfSources() cannot be executed!!!");

			std::wstringstream lwstringstream;

			lxmlDoc.print(lwstringstream);

			lxmlDoc.save_file("doc.xml");

			std::wstring lXMLDocumentString;

			lwstringstream.str(lXMLDocumentString);
		}
		
		TEST_METHOD(getSource)
		{
			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL, L"\n");

			LogPrintOut::getInstance().printOutln(
				LogPrintOut::INFO_LEVEL,
				__FUNCTIONW__, L"\n");


			xml_document lxmlDoc;

			auto ldeclNode = lxmlDoc.append_child(node_declaration);

			ldeclNode.append_attribute(L"version") = L"1.0";

			xml_node lcommentNode = lxmlDoc.append_child(node_comment);

			lcommentNode.set_value(L"XML Document of sources");

			auto lRootXMLElement = lxmlDoc.append_child(L"Sources");

			auto lresult = CaptureDeviceManager::getXMLDocOfSources(lRootXMLElement);

			Assert::IsFalse(lresult, L"CaptureDeviceManager: getXMLDocOfSources() cannot be executed!!!");


			auto l = lRootXMLElement.select_node(L"Source/Source.Attributes/Attribute[@Name='MF_DEVSOURCE_ATTRIBUTE_SOURCE_TYPE_AUDCAP_SYMBOLIC_LINK']/SingleValue/@Value");

			std::wstring lSymbolicLink(l.attribute().value());
			
			CComPtrCustom<IMFMediaSource> lMediaSource;

			lresult = CaptureDeviceManager::getSource(lSymbolicLink, &lMediaSource);

			Assert::IsFalse(lresult, L"CaptureDeviceManager: getSource() cannot be executed!!!");

			if (lMediaSource)
			{
				lMediaSource->Shutdown();
			}
		
		}
	};
}