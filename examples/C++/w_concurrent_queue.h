/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_concurrent_queue.h
	Description		 : A cross platform concurrent queue class 
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_CONCURRENT_QUEUE_H__
#define __W_CONCURRENT_QUEUE_H__

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace wolf
{
	namespace system
	{
        template <typename T>
        class w_concurrent_queue
        {
        public:
            void pop(T& pItem)
            {
                std::unique_lock<std::mutex> _lock(this->_mutex);
                {
                    while (this->_queue.empty())
                    {
                        this->_cv.wait(_lock);
                    }
                    pItem = this->_queue.front();
                    this->_queue.pop();
                }
                //unlock will be called automatically   
            }

            void push(const T& pItem)
            {
                //lock on queue
                std::unique_lock<std::mutex> _lock(this->_mutex);
                {
                    this->_queue.push(pItem);
                    _lock.unlock();
                }
                //then notify to conditional variable
                this->_cv.notify_one();
            }

            size_t unsafe_size()
            {
                return this->_queue.size();
            }

            size_t safe_size()
            {
                size_t _size = 0;
                std::unique_lock<std::mutex> _lock(this->_mutex);
                {   
                    _size = this->_queue.size();
                    _lock.unlock();
                }
                return _size;
            }

        private:
            std::queue<T>               _queue;
            std::mutex                  _mutex;
            std::condition_variable     _cv;
        };
	}
}

#endif //__W_CONCURRENT_QUEUE_H__