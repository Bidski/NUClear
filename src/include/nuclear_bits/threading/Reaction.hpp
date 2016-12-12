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

#ifndef NUCLEAR_THREADING_REACTION_HPP
#define NUCLEAR_THREADING_REACTION_HPP

#include <functional>
#include <atomic>
#include <memory>
#include <string>

#include "ReactionTask.hpp"

namespace NUClear {

// Forward declare reactor
class Reactor;

namespace threading {

	/**
	 * @brief This class holds the definition of a Reaction (call signature).
	 *
	 * @details
	 *  A reaction holds the information about a callback. It holds the options as to how to process it in the
	 * scheduler.
	 *  It also holds a function which is used to generate databound Task objects (callback with the function arguments
	 *  already loaded and ready to run).
	 */
	class Reaction {
		// Reaction handles are given to user code to enable and disable the reaction
		friend class ReactionHandle;
		friend class ReactionTask;

	public:
		// The type of the generator that is used to create functions for ReactionTask objects
		using TaskGenerator = std::function<std::pair<int, ReactionTask::TaskFunction>(Reaction&)>;

		/**
		 * @brief Constructs a new Reaction with the passed callback generator and options
		 *
		 * @param reactor        the reactor this belongs to
		 * @param identifier     string identifier information about the reaction to help identify it
		 * @param callback       the callback generator function (creates databound callbacks)
		 * @param unbinder       the function used to unbind this reaction and clean it up
		 */
		Reaction(Reactor& reactor,
				 std::vector<std::string> identifier,
				 TaskGenerator callback,
				 std::function<void(Reaction&)>&& unbinder);

		/**
		 * @brief creates a new databound callback task that can be executed.
		 *
		 * @return a unique_ptr to a Task which has the data for it's call bound into it
		 */
		std::unique_ptr<ReactionTask> getTask();

		/**
		 * @brief returns true if this reaction is currently enabled
		 */
		bool isEnabled();

		/// @brief the reactor this belongs to
		Reactor& reactor;

		/// @brief This holds the demangled name of the On function that is being called
		std::vector<std::string> identifier;

		/// @brief the unique identifier for this Reaction object
		const uint64_t id;

		/// @brief the number of currently active tasks (existing reaction tasks)
		std::atomic<int> activeTasks;

		/// @brief if this reaction object is currently enabled
		std::atomic<bool> enabled;

	private:
		/**
		 * @brief Unbinds this reaction from it's context
		 */
		void unbind();

		/// @brief a source for reactionIds, atomically creates longs
		static std::atomic<uint64_t> reactionIdSource;
		/// @brief the callback generator function (creates databound callbacks)
		TaskGenerator generator;
		/// @brief unbinds the reaction and cleans up
		std::function<void(Reaction&)> unbinder;
	};

}  // namespace threading
}  // namespace NUClear

#endif  // NUCLEAR_THREADING_REACTION_HPP
