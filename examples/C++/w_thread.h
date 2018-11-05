/*
	Project			 : Wolf Engine. Copyright(c) Pooya Eimandar (http://PooyaEimandar.com) . All rights reserved.
	Source			 : Please direct any bug to https://github.com/PooyaEimandar/Wolf.Engine/issues
	Website			 : http://WolfSource.io
	Name			 : w_thread.h
	Description		 : A cross platform thread class 
	Comment          :
*/

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __W_THREAD_H__
#define __W_THREAD_H__

#if  defined(__WIN32) || defined(__UWP)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#endif

#include <thread>
#include <functional>

namespace wolf
{
	namespace system
	{
        class w_thread_pimp;
        class w_thread
        {
        public:
            w_thread();
            ~w_thread();

            void add_job(const std::function<void()>& pJob);
            void wait();
            void release();

            //hardware_thread_contexts usually equal to number of CPU cores
            static unsigned int get_number_of_hardware_thread_contexts()
            {
                return std::thread::hardware_concurrency();
            }

#if defined(__WIN32) || defined(__UWP)
            static DWORD get_current_thread_id()
            {
                return GetCurrentThreadId();
            }
            static void sleep_current_thread(const DWORD& pMilliSecond)
            {
                Sleep(pMilliSecond);
            }
#else
            static size_t get_current_thread_id()
            {
                return std::hash<std::thread::id>{}(std::this_thread::get_id());
            }
            static void sleep_current_thread(const long long& pMilliSeconds)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(pMilliSeconds));
            }
#endif


        private:
            w_thread_pimp*                          _pimp;
        };
	}
}

#endif //__W_THREAD_H__