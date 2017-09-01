
//
//  ChunkInternal.hpp
//  NWCore
//
//  Created by User on 2017/9/1.
//  Copyright © 2017年 Infinideastudio. All rights reserved.
//

#pragma once 

#include "Chunk.h"
#include <ios>
#include <mutex>
#include <vector>
#include <deque>

namespace Core {
    namespace MetaData {
        struct Discriptor {
            uint32_t size;
            void* data;
        };
    }
    
    class Chunk {
    public:
        Chunk() {}
        virtual ~Chunk() {}
        // Access Lock
        auto& getLock() { return mChunkMutex; }
        // Block Access
        virtual void setBlock(int id, BlockData data);
        BlockData getBlock(int id) const { return mBlocks[id]; }
        // Meta Data Management
        void freeMetaData(uint16_t pos);
        bool readMetaData(uint16_t handle, void* mem);
        void writeMetaData(uint16_t pos, const void* mem, uint16_t size);
        // Chunk I/O, For Use Of World Manager
        void readChunk(std::istream& in);
        void writeChunk(std::ostream& out);
    private:
        BlockData mBlocks[4096];
        std::recursive_mutex mChunkMutex;
        // Meta Data Pool
        std::vector<MetaData::Discriptor> mMetaTable;
        std::vector<std::pair<uint16_t, uint16_t>> mMetaReflectTable;
    };
}
