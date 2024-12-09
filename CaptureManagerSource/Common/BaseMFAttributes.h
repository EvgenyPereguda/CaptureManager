#pragma once

#include "BaseUnknown.h"
#include "ComPtrCustom.h"
#include "MFHeaders.h"


namespace CaptureManager
{

	template <typename... Interfaces>
	struct BaseMFAttributes:
		public BaseUnknown<Interfaces...>
	{

	public:

		BaseMFAttributes(IMFAttributes* aPtrIMFAttributes)
		{
			mAttributes = aPtrIMFAttributes;
		}

		
		// IMFAttributes methods

		STDMETHODIMP GetItem(__RPC__in REFGUID guidKey, __RPC__inout_opt PROPVARIANT* pValue)
		{
			return mAttributes->GetItem(guidKey, pValue);
		}

		STDMETHODIMP GetItemType(__RPC__in REFGUID guidKey, __RPC__out MF_ATTRIBUTE_TYPE* pType)
		{
			return mAttributes->GetItemType(guidKey, pType);
		}

		STDMETHODIMP CompareItem(__RPC__in REFGUID guidKey, __RPC__in REFPROPVARIANT Value, __RPC__out BOOL* pbResult)
		{
			return mAttributes->CompareItem(guidKey, Value, pbResult);
		}

		STDMETHODIMP Compare(__RPC__in_opt IMFAttributes* pTheirs, MF_ATTRIBUTES_MATCH_TYPE MatchType, __RPC__out BOOL* pbResult)
		{
			return mAttributes->Compare(pTheirs, MatchType, pbResult);
		}

		STDMETHODIMP GetUINT32(__RPC__in REFGUID guidKey, __RPC__out UINT32* punValue)
		{
			return mAttributes->GetUINT32(guidKey, punValue);
		}

		STDMETHODIMP GetUINT64(__RPC__in REFGUID guidKey, __RPC__out UINT64* punValue)
		{
			return mAttributes->GetUINT64(guidKey, punValue);
		}

		STDMETHODIMP GetDouble(__RPC__in REFGUID guidKey, __RPC__out double* pfValue)
		{
			return mAttributes->GetDouble(guidKey, pfValue);
		}

		STDMETHODIMP GetGUID(__RPC__in REFGUID guidKey, __RPC__out GUID* pguidValue)
		{
			return mAttributes->GetGUID(guidKey, pguidValue);
		}

		STDMETHODIMP GetStringLength(__RPC__in REFGUID guidKey, __RPC__out UINT32* pcchLength)
		{
			return mAttributes->GetStringLength(guidKey, pcchLength);
		}

		STDMETHODIMP GetString(__RPC__in REFGUID guidKey, __RPC__out_ecount_full(cchBufSize) LPWSTR pwszValue, UINT32 cchBufSize, __RPC__inout_opt UINT32* pcchLength)
		{
			return mAttributes->GetString(guidKey, pwszValue, cchBufSize, pcchLength);
		}

		STDMETHODIMP GetAllocatedString(__RPC__in REFGUID guidKey, __RPC__deref_out_ecount_full_opt((*pcchLength + 1)) LPWSTR* ppwszValue, __RPC__out UINT32* pcchLength)
		{
			return mAttributes->GetAllocatedString(guidKey, ppwszValue, pcchLength);
		}

		STDMETHODIMP GetBlobSize(__RPC__in REFGUID guidKey, __RPC__out UINT32* pcbBlobSize)
		{
			return mAttributes->GetBlobSize(guidKey, pcbBlobSize);
		}

		STDMETHODIMP GetBlob(__RPC__in REFGUID guidKey, __RPC__out_ecount_full(cbBufSize) UINT8* pBuf, UINT32 cbBufSize, __RPC__inout_opt UINT32* pcbBlobSize)
		{
			return mAttributes->GetBlob(guidKey, pBuf, cbBufSize, pcbBlobSize);
		}

		STDMETHODIMP GetAllocatedBlob(__RPC__in REFGUID guidKey, __RPC__deref_out_ecount_full_opt(*pcbSize) UINT8** ppBuf, __RPC__out UINT32* pcbSize)
		{
			return mAttributes->GetAllocatedBlob(guidKey, ppBuf, pcbSize);
		}

		STDMETHODIMP GetUnknown(__RPC__in REFGUID guidKey, __RPC__in REFIID riid, __RPC__deref_out_opt LPVOID* ppv)
		{
			return mAttributes->GetUnknown(guidKey, riid, ppv);
		}

		STDMETHODIMP SetItem(__RPC__in REFGUID guidKey, __RPC__in REFPROPVARIANT Value)
		{
			return mAttributes->SetItem(guidKey, Value);
		}

		STDMETHODIMP DeleteItem(__RPC__in REFGUID guidKey)
		{
			return mAttributes->DeleteItem(guidKey);
		}

		STDMETHODIMP DeleteAllItems(void)
		{
			return mAttributes->DeleteAllItems();
		}

		STDMETHODIMP SetUINT32(__RPC__in REFGUID guidKey, UINT32 unValue)
		{
			return mAttributes->SetUINT32(guidKey, unValue);
		}

		STDMETHODIMP SetUINT64(__RPC__in REFGUID guidKey, UINT64 unValue)
		{
			return mAttributes->SetUINT64(guidKey, unValue);
		}

		STDMETHODIMP SetDouble(__RPC__in REFGUID guidKey, double fValue)
		{
			return mAttributes->SetDouble(guidKey, fValue);
		}

		STDMETHODIMP SetGUID(__RPC__in REFGUID guidKey, __RPC__in REFGUID guidValue)
		{
			return mAttributes->SetGUID(guidKey, guidValue);
		}

		STDMETHODIMP SetString(__RPC__in REFGUID guidKey, __RPC__in_string LPCWSTR wszValue)
		{
			return mAttributes->SetString(guidKey, wszValue);
		}

		STDMETHODIMP SetBlob(__RPC__in REFGUID guidKey, __RPC__in_ecount_full(cbBufSize) const UINT8* pBuf, UINT32 cbBufSize)
		{
			return mAttributes->SetBlob(guidKey, pBuf, cbBufSize);
		}

		STDMETHODIMP SetUnknown(__RPC__in REFGUID guidKey, __RPC__in_opt IUnknown* pUnknown)
		{
			return mAttributes->SetUnknown(guidKey, pUnknown);
		}

		STDMETHODIMP LockStore(void)
		{
			return mAttributes->LockStore();
		}

		STDMETHODIMP UnlockStore(void)
		{
			return mAttributes->UnlockStore();
		}

		STDMETHODIMP GetCount(__RPC__out UINT32* pcItems)
		{
			return mAttributes->GetCount(pcItems);
		}

		STDMETHODIMP GetItemByIndex(UINT32 unIndex, __RPC__out GUID* pguidKey, __RPC__inout_opt PROPVARIANT* pValue)
		{
			return mAttributes->GetItemByIndex(unIndex, pguidKey, pValue);
		}

		STDMETHODIMP CopyAllItems(__RPC__in_opt IMFAttributes* pDest)
		{
			return mAttributes->CopyAllItems(pDest);
		}


						
	protected:

		virtual ~BaseMFAttributes(){}

		virtual bool findInterface(
			REFIID aRefIID,
			void** aPtrPtrVoidObject)
		{
			if (aRefIID == __uuidof(IMFAttributes))
			{
				return castInterfaces(
					aRefIID,
					aPtrPtrVoidObject,
					static_cast<IMFAttributes*>(this));
			}
			else
			{
				return BaseUnknown::findInterface(
					aRefIID,
					aPtrPtrVoidObject);
			}
		}
		
	private:

		CComPtrCustom<IMFAttributes> mAttributes;
	};
}