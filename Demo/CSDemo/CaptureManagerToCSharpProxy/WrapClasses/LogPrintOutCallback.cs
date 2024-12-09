using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CaptureManagerToCSharpProxy.WrapClasses
{
    public enum LogLevel
    {
        INFO_LEVEL = 0,
        ERROR_LEVEL = INFO_LEVEL + 1
    };

    public delegate void WriteLogDelegate(LogLevel aLogLevel, string aPtrLogString);

    class LogPrintOutCallback : CaptureManagerLibrary.ILogPrintOutCallback
    {
        private WriteLogDelegate mWriteLogDelegate;

        public LogPrintOutCallback(
            WriteLogDelegate aWriteStateDelegate)
        {
            mWriteLogDelegate = aWriteStateDelegate;
        }

        public void invoke(uint aLevelType, string aPtrLogString)
        {
            LogLevel lLogLevel = (LogLevel)aLevelType;

            if (mWriteLogDelegate != null)
                mWriteLogDelegate(
                    lLogLevel,
                    aPtrLogString);
        }
    }
}
