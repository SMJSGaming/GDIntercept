#pragma once

#include <vector>
#include <string>

namespace proxy {
    template<typename K, typename V>
    class LookupTable {
    public:
        V& operator[](const K& key) {
            return this->at(key);
        }

        const V& operator[](const K& key) const {
            return this->at(key);
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
            return this->at(key);
        }

        const V& at(const K& key) const {
            for (const auto& pair : m_table) {
                if (pair.first == key) {
                    return pair.second;
                }
            }

            throw std::out_of_range("Key not found in LookupTable");
        }

        std::vector<K> keys() const {
            std::vector<K> keys;
            for (const auto& pair : m_table) {
                keys.push_back(pair.first);
            }
            return keys;
        }

        std::vector<V> values() const {
            std::vector<V> values;
            for (const auto& pair : m_table) {
                values.push_back(pair.second);
            }
            return values;
        }

        std::vector<std::pair<K, V>> pairs() const {
            return m_table;
        }

        bool contains(const K& key) const {
            for (const auto& pair : m_table) {
                if (pair.first == key) {
                    return true;
                }
            }
            return false;
        }

        void insert(const std::pair<K, V>& pair) {
            m_table.push_back(pair);
        }

        void insert(const K& key, const V& value) {
            m_table.push_back(std::make_pair(key, value));
        }

        void erase(const K& key) {
            for (auto it = m_table.begin(); it != m_table.end(); ++it) {
                if (it->first == key) {
                    m_table.erase(it);

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
}