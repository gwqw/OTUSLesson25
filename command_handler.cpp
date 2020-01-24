#include "command_handler.h"

#include <utility>
#include <sstream>

using namespace std;

std::string printResponseTable(const ResponseTable& rt) {
    stringstream ss;
    for (const auto& [id, r] : rt) {
        ss << id << ',' << r << '\n';
    }
    return ss.str();
}

void InsertResponse::parseCommand(CommandHolder input_cmd) {
    auto& cmd = static_cast<InsertCommand&>(*input_cmd);
    table_name = move(cmd.table_name);
    id = cmd.id;
    name = move(cmd.name);
}

std::string InsertResponse::runCommand(DataBase &db) {
    auto res = db.insert(table_name, id, move(name));
    if (res) {
        return "OK\n"s;
    } else {
        return "ERR duplicate " + to_string(id) + '\n';
    }
}

void TruncateResponse::parseCommand(CommandHolder input_cmd) {
    auto& cmd = static_cast<TruncateCommand&>(*input_cmd);
    table_name = move(cmd.table_name);
}

std::string TruncateResponse::runCommand(DataBase &db) {
    auto res = db.truncate(table_name);
    return res ? "OK\n" : "No such table" + table_name + '\n';
}

std::string IntersectionResponse::runCommand(DataBase &db) {
    auto res = db.intersection();
    return printResponseTable(res) + "OK\n";
}

std::string SymmetricDifferenceResponse::runCommand(DataBase &db) {
    auto res = db.symmetric_difference();
    return printResponseTable(res) + "OK\n";
}

void UnknownResponse::parseCommand(CommandHolder input_cmd) {
    auto& cmd = static_cast<UnknownCommand&>(*input_cmd);
    error_str = move(cmd.error_str);
}

std::string UnknownResponse::runCommand(DataBase &db) {
    return "ERR " + error_str + '\n';
}

ResponseHolder getResponseCommand(CommandHolder input_cmd) {
    ResponseHolder responseHolder;
    switch (input_cmd->com_type_) {
        case CommandType::INSERT:
            responseHolder = make_unique<InsertResponse>();
            break;
        case CommandType::TRUNCATE:
            responseHolder = make_unique<TruncateResponse>();
            break;
        case CommandType::INTERSECTION:
            responseHolder = make_unique<IntersectionResponse>();
            break;
        case CommandType::SYMMETRIC_DIFFERENCE:
            responseHolder = make_unique<SymmetricDifferenceResponse>();
            break;
        case CommandType::UNKNOWN:
            responseHolder = make_unique<UnknownResponse>();
            break;
        default:
            throw logic_error("Unrealized cmd");
    }
    if (responseHolder) {
        responseHolder->parseCommand(move(input_cmd));
    }
    return responseHolder;
}

