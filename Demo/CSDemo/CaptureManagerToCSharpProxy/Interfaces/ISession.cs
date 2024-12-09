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

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CaptureManagerToCSharpProxy.Interfaces
{
    public enum SessionCallbackEventCode:int
	{
		Unknown = 0,
		Error = Unknown + 1,
		Status_Error = Error + 1,
		Execution_Error = Status_Error + 1,
		ItIsReadyToStart = Execution_Error + 1,
		ItIsStarted = ItIsReadyToStart + 1,
		ItIsPaused = ItIsStarted + 1,
		ItIsStopped = ItIsPaused + 1,
		ItIsEnded = ItIsStopped + 1,
		ItIsClosed = ItIsEnded + 1,
		VideoCaptureDeviceRemoved = ItIsClosed + 1
	};

    public delegate void UpdateStateDelegate(uint aCallbackEventCode, uint aSessionDescriptor);

    public interface ISession
    {
        bool startSession(
            long aStartPositionInHundredNanosecondUnits,
            Guid aGUIDTimeFormat);
        bool pauseSession();
        bool stopSession();
        bool closeSession();
        bool getSessionDescriptor(out uint aSessionDescriptor);
        bool registerUpdateStateDelegate(
            UpdateStateDelegate aUpdateStateDelegate);
    }
}
