#pragma once

#include <vector>
#include <string>

template<typename K, typename V>
class LookupTable {
public:
    V& operator[](const K& key) {
        return this->at(key);
    }

    const V& operator[](const K& key) const {
        return this->at(key);
    }

    std::pair<K, V>& operator[](const size_t index) {
        return this->at(index);
    }

    const std::pair<K, V>& operator[](const size_t index) const {
        return this->at(index);
    }

    LookupTable() { }

    LookupTable(const std::vector<std::pair<K, V>>& table) : m_table(table) { }

    std::vector<std::pair<K, V>>::iterator begin() {
        return m_table.begin();
    }

    std::vector<std::pair<K, V>>::const_iterator begin() const {
        return m_table.begin();
    }

    std::vector<std::pair<K, V>>::iterator end() {
        return m_table.end();
    }

    std::vector<std::pair<K, V>>::const_iterator end() const {
        return m_table.end();
    }

    size_t size() const {
        return m_table.size();
    }

    V& at(const K& key) {
        for (auto& [first, second] : m_table) {
            if (first == key) {
                return second;
            }
        }

        throw std::out_of_range("Key not found in LookupTable");
    }

    const V& at(const K& key) const {
        for (const auto& [first, second] : m_table) {
            if (first == key) {
                return second;
            }
        }

        throw std::out_of_range("Key not found in LookupTable");
    }

    std::pair<K, V>& at(const size_t index) {
        return m_table.at(index);
    }

    const std::pair<K, V>& at(const size_t index) const {
        return m_table.at(index);
    }

    std::vector<K> keys() const {
        std::vector<K> keys;

        for (const auto& [first, _] : m_table) {
            keys.push_back(first);
        }

        return keys;
    }

    std::vector<V> values() const {
        std::vector<V> values;

        for (const auto& [_, second] : m_table) {
            values.push_back(second);
        }

        return values;
    }

    std::vector<std::pair<K, V>> pairs() const {
        return m_table;
    }

    bool contains(const K& key) const {
        for (const auto& [first, _] : m_table) {
            if (first == key) {
                return true;
            }
        }
        return false;
    }

    void insert(const std::pair<K, V>& pair) {
        this->erase(pair.first);
        m_table.push_back(pair);
    }

    void insert(const K& key, const V& value) {
        this->erase(key);
        m_table.push_back(std::make_pair(key, value));
    }

    void erase(const K& key) {
        for (size_t i = 0; i < m_table.size(); i++) {
            if (m_table.at(i).first == key) {
                m_table.erase(m_table.begin() + i);
                return;
            }
        }
    }

    void clear() {
        m_table.clear();
    }
private:
    std::vector<std::pair<K, V>> m_table;       
};