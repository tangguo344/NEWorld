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

#include "nwsafety.hpp"
#include "networkmanager.h"
#include <raknet/MessageIdentifiers.h>
#include <raknet/BitStream.h>
#include <raknet/RakSleep.h>
#include "gameconnection.h"

constexpr static const unsigned int max_client = 100; // TODO: get it from settingsmanager.

NetworkManager::NetworkManager(WorldManager& wm):mWorlds(wm)
{
    debugstream << "Raknet initializating...";
    mPeer = RakNet::RakPeerInterface::GetInstance();
    infostream << "Raknet initialized.";
}

NetworkManager::~NetworkManager()
{
    RakNet::RakPeerInterface::DestroyInstance(mPeer);
}

bool NetworkManager::start(const char *addr,unsigned short port)
{
    if(mPeer == nullptr)
    {
        fatalstream << "Get RakPeerInterface failed!";
        return false;
    }
    auto sd = RakNet::SocketDescriptor(port, addr);
    sd.socketFamily = AF_INET; // IPv4
    RakNet::StartupResult ret = mPeer->Startup(max_client,&sd,1);
    mPeer->SetMaximumIncomingConnections(max_client);
    mConns.reserve(max_client);
    if(ret == RakNet::StartupResult::RAKNET_STARTED)
    {
        return true;
    }
    fatalstream << "Failed to start Network Manager. Error code: " << ret;
    Assert(false);
    return false;
}

void NetworkManager::close()
{
    mPeer->Shutdown(5000, 0, PacketPriority::HIGH_PRIORITY);
}

void NetworkManager::loop()
{
    RakNet::Packet* p = nullptr;
    while(mPeer->IsActive())
    {
        for (p = mPeer->Receive(); p; mPeer->DeallocatePacket(p),p = mPeer->Receive())
        {
            switch(p->data[0])
            {
            case ID_NEW_INCOMING_CONNECTION:
            {
                newConnection(p->systemAddress);
                break;
            }
            case ID_DISCONNECTION_NOTIFICATION:
            case ID_CONNECTION_LOST:
                deleteConnection(p->systemAddress);
                break;
            case ID_USER_PACKET_ENUM:
            {
                union {short s; char c[2];} id; // 2 bytes.
                id.c[0] = p->data[2];
                id.c[1] = p->data[1];
                auto identifier = static_cast<Identifier>(id.s);
                if(identifier<=Identifier::Unknown|| identifier>=Identifier::EndIdentifier)
                {
                    warningstream << "Unexpected packet has been received. Packet ID:" << id.s
                                  << " From " << p->systemAddress.ToString(true);
                    break;
                }
                mConns[mPeer->GetIndexFromSystemAddress(p->systemAddress)]->handleReceivedData(identifier, p->data + 3, p->length - 3);
                break;
            }
            default:
                // TODO:handle unknown packet.
                break;
            }
        }
        for (auto& w : mWorlds)
            w->updateChunkLoadStatus();
    }
    debugstream << "Stop listening.";
}

GameConnection* NetworkManager::newConnection(RakNet::SystemAddress addr)
{
    GameConnection *c = new MultiplayerConnection(mWorlds,*this,mPeer,addr);
    mConns.insert(mConns.begin()+mPeer->GetIndexFromSystemAddress(addr), c);
    return c;
}

void NetworkManager::deleteConnection(RakNet::SystemAddress addr)
{
    delete mConns[mPeer->GetIndexFromSystemAddress(addr)];
}
