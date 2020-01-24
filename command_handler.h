#pragma once

#include <memory>
#include <string>

#include "command.h"
#include "database.h"



// interface of server response
struct IResponse {
    explicit IResponse(CommandType cmd_type) : cmd_type_(cmd_type) {}
    virtual ~IResponse() = default;
    virtual void parseCommand(CommandHolder input_cmd) = 0;
    virtual std::string runCommand(DataBase& db) = 0;
    CommandType cmd_type_;
};
using ResponseHolder = std::unique_ptr<IResponse>;

ResponseHolder getResponseCommand(CommandHolder input_cmd);


// specialization of server response
struct InsertResponse : public IResponse {
    InsertResponse() : IResponse(CommandType::INSERT) {}
    void parseCommand(CommandHolder input_cmd) override;
    std::string runCommand(DataBase& db) override;
    std::string table_name;
    int id = 0;
    std::string name;
};

struct TruncateResponse : public IResponse {
    TruncateResponse() : IResponse(CommandType::TRUNCATE) {}
    void parseCommand(CommandHolder input_cmd) override;
    std::string runCommand(DataBase& db) override;
    std::string table_name;
};

struct IntersectionResponse : public IResponse {
    IntersectionResponse() : IResponse(CommandType::INTERSECTION) {}
    void parseCommand(CommandHolder input_cmd) override {}; // nothing to do
    std::string runCommand(DataBase& db) override;
};

struct SymmetricDifferenceResponse : public IResponse {
    SymmetricDifferenceResponse() : IResponse(CommandType::SYMMETRIC_DIFFERENCE) {}
    void parseCommand(CommandHolder input_cmd) override {}; // nothing to do
    std::string runCommand(DataBase& db) override;
};

struct UnknownResponse : public IResponse {
    UnknownResponse() : IResponse(CommandType::UNKNOWN) {}
    void parseCommand(CommandHolder input_cmd) override; // nothing to do
    std::string runCommand(DataBase& db) override;
    std::string error_str;
};

