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

#include <fstream>
#include "LogPrintOut.h"
#include "LogPrintOut_wostream.h"
#include "../Common/Singleton.h"
#include "../Common/Common.h"
#include "../Common/Config.h"

namespace CaptureManager
{

	LogPrintOut::LogPrintOut()
	{
#ifndef COMMERCIAL
		addPtrLogPrintOutStream(LogPrintOut::INFO_LEVEL, &std::wcout, false);
#endif

		addPtrLogPrintOutStream(
			LogPrintOut::INFO_LEVEL,
			&Singleton<LogPrintOut_wostream>::getInstance(),
			false
		);	

		addPtrLogPrintOutStream(
			LogPrintOut::ERROR_LEVEL,
			&Singleton<LogPrintOut_wostream>::getInstance(),
			false
		);
	}

	LogPrintOut::~LogPrintOut()
	{
		releaseStreams();
	}

	LogPrintOut& LogPrintOut::getInstance()
	{
		static LogPrintOut lInstance;

		return lInstance;
	}


	void LogPrintOut::setVerbose(
		bool aState)
	{
		mVerbose = aState;
	}

	void LogPrintOut::setVerbose(
		LogPrintOut::Level aLevel,
		std::wstring aFilePath,
		bool aState)
	{
		std::lock_guard<std::mutex> llock(mLogPrintOutMutex);

		do
		{

			if (aFilePath.empty())
				break;

			auto lrangeItr = mLevelStreams.equal_range(aLevel);

			std::multimap<Level, StreamContainer>::iterator litr;

			for (litr = lrangeItr.first; litr != lrangeItr.second; ++litr)
			{
				if ((*litr).second.mPtrWOStream != nullptr)
				{
					if ((*litr).second.mName == aFilePath)
					{
						litr->second.mIsVerbose = aState;


						//std::wstringstream lwstringstream;

						//lwstringstream << litr->second.mIsVerbose;

						//MessageBox(NULL, lwstringstream.str().c_str(), L"lFirstArg.vt", 0);
						//if (litr->second.mPtrWOStream != nullptr)
						//{
						//	mLevelStreams.erase(litr);

						//	break;
						//}

						break;
					}

				}
			}

			//if (!aState)
			//{
			//	addFileLogPrintOutStream(
			//		aLevel,
			//		aFilePath);

			//}

		} while (false);
	}

	bool LogPrintOut::addPtrLogPrintOutStream(
		Level aLevel,
		std::wostream* aPtrWOStream,
		bool aSelfReleased)
	{
		std::lock_guard<std::mutex> llock(mLogPrintOutMutex);

		bool lResult = false;

		do
		{

			if (aPtrWOStream == nullptr)
				break;

			StreamContainer lStreamContainer;

			lStreamContainer.mPtrWOStream = aPtrWOStream;

			lStreamContainer.mSelfReleased = aSelfReleased;

			auto l = mLevelStreams.insert(std::pair<Level, StreamContainer>(aLevel, lStreamContainer));

			lResult = true;

		} while (false);

		return lResult;
	}

	bool LogPrintOut::addFileLogPrintOutStream(
		Level aLevel,
		std::wstring aFilePath)
	{
		std::lock_guard<std::mutex> llock(mLogPrintOutMutex);

		bool lResult = false;

		do
		{

			if (aFilePath.empty())
				break;

			if (aFilePath == L"cout")
			{
				return addPtrLogPrintOutStream(
					aLevel,
					&std::wcout,
					false);
			}

			auto lfindIter = mStreamContainersDictionary.find(aFilePath);

			StreamContainer lStreamContainer;

			lStreamContainer.mName = aFilePath;

			if (lfindIter != mStreamContainersDictionary.end())
			{

				lStreamContainer.mPtrWOStream = (*lfindIter).second.get();

				lStreamContainer.mSelfReleased = false;

				auto l = mLevelStreams.insert(std::pair<Level, StreamContainer>(aLevel, lStreamContainer));
			}
			else
			{
				std::unique_ptr<std::wfstream> lptrfstream(new std::wfstream);

				lptrfstream->open(aFilePath, std::wfstream::out);

				auto k = lptrfstream->is_open();

				if (!k)
				{
					break;
				}

				lStreamContainer.mPtrWOStream = lptrfstream.get();

				mStreamContainersDictionary.insert(
					std::pair<std::wstring, std::unique_ptr<std::wostream>>(
					aFilePath,
					std::move(lptrfstream)));

				lStreamContainer.mSelfReleased = false;

				auto l = mLevelStreams.insert(std::pair<Level, StreamContainer>(aLevel, lStreamContainer));
			}

			lResult = true;

		} while (false);

		return lResult;
	}

