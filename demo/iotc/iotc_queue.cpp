#include "iotc_queue.h"
#include <cstring>

using namespace iot::queue;
using namespace std;
using namespace std::chrono;

/*
 *Name:IotQueueElem::IotQueueElem
 *Brief:Constructor for class IotQueueElem.
 *Params:void* data ---- data buffer pointer.
 *		int size ---- data size.
 *		std::function<bool(void**)> release ---- data buffer release interface.
 *Return:None	
 */
IotQueueElem::IotQueueElem(void* data, int size, std::function<bool(void**)> release)
{
	_pData = data;
	_dataSize = size;
	_dataBufRelease = release;
}
/*
 *Name:IotQueueElem::~IotQueueElem.
 *Brief:Destructor for class IotQueueElem.
 *Params:void
 *Return:None	
 */
IotQueueElem::~IotQueueElem()
{
	if(_pData && !_refCount) {
		DataBufRelease();
		_pData = nullptr;
		_dataSize = 0;
		_dataBufRelease = 0;
	}
}
/*
 *Name:IotQueueElem::DataBufBind
 *Brief:bind member variable _pData with source data buffer pointer.
 *Params:void* pSrcData ---- the pointer pointing to source data buffer.
 *Return:false ---- param is nullptr.
 *		 true ---- operate successfully
 */
bool IotQueueElem::DataBufBind(void* pSrcData)
{
	if(!pSrcData) {
		return false;
	}
	_pData = pSrcData;
	return true;
}

/*
 *Name:IotQueueElem::DataBuf
 *Brief:return data buffer pointer.
 *Params:void
 *Return:the pointer to data buffer.
 */

void* IotQueueElem::DataBuf() const
{
	return _pData;
}

/*
 *Name:IotQueueElem::ReleaseMethodBind
 *Brief:bind data-buffer release function with _dataBufRelease to free data-buffer in heap area.
 *Params:std::function<bool(void**)> release ---- release function.
 *Return:always return true.
 */
bool IotQueueElem::ReleaseMethodBind(std::function<bool(void**)> release)
{
	_dataBufRelease = release;
	return true;
}
/*
 *Name:IotQueueElem::DataCopy
 *Brief:memcpy inner _pData to destination buffer.
 *Params:void* pBuf ---- the destination buffer pointer.
 *		int bufSize ---- the destination buffer's total-size.
 *Return:if successfully, return true
 *		if params aren't reasonable, return false.
 */
bool IotQueueElem::DataCopy(void* pBuf, int bufSize)
{
	if(!pBuf || !_pData) {
		return false;
	}
	if(bufSize < _dataSize) {
		return false;
	}
	memcpy(pBuf, _pData, _dataSize);
	return true;
}
/*
 *Name:IotQueueElem::DataCopy
 *Brief:memcpy inner _pData to destination buffer.
 *Params:void* header ---- data header 
 *		int headerSize ---- data header size
 *		void* pBuf ---- the destination buffer pointer.
 *		int bufSize ---- the destination buffer's total-size.
 *Return:if successfully, return true
 *		if params aren't reasonable, return false.
 */

bool IotQueueElem::DataCopy(void* header, int headerSize, void* pBuf, int bufSize)
{
	if(!pBuf || !_pData) {
		return false;
	}
	if(_dataSize < headerSize) {
		return false;
	}
	if(bufSize < (_dataSize - headerSize)) {
		return false;
	}
    if (header != nullptr && headerSize > 0) {
        memcpy(header, _pData, headerSize);
        memcpy(pBuf, ((char*)_pData) + headerSize, _dataSize - headerSize);
    } 
	else {
        memcpy(pBuf, _pData, _dataSize);
    }

	return true;
}

/*
 *Name:IotQueueElem::DataSize
 *Brief:Gain the current actual size of data.
 *Params:void
 *Return:the actual data size.
 */
int IotQueueElem::DataSize() const
{
	return _dataSize;
}
/*
 *Name:IotQueueElem::DataSize
 *Brief:Set the current actual size of data.
 *Params:the size of data
 *Return:true or false.
 */

bool IotQueueElem::DataSize(int size)
{
	if(size < 0) {
		return false;
	}
	_dataSize = size;
	return true;
}

/*
 *Name:IotQueueElem::DataClear
 *Brief:Clean up the data buffer allocated in heap area.
 *Params:void
 *Return: if the operated data buffer pointer is nullptr, return false. Otherwise, return true.
 */
bool IotQueueElem::DataClear()
{
	if(!_pData) {
		return false;
	}
	memset(_pData,0, _dataSize);
	_dataSize = 0;
	return true;
}

/*
 *Name:IotQueueElem::Reset
 *Brief:Reset all the memeber variables, if the data-buffer exists, free it.
 *Params:void
 *Return: if release data-buffer failed, return false. Otherwise, return true.
 */
bool IotQueueElem::Reset()
{
	if(_pData) {
		if(DataBufRelease()) {
			_pData = 0;
			_dataSize = 0;
			_dataBufRelease = 0;
			_refCount = 0;
			return true;
		}
		else {
			return false;
		}
	}
	return true;
}
/*
 *Name:IotQueueElem::DataBufRelease
 *Brief:Release data-buffer in heap area.
 *Params:void
 *Return:if successfully, return true.
 *		if release function not exists and data buffer exists, return false.
 *		if release function execute failly, return false.		
 */
