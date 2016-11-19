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
#include <nwsafety.hpp>
#include <raknet/MessageIdentifiers.h>
#include <raknet/BitStream.h>
#include <debug.h>

Connection::Connection(std::function<void(Identifier, unsigned char*, size_t)> userDataCallback)
    :mUserDataCallback(userDataCallback)
{
    infostream << "Raknet initializating...";
    mPeer = RakNet::RakPeerInterface::GetInstance();
    infostream << "Raknet initialized.";
}

Connection::~Connection()
{
    Assert(mUserClosed);
}

bool Connection::connect(const char *addr, unsigned short port)
{
    RakNet::SocketDescriptor sd;
    sd.socketFamily = AF_INET; // IPv4
    RakNet::StartupResult ret = mPeer->Startup(1, &sd, 1);
    if(ret == RakNet::StartupResult::RAKNET_STARTED)
    {
        using CAR = RakNet::ConnectionAttemptResult;
        CAR cret = mPeer->Connect(addr, port, nullptr, 0);

        switch(cret)
        {
        case CAR::CONNECTION_ATTEMPT_STARTED:
            mThread = std::thread([this]
            {
                infostream << "Start listening.";
                loop();
            });
            return true;
        default:
            return false;
        }
    }

    // TODO: throw an exception
    errorstream << "Failed to connect to" << addr << ":" << port << ". Error code: " << ret;
    return false;
}

void Connection::loop()
{
    RakNet::Packet* p;
    while(mPeer->IsActive())
    {
        for(p = mPeer->Receive(); p; mPeer->DeallocatePacket(p), p=mPeer->Receive())
        {
            switch(p->data[0])
            {
            case ID_CONNECTION_REQUEST_ACCEPTED:
                infostream << "Connected to the server.";
                mAddr = p->systemAddress;
                mConnected.set_value();
                break;
            case ID_CONNECTION_ATTEMPT_FAILED:
                errorstream << "Failed to connect to the server!";
                break;
            case ID_DISCONNECTION_NOTIFICATION:
                if(mUserClosed)
                    infostream << "Disconnected to the server.";
                else
                    errorstream << "Disconnected to the server!";
                break;
            case ID_USER_PACKET_ENUM:
            {
                union {short s; char c[2];} id; // 2 bytes.
                id.c[0] = p->data[2];
                id.c[1] = p->data[1];
                auto identifier = static_cast<Identifier>(id.s);
                if (identifier <= Identifier::Unknown || identifier >= Identifier::EndIdentifier)
                {
                    warningstream << "Unexpected packet has been received. Packet ID:" << id.s
                                  << " From " << p->systemAddress.ToString(true);
                    break;
                }
                mUserDataCallback(identifier, p->data + 3, p->length - 3);
                break;
            }
            default:
                // TODO:Handle unknown packet.
                break;
            }
        }

    }
}
void Connection::sendRawData(Identifier id, const unsigned char *data, uint32_t len, PacketPriority priority, PacketReliability reliability)
{
    RakNet::BitStream bsOut;
    bsOut.Write(static_cast<RakNet::MessageID>(ID_USER_PACKET_ENUM));
    bsOut.Write(static_cast<int16_t>(id));
    bsOut.WriteBits(data, len * CHAR_BIT);
    mPeer->Send(&bsOut, priority, reliability, 0, mAddr, false);
}
