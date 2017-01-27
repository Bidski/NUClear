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

#include "nuclear_bits/extension/NetworkController.hpp"
#include "nuclear_bits/extension/network/wire_protocol.hpp"

#include <algorithm>
#include <cerrno>

namespace NUClear {
namespace extension {

    void NetworkController::tcp_connection(const TCP::Connection& connection) {

        // Allocate data for our header
        std::vector<char> payload(sizeof(network::PacketHeader));

        // Read our header
        ::recv(connection.fd, payload.data(), payload.size(), 0);
        const network::PacketHeader& header = *reinterpret_cast<network::PacketHeader*>(payload.data());

        // We have to read this now as after we resize it'll move (maybe)
        uint32_t length = header.length;

        // Add enough space for our remaining packet
        payload.resize(payload.size() + length);

        // Read our remaining packet
        ::recv(connection.fd, payload.data() + sizeof(network::PacketHeader), length, 0);
        const network::AnnouncePacket& announce = *reinterpret_cast<network::AnnouncePacket*>(payload.data());

        // See if we can find our network target for this element
        auto target = udp_target.find(std::make_pair(connection.remote.address, announce.udp_port));

        // If it does not already exist, insert it
        if (target == udp_target.end()) {

            auto it = targets.emplace(targets.end(),
                                      std::string(&announce.name),
                                      connection.remote.address,
                                      announce.tcp_port,
                                      announce.udp_port,
                                      connection.fd);
            name_target.insert(std::make_pair(std::string(&announce.name), it));
            udp_target.insert(std::make_pair(std::make_pair(connection.remote.address, announce.udp_port), it));
            tcp_target.insert(std::make_pair(it->tcp_fd, it));

            // Bind our new reaction
            it->handle = on<IO, Sync<NetworkController>>(it->tcp_fd, IO::READ | IO::ERROR | IO::CLOSE)
                             .then("Network TCP Handler", [this](const IO::Event& e) { tcp_handler(e); });

            // emit a message that says who connected
            auto j      = std::make_unique<message::NetworkJoin>();
            j->name     = &announce.name;
            j->address  = connection.remote.address;
            j->tcp_port = announce.tcp_port;
            j->udp_port = announce.udp_port;
            emit(j);
        }
        else {
            // Close the connection that was made, we made one to them faster
            // And as it was never bound to a callback, it won't show up as an event
            close(connection.fd);
        }
    }
}
}