bool IotQueueElem::DataBufRelease()
{
	if(!_pData) {
		return true;
	}
	if(!_dataBufRelease) {
		return false;
	}
	if(_dataBufRelease(&_pData)) {
		_pData = 0;
		_dataSize = 0;
		return true;
	}
	return false;
}
/*
 *Name:IotQueueElem::RefCountPlusPlus
 *Brief:member variable _refCount++.
 *Params:void
 *Return:void	
 */
void IotQueueElem::RefCountPlusPlus()
{
	_refCount++;
}
/*
 *Name:IotQueueElem::RefCountMinusMinus
 *Brief:member variable _refCount--.
 *Params:void
 *Return:void	
 */
void IotQueueElem::RefCountMinusMinus()
{
	_refCount = (_refCount-1<=0)? 0 : (_refCount-1);
}

////////////////////////////////////////////////////////////////////////////////////
/*
 *Name:IotBlockingQueue::IotBlockingQueue
 *Brief:Constructor without parameter for class IotBlockingQueue.
 *Params:void
 *Return:None	
 */
IotBlockingQueue::IotBlockingQueue()
	:BlockingConcurrentQueue()
{

}
/*
 *Name:IotBlockingQueue::IotBlockingQueue
 *Brief:Constructor which is with parameter for class IotBlockingQueue.
 *Params:size_t capacity ---- the preset capacity for data-transfer queue.
 *Return:None	
 */
IotBlockingQueue::IotBlockingQueue(size_t capacity)
	:BlockingConcurrentQueue(capacity)
{

}
/*
 *Name:IotBlockingQueue::~IotBlockingQueue
 *Brief:Destructor for class IotBlockingQueue.
 *Params:void
 *Return:None	
 */
IotBlockingQueue::~IotBlockingQueue()
{
	Clear();
}
/*
 *Name:IotBlockingQueue::Enqueue
 *Brief:Enqueue data to the data-transfer queue. If queue is full, it will expand capacity automatically.
 *Params:IotQueueElem& elem ---- the item to be enqueued.
 *Return:if successfully,return true. Otherwise, return false.	
 */
bool IotBlockingQueue::Enqueue(IotQueueElem& elem)
{
	elem.RefCountPlusPlus();
	if(!BlockingConcurrentQueue::enqueue(elem)) {
		elem.RefCountMinusMinus();
		return false;
	}
	return true;
}
/*
 *Name:IotBlockingQueue::Try_enqueue
 *Brief:Enqueue data to the data-transfer queue. If queue is full, it will return false immediately.
 *Params:IotQueueElem& elem ---- the item to be enqueued.
 *Return:if successfully,return true. Otherwise, return false.	
 */
bool IotBlockingQueue::Try_enqueue(IotQueueElem& elem)
{
	elem.RefCountPlusPlus();
	if(!BlockingConcurrentQueue::try_enqueue(elem)) {
		elem.RefCountMinusMinus();
		return false;
	}
	return true;
}
/*
 *Name:IotBlockingQueue::Try_dequeue
 *Brief:Dequeue data from the data-transfer queue. If queue is empty, it will return false immediately.
 *Params:IotQueueElem& elem ---- the item to be dequeued.
 *Return:if successfully,return true. Otherwise, return false.	
 */
bool IotBlockingQueue::Try_dequeue(IotQueueElem& elem)
{
	if(BlockingConcurrentQueue::try_dequeue<IotQueueElem>(elem)) {
		elem.RefCountMinusMinus();
		return true;
	}
	return false;
}
/*
 *Name:IotBlockingQueue::Wait_dequeue_for
 *Brief:Dequeue data from the data-transfer queue. If queue is empty, it will return false when the timeout-time expires.
 *Params:IotQueueElem& elem ---- the item to be dequeued.
 *		const std::int64_t timeoutUs ---- timeout in microseconds.
 *Return:if successfully,return true. Otherwise, return false.	
 */
bool IotBlockingQueue::Wait_dequeue_for(IotQueueElem& elem, const std::int64_t timeoutUs)
{

	if(BlockingConcurrentQueue::wait_dequeue_timed<IotQueueElem>(elem, timeoutUs)) {
		elem.RefCountMinusMinus();
		return true;
	}
	return false;
}
/*
 *Name:IotBlockingQueue::Wait_dequeue_for
 *Brief:Dequeue data from the data-transfer queue. If queue is empty, it will return false when the timeout-time expires.
 *Params:IotQueueElem& elem ---- the item to be dequeued.
 *		const std::chrono::microseconds timeout --- timeout in microseconds.
 *Return:if successfully,return true. Otherwise, return false.	
 */
bool IotBlockingQueue::Wait_dequeue_for(IotQueueElem& elem, const std::chrono::microseconds timeout)
{
	if(BlockingConcurrentQueue::wait_dequeue_timed<IotQueueElem>(elem, timeout)) {
		elem.RefCountMinusMinus();
		return true;
	}
	return false;
}
/*
 *Name:IotBlockingQueue::Clear
 *Brief:clear all the items in the data-transfer queue.
 *Params:void
 *Return:if successfully,return true. Otherwise, return false.	
 */
bool IotBlockingQueue::Clear()
{
	IotQueueElem item;
	do{
		item.Reset();
		if(Try_dequeue(item)) {
			item.DataBufRelease();
		}
		else {
			break;
		}
	}while(1);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////

