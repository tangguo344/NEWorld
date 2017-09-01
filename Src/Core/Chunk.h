//
//  Chunk.h
//  NWCore
//
//  Created by User on 2017/9/1.
//  Copyright © 2017年 Infinideastudio. All rights reserved.
//

#pragma once

#include "Block.h"

typedef void* ChunkHandler;

#define nwChunkPackCrood(x, y, z) (x << 8 | y << 4 | z)

NWCORE_API ChunkHandler nwCreateChunk();
NWCORE_API void nwDestroyChunk(ChunkHandler);

NWCORE_API void nwChunkAcquireModify(ChunkHandler);
NWCORE_API BlockData nwChunkGetBlock(ChunkHandler, int id);
NWCORE_API void nwChunkSetBlock(ChunkHandler, int id, BlockData data);
NWCORE_API void nwChunkReleaseModify(ChunkHandler);

NWCORE_API void nwChunkFreeMetaData(ChunkHandler, uint16_t pos);
NWCORE_API bool nwChunkReadMetaData(ChunkHandler, uint16_t pos, void* mem);
NWCORE_API void nwChunkWriteMetaData(ChunkHandler, uint16_t pos, const void* mem, uint16_t size);
