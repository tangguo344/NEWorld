#include <common/consolecolor.h>
#include "server.h"
#include <common/jsonhelper.h>
#include <functional>

void Server::initBuiltinCommands()
{
    mCommands.registerCommand("help", { "internal","Help" }, [this](Command cmd)->CommandExecuteStat
    {
        std::string helpString = "\nAvailable commands:\n";
        for (const auto& command : mCommands.getCommandMap())
        {
            helpString += command.first + " - " + command.second.first.author + " : " + command.second.first.help + "\n";
        }
        return{ true, helpString };
    });
    mCommands.registerCommand("test.hello", { "internal","Say hello" }, [this](Command cmd)->CommandExecuteStat
    {
        return{ true, "Hello!" };
    });
    mCommands.registerCommand("test.log", { "internal","test the logger." }, [this](Command cmd)->CommandExecuteStat
    {
        fatalstream << "execute test.fatal!";
        errorstream << "execute test.error!";
        warningstream << "Your computer will explode in three seconds!!!";
        return{ true , "" };
    });
    mCommands.registerCommand("test.rainbow", { "internal","test colors." }, [this](Command cmd)->CommandExecuteStat
    {
        verbosestream << "Grayscales:";
        verbosestream << LColor::black << "2333333333 [0%]";
        verbosestream << LColor::lblack << "2333333333 [50%]";
        verbosestream << LColor::white << "2333333333 [75%]";
        verbosestream << LColor::lwhite << "2333333333 [100%]";
        verbosestream << "Dark colors:";
        verbosestream << LColor::red << "2333333333 [dark red]";
        verbosestream << LColor::yellow << "2333333333 [dark yellow]";
        verbosestream << LColor::green << "2333333333 [dark green]";
        verbosestream << LColor::cyan << "2333333333 [dark cyan]";
        verbosestream << LColor::blue << "2333333333 [dark blue]";
        verbosestream << LColor::magenta << "2333333333 [dark magenta]";
        verbosestream << "Bright colors:";
        verbosestream << LColor::lred << "2333333333 [red]";
        verbosestream << LColor::lyellow << "2333333333 [yellow]";
        verbosestream << LColor::lgreen << "2333333333 [green]";
        verbosestream << LColor::lcyan << "2333333333 [cyan]";
        verbosestream << LColor::lblue << "2333333333 [blue]";
        verbosestream << LColor::lmagenta << "2333333333 [magenta]";
        return{ true , "" };
    });
    mCommands.registerCommand("server.stop", { "internal","Stop the server." }, [this](Command cmd)->CommandExecuteStat
    {
        stop();
        return{ true, "" };
    });
    mCommands.registerCommand("conf.get", { "internal","Get one configuration item. Usage: conf.get <confname>" }, [this](Command cmd)->CommandExecuteStat
    {
        if (cmd.args.size() == 1)
        {
            auto keys = split(cmd.args[0], '.');
            Json now = getSettings();
            bool exist = true;
            for (auto key : keys)
            {
                auto iter = now.find(key);
                if (iter == now.end())
                {
                    exist = false;
                    break;
                }
                now = iter.value();
            }

            if (exist)
                return{ true, cmd.args[0] + " = " + now.dump() };
            else
                return{ false, "The configuration item does not exist." };
        }
        else
        {
            return{ false, "Usage: conf.get <confname>" };
        }
    });
    mCommands.registerCommand("conf.show", { "internal","Show the configuration." }, [this](Command cmd)->CommandExecuteStat
    {
        return{ true, getSettings().dump() };
    });
    mCommands.registerCommand("conf.save", { "internal","Save the configuration." }, [this](Command cmd)->CommandExecuteStat
    {
        writeJsonToFile(SettingsFilename, getSettings());
        return{ true,"Done!" };
    });
    mCommands.registerCommand("server.ups", { "internal","Show the ups." }, [this](Command cmd)->CommandExecuteStat
    {
        // TODO: Add UPS counter for server
        return{ true,"[Server UPS counter not finished yet!]" };
    });
    mCommands.registerCommand("chunks.count", { "internal","Show how many chunks are loaded" }, [this](Command cmd)->CommandExecuteStat
    {
        size_t sum = 0;
        for (World* world : mWorlds)
        {
            sum += world->getChunkCount();
        }
        return{ true,"Chunks loaded: " + std::to_string(sum) };
    });
}
