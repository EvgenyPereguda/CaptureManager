/*
MIT License

Copyright(c) 2020 Evgeny Pereguda

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "SourceOperation.h"
#include "../Common/Common.h"
#include "../LogPrintOut/LogPrintOut.h"


namespace CaptureManager
{
	namespace Sources
	{
		SourceOperation::SourceOperation(
			SourceOperationType aSourceOperationType,
			IUnknown* aPtrObject):
			mSourceOperationType(aSourceOperationType)
		{
			mObject = aPtrObject;
		}

		SourceOperation::SourceOperation(
			SourceOperationType aSourceOperationType,
			DWORD aStreamIdentifier,
			IUnknown* aPtrObject):
			mSourceOperationType(aSourceOperationType),
			mStreamIdentifier(aStreamIdentifier)
		{
			mObject = aPtrObject;
		}

		SourceOperation::~SourceOperation()
		{
		}
		
		// ISourceOperation interface
		HRESULT SourceOperation::getObject(
			IUnknown** aPtrPtrObject)
		{
			HRESULT lresult(S_OK);

			do
			{
				if (mObject)
				{
					LOG_CHECK_PTR_MEMORY(aPtrPtrObject);

					LOG_CHECK_PTR_MEMORY(mObject);

					LOG_INVOKE_QUERY_INTERFACE_METHOD(mObject, aPtrPtrObject);
				}

			} while (false);

			return lresult;
		}		

		HRESULT SourceOperation::getSourceOperationType(
			SourceOperationType* aPtrSourceOperationType)
		{
			HRESULT lresult(S_OK);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrSourceOperationType);
				
				*aPtrSourceOperationType = mSourceOperationType;

			} while (false);

			return lresult;			
		}
		
		HRESULT SourceOperation::getStreamIdentifier(
			DWORD* aPtrStreamIdentifier)
		{
			HRESULT lresult(S_OK);

			do
			{
				LOG_CHECK_PTR_MEMORY(aPtrStreamIdentifier);

				*aPtrStreamIdentifier = mStreamIdentifier;

			} while (false);

			return lresult;
		}
	}
}