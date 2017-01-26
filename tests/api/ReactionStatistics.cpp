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

#include <catch.hpp>

#include "nuclear"

// Anonymous namespace to keep everything file local
namespace {

template <int id>
struct Message {};

bool seenMessage0       = false;
bool seenMessageStartup = false;

class TestReactor : public NUClear::Reactor {
public:
    TestReactor(std::unique_ptr<NUClear::Environment> environment) : Reactor(std::move(environment)) {

        on<Trigger<NUClear::message::ReactionStatistics>>().then(
            "Reaction Stats Handler", [this](const NUClear::message::ReactionStatistics& stats) {

                // Flag if we have seen the message handler
                if (stats.identifier[0] == "Message Handler") {
                    seenMessage0 = true;
                }
                // Flag if we have seen the startup handler
                else if (stats.identifier[0] == "Startup Handler") {
                    seenMessageStartup = true;
                }

                // Ensure exceptions are passed through correctly in the exception handler
                if (stats.exception) {
                    REQUIRE(stats.identifier[0] == "Exception Handler");
                    try {
                        std::rethrow_exception(stats.exception);
                    }
                    catch (const std::exception& e) {
                        REQUIRE(std::string(e.what()) == std::string("Exceptions happened"));

                        // We are done
                        powerplant.shutdown();
                    }
                }
            });

        on<Trigger<Message<0>>>().then("Message Handler", [this] { emit(std::make_unique<Message<1>>()); });

        on<Trigger<Message<1>>>().then("Exception Handler",
                                       [this] { throw std::runtime_error("Exceptions happened"); });

        on<Startup>().then("Startup", [this] { emit(std::make_unique<Message<0>>()); });
    }
};
}

TEST_CASE("Testing reaction statistics functionality", "[api][reactionstatistics]") {

    NUClear::PowerPlant::Configuration config;
    config.threadCount = 1;
    NUClear::PowerPlant plant(config);

    // We are installing with an initial log level of debug
    plant.install<TestReactor>();

    plant.start();
}
