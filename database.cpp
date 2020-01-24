#include "database.h"

#include <algorithm>

using namespace std;

bool operator<(const Field& lhs, const Field& rhs) {
    return lhs.id < rhs.id;
}

bool operator==(const Field& lhs, const Field& rhs) {
    return tie(lhs.id, lhs.name) == tie(rhs.id, rhs.name);
}

std::ostream& operator<<(std::ostream& out, const Field& field) {
    out << field.id << " " << field.name;
    return out;
}

bool operator<(const ResponseField& lhs, const ResponseField& rhs) {
    return lhs.id < rhs.id;
}

bool operator==(const ResponseField& lhs, const ResponseField& rhs) {
    return tie(lhs.id, lhs.name1, lhs.name2) == tie(rhs.id, rhs.name1, rhs.name2);
}

std::ostream& operator<<(std::ostream& out, const ResponseField& field) {
    out << field.id << ',' << field.name1 << ',' << field.name2;
    return out;
}

bool Table::insert(int id, const std::string &name) {
    /// Complexity: O(log(N))
    auto [_, res] = data_.emplace(id, name);
    return res;
}

void Table::truncate() {
    /// Complexity: O(N)
    data_.clear();
}

ResponseTable Table::intersection(const Table &lhs, const Table &rhs) {
    /** N1 = lhs.size(), N2 = rh2.size()
     *  Complexity: O(N1 + N2)
    */
    ResponseTable res;
    auto first1 = lhs.data_.begin();
    auto last1 = lhs.data_.end();
    auto first2 = rhs.data_.begin();
    auto last2 = rhs.data_.end();
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2)
            ++first1;
        else if (*first2 < *first1)
            ++first2;
        else {
            res.emplace_hint(res.end(), first1->id, first1->name, first2->name);
            ++first1;
            ++first2;
        }
    }
    return res;
}

ResponseTable Table::symmetric_difference(const Table &lhs, const Table &rhs) {
    /** N1 = lhs.size(), N2 = rh2.size()
     *  Complexity: O(N1 + N2)
    */
    ResponseTable res;
    auto first1 = lhs.data_.begin();
    auto last1 = lhs.data_.end();
    auto first2 = rhs.data_.begin();
    auto last2 = rhs.data_.end();
    while (first1 != last1 && first2 != last2) {
        if (*first1 < *first2) {
            res.emplace_hint(res.end(), first1->id, first1->name, "");
            ++first1;
        } else if (*first2 < *first1) {
            res.emplace_hint(res.end(), first2->id, "", first2->name);
            ++first2;
        } else {
            ++first1;
            ++first2;
        }
    }
    for ( ; first2 != last2; ++first2) {
        res.emplace_hint(res.end(), first2->id, "", first2->name);
    }
    return res;
}

void DataBase::createTable(std::string table_name) {
    // shared_lock is used because of "No iterators or references are invalidated" for un_map::try_emplace
    shared_lock<mutex_t> lk(mtx_);
    tables_.try_emplace(std::move(table_name), Table());
}

bool DataBase::insert(const std::string &table_name, int id, const std::string &name) {
    // shared_lock is used because of "No iterators or references are invalidated" for set::emplace
    shared_lock<mutex_t> lk(mtx_);
    if (tables_.count(table_name)) {
        auto& table = tables_[table_name];
        return table.insert(id, name);
    }
    return false;
}

bool DataBase::truncate(const std::string &table_name) {
    unique_lock<mutex_t> lk(mtx_);
    if (tables_.count(table_name)) {
        auto &table = tables_[table_name];
        table.truncate();
        return true;
    }
    return false;
}

ResponseTable DataBase::intersection() const {
    shared_lock<mutex_t> lk(mtx_);
    auto& table1 = tables_.at("A");
    auto& table2 = tables_.at("B");
    return Table::intersection(table1, table2);
}

ResponseTable DataBase::symmetric_difference() const {
    shared_lock<mutex_t> lk(mtx_);
    auto& table1 = tables_.at("A");
    auto& table2 = tables_.at("B");
    return Table::symmetric_difference(table1, table2);
}

const Table& DataBase::getTable(const std::string &table_name) const {
    return tables_.at(table_name);
}

DataBase::DataBase() {
    createTable("A");
    createTable("B");
}


