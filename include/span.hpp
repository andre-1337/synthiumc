#pragma once

#include "common.hpp"

namespace Synthium {
    class Span {
        private: string _start;
        private: string _end;

        public: inline Span(string start, string end) : _start(start), _end(end) {}

        public: inline Span() : _start(nullptr), _end(nullptr) {}

        public: inline string get_start() {
            return this->_start;
        }

        public: inline string get_end() {
            return this->_end;
        }

        public: inline Span merge(Span first, Span second) {
            string start = first.get_start();
            string end = second.get_end();

            if (second.get_start() < start) {
                start = second.get_start();
            }

            if (first.get_end() > end) {
                end = first.get_end();
            }

            return Span(start, end);
        }

        public: inline Span merge(Span other) {
            return merge(*this, other);
        }

        public: inline usize size() {
            return ((usize) (this->get_end().size() - this->get_start().size()));
        }
    };
}
