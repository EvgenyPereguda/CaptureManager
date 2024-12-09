#pragma once

#include "CaptureManagerTypeInfo.h"
#include "BaseDispatch.h"
#include "IContainer.h"
#include "../CaptureManagerBroker/SinkCommon.h"


namespace CaptureManager
{
	namespace COMServer
	{
		class SampleGrabberCallSinkFactory :
			public BaseDispatch<ISampleGrabberCallSinkFactory, IContainer>
		{
		public:
			SampleGrabberCallSinkFactory();

			static GUIDToNamePair getGUIDToNamePair();

			// ISampleGrabberCallSinkFactory interface

			STDMETHOD(createOutputNode)(
				REFGUID aRefMajorType,
				REFGUID aRefSubType,
				DWORD aSampleByteSize,
				REFIID aREFIID,
				IUnknown **aPtrPtrISampleGrabberCall);

			// IDispatch interface stub

			STDMETHOD(GetIDsOfNames)(
				__RPC__in REFIID riid,
				/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
				/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
				LCID lcid,
				/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);

			virtual HRESULT invokeMethod(
				/* [annotation][in] */
				_In_  DISPID dispIdMember,
				/* [annotation][out][in] */
				_In_  DISPPARAMS *pDispParams,
				/* [annotation][out] */
				_Out_opt_  VARIANT *pVarResult);

			// IContainer interface

			STDMETHOD(setContainerFormat)(
				REFGUID aRefContainerTypeGUID);

			GUID mReadMode;

		private:
			
			static void getReadModes(std::vector<GUIDToNamePair>& aRefReadModes);
		};
	}
}