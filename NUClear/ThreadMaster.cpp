/**
 * Copyright (C) 2013 Jake Woods <jake.f.woods@gmail.com>, Trent Houliston <trent@houliston.me>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
 * Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "ReactorController.h"

namespace NUClear {
    
    ReactorController::ThreadMaster::ThreadMaster(ReactorController* parent) :
    ReactorController::BaseMaster(parent) {
    }

    void ReactorController::ThreadMaster::start() {
        // Start our internal service threads
        for(auto it = std::begin(m_internalTasks); it != std::end(m_internalTasks); ++it) {
            // Execute our init function
            it->first();
            
            // Start a thread worker with our task
            std::unique_ptr<Internal::ThreadWorker> thread = std::unique_ptr<Internal::ThreadWorker>(new Internal::ThreadWorker(it->second));
            m_threads.insert(std::pair<std::thread::id, std::unique_ptr<Internal::ThreadWorker>>(thread->getThreadId(), std::move(thread)));
        }
        
        // Start our pool threads
        for(int i = 0; i < numThreads; ++i) {
            std::unique_ptr<Internal::ThreadWorker> thread = std::unique_ptr<Internal::ThreadWorker>(new Internal::ThreadWorker(&m_scheduler));
            m_threads.insert(std::pair<std::thread::id, std::unique_ptr<Internal::ThreadWorker>>(thread->getThreadId(), std::move(thread)));
        }
        
        // Now wait for all the threads to finish executing
        for(auto it = std::begin(m_threads); it != std::end(m_threads); ++it) {
            it->second->join();
        }
    }
    
    void ReactorController::ThreadMaster::internalTask(std::function<void ()> init, std::function<void ()> task) {
        m_internalTasks.push_back(std::move(std::make_pair(init, task)));
    }
    
    void ReactorController::ThreadMaster::submit(std::unique_ptr<Internal::Reaction::Task>&& task) {
        m_scheduler.submit(std::move(task));
    }
}
