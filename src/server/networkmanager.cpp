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

#include "logger.h"
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
    debugstream << "Waiting for the raknet thread.";
    mThread.join();
    RakNet::RakPeerInterface::DestroyInstance(mPeer);
}

bool NetworkManager::run(const char *addr,unsigned short port)
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
        mThread = std::thread([this] {loop();});
        return true;
    }
    fatalstream << "Failed to start Network Manager. Error code: " << ret;
    Assert(false);
    return false;
}

void NetworkManager::close()
{
    mPeer->Shutdown(300);
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
            default:
                auto identifier = static_cast<Identifier>(p->data[0]);
                if(identifier<=Identifier::Unknown|| identifier>=Identifier::EndIdentifier)
                {
                    warningstream << "Unexcepted packet is received. Packet ID:" << static_cast<int>(p->data[0])
                                  << " From " << p->systemAddress.ToString(true);
                    break;
                }
                mConns[mPeer->GetIndexFromSystemAddress(p->systemAddress)]->handleReceivedData(identifier, p->data + 1);
                break;
            }
        }
        for (auto& w : mWorlds)
            w->updateChunkLoadStatus();
        RakSleep(30);
    }
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
