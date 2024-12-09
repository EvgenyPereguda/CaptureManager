#pragma once

#include <Unknwn.h>

MIDL_INTERFACE("1AC5C4DE-7183-45F8-9D0D-2A7E1C218303")
IInnerCaptureProcessor : public IUnknown
{
public:
	// get friendly name of Processor
	virtual HRESULT getFrendlyName(
		BSTR* aPtrString) = 0;

	// get synbolicLink of Processor
	virtual HRESULT getSymbolicLink(
		BSTR* aPtrString) = 0;

	// get supported amount of streams
	virtual HRESULT getStreamAmount(
		UINT32* aPtrStreamAmount) = 0;

	// get supported media types of stream
	virtual HRESULT getMediaTypes(
		UINT32 aStreamIndex,
		BSTR* aPtrStreamNameString,
		IUnknown*** aPtrPtrPtrMediaType,
		UINT32* aPtrMediaTypeCount) = 0;

	// set supported media type to stream
	virtual HRESULT setCurrentMediaType(
		UINT32 aStreamIndex,
		IUnknown* aPtrMediaType) = 0;

	// get new sample of stream
	virtual HRESULT getNewSample(
		DWORD aStreamIdentifier,
		IUnknown** ppSample) = 0;

	// start processor
	virtual HRESULT start() = 0;

	// stop processor
	virtual HRESULT stop() = 0;

	// pause processor
	virtual HRESULT pause() = 0;

	// restart processor
	virtual HRESULT restart() = 0;

	// shutdown processor
	virtual HRESULT shutdown() = 0;

	// register Media Event interface
	virtual HRESULT registerMediaEvent(IUnknown* pUnkIMediaEvent) = 0;

};

