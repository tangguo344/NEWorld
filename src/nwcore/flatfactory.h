#ifndef FLATFACTORY_H
#define FLATFACTORY_H

#include "../protocol/gen/protocol.h"
#include <common/common.h>
#include <common/nwmath.hpp>

namespace FlatFactory
{
    static flatbuffers::FlatBufferBuilder fbb;
    namespace c2s
    {
        inline flatbuffers::FlatBufferBuilder &Login(const char *username, const char *password)
        {
            fbb.Clear();
            auto login = ::c2s::CreateLoginDirect(fbb, username, password, NEWorldVersion);
            ::c2s::FinishLoginBuffer(fbb, login);
            return fbb;
        }

        inline flatbuffers::FlatBufferBuilder &RequestChunk(Vec3i pos)
        {
            fbb.Clear();
            auto req = ::c2s::CreateRequestChunk(fbb, pos.x, pos.y, pos.z);
            ::c2s::FinishRequestChunkBuffer(fbb, req);
            return fbb;
        }
    }

    namespace s2c
    {
        inline flatbuffers::FlatBufferBuilder &Chunk(::s2c::Vec3 pos,std::vector<int> blocks)
        {
            fbb.Clear();
            auto c = ::s2c::CreateChunkDirect(fbb, &pos, &blocks);
            ::s2c::FinishChunkBuffer(fbb, c);
            return fbb;
        }
    }
}

#endif
