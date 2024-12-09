#pragma once

#include "pch.h"

using namespace Windows::Data::Xml::Dom;
using namespace Windows::Foundation::Collections;
using namespace Platform;

namespace WindowsStoreWebCamViewer
{
	public interface class IRenderControl
	{
		virtual bool init(
			IXmlNode^ aSourceXmlNode,
			IXmlNode^ aMediaTypeXmlNode,
			IMap<String^, String^>^ aDataCollection) = 0;

		virtual bool start() = 0;

		virtual bool pause() = 0;

		virtual bool stop() = 0;

		virtual bool close() = 0;
	};
}