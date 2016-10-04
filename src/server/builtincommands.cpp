#include <consolecolor.h>
#include "server.h"
#include "settings.h"
#include <functional>

void Server::initBuiltinCommands()
{
    m_commands.addCommand("help", { "internal","Help" }, [this](Command cmd)->CommandExecuteStat
    {
        std::string helpString = "\nAvailable commands:\n";
        for (const auto& command : m_commands.getCommandMap())
        {
            helpString += command.first + " - " + command.second.first.author + " : " + command.second.first.help + "\n";
        }
        return{ true, helpString };
    });
    m_commands.addCommand("test.hello", { "internal","Say hello" }, [this](Command cmd)->CommandExecuteStat
    {
        return{ true, "Hello!" };
    });
    m_commands.addCommand("test.log", { "internal","test the logger." }, [this](Command cmd)->CommandExecuteStat
    {
        fatalstream << "execute test.fatal!";
        errorstream << "execute test.error!";
        warningstream << "Your computer will explode in three seconds!!!";
        return{ true , "" };
    });
    m_commands.addCommand("test.rainbow", { "internal","test colors." }, [this](Command cmd)->CommandExecuteStat
    {
        infostream << "Grayscales:";
        infostream << LColor::black << "2333333333 [0%]";
        infostream << LColor::lblack << "2333333333 [50%]";
        infostream << LColor::white << "2333333333 [75%]";
        infostream << LColor::lwhite << "2333333333 [100%]";
        infostream << "Dark colors:";
        infostream << LColor::red << "2333333333 [dark red]";
        infostream << LColor::yellow << "2333333333 [dark yellow]";
        infostream << LColor::green << "2333333333 [dark green]";
        infostream << LColor::cyan << "2333333333 [dark cyan]";
        infostream << LColor::blue << "2333333333 [dark blue]";
        infostream << LColor::magenta << "2333333333 [dark magenta]";
        infostream << "Bright colors:";
        infostream << LColor::lred << "2333333333 [red]";
        infostream << LColor::lyellow << "2333333333 [yellow]";
        infostream << LColor::lgreen << "2333333333 [green]";
        infostream << LColor::lcyan << "2333333333 [cyan]";
        infostream << LColor::lblue << "2333333333 [blue]";
        infostream << LColor::lmagenta << "2333333333 [magenta]";
        return{ true , "" };
    });
    m_commands.addCommand("server.stop", { "internal","Stop the server." }, [this](Command cmd)->CommandExecuteStat
    {
        stop();
        return{ true, "" };
    });
    m_commands.addCommand("conf.get", { "internal","Get one configuration item. Usage: conf.get <confname>" }, [this](Command cmd)->CommandExecuteStat
    {
        if (cmd.args.size() == 1)
        {
            auto keys = split(cmd.args[0], ".");
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
    m_commands.addCommand("conf.show", { "internal","Show the configuration." }, [this](Command cmd)->CommandExecuteStat
    {
        return{ true, getSettings().dump() };
    });
    m_commands.addCommand("conf.save", { "internal","Save the configuration." }, [this](Command cmd)->CommandExecuteStat
    {
        saveSettings();
        return{ true,"Done!" };
    });
    m_commands.addCommand("server.ups", { "internal","Show the ups." }, [this](Command cmd)->CommandExecuteStat
    {
        return{ true,"Ups: " + std::to_string(m_ups.getRate()) };
    });
    m_commands.addCommand("chunks.count", { "internal","Show how many chunks are loaded" }, [this](Command cmd)->CommandExecuteStat
    {
        int sum = 0;
        for (World* world : m_worlds)
        {
            sum += world->getChunkCount();
        }
        return{ true,"Chunks loaded: " + std::to_string(sum) };
    });
}
