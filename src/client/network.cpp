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
#include "network.h"
#include <logger.h>

const int UpdateInterval = 10;

void errorHandle(const boost::asio::ip::tcp::socket&, boost::system::error_code ec)
{
    errorstream << "Network error, code:" << ec.value();
}

bool Connection::init(boost::asio::ip::tcp::socket& socket)
{
    boost::asio::ip::tcp::resolver resolver(m_ioService);
    try
    {
        boost::asio::connect(socket, resolver.resolve({m_hostIP, std::to_string(m_port)}));
        return true;
    }
    catch (std::exception& e)
    {
        fatalstream << "Exception: " << e.what();
        return false;
    }
}

void Connection::mainLoop()
{
    boost::asio::ip::tcp::socket socket(m_ioService);
    if (!init(socket))
        exit(-1);

    m_session = std::make_shared<Session>(std::move(socket));
    m_session->start();
    m_session->addRequest(std::move(LoginPacket("test", "123456", NEWorldVersion).makePacket()));
    m_ioService.run();
}

void Session::doUpdate()
{
    auto self(shared_from_this());
    m_updateTimer.expires_from_now(boost::posix_time::milliseconds(UpdateInterval));
    m_updateTimer.async_wait([this, self](boost::system::error_code)
    {
        doWrite();
    });
}
