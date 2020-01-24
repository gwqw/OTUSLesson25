#include "command.h"

#include <stdexcept>
#include <unordered_map>

#include "str_utils.h"

using namespace std;

const unordered_map<string, CommandType> GET_COMM_TYPE_FROM_STR = {
        {"INSERT", CommandType::INSERT},
        {"TRUNCATE", CommandType::TRUNCATE},
        {"INTERSECTION", CommandType::INTERSECTION},
        {"SYMMETRIC_DIFFERENCE", CommandType::SYMMETRIC_DIFFERENCE},
};

CommandHolder getCommandFromType(CommandType command_type) {
    switch (command_type) {
        case CommandType::INSERT:
            return make_unique<InsertCommand>();
        case CommandType::TRUNCATE:
            return make_unique<TruncateCommand>();
        case CommandType::INTERSECTION:
            return make_unique<IntersectionCommand>();
        case CommandType::SYMMETRIC_DIFFERENCE:
            return make_unique<SymmetricDifferenceCommand>();
        default:
            return nullptr;
    }
}

void InsertCommand::parseFrom(std::string_view comm_str) {
    // Format: INSERT table id name (INSERT is already read)
    auto token = read_token(comm_str, " ");
    if (token.empty())
        throw invalid_argument("incorrect arguments for insert: no table_name, id, name");
    table_name = string(token);
    token = read_token(comm_str, " ");
    if (token.empty())
        throw invalid_argument("incorrect arguments for insert: no id and name");
    id = convertToInt(token);
    token = read_token(comm_str, " ");
    if (token.empty())
        throw invalid_argument("incorrect arguments for insert: no name");
    name = string(token);
}

void TruncateCommand::parseFrom(std::string_view comm_str) {
    // Format: TRUNCATE table (TRUNCATE is already read)
    auto token = read_token(comm_str, " ");
    if (token.empty())
        throw invalid_argument("incorrect arguments for insert: " + string(comm_str));
    table_name = string(token);
}

void UnknownCommand::parseFrom(std::string_view comm_str) {
    error_str = comm_str;
}

CommandType getCommandType(std::string_view s) {
    if (auto it = GET_COMM_TYPE_FROM_STR.find(string(s)); it != end(GET_COMM_TYPE_FROM_STR)) {
        return it->second;
    } else {
        throw invalid_argument("unknown error_str type " + string(s));
    }
}

CommandHolder parseCommandFrom(std::string_view sv) {
    try {
        auto token = read_token(sv, " ");
        CommandHolder command = getCommandFromType(getCommandType(token));
        if (command) {
            command->parseFrom(sv);
        }
        return command;
    } catch (const exception& e) {
        CommandHolder command = make_unique<UnknownCommand>();
        command->parseFrom(e.what());
        return command;
    }
}

