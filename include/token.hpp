#pragma once

#include "common.hpp"

namespace Synthium {
    class Token {
        public: enum class Type : u8 {

        };

        public: u64 line;
        public: u64 column;
        public: Type type;
        public: string lexeme;

        
    };
}