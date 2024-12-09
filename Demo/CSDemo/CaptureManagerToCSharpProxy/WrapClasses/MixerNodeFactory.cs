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

using CaptureManagerToCSharpProxy.Interfaces;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace CaptureManagerToCSharpProxy.WrapClasses
{
    class MixerNodeFactory : IMixerNodeFactory, IMixerNodeFactoryAsync
    {
        private CaptureManagerLibrary.IMixerNodeFactory mIMixerNodeFactory;

        public MixerNodeFactory(
            CaptureManagerLibrary.IMixerNodeFactory aIMixerNodeFactory)
        {
            mIMixerNodeFactory = aIMixerNodeFactory;
        }

        private async Task<List<object>> createMixerNodesTask(
            object aPtrDownStreamTopologyNode,
            uint aInputNodeAmount,
            bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                List<object> lresult = new List<object>();

                do
                {
                    if (mIMixerNodeFactory == null)
                        break;


                    try
                    {
                        object lArrayMediaNodes = new Object();

                        mIMixerNodeFactory.createMixerNodes(
                            aPtrDownStreamTopologyNode,
                            aInputNodeAmount,
                            out lArrayMediaNodes);

                        if (lArrayMediaNodes == null)
                            break;

                        object[] lArray = lArrayMediaNodes as object[];

                        if (lArray == null)
                            break;

                        lresult.AddRange(lArray);
                    }
                    catch (Exception)
                    {
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public bool createMixerNodes(
            object aPtrDownStreamTopologyNode, 
            uint aInputNodeAmount, 
            out List<object> aTopologyInputNodesList)
        {
            bool lresult = false;

            aTopologyInputNodesList = createMixerNodesTask(aPtrDownStreamTopologyNode, aInputNodeAmount, false).Result;

            lresult = aTopologyInputNodesList.Count != 0;

            return lresult;
        }

        public async Task<List<object>> createMixerNodesAsync(object aPtrDownStreamTopologyNode, uint aInputNodeAmount)
        {
            return await createMixerNodesTask(aPtrDownStreamTopologyNode, aInputNodeAmount, true);
        }
    }
}
