#pragma once
#include <Unknwnbase.h>

//struct IMFPresentationDescriptor;
//struct IMFAsyncResult;

namespace CaptureManager
{
	namespace Sources
	{

		enum SourceOperationType : int
		{
			None = -1,
			SourceOperationOpen = 0,
			SourceOperationStart = SourceOperationOpen + 1,
			SourceOperationPause = SourceOperationStart + 1,
			SourceOperationStop = SourceOperationPause + 1,
			SourceOperationStreamNeedData = SourceOperationStop + 1,
			SourceOperationEndOfStream = SourceOperationStreamNeedData + 1
		};
		
		MIDL_INTERFACE("4A4D8347-FDB3-4A8E-A45F-4AEC9D2F68CF")
		ISourceOperation : public IUnknown
		{
		public:
			virtual HRESULT getObject(
				IUnknown** aPtrPtrObject) = 0;
									
			virtual HRESULT getSourceOperationType(
				SourceOperationType* aPtrSourceOperationType) = 0;
			
			virtual HRESULT getStreamIdentifier(
				DWORD* aPtrStreamIdentifier) = 0;
		};
	}
}