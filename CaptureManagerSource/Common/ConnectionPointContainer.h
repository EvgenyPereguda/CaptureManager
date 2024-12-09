#pragma once

#include <ocidl.h>
#include <map>
#include "BaseUnknown.h"
#include "ComPtrCustom.h"

namespace CaptureManager
{

	struct ConnectionPointContainer :
		public BaseUnknown<IConnectionPointContainer>
	{
	private:

		struct EnumSessionConnectionPoints :
			public BaseUnknown<IEnumConnectionPoints>
		{
		private:

			struct GUIDComparer
			{
				bool operator()(const GUIDToNamePair& Left, const GUIDToNamePair& Right) const
				{
					return memcmp(&(Left.mGUID), &(Right.mGUID), sizeof(Right.mGUID)) < 0;
				}

				bool operator()(const GUID& Left, const GUID& Right) const
				{
					return memcmp(&Left, &Right, sizeof(Right)) < 0;
				}
			};


			std::map<IID, CComPtrCustom<IUnknown>, GUIDComparer> mIConnectionPointCollection;

			ULONG mCurrentPosition;

		public:

			EnumSessionConnectionPoints() :
				mCurrentPosition(0)
			{
			}

			EnumSessionConnectionPoints(
				std::map<IID, CComPtrCustom<IUnknown>, GUIDComparer>& aRefIConnectionPointCollection,
				ULONG aCurrentPosition = 0) :
				mCurrentPosition(aCurrentPosition)
			{
				mIConnectionPointCollection = aRefIConnectionPointCollection;
			}

			HRESULT addConnectionPoint(CComPtrCustom<IConnectionPoint>& aRefConnectionPoint)
			{
				HRESULT lresult;

				do
				{
					if (!aRefConnectionPoint)
					{
						lresult = E_OUTOFMEMORY;

						break;
					}

					IID lIID;

					lresult = aRefConnectionPoint->GetConnectionInterface(&lIID);

					if (FAILED(lresult))
					{
						break;
					}

					mIConnectionPointCollection[lIID] = aRefConnectionPoint;

				} while (false);

				return lresult;
			}

			STDMETHOD(Next)(
				ULONG cConnections,
				/* [length_is][size_is][out] */ LPCONNECTIONPOINT* ppCP,
				ULONG* pcFetched)
			{

				HRESULT lresult;

				do
				{
					ULONG lFetched = 0;

					auto lter = mIConnectionPointCollection.begin();

					while (lFetched < cConnections && mCurrentPosition < mIConnectionPointCollection.size())
					{
						LPCONNECTIONPOINT* lPtrPtrCP = nullptr;

						LOG_INVOKE_QUERY_INTERFACE_METHOD((*lter).second, lPtrPtrCP);

						LOG_CHECK_PTR_MEMORY(lPtrPtrCP);

						ppCP[lFetched] = *lPtrPtrCP;

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
						mCurrentPosition < mIConnectionPointCollection.size())
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

			STDMETHOD(Clone)(IEnumConnectionPoints** ppEnum)
			{

				HRESULT lresult(E_FAIL);

				do
				{
					*ppEnum = new (std::nothrow) EnumSessionConnectionPoints(this->mIConnectionPointCollection, mCurrentPosition);

					if (*ppEnum != nullptr)
						lresult = S_OK;

				} while (false);

				return lresult;
			}


			STDMETHOD(FindConnectionPoint)(
				__RPC__in REFIID riid,
				__RPC__deref_out_opt IConnectionPoint** ppCP)
			{

				HRESULT lresult;

				do
				{
					if (ppCP == nullptr)
					{
						lresult = E_POINTER;

						break;
					}

					auto lfind = mIConnectionPointCollection.find(riid);

					if (lfind == mIConnectionPointCollection.end())
					{
						lresult = E_BOUNDS;

						break;
					}

					lresult = (*lfind).second->QueryInterface(IID_PPV_ARGS(ppCP));

				} while (false);

				return lresult;
			}

		};


		CComPtrCustom<EnumSessionConnectionPoints> mEnumConnectionPoints;

	public:

		ConnectionPointContainer() :
			mEnumConnectionPoints(new (std::nothrow) EnumSessionConnectionPoints())
		{
		}

		HRESULT addConnectionPoint(IConnectionPoint* aPtrConnectionPoint)
		{
			HRESULT lresult;

			do
			{
				if (aPtrConnectionPoint == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (!mEnumConnectionPoints)
				{
					lresult = E_OUTOFMEMORY;

					break;
				}

				CComPtrCustom<IConnectionPoint> lConnectionPoint = aPtrConnectionPoint;

				lresult = mEnumConnectionPoints->addConnectionPoint(lConnectionPoint);

			} while (false);

			return lresult;
		}

		STDMETHOD(EnumConnectionPoints)(
			__RPC__deref_out_opt IEnumConnectionPoints** ppEnum)
		{

			HRESULT lresult;

			do
			{
				if (ppEnum == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (!mEnumConnectionPoints)
				{
					lresult = E_OUTOFMEMORY;

					break;
				}

				mEnumConnectionPoints->QueryInterface(IID_PPV_ARGS(ppEnum));

			} while (false);

			return lresult;
		}

		STDMETHOD(FindConnectionPoint)(
			__RPC__in REFIID riid,
			__RPC__deref_out_opt IConnectionPoint** ppCP)
		{

			HRESULT lresult;

			do
			{
				if (ppCP == nullptr)
				{
					lresult = E_POINTER;

					break;
				}

				if (!mEnumConnectionPoints)
				{
					lresult = E_OUTOFMEMORY;

					break;
				}

				lresult = mEnumConnectionPoints->FindConnectionPoint(
					riid,
					ppCP);

			} while (false);

			return lresult;
		}
	};
}