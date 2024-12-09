/*
MIT License

Copyright(c) 2021 Evgeny Pereguda

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
using System.Threading.Tasks;

namespace CaptureManagerToCSharpProxy.Interfaces
{
    public interface ISourceControlAsync
    {
        Task<object> createSourceNodeWithDownStreamConnectionAsync(
           string aSymbolicLink,
           uint aIndexStream,
           uint aIndexMediaType,
           object aPtrDownStreamTopologyNode);

        Task<object> getSourceOutputMediaTypeAsync(
           string aSymbolicLink,
           uint aIndexStream,
           uint aIndexMediaType);

        Task<object> createSourceAsync(string aSymbolicLink);

        Task<object> createSourceFromCaptureProcessorAsync(object aPtrCaptureProcessor);

        Task<object> createSourceNodeAsync(
            string aSymbolicLink,
            uint aIndexStream,
            uint aIndexMediaType);

        Task<object> createSourceNodeFromExternalSourceAsync(
            object aPtrMediaSource,
            uint aIndexStream,
            uint aIndexMediaType);

        Task<object> createSourceNodeFromExternalSourceWithDownStreamConnectionAsync(
            object aPtrMediaSource,
            uint aIndexStream,
            uint aIndexMediaType,
            object aPtrDownStreamTopologyNode);

        Task<string> getCollectionOfSourcesAsync();

        Task<object> getSourceOutputMediaTypeFromMediaSourceAsync(
            object aPtrMediaSource,
            uint aIndexStream,
            uint aIndexMediaType);

        Task<IWebCamControlAsync> createWebCamControlAsync(string aSymbolicLink);
    }
}
