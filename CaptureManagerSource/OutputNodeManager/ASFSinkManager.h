#pragma once

#include <guiddef.h>
#include <string>
#include <vector>
#include <Unknwn.h>

#include "../CaptureManagerBroker/EncodingSettings.h"
#include "../Common/ComPtrCustom.h"


struct IMFASFProfile;
struct IMFASFContentInfo;
struct IMFTopologyNode;
struct IMFActivate;
struct IMFMediaType;
struct IPropertyStore;


namespace CaptureManager
{
	namespace Sinks
	{
		namespace ASF
		{

			class ASFSinkManager
			{
			public:
								
				static HRESULT createASFOutputNodes(
					std::wstring& aRefFileName,
					std::vector<IUnknown*> aCompressedMediaTypes,
					std::vector<CComPtrCustom<IUnknown>>& aRefTopologyASFOutputNodes);

				static HRESULT createASFOutputNodes(
					IUnknown* aPtrByteStreamActivate,
					std::vector<IUnknown*> aCompressedMediaTypes,
					std::vector<CComPtrCustom<IUnknown>>& aRefTopologyASFOutputNodes);


			private:
				ASFSinkManager() = delete;
				~ASFSinkManager() = delete;
				ASFSinkManager(const ASFSinkManager&) = delete;
				ASFSinkManager& operator=(const ASFSinkManager&) = delete;

				static HRESULT setEncodingProperties(
					REFGUID aRefGUIDMediaType,
					IPropertyStore* aPtrPropertyStore,
					EncodingSettings aEncodingSettings);

				static HRESULT addASFStreamConfig(
					IMFASFProfile* aPtrASFProfile,
					IMFMediaType* aPtrMediaType,
					WORD aStreamNumber);

				static HRESULT createOutputNode(
					IMFActivate* aPtrActivate,
					DWORD aStreamNumber,
					IMFTopologyNode** aPtrPtrTopologyNode);
			};
		}
	}
}