/*
 * Copyright (C) 2013-2016 Trent Houliston <trent@houliston.me>, Jake Woods <jake.f.woods@gmail.com>
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

#ifndef NUCLEAR_DSL_WORD_SYNC_HPP
#define NUCLEAR_DSL_WORD_SYNC_HPP

namespace NUClear {
    namespace dsl {
        namespace word {

            /**
             * @brief This option sets the Synchronization group of the task
             *
             * @details
             *  The synchronization group of a task is a compile time mutex which will allow only a single task from
             *  each distinct execution task to execute at a time. For example, if two tasks both had Sync<int> then only
             *  one of those tasks would execute at a time.
             *
             * @tparam TSync the type with which to synchronize on
             */
            template <typename TSync>
            struct Sync {

                using task_ptr = std::unique_ptr<threading::ReactionTask>;

                /// @brief our queue which sorts tasks by priority
                static std::priority_queue<task_ptr> queue;
                /// @brief how many tasks are currently running
                static volatile bool running;
                /// @brief a mutex to ensure data consistency
                static std::mutex mutex;

                template <typename DSL>
                static inline std::unique_ptr<threading::ReactionTask> reschedule(std::unique_ptr<threading::ReactionTask>&& task) {

                    // Lock our mutex
                    std::lock_guard<std::mutex> lock(mutex);

                    // If we are already running then queue, otherwise return and set running
                    if (running) {
                        queue.push(std::move(task));
                        return std::unique_ptr<threading::ReactionTask>(nullptr);
                    }
                    else {
                        running = true;
                        return std::move(task);
                    }
                }

                template <typename DSL>
                static void postcondition(threading::ReactionTask& task) {

                    // Lock our mutex
                    std::lock_guard<std::mutex> lock(mutex);

                    // We are finished running
                    running = false;

                    // If we have another task, add it
                    if (!queue.empty()) {
                        std::unique_ptr<threading::ReactionTask> nextTask(std::move(const_cast<std::unique_ptr<threading::ReactionTask>&>(queue.top())));
                        queue.pop();

                        // Resubmit this task to the reaction queue
                        task.parent.reactor.powerplant.submit(std::move(nextTask));
                    }
                }
            };

            template <typename TSync>
            std::priority_queue<typename Sync<TSync>::task_ptr> Sync<TSync>::queue;

            template <typename TSync>
            volatile bool Sync<TSync>::running = false;

            template <typename TSync>
            std::mutex Sync<TSync>::mutex;

        }  // namespace word
    }  // namespace dsl
}  // namespace NUClear

#endif  // NUCLEAR_DSL_WORD_SYNC_HPP
