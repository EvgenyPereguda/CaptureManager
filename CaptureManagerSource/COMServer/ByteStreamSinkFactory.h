#pragma once

#include "CaptureManagerTypeInfo.h"
#include "BaseDispatch.h"
#include "IContainer.h"
#include "../Common/ComPtrCustom.h"
#include "../CaptureManagerBroker/IOutputNodeFactory.h"
#include "../CaptureManagerBroker/SinkCommon.h"


namespace CaptureManager
{
	namespace COMServer
	{
		class ByteStreamSinkFactory :
			public BaseUnknown<IByteStreamSinkFactory, IContainer>
		{
		public:

			static GUIDToNamePair getGUIDToNamePair();

			// IFileSinkFactory interface

			STDMETHOD(createOutputNodes)(
				VARIANT aArrayPtrCompressedMediaTypes,
				IUnknown *aPtrByteStreamActivate,
				VARIANT *aPtrArrayPtrTopologyOutputNodes);

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

		private:

			CComPtrCustom<IOutputNodeFactory> mIOutputNodeFactory;

		};
	}
}