	bool LogPrintOut::removeFileLogPrintOutStream(
		Level aLevel,
		std::wstring aFilePath)
	{
		std::lock_guard<std::mutex> llock(mLogPrintOutMutex);

		bool lResult = false;

		do
		{

			if (aFilePath.empty())
				break;

			auto lrangeItr = mLevelStreams.equal_range(aLevel);

			std::multimap<Level, StreamContainer>::iterator litr;

			for (litr = lrangeItr.first; litr != lrangeItr.second; ++litr)
			{
				if ((*litr).second.mPtrWOStream != nullptr)
				{
					if ((*litr).second.mName == aFilePath)
					{
						if (litr->second.mPtrWOStream != nullptr)
						{
							lResult = true;


							//std::wstringstream lwstringstream;

							//lwstringstream << litr->second.mIsVerbose;

							//MessageBox(NULL, lwstringstream.str().c_str(), L"lFirstArg.vt", 0);
							mLevelStreams.erase(litr);

							break;
						}

						break;
					}

				}
			}

		} while (false);

		return lResult;
	}

	bool LogPrintOut::releaseLogPrintOutStream(
		Level aLevel)
	{
		std::lock_guard<std::mutex> llock(mLogPrintOutMutex);

		return releaseStream(aLevel);
	}

	bool LogPrintOut::releaseLogPrintOutStream(
		std::wostream* aPtrWOStream)
	{
		std::lock_guard<std::mutex> llock(mLogPrintOutMutex);

		return releaseStream(aPtrWOStream);
	}

	void LogPrintOut::addCallbackInner(IUnknown* aPtrUnkCallbackInner)
	{
		Singleton<LogPrintOut_wostream>::getInstance().addCallbackInner(aPtrUnkCallbackInner);
	}


	void LogPrintOut::releaseStreams()
	{
		auto itr = mLevelStreams.begin();

		for (; itr != mLevelStreams.end(); ++itr)
		{
			if (itr->second.mSelfReleased && itr->second.mPtrWOStream != nullptr)
			{
				itr->second.mPtrWOStream->flush();
			}
		}

		mLevelStreams.clear();
	}

	bool LogPrintOut::releaseStream(
		Level aLevel)
	{

		bool lResult = false;

		auto lrangeItr = mLevelStreams.equal_range(aLevel);

		std::multimap<Level, StreamContainer>::iterator litr;

		for (litr = lrangeItr.first; litr != lrangeItr.second; ++litr)
		{
			if ((*litr).second.mSelfReleased && (*litr).second.mPtrWOStream != nullptr)
			{
				(*litr).second.mPtrWOStream->flush();

				delete (*litr).second.mPtrWOStream;
			}
		}

		mLevelStreams.erase(aLevel);

		lResult = true;

		return lResult;

	}

	bool LogPrintOut::releaseStream(
		std::wostream* aPtrWOStream)
	{

		bool lResult = false;


		auto itr = mLevelStreams.begin();

		for (; itr != mLevelStreams.end(); ++itr)
		{
			if (itr->second.mPtrWOStream != nullptr && itr->second.mPtrWOStream == aPtrWOStream)
			{
				lResult = true;

				mLevelStreams.erase(itr);

				break;
			}
		}

		return lResult;

	}

}