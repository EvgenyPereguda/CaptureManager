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
    class SwitcherNodeFactory: ISwitcherNodeFactory, ISwitcherNodeFactoryAsync
    {
        CaptureManagerLibrary.ISwitcherNodeFactory mSwitcherNodeFactory;

        public SwitcherNodeFactory(CaptureManagerLibrary.ISwitcherNodeFactory aSwitcherNodeFactory)
        {
            mSwitcherNodeFactory = aSwitcherNodeFactory;
        }

        private async Task<object> createSwitcherNodeTask(object aPtrDownStreamTopologyNode, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                object lresult = false;

                do
                {
                    if (mSwitcherNodeFactory == null)
                        break;

                    try
                    {

                        mSwitcherNodeFactory.createSwitcherNode(
                            aPtrDownStreamTopologyNode,
                            out lresult);

                        if (lresult == null)
                            break;
                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public bool createSwitcherNode(object aPtrDownStreamTopologyNode, out object aPtrPtrTopologySwitcherNode)
        {
            bool lresult = false;

            aPtrPtrTopologySwitcherNode = createSwitcherNodeTask(aPtrDownStreamTopologyNode, false).Result;

            lresult = aPtrPtrTopologySwitcherNode != null;

            return lresult;
        }

        public async Task<object> createSwitcherNodeAsync(object aPtrDownStreamTopologyNode)
        {
            return await createSwitcherNodeTask(aPtrDownStreamTopologyNode, true);
        }
    }
}
