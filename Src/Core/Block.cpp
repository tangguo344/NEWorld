//
//  Block.cpp
//  NWCore
//
//  Created by User on 2017/9/1.
//  Copyright © 2017年 Infinideastudio. All rights reserved.
//

#include "Block.h"

namespace {
    struct Helper {
        uint32_t d;
        BlockData r;
        Helper(int i) : d(i) {}
    };
}

BlockData nwBlockDataH4(int id, uint8_t b, uint8_t h0, uint8_t h1, uint8_t h2, uint8_t h3) {
    return Helper((id << 20) | (b << 16) | (h0 << 12) | (h1 << 8) | (h2 << 4) | h3).r;
}

BlockData nwBlockDataH2B1(int id, uint8_t b, uint8_t h0, uint8_t h1, uint8_t b0) {
    return Helper((id << 20) | (b << 16) | (h0 << 12) | (h1 << 8) | b0).r;
}

BlockData nwBlockDataB2(int id, uint8_t b, uint8_t b0, uint8_t b1) {
    return Helper((id << 20) | (b << 16) | (b0 << 8) | b1).r;
}

BlockData nwBlockDataW1(int id, uint8_t b, uint16_t w0) {
    return Helper((id << 20) | (b << 16) | w0).r;
}
