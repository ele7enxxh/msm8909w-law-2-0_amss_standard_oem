/*
 * Copyright (c) 2016 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */
extern "C" {
#include <inttypes.h>
#include <stdlib.h>
}
#include "ConsoleSource.h"
#include "CommandSource.h"
#include "MainLoop.h"
#include "Logger.h"
#include "Command.h"
#include <sstream>
#include <string>
#include <iostream>

using namespace QcSettingsD;

ConsoleSource::ConsoleSource(): CommandSource()
{
    init();
}

ConsoleSource::ConsoleSource(const std::string &tag): CommandSource(), tag(tag)
{
    init();
}

void ConsoleSource::onErrEvent(int fd)
{
    LOG_HIGH("Error in fd %d: %p. errno: %d", fd, this, errno);
}

#define TRIM_CHARS "\r\n\t\v "
void trim(std::string &line)
{
    LOG_ENTER("(line: %s)", line.c_str());
    line.erase(0, line.find_first_not_of(TRIM_CHARS));
    line.erase(line.find_last_not_of(TRIM_CHARS) + 1, line.size());
    LOG_LEAVE("line: %s", line.c_str());
}

std::shared_ptr<CommandParameters> getParams(const std::string &line, std::string &name)
{
    LOG_ENTER("(line: %s. name: %s)", line.c_str(), name.c_str());
    std::string param_value;
    std::shared_ptr<CommandParameters> params = std::make_shared<CommandParameters>();
    std::istringstream l(line);
    std::getline(l, name, ' ');
    trim(name);

    do
    {
        param_value.clear();
        std::getline(l, param_value, ' ');
        trim(param_value);
        params->addParam(param_value);
    } while(!param_value.empty());
    LOG_LEAVE("(name: %s)", name.c_str());
    return params;
}

bool ConsoleSource::isCommandAllowed(const Command &cmd)
{
    return (1 << ((uint32_t) cmd.getCategory())) & allowed_categories;
}

void ConsoleSource::onReadEvent(int fd)
{
    char line[256] = {0};
    ssize_t bytes = -1;
    bool rc = false;
    std::string cmdName;
    CommandResults results;

    LOG_ENTER("%p", this);
    bytes = read(fd, line, sizeof(line));
    if (bytes <= 0) {
        LOG_DEBUG("%p: closing fd %d(%d). bytes: %d", this, fd, errno, bytes);
        close(fd);
        delFd(fd);
    }
    std::shared_ptr<CommandParameters> pars = getParams(line, cmdName);
    if (!cmdName.empty())
    {
        std::shared_ptr<Command> cmd = CommandRegistry::findCommand(cmdName);
        if (cmd && isCommandAllowed(*cmd))
        {
            LOG_DEBUG("New Command. Name: <%s>", cmdName.c_str());
            rc = cmd->run(*pars, results);
            if (!rc)
            {
                std::cerr << "Error executing command " << cmdName << std::endl;
            }
            for( const CommandParameter &param: results.getParamList())
            {
                std::cout << param.getString() << std::endl;
            }
        }
        else
        {
            LOG_HIGH("Unknown command %s", cmdName.c_str());
        }
    }
    LOG_LEAVE("");
}

