#pragma once

#include <string>
#include <set>
#include <map>
#include <utility>
#include <unordered_map>
#include <iostream>
#include <shared_mutex>
#include <array>

using Field = std::string;

struct ResponseField {
    Field name1;
    Field name2;
    ResponseField(std::string name1, std::string name2):
        name1(std::move(name1)), name2(std::move(name2))
    {}
};
bool operator==(const ResponseField& lhs, const ResponseField& rhs);
std::ostream& operator<<(std::ostream& out, const ResponseField& field);

using ResponseTable  = std::map<int, ResponseField>;

class Table {
public:
    using table_t = std::map<int, Field>;
public:
    bool insert(int id, std::string name);
    void truncate();
    [[nodiscard]] const table_t& getTable() const {return data_;}
    static ResponseTable intersection(const Table& lhs, const Table& rhs);
    static ResponseTable symmetric_difference(const Table& lhs, const Table& rhs);
private:
    table_t data_;
};

class DataBase {
    using mutex_t = std::shared_mutex; // not to be confused with `shared` word everywhere
public:
    DataBase();
    bool insert(const std::string& table_name, int id, std::string name);
    bool truncate(const std::string& table_name);
    [[nodiscard]] const Table& getTable(const std::string& table_name) const;
    ResponseTable intersection() const;
    ResponseTable symmetric_difference() const;
private:
    static constexpr std::size_t TABLE_COUNT = 2;
    //std::unordered_map<std::string, Table> tables_;
    std::array<Table, TABLE_COUNT> tables_;
    mutable mutex_t mtx_;

    static int nameToTableIdx(const std::string& table_name);
};
