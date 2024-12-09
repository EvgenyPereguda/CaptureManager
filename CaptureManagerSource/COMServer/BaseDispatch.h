#pragma once
#include "../Common/BaseUnknown.h"
#include "../Common/Singleton.h"
#include <comdef.h>
#include "ClassFactory.h"

namespace CaptureManager
{
	namespace COMServer
	{
		template <typename... Interfaces>
		struct BaseDispatch :
			public BaseUnknown <
			Interfaces... >
		{
			BaseDispatch()
			{
				Singleton<ClassFactory>::getInstance().lock();
			}


			// IDispatch interface stub

			STDMETHOD(GetTypeInfoCount)(
				UINT *pctinfo){
				do
				{
					if (pctinfo != nullptr)
						*pctinfo = 0;

				} while (false);

				return S_OK;
			}

			STDMETHOD(GetTypeInfo)(
				UINT iTInfo,
				LCID lcid,
				ITypeInfo **ppTInfo){
				do
				{
					if (ppTInfo != nullptr)
						*ppTInfo = nullptr;

				} while (false);

				return S_OK;
			}

			STDMETHOD(GetIDsOfNames)(
				__RPC__in REFIID riid,
				/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
				/* [range][in] */ __RPC__in_range(0, 16384) UINT cNames,
				LCID lcid,
				/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId){
				return E_NOTIMPL;
			}

			STDMETHOD(Invoke)(
				/* [annotation][in] */
				_In_  DISPID dispIdMember,
				/* [annotation][in] */
				_In_  REFIID riid,
				/* [annotation][in] */
				_In_  LCID lcid,
				/* [annotation][in] */
				_In_  WORD wFlags,
				/* [annotation][out][in] */
				_In_  DISPPARAMS *pDispParams,
				/* [annotation][out] */
				_Out_opt_  VARIANT *pVarResult,
				/* [annotation][out] */
				EXCEPINFO *pExcepInfo,
				/* [annotation][out] */
				UINT *puArgErr){

				HRESULT lresult = E_NOTIMPL;

				do
				{
					if (lcid != 0 && lcid != 2048)
					{
						lresult = DISP_E_UNKNOWNLCID;

						break;
					}
					
					lresult = invokeMethod(
						dispIdMember,
						pDispParams,
						pVarResult);

				} while (false);

				if (FAILED(lresult) && pExcepInfo != nullptr)
				{
					_com_error lerr(lresult);

					pExcepInfo->bstrDescription = SysAllocString(lerr.ErrorMessage());
				}

				return lresult;
			}

		protected:

			virtual ~BaseDispatch()
			{
				Singleton<ClassFactory>::getInstance().unlock();
			}

			virtual HRESULT invokeMethod(
				/* [annotation][in] */
				_In_  DISPID dispIdMember,
				/* [annotation][out][in] */
				_In_  DISPPARAMS *pDispParams,
				/* [annotation][out] */
				_Out_opt_  VARIANT *pVarResult)
			{
				return DISP_E_UNKNOWNNAME;
			}

			virtual bool findInterface(
				REFIID aRefIID,
				void** aPtrPtrVoidObject)
			{
				if (aRefIID == IID_IDispatch)
				{
					return castToIDispatch(
						aPtrPtrVoidObject,
						static_cast<Interfaces*>(this)...);
				}
				else
				{
					return BaseUnknown::findInterface(
						aRefIID,
						aPtrPtrVoidObject);
				}
			}

		private:

			template<typename Interface, typename... Args>
			bool castToIDispatch(
				void** aPtrPtrVoidObject,
				Interface* aThis,
				Args... aRest)
			{
				return castToIDispatch(aPtrPtrVoidObject, aThis);
			}

			template<typename Interface>
			bool castToIDispatch(
				void** aPtrPtrVoidObject,
				Interface* aThis)
			{
				bool lresult = false;

				*aPtrPtrVoidObject = static_cast<IDispatch*>(aThis);

				if (*aPtrPtrVoidObject != nullptr)
					lresult = true;

				return lresult;
			}

		};
	}
}