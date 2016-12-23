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

#include <renderer/chunkrenderer.h>
#include <network/clientgameconnection.h>
#include <engine/common.h>
#include <flatfactory.h>

bool NWAPICALL nwInitServer(void*);
void NWAPICALL nwRunServer();
void NWAPICALL nwStopServer();
World* NWAPICALL nwLocalServerGetWorld(size_t id);
void NWAPICALL nwLocalServerLogin(const char* username, const char* password);
Chunk* NWAPICALL nwLocalServerGetChunk(int32_t x, int32_t y, int32_t z);

MultiplayerConnection::MultiplayerConnection(const std::string& host, unsigned short port)
    :mConn([this](Identifier id, unsigned char* data, size_t len)
{
    flatbuffers::Verifier v(data,len);
    switch (id)
    {
    case Identifier::s2cChunk:
    {
        if (s2c::VerifyChunkBuffer(v))
        {
            //std::lock_guard<std::mutex> lock(mMutex);
            const s2c::Chunk *fbChunk = s2c::GetChunk(data);// TODO: Optimize
            Chunk* nwchunk = new Chunk({ fbChunk->pos()->x(), fbChunk->pos()->y(), fbChunk->pos()->z() }, *mWorld);
            for (auto i = 0; i < Chunk::Size() * Chunk::Size() * Chunk::Size(); i++)
                nwchunk->getBlocks()[i] = BlockData(fbChunk->blocks()->Get(i));
            auto c = nwchunk;
            c->setUpdated(true);
            constexpr std::array<Vec3i, 6> delta
            {
                Vec3i(1, 0, 0), Vec3i(-1, 0, 0),
                Vec3i(0, 1, 0), Vec3i(0,-1, 0),
                Vec3i(0, 0, 1), Vec3i(0, 0,-1)
            };
            for (auto&& p : delta)
                mWorld->doIfChunkLoaded(c->getPosition() + p, [](Chunk& chk)
            { chk.setUpdated(true); });
            mWorld->resetChunk(c->getPosition(), c);
        }
        break;
    }
    default:
        warningstream << "Unsupported game packet received: " << static_cast<int>(id);
    }
}), mHost(host),
mPort(port)
{
}

void MultiplayerConnection::connect()
{
    mConn.connect(mHost.c_str(), mPort);
}

void MultiplayerConnection::disconnect()
{
    mConn.stop();
}

World* MultiplayerConnection::getWorld(size_t)
{
    //not implement
    warningstream << "Function not implemented";
    Assert(false);
    return nullptr;
}

void MultiplayerConnection::waitForConnected()
{
    mConn.waitForConnected();
}

void MultiplayerConnection::login(const char *username, const char *password)
{
    mConn.send<c2s::Login>(FlatFactory::c2s::Login(username,password),PacketPriority::HIGH_PRIORITY, PacketReliability::RELIABLE_ORDERED_WITH_ACK_RECEIPT);
}

void MultiplayerConnection::getChunk(Vec3i pos)
{
    // Pre-Load the Chunk.
    mWorld->addChunk(pos);
    // Requese From The Server
    mConn.send<c2s::RequestChunk>(FlatFactory::c2s::RequestChunk(pos), PacketPriority::MEDIUM_PRIORITY, PacketReliability::UNRELIABLE);
}

LocalConnection::LocalConnection():
    mTimeout(getJsonValue<int>(getSettings()["client"]["server_start_timeout"], 30))
{
}

void LocalConnection::connect()
{
    mLocalServerThread = std::thread([this]()
    {
        bool opened = nwInitServer((void*)1);
        if (opened)
        {
            mReady.store(true);
            nwRunServer();
        }
        else
            fatalstream << "Failed to start local server";
    });

    mStartTime = std::chrono::system_clock::now();

    while (!mReady)
    {
        std::this_thread::yield();
        if (mTimeout != -1 && std::chrono::system_clock::now() - mStartTime > std::chrono::seconds(mTimeout))
        {
            fatalstream << "Local server timeout!";
            mLocalServerThread.detach();
            return;
        }
    }
}

void LocalConnection::disconnect()
{
    if (mLocalServerThread.joinable())
    {
        debugstream << "Call nwStopServer";
        nwStopServer();
        debugstream << "Waiting for local server thread...";
        mLocalServerThread.join();
        debugstream << "Local server thread exited!";
    }
}

void LocalConnection::waitForConnected()
{
}

void LocalConnection::login(const char * username, const char * password)
{
    nwLocalServerLogin(username, password);
}

void LocalConnection::getChunk(Vec3i pos)
{
    auto c = nwLocalServerGetChunk(pos.x, pos.y, pos.z);
    Assert(c);
    c->setUpdated(true);
    constexpr std::array<Vec3i, 6> delta
    {
        Vec3i(1, 0, 0), Vec3i(-1, 0, 0),
        Vec3i(0, 1, 0), Vec3i(0,-1, 0),
        Vec3i(0, 0, 1), Vec3i(0, 0,-1)
    };
    for (auto&& p : delta)
        mWorld->doIfChunkLoaded(c->getPosition() + p, [](Chunk& chk)
    { chk.setUpdated(true); });
    c->increaseRef();
    mWorld->insertChunk(c->getPosition(), std::move(ChunkManager::data_t(c, ChunkOnReleaseBehavior::Behavior::DeReference)));
}

World * LocalConnection::getWorld(size_t id)
{
    return nwLocalServerGetWorld(id);
}
