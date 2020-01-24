#include "database.h"

#include <algorithm>

using namespace std;

bool operator==(const ResponseField& lhs, const ResponseField& rhs) {
    return tie(lhs.name1, lhs.name2) == tie(rhs.name1, rhs.name2);
}

std::ostream& operator<<(std::ostream& out, const ResponseField& field) {
    out << field.name1 << ',' << field.name2;
    return out;
}

bool Table::insert(int id, std::string name) {
    /// Complexity: O(log(N))
    auto [_, res] = data_.try_emplace(id, move(name));
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
        if (first1->first < first2->first)
            ++first1;
        else if (first2->first < first1->first)
            ++first2;
        else {
            res.try_emplace(res.end(), first1->first, first1->second, first2->second);
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
        if (first1->first < first2->first) {
            res.try_emplace(res.end(), first1->first, first1->second, string{});
            ++first1;
        } else if (first2->first < first1->first) {
            res.try_emplace(res.end(), first2->first, string{}, first2->second);
            ++first2;
        } else {
            ++first1;
            ++first2;
        }
    }
    for ( ; first2 != last2; ++first2) {
        res.try_emplace(res.end(), first2->first, string{}, first2->second);
    }
    return res;
}

DataBase::DataBase() {
    tables_[0] = Table();
    tables_[1] = Table();
}

bool DataBase::insert(const std::string &table_name, int id, std::string name) {
    // shared_lock is used because of "No iterators or references are invalidated" for set::emplace
    auto t_idx = nameToTableIdx(table_name);
    if (t_idx != -1) {
        shared_lock<mutex_t> lk(mtx_);
        return tables_[t_idx].insert(id, move(name));
    }
    return false;
}

bool DataBase::truncate(const std::string &table_name) {
    auto t_idx = nameToTableIdx(table_name);
    if (t_idx != -1) {
        unique_lock<mutex_t> lk(mtx_);
        tables_[t_idx].truncate();
        return true;
    }
    return false;
}

ResponseTable DataBase::intersection() const {
    auto& table1 = tables_[0];
    auto& table2 = tables_[1];
    shared_lock<mutex_t> lk(mtx_);
    return Table::intersection(table1, table2);
}

ResponseTable DataBase::symmetric_difference() const {
    auto& table1 = tables_[0];
    auto& table2 = tables_[1];
    shared_lock<mutex_t> lk(mtx_);
    return Table::symmetric_difference(table1, table2);
}

const Table& DataBase::getTable(const std::string &table_name) const {
    return tables_.at(nameToTableIdx(table_name));
}

int DataBase::nameToTableIdx(const std::string& table_name) {
    if (table_name == "A") {
        return 0;
    } else if (table_name == "B") {
        return 1;
    } else {
        return -1;
    }
}




