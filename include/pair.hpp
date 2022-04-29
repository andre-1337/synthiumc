#pragma once

#include "common.hpp"

namespace Synthium {
    template <typename K, typename V> class Pair {
        private: K _key;
        private: V _value;

        public: inline Pair(K key, V value) : _key(key), _value(value) {}

        public: inline K get_key() {
            return this->_key;
        }

        public: inline V get_value() {
            return this->_value;
        }
    };
}
