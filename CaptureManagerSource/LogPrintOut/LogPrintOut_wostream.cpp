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

#include "LogPrintOut_wostream.h"
#include "../Common/MFHeaders.h"
#include "../Common/ComPtrCustom.h"
#include "../MediaFoundationManager/MediaFoundationManager.h"
#include "LogPrintOutAsyncCallback.h"

namespace CaptureManager
{
    using namespace std;
        
	LogPrintOut_wostream::LogPrintOut_wostream()
		:std::wostream(_Uninitialized::_Noinit)
        , m_size(512)
	{
        mSyncWorkerQueue = MFASYNC_CALLBACK_QUEUE_LONG_FUNCTION;

        mLogPrintOutAsyncCallback =
            new (std::nothrow) Log::LogPrintOutAsyncCallback();

        current_line.reset(new wchar_t[m_size]);
        setg(0, 0, 0);
        setp(current_line.get(), current_line.get() + m_size);

        init(this);
	}

	LogPrintOut_wostream::~LogPrintOut_wostream()
	{
	}

    void LogPrintOut_wostream::addCallbackInner(IUnknown* aPtrUnkCallbackInner)
    {
        do
        {
            if (aPtrUnkCallbackInner == nullptr)
                break;

            if (mLogPrintOutAsyncCallback)
                mLogPrintOutAsyncCallback->AddCallbackInner(aPtrUnkCallbackInner);

        } while (false);
    }

    wstreambuf::int_type LogPrintOut_wostream::underflow()
    {
        return wstreambuf::traits_type::to_int_type(*gptr());
    }

    wstreambuf::int_type LogPrintOut_wostream::overflow(wstreambuf::int_type value)
    {
        streamsize write = pptr() - pbase();
        if (write)
        {
            // Write line to original buffer

            CComPtrCustom<
                Log::LogPrintOutAsyncCallback::ILogPrintOutAsyncCallbackRequest> lAsyncCallbackRequest =
                new (std::nothrow) Log::LogPrintOutAsyncCallback::LogPrintOutAsyncCallbackRequest(
                    current_line.get(),
                    write);

            if (lAsyncCallbackRequest) {

                CComQIPtrCustom<IMFAsyncCallback> lAsyncCallback(mLogPrintOutAsyncCallback);

                Core::MediaFoundation::MediaFoundationManager::MFPutWorkItem(
                    mSyncWorkerQueue,
                    lAsyncCallback,
                    lAsyncCallbackRequest);
            }
        }

        setp(current_line.get(), current_line.get() + m_size);
        if (!wstreambuf::traits_type::eq_int_type(value, wstreambuf::traits_type::eof())) sputc(value);
        return wstreambuf::traits_type::not_eof(value);
    };

    int LogPrintOut_wostream::sync() {
        wstreambuf::int_type result = this->overflow(wstreambuf::traits_type::eof());

        return wstreambuf::traits_type::eq_int_type(result, wstreambuf::traits_type::eof()) ? -1 : 0;
	}
}