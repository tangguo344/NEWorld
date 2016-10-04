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
* MERCHANTABILITY or FITNESSRaknet FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with NEWorld.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "network.h"
#include <logger.h>
#include <raknet/MessageIdentifiers.h>
#include <raknet/BitStream.h>

Connection::Connection()
{
    infostream << "Raknet initializating...";
    mPeer = RakNet::RakPeerInterface::GetInstance();
}

Connection::~Connection()
{
    RakNet::RakPeerInterface::DestroyInstance(mPeer);
}

bool Connection::connect(const char *addr, unsigned short port)
{
    RakNet::SocketDescriptor sd;
    sd.socketFamily = AF_INET; // IPv4
    RakNet::StartupResult ret = mPeer->Startup(1,&sd,1);
    if(ret == RakNet::StartupResult::RAKNET_STARTED)
    {
        using CAR = RakNet::ConnectionAttemptResult;
        CAR cret = mPeer->Connect(addr,port,nullptr,0);
        
        switch(cret)
        {
        case CAR::CONNECTION_ATTEMPT_STARTED:
            mThread = std::thread([this] {loop();});
            return true;
        default:
            return false;
        }
    }
    else
    {
        // TODO: throw an exception
        return false;
    }
}

void Connection::loop()
{
    RakNet::Packet* p = nullptr;
    while(mPeer->IsActive())
    {
        for(p = mPeer->Receive(); p; mPeer->DeallocatePacket(p), p=mPeer->Receive())
        {
            switch(p->data[0])
            {
                case ID_CONNECTION_REQUEST_ACCEPTED:
                {
                    debugstream << "ID_CONNECTION_REQUEST_ACCEPTED";
                    break;
                }
                default:
                    break;
            }
        }
        
    }
}