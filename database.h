#pragma once

#include <string>
#include <set>
#include <map>
#include <utility>
#include <unordered_map>
#include <iostream>
#include <shared_mutex>

struct Field {
    int id = 0;
    std::string name;
    Field(int id, std::string name) : id(id), name(std::move(name)) {}
};
bool operator<(const Field& lhs, const Field& rhs);
bool operator==(const Field& lhs, const Field& rhs);
std::ostream& operator<<(std::ostream& out, const Field& field);

struct ResponseField {
    int id;
    std::string name1;
    std::string name2;
    ResponseField(int id, std::string name1, std::string name2):
        id(id), name1(std::move(name1)), name2(std::move(name2))
    {}
};
bool operator<(const ResponseField& lhs, const ResponseField& rhs);
bool operator==(const ResponseField& lhs, const ResponseField& rhs);
std::ostream& operator<<(std::ostream& out, const ResponseField& field);

using ResponseTable  = std::set<ResponseField>;

class Table {
public:
    bool insert(int id, const std::string& name);
    void truncate();
    [[nodiscard]] const std::set<Field>& getTable() const {return data_;}
    static ResponseTable intersection(const Table& lhs, const Table& rhs);
    static ResponseTable symmetric_difference(const Table& lhs, const Table& rhs);
private:
    std::set<Field> data_;
};

class DataBase {
    using mutex_t = std::shared_mutex; // not to be confused with `shared` word everywhere
public:
    DataBase();
    void createTable(std::string table_name);
    bool insert(const std::string& table_name, int id, const std::string& name);
    bool truncate(const std::string& table_name);
    [[nodiscard]] const Table& getTable(const std::string& table_name) const;
    ResponseTable intersection() const;
    ResponseTable symmetric_difference() const;
private:
    std::unordered_map<std::string, Table> tables_;
    mutable mutex_t mtx_;
};
