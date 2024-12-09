#pragma once

#include <ocidl.h>
#include <map>
#include "BaseUnknown.h"
#include "ComPtrCustom.h"

namespace CaptureManager
{

	template <typename CallbackInner>
	struct EnumConnections :
		BaseUnknown<
		IEnumConnections>
	{
		EnumConnections(
			CallbackInner* aCallbackInner,
			ULONG aCurrentPosition = 0) :
			mCurrentPosition(aCurrentPosition)
		{
			mCallbackInner = aCallbackInner;
		}

		STDMETHOD(Next)(
			ULONG cConnections,
			/* [length_is][size_is][out] */ LPCONNECTDATA rgcd,
			ULONG* pcFetched)
		{

			HRESULT lresult;

			do
			{
				LOG_CHECK_PTR_MEMORY(pcFetched);

				ULONG lFetched = 0;

				auto lter = this->mCallbackInner->mCallbackMassive.begin();

				while (lFetched < cConnections && mCurrentPosition < this->mCallbackInner->mCallbackMassive.size())
				{
					(*lter).second->QueryInterface(IID_PPV_ARGS(&rgcd[lFetched].pUnk));

					rgcd[lFetched].dwCookie = (*lter).first;

					lFetched++;

					mCurrentPosition++;

					lter++;
				}

				*pcFetched = lFetched;

				lresult = lFetched == cConnections ? S_OK : S_FALSE;

			} while (false);

			return lresult;
		}


		STDMETHOD(Skip)(
			ULONG cConnections)
		{

			HRESULT lresult;

			do
			{
				ULONG lSkipped = 0;

				while (lSkipped < cConnections &&
					mCurrentPosition < this->mCallbackInner->mCallbackMassive.size())
				{
					lSkipped++;

					mCurrentPosition++;
				}

				if (lSkipped == cConnections)
					lresult = S_OK;
				else
					lresult = S_FALSE;

			} while (false);

			return lresult;
		}

		STDMETHOD(Reset)(void)
		{

			HRESULT lresult;

			do
			{
				mCurrentPosition = 0;

				lresult = S_OK;

			} while (false);

			return lresult;
		}

		STDMETHOD(Clone)(IEnumConnections** ppEnum)
		{

			HRESULT lresult(E_FAIL);

			do
			{
				LOG_CHECK_PTR_MEMORY(ppEnum);

				*ppEnum = new (std::nothrow) EnumConnections(this->mCallbackInner, mCurrentPosition);

				if (*ppEnum != nullptr)
					lresult = S_OK;

			} while (false);

			return lresult;
		}

	private:

		ULONG mCurrentPosition;

		CComPtrCustom<CallbackInner> mCallbackInner;

	};
}