#pragma once

#include <string>
#include <mutex>

#include "CaptureManagerTypeInfo.h"
#include "BaseDispatch.h"
//#include "Common.h"
//#include "CommonFile.h"
//#include "ISessionCallbackInner.h"

namespace CaptureManager
{
	namespace COMServer
	{
		class MediaTypeParser :
			public BaseDispatch<IMediaTypeParser>
		{
		public:

			STDMETHOD(parse)(
				IUnknown *aPtrMediaType,
				BSTR *aPtrPtrXMLstring);

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

