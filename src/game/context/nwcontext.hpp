#pragma once
#include "./../plugin/pluginmanager.h"
#include "./../world/nwblock.h"

struct GameContext
{
    BlockManager blocks;
   PluginManager plugins;
};

NWCOREAPI extern GameContext context;
