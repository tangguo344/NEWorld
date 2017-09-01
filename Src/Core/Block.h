//
//  Block.h
//  NWCore
//
//  Created by User on 2017/9/1.
//  Copyright © 2017年 Infinideastudio. All rights reserved.
//

#pragma once

#include "Config.h"
#include <stdint.h>

struct BlockData {
    unsigned int id : 12;
    uint8_t illuminance : 4;
    union {
        struct {
            union {
                struct {
                    uint8_t hex0 : 4;
                    uint8_t hex1 : 4;
                };
                uint8_t byte0;
            };
            union {
                struct {
                    uint8_t hex2 : 4;
                    uint8_t hex3 : 4;
                };
                uint8_t byte1;
            };
        };
        uint16_t word;
    };
};

NWCORE_API BlockData nwBlockDataH4(int, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);
NWCORE_API BlockData nwBlockDataH2B1(int, uint8_t, uint8_t, uint8_t, uint8_t);
NWCORE_API BlockData nwBlockDataB2(int, uint8_t, uint8_t, uint8_t);NWCORE_API BlockData nwBlockDataW1(int, uint8_t, uint16_t);

