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

#ifndef NUCLEAR_EXTENSION_IOCONTROLLER_POSIX_HPP
#define NUCLEAR_EXTENSION_IOCONTROLLER_POSIX_HPP

#include "nuclear"
#include "nuclear_bits/dsl/word/IO.hpp"

#include <unistd.h>
#include <poll.h>

namespace NUClear {
namespace extension {

	class IOController : public Reactor {
	private:
		struct Task {
			Task() : fd(), events(0), reaction() {}
			Task(const fd_t& fd, short events, const std::shared_ptr<threading::Reaction>& reaction)
				: fd(fd), events(events), reaction(reaction) {}

			fd_t fd;
			short events;
			std::shared_ptr<threading::Reaction> reaction;

			bool operator<(const Task& other) const {
				return fd == other.fd ? events < other.events : fd < other.fd;
			}
		};

	public:
		explicit IOController(std::unique_ptr<NUClear::Environment> environment);

	private:
		fd_t notifyRecv;
		fd_t notifySend;

		bool shutdown = false;
		bool dirty	= true;
		std::mutex reactionMutex;
		std::vector<pollfd> fds;
		std::vector<Task> reactions;
	};

}  // namespace extension
}  // namespace NUClear

#endif  // NUCLEAR_EXTENSION_IOCONTROLLER_POSIX_HPP
