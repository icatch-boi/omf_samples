#ifndef __IOT_QUEUE_H__
#define __IOT_QUEUE_H__

#include <atomic>
#include <functional>
#include "blockingconcurrentqueue.h"

using namespace moodycamel;

namespace iot {
	namespace queue {
		class IotQueueElem {
		public:
			IotQueueElem(void* data=0, int size=0, std::function<bool(void**)> release=0);
			virtual ~IotQueueElem();
		public:
			bool DataBufBind(void* pSrcData);/* bind pData pointer with actual data-memory */
			bool ReleaseMethodBind(std::function<bool(void**)> release);
			bool DataCopy(void* pBuf, int bufSize);/* memcopy vaild data to pre-allocated memory */
	        bool DataCopy(void* header, int headerSize, void* pBuf, int bufSize);/* memcopy vaild data to pre-allocated memory */
			bool DataClear();/* clear data which in actual data-memory */
			bool Reset();/* reset item which is in form of IotQueueElem*/
			bool DataBufRelease();/* release actual data-memory */
			void RefCountPlusPlus();
			void RefCountMinusMinus();
			int DataSize() const;
			bool DataSize(int size);
			void *DataBuf() const;
		private:
			void* _pData = nullptr;
			int _dataSize = 0;
			std::function<bool(void** pBuf)> _dataBufRelease = 0;
			mutable int _refCount = 0;
		};
		/////////////////////////////////
		class IotBlockingQueue :public BlockingConcurrentQueue <IotQueueElem> {
		public:
			IotBlockingQueue();
			IotBlockingQueue(size_t capacity);
			virtual ~IotBlockingQueue();
		public:
			bool Enqueue(IotQueueElem& elem);
			bool Try_enqueue(IotQueueElem& elem);
			bool Try_dequeue(IotQueueElem& elem);
			bool Wait_dequeue_for(IotQueueElem& elem, const std::int64_t timeoutUs);
			bool Wait_dequeue_for(IotQueueElem& elem, const std::chrono::microseconds timeout);
			bool Clear();
		};
	}
}
#endif
