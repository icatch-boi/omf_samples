#pragma once
#include "stdlib.h"
#include "_chrono.h"
namespace omf{
	class Semaphore {
	public:
		explicit Semaphore(int count = 0);
		Semaphore(const Semaphore&) = delete;
		~Semaphore();
		Semaphore& operator=(const Semaphore&) = delete;
	public:
		bool Wait();
		bool TryWait();
		bool Post();
		bool WaitFor(Duration);
		bool WaitUntil(TimePoint);
		int Count()const;

		bool WaitAbort(bool keep = false);
		bool WaitAbortOne();

	protected:
		void* _sem = 0;
	};
}
