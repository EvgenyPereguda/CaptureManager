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
    public interface IEVRStreamControlAsync
    {
        Task<bool> flushAsync(object aPtrEVROutputNode);

        Task<EVRStreamControlPosition> getPositionAsync(object aPtrEVROutputNode);

        Task<EVRStreamControlPosition> getSrcPositionAsync(object aPtrEVROutputNode);

        Task<uint> getZOrderAsync(object aPtrEVROutputNode);

        Task<bool> setPositionAsync(object aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom);

        Task<bool> setSrcPositionAsync(object aPtrEVROutputNode, float aLeft, float aRight, float aTop, float aBottom);

        Task<bool> setZOrderAsync(object aPtrEVROutputNode, uint aZOrder);

        Task<string> getCollectionOfFiltersAsync(object aPtrEVROutputNode);

        Task<string> getCollectionOfOutputFeaturesAsync(object aPtrEVROutputNode);

        Task setFilterParametrAsync(object aPtrEVROutputNode, uint aParametrIndex, int aNewValue, bool aIsEnabled);

        Task setOutputFeatureParametrAsync(object aPtrEVROutputNode, uint aParametrIndex, int aNewValue);
    }

    public struct EVRStreamControlPosition
    {
        public float aPtrLeft;
        public float aPtrRight;
        public float aPtrTop;
        public float aPtrBottom;
    }
}
