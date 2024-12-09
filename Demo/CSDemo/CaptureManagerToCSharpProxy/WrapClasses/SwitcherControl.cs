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
    class SwitcherControl : ISwitcherControl, ISwitcherControlAsync
    {
        CaptureManagerLibrary.ISwitcherControl mSwitcherControl = null;

        public SwitcherControl(CaptureManagerLibrary.ISwitcherControl aSwitcherControl)
        {
            mSwitcherControl = aSwitcherControl;
        }

        private async Task<bool> pauseSwitchersTask(ISession aSession, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    try
                    {
                        if (mSwitcherControl == null)
                            break;

                        if (aSession == null)
                            break;

                        uint lSessionDescriptor = 0;

                        if (!aSession.getSessionDescriptor(out lSessionDescriptor))
                            break;

                        mSwitcherControl.pauseSwitchers(lSessionDescriptor);

                        lresult = true;

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<bool> pauseSwitchersAsync(ISessionAsync aSession)
        {
            return await pauseSwitchersTask(aSession as ISession, true);
        }

        public bool pauseSwitchers(ISession aSession)
        {
            return pauseSwitchersTask(aSession, false).Result;
        }

        private async Task<bool> resumeSwitchersTask(ISession aSession, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    try
                    {
                        if (mSwitcherControl == null)
                            break;

                        if (aSession == null)
                            break;

                        uint lSessionDescriptor = 0;

                        if (!aSession.getSessionDescriptor(out lSessionDescriptor))
                            break;

                        mSwitcherControl.resumeSwitchers(lSessionDescriptor);

                        lresult = true;

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<bool> resumeSwitchersAsync(ISessionAsync aSession)
        {
            return await resumeSwitchersTask(aSession as ISession, true);
        }

        public bool resumeSwitchers(ISession aSession)
        {
            return resumeSwitchersTask(aSession, false).Result;
        }

        private async Task<bool> detachSwitchersTask(ISession aSession, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    try
                    {
                        if (mSwitcherControl == null)
                            break;

                        if (aSession == null)
                            break;

                        uint lSessionDescriptor = 0;

                        if (!aSession.getSessionDescriptor(out lSessionDescriptor))
                            break;

                        mSwitcherControl.detachSwitchers(lSessionDescriptor);

                        lresult = true;

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<bool> detachSwitchersAsync(ISessionAsync aSession)
        {
            return await detachSwitchersTask(aSession as ISession, true);
        }

        public bool detachSwitchers(ISession aSession)
        {
            return detachSwitchersTask(aSession, false).Result;
        }

        private async Task<bool> atttachSwitcherTask(object aSwitcherNode, object aAttachedNode, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    try
                    {
                        if (mSwitcherControl == null)
                            break;

                        if (aSwitcherNode == null)
                            break;

                        if (aAttachedNode == null)
                            break;

                        mSwitcherControl.attachSwitcher(aSwitcherNode, aAttachedNode);

                        lresult = true;

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<bool> atttachSwitcherAsync(object aSwitcherNode, object aAttachedNode)
        {
            return await atttachSwitcherTask(aSwitcherNode, aAttachedNode, true);
        }

        public bool atttachSwitcher(object aSwitcherNode, object aAttachedNode)
        {
            return atttachSwitcherTask(aSwitcherNode, aAttachedNode, false).Result;
        }

        private async Task<bool> pauseSwitcherTask(object aSwitcherNode, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    try
                    {
                        if (mSwitcherControl == null)
                            break;

                        if (aSwitcherNode == null)
                            break;

                        mSwitcherControl.pauseSwitcher(aSwitcherNode);

                        lresult = true;

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<bool> pauseSwitcherAsync(object aSwitcherNode)
        {
            return await pauseSwitcherTask(aSwitcherNode, true);
        }

        public bool pauseSwitcher(object aSwitcherNode)
        {
            return pauseSwitcherTask(aSwitcherNode, false).Result;
        }

        private async Task<bool> resumeSwitcherTask(object aSwitcherNode, bool aIsAwait)
        {
            return await Task.Run(() =>
            {
                bool lresult = false;

                do
                {
                    try
                    {
                        if (mSwitcherControl == null)
                            break;

                        if (aSwitcherNode == null)
                            break;

                        mSwitcherControl.resumeSwitcher(aSwitcherNode);

                        lresult = true;

                    }
                    catch (Exception exc)
                    {
                        LogManager.getInstance().write(exc.Message);
                    }

                } while (false);

                return lresult;
            }).ConfigureAwait(aIsAwait);
        }

        public async Task<bool> resumeSwitcherAsync(object aSwitcherNode)
        {
            return await resumeSwitcherTask(aSwitcherNode, true);
        }

        public bool resumeSwitcher(object aSwitcherNode)
        {
            return resumeSwitcherTask(aSwitcherNode, false).Result;
        }
    }
}
