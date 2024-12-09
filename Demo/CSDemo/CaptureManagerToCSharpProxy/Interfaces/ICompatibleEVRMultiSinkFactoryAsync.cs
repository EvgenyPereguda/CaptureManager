using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CaptureManagerToCSharpProxy.Interfaces
{
    public interface ICompatibleEVRMultiSinkFactoryAsync
    {
        Task<List<object>> createOutputNodesAsync(
            IntPtr aHWND,
            uint aOutputNodeAmount);

        Task<List<object>> createOutputNodesAsync(
            IntPtr aHandle,
            object aPtrUnkSharedResource,
            uint aOutputNodeAmount);
    }
}
