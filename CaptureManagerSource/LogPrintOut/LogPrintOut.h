#pragma once

#include <iostream>
#include <iomanip>
#include <sstream>
#include <map>
#include <mutex>
#include <exception>
#include <memory>



#include "../DataParser/DataParser.h"
#include "../Common/Common.h"

namespace CaptureManager
{
	class LogPrintOut
	{
	public:



		enum Level :int
		{
			INFO_LEVEL = 0,
			ERROR_LEVEL = INFO_LEVEL + 1
		};

		static LogPrintOut& getInstance();

		void setVerbose(
			bool aState);

		void setVerbose(
			LogPrintOut::Level aLevel,
			std::wstring aFilePath,
			bool aState);

		bool addPtrLogPrintOutStream(
			Level aLevel,
			std::wostream* aPtrWOStream,
			bool aSelfReleased);

		bool addFileLogPrintOutStream(
			Level aLevel,
			std::wstring aFilePath);

		bool removeFileLogPrintOutStream(
			Level aLevel,
			std::wstring aFilePath);

		bool releaseLogPrintOutStream(
			Level aLevel);

		bool releaseLogPrintOutStream(
			std::wostream* aPtrWOStream);

		void addCallbackInner(IUnknown* aPtrUnkCallbackInner);

		template <typename... Args>
		inline bool printOutln(
			Level aLevel,
			const Args... aRest)
		{
			std::lock_guard<std::mutex> llock(mLogPrintOutMutex);

			bool lResult = false;

			if (mVerbose)
			{
				lResult = writeToStream(aLevel, [this](std::wostream &aWOStream, const Args... aRest){this->write(aWOStream, aRest...); aWOStream << std::endl; }, aRest...);
			}

			return lResult;
		}

		template <typename... Args>
		inline bool printOutlnUnlock(
			Level aLevel,
			const Args... aRest)
		{
			bool lResult = false;

			if (mVerbose)
			{
				lResult = writeToStream(aLevel, [this](std::wostream &aWOStream, const Args... aRest){this->write(aWOStream, aRest...); aWOStream << std::endl; }, aRest...);
			}

			return lResult;
		}

		template <typename... Args>
		inline bool printOut(
			Level aLevel,
			const Args... aRest)
		{
			std::lock_guard<std::mutex> llock(mLogPrintOutMutex);

			bool lResult = false;

			if (mVerbose)
			{
				lResult = writeToStream(aLevel, [this](std::wostream &aWOStream, const Args... aRest){this->write(aWOStream, aRest...); }, aRest...);
			}

			return lResult;
		}


	private:

		struct StreamContainer
		{
			std::wostream* mPtrWOStream = nullptr;

			bool mSelfReleased = false;

			bool mIsVerbose = true;

			std::wstring mName;
		};

		std::multimap<Level, StreamContainer> mLevelStreams;

		bool mVerbose = true;

		std::mutex mLogPrintOutMutex;

		std::map<std::wstring, std::unique_ptr<std::wostream>> mStreamContainersDictionary;




		LogPrintOut();

		~LogPrintOut();


		LogPrintOut(
			const LogPrintOut&) = delete;

		LogPrintOut& operator=(
			const LogPrintOut&) = delete;


		template<typename WriteFunc, typename... Args>
		bool writeToStream(
			Level aLevel,
			const WriteFunc aWriteFunc,
			const Args... aRest)
		{
			bool lResult = false;

			do
			{
				if (!mVerbose)
					break;

				auto lrangeItr = mLevelStreams.equal_range(aLevel);

				std::multimap<Level, StreamContainer>::iterator litr;

				for (litr = lrangeItr.first; litr != lrangeItr.second; ++litr)
				{
					if ((*litr).second.mPtrWOStream != nullptr && (*litr).second.mIsVerbose)
					{

						//std::wstringstream lwstringstream;

						//lwstringstream << (*litr).second.mIsVerbose;

						//MessageBox(NULL, lwstringstream.str().c_str(), L"lFirstArg.vt", 0);

						(*litr).second.mPtrWOStream->flush();

						switch (aLevel)
						{
						case LogPrintOut::INFO_LEVEL:
							(*((*litr).second.mPtrWOStream)) << L"INFO_LEVEL: ";
							break;
						case LogPrintOut::ERROR_LEVEL:
							(*((*litr).second.mPtrWOStream)) << L"ERROR_LEVEL: ";
							break;
						default:
							break;
						}

						aWriteFunc((*((*litr).second.mPtrWOStream)), aRest...);

						(*litr).second.mPtrWOStream->flush();

					}
				}

				lResult = true;
			} while (false);

			return lResult;
		}

		void releaseStreams();

		bool releaseStream(
			Level aLevel);

		bool releaseStream(
			std::wostream* aPtrWOStream);

		template <typename T>
		auto convertPointerToRef(
			T *pointer) -> decltype(*pointer)
		{
			return *pointer;
		}

		template <typename T>
		auto convertPointerToRef(
			T &ref) -> decltype(ref) // Stub convert method
		{
			return ref;
		}

		inline void write(
			std::wostream &pwostream,
			const wchar_t *aData) throw()
		{
			pwostream << aData;
		}

		inline void write(
			std::wostream &pwostream,
			const char *aData) throw()
		{
			pwostream << aData;
		}

		template <typename T>
		inline void write(
			std::wostream &pwostream,
			const T &aData) throw()
		{
			pwostream << aData;
		}

		template <>
		inline void write(
			std::wostream &pwostream,
			const HRESULT &aData) throw()
		{
			pwostream << "" << std::showbase << std::internal << std::setfill(L'0') << std::setw(10) << std::hex << aData << ", Description: " << DataParser::getHRESULTDescription(aData) << std::dec;
		}


		template <typename T, typename... Args>
		inline void write(
			std::wostream &pwostream,
			const T &aData,
			const Args... aRest)
		{
			write(pwostream, aData);

			write(pwostream, aRest...);
		}

	};
}