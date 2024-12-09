#include "pch.h"
#include "ByteStreamSinkFactory.h"


namespace CaptureManagerToWinRTProxy
{
	ByteStreamSinkFactory::ByteStreamSinkFactory(IntPtr aIntPtr)
	{
		do
		{			
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mIByteStreamSinkFactory));

		} while (false);
	}


	void ByteStreamSinkFactory::createOutputNodes(
		IVector<Object^>^ aArrayPtrCompressedMediaTypes,
		Object^ aPtrByteStream,
		IVector<Object^>^ aPtrArrayPtrTopologyOutputNodes)
	{

		do
		{
			if (!mIByteStreamSinkFactory)
				break;

			if (aPtrByteStream == nullptr)
				break;

			if (aArrayPtrCompressedMediaTypes == nullptr)
				break;

			if (aPtrArrayPtrTopologyOutputNodes == nullptr)
				break;

			CComPtrCustom<IUnknown> lByteStream;
			
			lByteStream = reinterpret_cast<IUnknown*>(aPtrByteStream);

			
			SAFEARRAY *lPtrSAFEARRAY = nullptr;

			SAFEARRAYBOUND lbound[1];

			lbound[0].lLbound = 0;

			lbound[0].cElements = aArrayPtrCompressedMediaTypes->Size;

			lPtrSAFEARRAY = SafeArrayCreate(
				VT_VARIANT,
				1,
				lbound);

			LONG lElementIndex = 0;

			for (decltype(aArrayPtrCompressedMediaTypes->Size) lIndex = 0;
				lIndex < aArrayPtrCompressedMediaTypes->Size;
				lIndex++)
			{
				auto lObjectSourceNode = aArrayPtrCompressedMediaTypes->GetAt(lIndex);

				if (lObjectSourceNode == nullptr)
					break;

				auto lPtrUnk = reinterpret_cast<IUnknown*>(lObjectSourceNode);

				if (lPtrUnk == nullptr)
					break;

				auto lISourceTopologyNode = lPtrUnk;

				VARIANT lVar;

				VariantInit(&lVar);

				lVar.vt = VT_UNKNOWN;

				lVar.punkVal = lISourceTopologyNode;

				SafeArrayPutElement(lPtrSAFEARRAY, &lElementIndex, &lVar);

				++lElementIndex;
			}


			VARIANT lCompressedMediaTypesSAFEARRAY;

			VariantInit(&lCompressedMediaTypesSAFEARRAY);

			lCompressedMediaTypesSAFEARRAY.vt = VT_UNKNOWN | VT_SAFEARRAY;

			lCompressedMediaTypesSAFEARRAY.parray = lPtrSAFEARRAY;


			VARIANT lOutputNodesSAFEARRAY;

			auto lhresult = mIByteStreamSinkFactory->createOutputNodes(
				lCompressedMediaTypesSAFEARRAY,
				lByteStream,
				&lOutputNodesSAFEARRAY);

			SafeArrayDestroy(lPtrSAFEARRAY);

			if (lhresult != 0)
				break;

			if (lOutputNodesSAFEARRAY.parray == nullptr)
				break;

			if (lOutputNodesSAFEARRAY.vt != (VT_UNKNOWN | VT_SAFEARRAY))
				break;

			LONG lBoundMediaTypes(0);

			LONG uBoundMediaTypes(0);

			SafeArrayGetUBound(lOutputNodesSAFEARRAY.parray, 1, &uBoundMediaTypes);

			SafeArrayGetUBound(lOutputNodesSAFEARRAY.parray, 1, &lBoundMediaTypes);


			for (LONG lIndex = lBoundMediaTypes; lIndex <= uBoundMediaTypes; lIndex++)
			{
				VARIANT lVar;

				auto lr = SafeArrayGetElement(lOutputNodesSAFEARRAY.parray, &lIndex, &lVar);

				if (SUCCEEDED(lr) && lVar.vt == VT_UNKNOWN && lVar.punkVal != nullptr)
				{
					auto lobject = reinterpret_cast<Object^>(lVar.punkVal);

					aPtrArrayPtrTopologyOutputNodes->Append(lobject);
				}

				VariantClear(&lVar);
			}

			SafeArrayDestroy(lOutputNodesSAFEARRAY.parray);

		} while (false);

	}
}