#pragma once
#include "Includes.h"

template <class asyncOUT>
struct AsyncRequestStatus
{
	bool requestServed = false;
	std::mutex handleLock;
	std::condition_variable waitCnd;

	asyncOUT returnData;
};

template <class asyncOUT>
using AsyncRequestHandle = std::shared_ptr<AsyncRequestStatus<asyncOUT>>;

template <class asyncOUT>
using AsyncCallback = void(*)(AsyncRequestHandle<asyncOUT>);

template <class asyncIN, class asyncOUT>
struct AsyncRequest
{
	bool terminateThread = false;
	AsyncCallback<asyncOUT> callback = nullptr;
	asyncIN inData;
	AsyncRequestHandle<asyncOUT> handle = nullptr;
};

template <class asyncIN, class asyncOUT>
class AsyncFunctionality
{
public:
	AsyncFunctionality() = delete;
	AsyncFunctionality(uint32_t asyncRequestThreads);
	~AsyncFunctionality();

	void AsynchRequestWait(const AsyncRequestHandle<asyncOUT> request);
	bool AsynchRequestCompleted(const AsyncRequestHandle<asyncOUT> request);

protected:
	AsyncRequestHandle<asyncOUT> EnqueueRequest(const asyncIN& inData, AsyncCallback<asyncOUT> callback);

	asyncOUT* ReturnDataFromHandle(AsyncRequestHandle<asyncOUT> request);

	virtual void HandleRequest(const asyncIN& requestIN, asyncOUT* o_requestOUT) = 0;

private:
	void RequestThread();

	std::thread* m_agentThreads;
	uint32_t m_threadCount;

	std::queue<AsyncRequest<asyncIN, asyncOUT>> m_requestQueue;
	std::mutex m_queueLock;
	std::condition_variable m_dequeueCnd;
};

template<class asyncIN, class asyncOUT>
inline AsyncFunctionality<asyncIN, asyncOUT>::AsyncFunctionality(uint32_t asyncRequestThreads)
{
	m_threadCount = asyncRequestThreads;
	m_agentThreads = new std::thread[m_threadCount];

	for (uint32_t i = 0; i < m_threadCount; i++)
	{
		m_agentThreads[i] = std::thread(&AsyncFunctionality::RequestThread, this);
	}
}

template<class asyncIN, class asyncOUT>
inline AsyncFunctionality<asyncIN, asyncOUT>::~AsyncFunctionality()
{
	AsyncRequest<asyncIN, asyncOUT> terminationRequest;
	terminationRequest.terminateThread = true;

	for (uint32_t i = 0; i < m_threadCount; i++)
	{
		// enqueues request
		m_queueLock.lock();
		m_requestQueue.push(terminationRequest);
		m_queueLock.unlock();
		// wakes queue agent if waiting
		m_dequeueCnd.notify_one();
	}
	for (uint32_t i = 0; i < m_threadCount; i++)
	{
		m_agentThreads[i].join();
	}
	delete[] m_agentThreads;
}

template<class asyncIN, class asyncOUT>
inline void AsyncFunctionality<asyncIN, asyncOUT>::AsynchRequestWait(const AsyncRequestHandle<asyncOUT> request)
{
	std::unique_lock<std::mutex> cndLock(request->handleLock);
	while (!request->requestServed) request->waitCnd.wait(cndLock);
	cndLock.unlock();
}

template<class asyncIN, class asyncOUT>
inline bool AsyncFunctionality<asyncIN, asyncOUT>::AsynchRequestCompleted(const AsyncRequestHandle<asyncOUT> request)
{
	return request->requestServed;
}

template<class asyncIN, class asyncOUT>
inline AsyncRequestHandle<asyncOUT> AsyncFunctionality<asyncIN, asyncOUT>::EnqueueRequest(const asyncIN& inData, AsyncCallback<asyncOUT> callback)
{
	AsyncRequest<asyncIN, asyncOUT> request =
	{
		false,
		callback,
		inData,
		AsyncRequestHandle<asyncOUT>(new AsyncRequestStatus<asyncOUT>)
	};
	
	// enqueues request
	m_queueLock.lock();
	m_requestQueue.push(request);
	m_queueLock.unlock();
	// wakes queue agent if waiting
	m_dequeueCnd.notify_one();
	
	return request.handle;
}

template<class asyncIN, class asyncOUT>
inline asyncOUT* AsyncFunctionality<asyncIN, asyncOUT>::ReturnDataFromHandle(AsyncRequestHandle<asyncOUT> request)
{
	return &(request->returnData);
}

template<class asyncIN, class asyncOUT>
inline void AsyncFunctionality<asyncIN, asyncOUT>::RequestThread()
{
	AsyncRequest<asyncIN, asyncOUT> request;
	std::unique_lock<std::mutex> cndLock(m_queueLock);
	while (true)
	{
		while (m_requestQueue.empty()) m_dequeueCnd.wait(cndLock);
		request = m_requestQueue.front();
		m_requestQueue.pop();

		cndLock.unlock();
		if (request.terminateThread)
		{
			break;
		}
		HandleRequest(request.inData, &request.handle->returnData);
		// notify and post original thread
		request.handle->handleLock.lock();
		request.handle->requestServed = true;
		request.handle->handleLock.unlock();
		request.handle->waitCnd.notify_all();

		// Calls callback function
		if (request.callback != nullptr)
		{
			request.callback(request.handle);
		}
		cndLock.lock();
	}
}
