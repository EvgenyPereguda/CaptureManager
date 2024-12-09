#pragma once

namespace CaptureManager
{
	template <typename T>
	class Singleton :
		public T
	{
	public:
		static Singleton& getInstance()
		{
			static Singleton lInstance;

			return lInstance;
		}
	private:
		Singleton(){};
		virtual ~Singleton(){};
		Singleton(const Singleton&) = delete;
		Singleton& operator= (const Singleton&) = delete;
	};
}