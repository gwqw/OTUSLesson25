#pragma once

#include <vector>
#include <string>

#include "command.h"

class CommandParser {
public:
    using Commands = std::vector<CommandHolder>;
public:
    Commands parseCommands(char* data, std::size_t data_size);
private:
    std::string buffer_;
};
