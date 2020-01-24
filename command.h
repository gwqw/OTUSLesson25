#pragma once

#include <string>
#include <string_view>
#include <memory>

enum class CommandType {
    INSERT, TRUNCATE, INTERSECTION, SYMMETRIC_DIFFERENCE, UNKNOWN
};

/* Format:
INSERT table id name
TRUNCATE table
INTERSECTION
SYMMETRIC_DIFFERENCE
 */

// interface of input commands
struct CommonCommand {
    explicit CommonCommand(CommandType com_type) : com_type_(com_type) {}
    virtual void parseFrom(std::string_view comm_str) = 0;
    virtual ~CommonCommand() = default;
    CommandType com_type_;
};
std::ostream& operator<<(std::ostream& out, const CommonCommand& cmd);
using CommandHolder = std::unique_ptr<CommonCommand>;

CommandHolder parseCommandFrom(std::string_view s);

// specialization of input commands
struct InsertCommand : public CommonCommand {
    InsertCommand() : CommonCommand(CommandType::INSERT) {}
    void parseFrom(std::string_view comm_str) override;
    std::string table_name;
    int id = 0;
    std::string name;
};

struct TruncateCommand : public CommonCommand {
    TruncateCommand() : CommonCommand(CommandType::TRUNCATE) {}
    void parseFrom(std::string_view comm_str) override;
    std::string table_name;
};

struct IntersectionCommand : public CommonCommand {
    IntersectionCommand() : CommonCommand(CommandType::INTERSECTION) {}
    void parseFrom(std::string_view comm_str) override {}
};

struct SymmetricDifferenceCommand : public CommonCommand {
    SymmetricDifferenceCommand()
        : CommonCommand(CommandType::SYMMETRIC_DIFFERENCE) {}
    void parseFrom(std::string_view comm_str) override {}
};

struct UnknownCommand  : public CommonCommand {
    UnknownCommand() : CommonCommand(CommandType::UNKNOWN) {}
    void parseFrom(std::string_view comm_str) override;
    std::string error_str;
};