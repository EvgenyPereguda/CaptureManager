#pragma once

#include <Unknwnbase.h>
#include <strmif.h>

namespace CaptureManager
{
	namespace Sources
	{
		namespace DirectShow
		{
			class DirectShowCommon
			{
			public:
				static void _FreeMediaType(AM_MEDIA_TYPE& mt)
				{
					if (mt.cbFormat != 0)
					{
						CoTaskMemFree((PVOID)mt.pbFormat);
						mt.cbFormat = 0;
						mt.pbFormat = NULL;
					}
					if (mt.pUnk != NULL)
					{
						// pUnk should not be used.
						mt.pUnk->Release();
						mt.pUnk = NULL;
					}
				}


				// Delete a media type structure that was allocated on the heap.
				static void _DeleteMediaType(AM_MEDIA_TYPE *pmt)
				{
					if (pmt != NULL)
					{
						_FreeMediaType(*pmt);
						CoTaskMemFree(pmt);
					}
				}
			};
		}
	}
}