#pragma once

#include <map>
#include <Unknwnbase.h>

#include "../PugiXML/pugixml.hpp"


namespace CaptureManager
{
	class DataParser
	{
	public:

		static LPCWSTR getHRESULTDescription(
			const HRESULT& aValue);

		static HRESULT GetGUIDName(
			REFGUID aGUID,
			std::wstring& aNameGUID);

		static HRESULT readMediaType(
			IUnknown* aPtrMediaType,
			pugi::xml_node& aRefRootAttributeNode);

		static HRESULT readSourceActivate(
			IUnknown* aPtrSourceActivate,
			pugi::xml_node& aRefRootAttributeNode);

		static HRESULT readPresentationDescriptor(
			IUnknown* aPtrPresentationDescriptor,
			pugi::xml_node& aRefRootAttributeNode);

		static HRESULT readStreamDescriptor(
			IUnknown* aPtrStreamDescriptor,
			pugi::xml_node& aRefRootAttributeNode);

		static void unpack2UINT32AsUINT64(
			const PROPVARIANT& aVar,
			UINT32& aHigh,
			UINT32& aLow);
		

	private:

		std::map<HRESULT, std::wstring> mHRESULTDescriptionDictionary;

		DataParser() = delete;
		~DataParser() = delete;
		DataParser(
			const DataParser&) = delete;
		DataParser& operator=(
			const DataParser&) = delete;
		
		static HRESULT parsVideoMediaFormatAttributesValueByIndex(
			IUnknown* aPtrAttributes,
			DWORD aIndex,
			pugi::xml_node& aRefRootAttributeNode);

		static HRESULT parsAudioMediaFormatAttributesValueByIndex(
			IUnknown* aPtrAttributes,
			DWORD aIndex,
			pugi::xml_node& aRefRootAttributeNode);

		static HRESULT parsVideoMediaFormatFormatAttributesValue(
			REFGUID aGUID,
			const PROPVARIANT& aVar,
			pugi::xml_node& aRefAttributeNode);

		static HRESULT parsAudioMediaFormatFormatAttributesValue(
			REFGUID aGUID,
			const PROPVARIANT& aVar,
			pugi::xml_node& aRefAttributeNode);

		static HRESULT parsGeneralMediaTypeFormatAttributesValueByIndex(
			REFGUID aGUID,
			const PROPVARIANT& aVar,
			pugi::xml_node& aRefAttributeNode);

		static HRESULT parsSourceActivateAttributeValueByIndex(
			IUnknown* aPtrAttributes,
			DWORD aIndex,
			pugi::xml_node& aRefAttributeNode);

		static HRESULT parsPresentationDescriptorAttributeValueByIndex(
			IUnknown* aPtrAttributes,
			DWORD aIndex,
			pugi::xml_node& aRefAttributeNode);
		
		static LPCWSTR GetGUIDNameConst(
			const GUID& aGUID);

		static HRESULT parsCaptureDeviceAttributeValue(
			REFGUID aGUID,
			const PROPVARIANT& aVar,
			pugi::xml_node& aRefValueNode);

		static HRESULT specialVideoAttributeValue(
			REFGUID aGUID,
			const PROPVARIANT& aVar,
			pugi::xml_node& aRefValueNode);

		static HRESULT parsStreamDescriptorAttributeValueByIndex(
			IUnknown* aPtrAttributes,
			DWORD aIndex,
			pugi::xml_node& aRefAttributeNode);
		
		static HRESULT readAttribute(
			const PROPVARIANT& aVar,
			pugi::xml_node& aRefValueNode,
			bool aIsBoolable = false,
			bool aIsSigned = false);

	};
}