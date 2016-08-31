/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This file is part of NEWorld.
* NEWorld is free software: you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* NEWorld is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SESSION_H_
#define SESSION_H_

#include <queue>
#include "networkshared.h"
#include "packet.h"

class Session :public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket)
        : m_socket(std::move(socket)), m_updateTimer(m_socket.get_io_service())
    {
    }

    void start()
    {
        doRead();
        doUpdate();
    }

    void addRequest(Packet& packet)
    {
        m_packets.push(packet);
    }

    void addRequest(Packet&& packet)
    {
        m_packets.push(packet);
    }

private:
    void doUpdate();
    void doRead();
    void doWrite();

    tcp::socket m_socket;
    std::queue<Packet> m_packets; // Packets need sent
    Packet m_packetRead;

    boost::asio::deadline_timer m_updateTimer;
};

#endif // !SESSION_H_
