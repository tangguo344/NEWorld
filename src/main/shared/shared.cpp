#include "shared.h"
int32_t RockID;

void registerBlocks()
{
    NWblocktype rock;
    rock.blockname = "Rock";
    rock.solid = 1;
    rock.translucent = 0;
    rock.opaque = 1;
    rock.explodePower = 0;
    rock.hardness = 2;
    RockID = nwRegisterBlock(&rock);
}

NWplugindata* getInfo(bool client)
{
    auto plugin = new NWplugindata();
    plugin->pluginName = "NEWorld";
    plugin->authorName = "INFINIDEAS";
    plugin->internalName = client ? "infinideas.neworld.client" : "infinideas.neworld.server";
    plugin->isClientPlugin = client;
    return plugin;
}

void sharedInit()
{
    registerBlocks();
}
