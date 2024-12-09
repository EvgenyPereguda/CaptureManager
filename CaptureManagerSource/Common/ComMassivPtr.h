#pragma once

namespace CaptureManager
{
	template <class T>
	class CComMassivPtr
	{
	public:

		CComMassivPtr(void)
			:size(0), elements(0)
		{
		}

		virtual ~CComMassivPtr(void)
		{
			release();
		}

		T** getMassivPtr()
		{
			return elements;
		}

		T*** getPtrMassivPtr()
		{
			return &elements;
		}

		UINT32 *getPtrSizeMassiv()
		{
			return &size;
		}

		UINT32 getSizeMassiv()
		{
			return size;
		}

		UINT32& getRefSizeMassiv()
		{
			return size;
		}

		void release()
		{
			ULONG e = -1;

			for (UINT32 index = 0; index < size; index++)
			{
				e = elements[index]->Release();
			}

			CoTaskMemFree(elements);
		}

		T *operator [] (
			UINT32 index) const
		{
			if (index >= size)
				throw std::exception("Error!! Index out of size!!");

			return elements[index];
		}

	private:

		UINT32 size;
		T** elements;

		CComMassivPtr(const CComMassivPtr&) = delete;

		CComMassivPtr& operator = (const CComMassivPtr&) = delete;

	};
	
}