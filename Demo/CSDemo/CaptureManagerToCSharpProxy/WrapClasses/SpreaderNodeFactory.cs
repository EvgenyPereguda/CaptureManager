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
using System.Threading.Tasks;
using CaptureManagerToCSharpProxy.Interfaces;

namespace CaptureManagerToCSharpProxy.WrapClasses
{
    class SpreaderNodeFactory : ISpreaderNodeFactory, ISpreaderNodeFactoryAsync
    {
        CaptureManagerLibrary.ISpreaderNodeFactory mISpreaderNodeFactory = null;

        public SpreaderNodeFactory(
            CaptureManagerLibrary.ISpreaderNodeFactory aISpreaderNodeFactory)
        {
            mISpreaderNodeFactory = aISpreaderNodeFactory;
        }

        private async Task<object> createSpreaderNodeTask(
            List<object> aDownStreamTopologyNodelist,
            bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                object lresult = null;

                do
                {
                    if (mISpreaderNodeFactory == null)
                        break;

                    if (aDownStreamTopologyNodelist == null ||
                        aDownStreamTopologyNodelist.Count == 0)
                        break;

                    object lArrayDownStreamTopologyNodelist = aDownStreamTopologyNodelist.ToArray();

                    mISpreaderNodeFactory.createSpreaderNode(
                        lArrayDownStreamTopologyNodelist,
                        out lresult);

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<object> createSpreaderNodeAsync(List<object> aDownStreamTopologyNodelist)
        {
            return await createSpreaderNodeTask(aDownStreamTopologyNodelist, true);
        }

        public bool createSpreaderNode(
            List<object> aDownStreamTopologyNodelist, 
            out object aTopologyNode)
        {
            bool lresult = false;

            aTopologyNode = createSpreaderNodeTask(aDownStreamTopologyNodelist, false).Result;

            lresult = aTopologyNode != null;

            return lresult;
        }
    }
}
