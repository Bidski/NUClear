/*
 * Copyright (C) 2013 Trent Houliston <trent@houliston.me>, Jake Woods <jake.f.woods@gmail.com>
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

namespace {
    
    constexpr unsigned short port = 40009;
    int messagesReceived = 0;
    
    const std::string testString = "Hello TCP World!";
    
    struct Message {
    };
    
    class TestReactor : public NUClear::Reactor {
    public:
        TestReactor(std::unique_ptr<NUClear::Environment> environment) : Reactor(std::move(environment)) {
            
            // Bind to a known port
            on<TCP>(port).then([this](const TCP::Connection& connection) {
                
                on<IO>(connection.fd, IO::READ).then([this] (IO::Event event) {
                    
                    char buff[1024];
                    memset(buff, 0, sizeof(buff));
                    
                    // Read into the buffer
                    int len = read(event.fd, buff, testString.size());
                    
                    // The connection was closed and the other test finished
                    if (len == 0 && messagesReceived == 2) {
                        powerplant.shutdown();
                    }
                    else {
                        REQUIRE(len == testString.size());
                        REQUIRE(testString == std::string(buff));
                        ++messagesReceived;
                    }
                });
            });
            
            // Bind to an unknown port and get the port number
            int boundPort;
            std::tie(std::ignore, boundPort) = on<TCP>(0).then([this](const TCP::Connection& connection) {
                on<IO>(connection.fd, IO::READ).then([this] (IO::Event event) {
                    
                    char buff[1024];
                    memset(buff, 0, sizeof(buff));
                    
                    // Read into the buffer
                    int len = read(event.fd, buff, testString.size());
                    
                    // The connection was closed and the other test finished
                    if (len == 0 && messagesReceived == 2) {
                        powerplant.shutdown();
                    }
                    else {
                        REQUIRE(len == testString.size());
                        REQUIRE(testString == std::string(buff));
                        ++messagesReceived;
                    }
                });
            });
            
            // Send a test message to the known port
            on<Trigger<Message>>().then([this] {
            
                // Open a random socket
                int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                
                // Our address to our local connection
                sockaddr_in address;
                address.sin_family = AF_INET;
                address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
                address.sin_port = htons(port);
                
                // Connect to ourself
                ::connect(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address));
                
                // Write on our socket
                size_t sent = write(fd, testString.data(), testString.size());
                
                // Close our connection
                close(fd);
                
                // We must have sent the right amount of data
                REQUIRE(sent == testString.size());
            });
            
            // Send a test message to the freely bound port
            on<Trigger<Message>>().then([this, boundPort] {
                // Open a random socket
                int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
                
                // Our address to our local connection
                sockaddr_in address;
                address.sin_family = AF_INET;
                address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
                address.sin_port = htons(boundPort);
                
                // Connect to ourself
                ::connect(fd, reinterpret_cast<sockaddr*>(&address), sizeof(address));
                
                // Write on our socket
                size_t sent = write(fd, testString.data(), testString.size());
                
                // Close our connection
                close(fd);
                
                // We must have sent the right amount of data
                REQUIRE(sent == testString.size());
            });
            
            on<Startup>().then([this] {
                
                // Emit a message just so it will be when everything is running
                emit(std::make_unique<Message>());
            });
                               
        }
    };
}

TEST_CASE("Testing listening for TCP connections and receiving data messages", "[api][network][tcp]") {
    
    NUClear::PowerPlant::Configuration config;
    config.threadCount = 1;
    NUClear::PowerPlant plant(config);
    plant.install<TestReactor>();
    
    plant.start();
}