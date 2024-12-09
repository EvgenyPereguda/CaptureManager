#pragma once

#include "CaptureManagerTypeInfo.h"
#include "BaseDispatch.h"
#include "SinkCollection.h"
#include "../CaptureManagerBroker/StreamControlCommon.h"


namespace CaptureManager
{
	namespace COMServer
	{
		class SpreaderNodeFactory :
			public BaseDispatch<ISpreaderNodeFactory>
		{
		public:

			static StreamControlInfo getStreamControlInfo();

			// ISpreaderNodeFactory interface

			STDMETHOD(createSpreaderNode)(
				VARIANT aArrayPtrDownStreamTopologyNodes,
				IUnknown **aPtrPtrTopologySpreaderNode);


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
		};
	}
}