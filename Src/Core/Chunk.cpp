//
//  Chunk.cpp
//  NWCore
//
//  Created by User on 2017/9/1.
//  Copyright © 2017年 Infinideastudio. All rights reserved.
//

#include "ChunkInternal.hpp"
#include <cstdlib>
#include <cstring>
#include <istream>
#include <ostream>

namespace Core {
    void Chunk::setBlock(int id, BlockData data) {
        mBlocks[id] = data;
    }
    
    void Chunk::freeMetaData(uint16_t pos) {
        auto it = std::find_if(mMetaReflectTable.begin(), mMetaReflectTable.end(),
                [pos](std::pair<uint16_t, uint16_t> l) { return l.first == pos; });
        if (it != mMetaReflectTable.end()) {
            auto index = it->second;
            free(mMetaTable[index].data);
            mMetaReflectTable.erase(it);
            for (auto&& x : mMetaReflectTable) if (x.second > index) --x.second;
        }

    }
    
    bool Chunk::readMetaData(uint16_t pos, void* mem) {
        auto it = std::find_if(mMetaReflectTable.begin(), mMetaReflectTable.end(),
                [pos](std::pair<uint16_t, uint16_t> l) { return l.first == pos; });
        if (it != mMetaReflectTable.end()) {
            auto index = it->second;
            memcpy(mem, mMetaTable[index].data, mMetaTable[index].size);
            return true;
        }
        return false;
    }

    void Chunk::writeMetaData(uint16_t pos, const void* mem, uint16_t size) {
        auto it = std::find_if(mMetaReflectTable.begin(), mMetaReflectTable.end(),
                [pos](std::pair<uint16_t, uint16_t> l) { return l.first == pos; });
        if (it != mMetaReflectTable.end()) {
            auto index = it->second;
            if (mMetaTable[index].size != size) {
                mMetaTable[index].data = realloc(mMetaTable[index].data, size);
                mMetaTable[index].size = size;
            }
            memcpy(mMetaTable[index].data, mem, size);
        } else {
            mMetaReflectTable.push_back(std::pair<uint16_t, uint16_t>(pos, mMetaTable.size()));
            void* sec = malloc(size);
            memcpy(sec, mem, size);
            mMetaTable.push_back(MetaData::Discriptor{ size, sec });
        }
    }
    
    /* Chunk Storage Spec [Brief]
     4096 * 4 Bytes : Blocks
     2 Bytes : metaCount
     2 * metaCount Bytes : ReflectTable
     ...... metaData
     */

    void Chunk::readChunk(std::istream& in) {
        in.read(reinterpret_cast<char*>(mBlocks), sizeof(mBlocks));
        uint16_t count; // nameHole, metaCount;
        in.read(reinterpret_cast<char*>(&count), sizeof(count));
        mMetaReflectTable.resize(count);
        mMetaTable.resize(count);
        in.read(reinterpret_cast<char*>(mMetaReflectTable.data()), count * 4);
        for (int i = 0; i < count; ++i) {
            in.read(reinterpret_cast<char*>(&mMetaTable[i].size), 4);
            mMetaTable[i].data = malloc(mMetaTable[i].size);
            in.read(reinterpret_cast<char*>(mMetaTable[i].data), mMetaTable[i].size);
        }
    }

    void Chunk::writeChunk(std::ostream& out) {
        out.write(reinterpret_cast<char*>(mBlocks), sizeof(mBlocks));
        uint16_t count = mMetaTable.size();
        out.write(reinterpret_cast<char*>(mMetaReflectTable.data()), count * 4);
        for (int i = 0; i < count; ++i) {
            out.write(reinterpret_cast<char*>(&mMetaTable[i].size), 4);
            out.write(reinterpret_cast<char*>(mMetaTable[i].data), mMetaTable[i].size);
        }
    }

}

ChunkHandler nwCreateChunk() {
    return new Core::Chunk;
}

void nwDestroyChunk(ChunkHandler h) {
    delete reinterpret_cast<Core::Chunk*>(h);
}

void nwChunkAcquireModify(ChunkHandler h) {
    reinterpret_cast<Core::Chunk*>(h)->getLock().lock();
}

BlockData nwChunkGetBlock(ChunkHandler h, int id) {
    return reinterpret_cast<Core::Chunk*>(h)->getBlock(id);
}

void nwChunkSetBlock(ChunkHandler h, int id, BlockData data) {
    reinterpret_cast<Core::Chunk*>(h)->setBlock(id, data);
}

void nwChunkReleaseModify(ChunkHandler h) {
    reinterpret_cast<Core::Chunk*>(h)->getLock().unlock();
}


void nwChunkFreeMetaData(ChunkHandler h, uint16_t pos) {
    reinterpret_cast<Core::Chunk*>(h)->freeMetaData(pos);
}

bool nwChunkReadMetaData(ChunkHandler h, uint16_t pos, void* mem) {
    return reinterpret_cast<Core::Chunk*>(h)->readMetaData(pos, mem);
}

void nwChunkWriteMetaData(ChunkHandler h, uint16_t pos, const void* mem, uint16_t size) {
    reinterpret_cast<Core::Chunk*>(h)->writeMetaData(pos, mem, size);
}

