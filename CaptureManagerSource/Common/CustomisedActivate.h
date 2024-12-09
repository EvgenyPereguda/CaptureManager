#pragma once

#include <atomic>
#include <mfobjects.h>
#include <mfidl.h>

#include "../Common/MFHeaders.h"
#include "../Common/BaseUnknown.h"
#include "../Common/ComPtrCustom.h"

namespace CaptureManager
{
	namespace Activates
	{
		namespace CustomisedActivate
		{
			class CustomisedActivate :
				public BaseUnknown<IMFActivate>
			{
			private:

				CComPtrCustom<IUnknown> mIUnknown;

				CComPtrCustom<IMFAttributes> mAttributes;

			public:

				CustomisedActivate(IMFMediaSink* aPtrMediaSink)
				{
					mIUnknown = aPtrMediaSink;
				}


				virtual ~CustomisedActivate()
				{
					mIUnknown.Release();
				}

				CustomisedActivate(
					IMFMediaSink* aPtrMediaSink, 
					IMFAttributes* aPtrAttributes)
				{
					mIUnknown = aPtrMediaSink;

					mAttributes = aPtrAttributes;
				}

				CustomisedActivate(IMFByteStream* aPtByteStream)
				{
					mIUnknown = aPtByteStream;
				}


				

				// IMFActivate interface:

				virtual HRESULT STDMETHODCALLTYPE ActivateObject(
					__RPC__in REFIID riid,
					/* [retval][iid_is][out] */ __RPC__deref_out_opt void **ppv)
				{
					HRESULT lresult;

					do
					{
						if (!mIUnknown)
						{
							lresult = E_POINTER;

							break;
						}
						
						lresult = mIUnknown->QueryInterface(riid, ppv);

						if (FAILED(lresult))
						{
							break;
						}

						lresult = S_OK;

					} while (false);

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE ShutdownObject(void)
				{
					HRESULT lresult;

					do
					{
						if (!mIUnknown)
						{
							lresult = E_POINTER;

							break;
						}


						CComPtrCustom<IMFMediaSink> lMediaSink;

						lresult = mIUnknown->QueryInterface(IID_PPV_ARGS(&lMediaSink));

						if (SUCCEEDED(lresult))
						{
							lMediaSink->Shutdown();
						}

						lresult = S_OK;

					} while (false);

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE DetachObject(void)
				{
					HRESULT lresult(E_FAIL);

					do
					{
						if (!mIUnknown)
						{
							lresult = E_POINTER;

							break;
						}

						//mIUnknown.detach();

					} while (false);

					return lresult;
				}


				// IMFAttributes inteface

			public:
				virtual HRESULT STDMETHODCALLTYPE GetItem(
					__RPC__in REFGUID guidKey,
					/* [full][out][in] */ __RPC__inout_opt PROPVARIANT *pValue)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE GetItemType(
					__RPC__in REFGUID guidKey,
					__RPC__out MF_ATTRIBUTE_TYPE *pType)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE CompareItem(
					__RPC__in REFGUID guidKey,
					__RPC__in REFPROPVARIANT Value,
					__RPC__out BOOL *pbResult)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE Compare(
					IMFAttributes *pTheirs,
					MF_ATTRIBUTES_MATCH_TYPE MatchType,
					__RPC__out BOOL *pbResult)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE GetUINT32(
					__RPC__in REFGUID guidKey,
					__RPC__out UINT32 *punValue)
				{
					HRESULT lresult = E_NOTIMPL;

					do
					{
						if (mAttributes == nullptr)
							break;

						lresult = mAttributes->GetUINT32(
							guidKey,
							punValue);

					} while (false);

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE GetUINT64(
					__RPC__in REFGUID guidKey,
					__RPC__out UINT64 *punValue)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE GetDouble(
					__RPC__in REFGUID guidKey,
					__RPC__out double *pfValue)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE GetGUID(
					__RPC__in REFGUID guidKey,
					__RPC__out GUID *pguidValue)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE GetStringLength(
					__RPC__in REFGUID guidKey,
					__RPC__out UINT32 *pcchLength)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE GetString(
					__RPC__in REFGUID guidKey,
					/* [size_is][out] */ __RPC__out_ecount_full(cchBufSize) LPWSTR pwszValue,
					UINT32 cchBufSize,
					/* [full][out][in] */ __RPC__inout_opt UINT32 *pcchLength)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE GetAllocatedString(
					__RPC__in REFGUID guidKey,
					/* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt((*pcchLength + 1)) LPWSTR *ppwszValue,
					__RPC__out UINT32 *pcchLength)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE GetBlobSize(
					__RPC__in REFGUID guidKey,
					__RPC__out UINT32 *pcbBlobSize)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE GetBlob(
					__RPC__in REFGUID guidKey,
					/* [size_is][out] */ __RPC__out_ecount_full(cbBufSize) UINT8 *pBuf,
					UINT32 cbBufSize,
					/* [full][out][in] */ __RPC__inout_opt UINT32 *pcbBlobSize)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE GetAllocatedBlob(
					__RPC__in REFGUID guidKey,
					/* [size_is][size_is][out] */ __RPC__deref_out_ecount_full_opt(*pcbSize) UINT8 **ppBuf,
					__RPC__out UINT32 *pcbSize)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE GetUnknown(
					__RPC__in REFGUID guidKey,
					__RPC__in REFIID riid,
					/* [iid_is][out] */ __RPC__deref_out_opt LPVOID *ppv)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE SetItem(
					__RPC__in REFGUID guidKey,
					__RPC__in REFPROPVARIANT Value)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE DeleteItem(
					__RPC__in REFGUID guidKey)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE DeleteAllItems(void)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE SetUINT32(
					__RPC__in REFGUID guidKey,
					UINT32 unValue)
				{
					HRESULT lresult = E_NOTIMPL;

					do
					{
						if (mAttributes == nullptr)
							break;

						lresult = mAttributes->SetUINT32(
							guidKey,
							unValue);

					} while (false);

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE SetUINT64(
					__RPC__in REFGUID guidKey,
					UINT64 unValue)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE SetDouble(
					__RPC__in REFGUID guidKey,
					double fValue)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE SetGUID(
					__RPC__in REFGUID guidKey,
					__RPC__in REFGUID guidValue)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE SetString(
					__RPC__in REFGUID guidKey,
					/* [string][in] */ __RPC__in_string LPCWSTR wszValue)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE SetBlob(
					__RPC__in REFGUID guidKey,
					/* [size_is][in] */ __RPC__in_ecount_full(cbBufSize) const UINT8 *pBuf,
					UINT32 cbBufSize)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE SetUnknown(
					__RPC__in REFGUID guidKey,
					IUnknown *pUnknown)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE LockStore(void)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE UnlockStore(void)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE GetCount(
					__RPC__out UINT32 *pcItems)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE GetItemByIndex(
					UINT32 unIndex,
					__RPC__out GUID *pguidKey,
					/* [full][out][in] */ __RPC__inout_opt PROPVARIANT *pValue)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}

				virtual HRESULT STDMETHODCALLTYPE CopyAllItems(
					IMFAttributes *pDest)
				{
					HRESULT lresult = E_NOTIMPL;

					return lresult;
				}
			};
		}
	}
}