void ConsoleSource::setAllowedCategories(uint32_t cats)
{
    allowed_categories = cats;
}
void ConsoleSource::init()
{
    LOG_DEBUG("Enabling console");
    addFd("stdin", 0);
#ifndef __ANDROID__
    allowed_categories =
            1 << ((uint32_t)Command::CMD_CAT_EXTERNAL) |
            1 << ((uint32_t)Command::CMD_CAT_INTERNAL);
#else
    allowed_categories =
            1 << ((uint32_t)Command::CMD_CAT_EXTERNAL);
#endif
    std::shared_ptr<Command> commands[] =
    {
        ActionCmd::make_cmd("help", "Show the list of commands.\n",
            "help        - Show the list of commands.\n"
            "help remote - Show the list of remote commands.\n"
            "help <cmd>  - Provide detailed information about <cmd>\n",
            Command::CMD_CAT_EXTERNAL,
            Command::CMD_EXEC_SYNC,
            [this](const std::string *id, std::shared_ptr<Command> cmd, const CommandParameters &pars, CommandResults &results) -> bool {
                (void)id;
                (void)cmd;
                unsigned i = 0;
                bool ret = false;
                std::shared_ptr<Command> command;
                std::string registry = "default";
                if (pars[i].getString() == std::string("remote"))
                {
                    registry = "remote";
                    i++;
                }
                if (pars.size() > i && !pars[i].getString().empty())
                {
                    if ( (command = CommandRegistry::findCommand(registry, pars[i].getString())) && isCommandAllowed(*command) )
                    {
                        results.addParam(command->getName() + ": " + command->getDescription());
                        results.addParam(command->getLongDescription());
                        ret = true;
                    }
                    else
                    {
                        results.addParam("Unknown command '" + pars[i].getString() + "'");
                    }
                }
                else
                {
                    CommandRegistry::for_each(registry,
                        [this, &results](const std::string &name, std::shared_ptr<Command> cmd) -> bool {
                            if (cmd && isCommandAllowed(*cmd))
                            {
                                results.addParam(name + ": " + cmd->getDescription());
                            }
                            return true;
                        }
                    );
                    ret = true;
                }
                return ret;
            }
        ),
        ActionCmd::make_cmd("exit", "Exit the console session.","",
            Command::CMD_CAT_EXTERNAL,
            Command::CMD_EXEC_SYNC,
            [this](const std::string *id, std::shared_ptr<Command> UNUSED(command), CommandParameters pars, CommandResults results) -> bool {
                (void)id;
                (void)pars;
                (void)results;
                MainLoop::endLoop();
                return true;
            }
        ),
        ActionCmd::make_cmd("set_allowed_categories", "Set the allowed command categories","",
            Command::CMD_CAT_INTERNAL,
            Command::CMD_EXEC_SYNC,
            [this](const std::string *id, std::shared_ptr<Command> UNUSED(command), const CommandParameters &pars, CommandResults results) -> bool {
                (void)id;
                (void)pars;
                (void)results;
                bool ret = false;
                uint32_t cats;

                if (pars.size() >= 1)
                {
                    cats = pars[0].getLong();
                    LOG_DEBUG("Received new category %" PRIu32, cats);
                    setAllowedCategories(cats);
                    ret = true;
                }
                return ret;
            }
        ),
        ActionCmd::make_cmd("remote", "Execute a remote command","",
            Command::CMD_CAT_EXTERNAL,
            Command::CMD_EXEC_SYNC,
            [this](const std::string *UNUSED(id), std::shared_ptr<Command> command, const CommandParameters &pars, CommandResults UNUSED(results)) -> bool {
                (void)command;
                bool ret = false;

                if (pars.size() >= 1)
                {
                    std::string cmd_name = pars[0].getString();
                    std::shared_ptr<Command> cmd = CommandRegistry::findCommand(REMOTE_REGISTRY, cmd_name);
                    LOG_DEBUG("cmd: %s", cmd ? "true" : "false");
                    if (cmd)
                    {
                        CommandParameters newpar(pars.size() - 1);
                        std::copy(pars.begin() + 1, pars.end(), newpar.begin());
                        cmd->enqueue(newpar, *new CommandResults(), [](bool UNUSED(res),
                                    Command::event_type UNUSED(evt_type),
                                    const std::shared_ptr<Command> UNUSED(ptr),
                                    const CommandParameters &UNUSED(pars),
                                    CommandResults &UNUSED(incoming_results)) {
                            LOG_DEBUG("Execution of remote completed");
                        });
                        ret = true;
                    }
                }
                return ret;
            }
        ),
    };

    for(std::shared_ptr<Command> cmd: commands)
    {
        CommandRegistry::registerCommand(cmd->getName(), cmd);
    }
}
