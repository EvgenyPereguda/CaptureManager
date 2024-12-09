#include "pch.h"
#include "FileSinkFactory.h"
#include "MediaType.h"
#include "TopologyNode.h"


namespace CaptureManagerToWinRTProxy
{
	FileSinkFactory::FileSinkFactory(IntPtr aIntPtr)
	{
		do
		{
			auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)aIntPtr);

			if (lPtrUnk == nullptr)
				break;

			lPtrUnk->QueryInterface(IID_PPV_ARGS(&mIFileSinkFactory));

		} while (false);
	}
	
	void FileSinkFactory::createOutputNodes(
		IVector<IMediaType^>^ aArrayPtrCompressedMediaTypes,
		String^ aPtrFileName,
		IVector<ITopologyNode^>^ aPtrArrayPtrTopologyOutputNodes)
	{
		
		do
		{
			if (!mIFileSinkFactory)
				break;

			if (aPtrFileName == nullptr)
				break;

			if (aArrayPtrCompressedMediaTypes == nullptr)
				break;

			if (aPtrArrayPtrTopologyOutputNodes == nullptr)
				break;

			std::unique_ptr<OLECHAR[]> lStrFileName(new (std::nothrow)OLECHAR[aPtrFileName->Length() + 1]);
			
			memcpy(lStrFileName.get(),
				aPtrFileName->Data(),
				(aPtrFileName->Length() + 1)*sizeof(OLECHAR));

			lStrFileName.get()[aPtrFileName->Length()] = '\0';
			
			BSTR lFileName = lStrFileName.get();


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
				auto lCompressedMediaTypes = aArrayPtrCompressedMediaTypes->GetAt(lIndex);

				if (lCompressedMediaTypes == nullptr)
					break;

				MediaType^ lMediaType = (MediaType^)lCompressedMediaTypes;

				if (lMediaType == nullptr)
					break;

				auto lPtrUnk = reinterpret_cast<IUnknown*>((void*)lMediaType->getMediaType());

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

			auto lhresult = mIFileSinkFactory->createOutputNodes(
				lCompressedMediaTypesSAFEARRAY,
				lFileName,
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
					TopologyNode^ lTopologyNode = ref new TopologyNode(IntPtr(lVar.punkVal));
					
					aPtrArrayPtrTopologyOutputNodes->Append(lTopologyNode);
				}

				VariantClear(&lVar);
			}
			
			SafeArrayDestroy(lOutputNodesSAFEARRAY.parray);

		} while (false);

	}
}