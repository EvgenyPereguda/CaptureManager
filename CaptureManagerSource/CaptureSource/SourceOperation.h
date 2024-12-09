#pragma once

#include "../Common/BaseUnknown.h"
#include "../Common/ComPtrCustom.h"
#include "ISourceOperation.h"

namespace CaptureManager
{
	namespace Sources
	{
		class SourceOperation :
			public BaseUnknown<ISourceOperation>
		{
		public:
			SourceOperation(
				SourceOperationType aSourceOperationType,
				IUnknown* aPtrObject = nullptr);

			SourceOperation(
				SourceOperationType aSourceOperationType,
				DWORD aStreamIdentifier,
				IUnknown* aPtrObject);

			virtual ~SourceOperation();


			// ISourceOperation interface
			virtual HRESULT getObject(
				IUnknown** aPtrPtrObject);

			virtual HRESULT getSourceOperationType(
				SourceOperationType* aPtrSourceOperationType);
			
			virtual HRESULT getStreamIdentifier(
				DWORD* aPtrStreamIdentifier);

		private:

			CComPtrCustom<IUnknown> mObject;

			SourceOperationType mSourceOperationType;

			DWORD mStreamIdentifier;
			
		};
	}
}