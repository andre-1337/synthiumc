#pragma once

#include "span.hpp"
#include "common.hpp"

namespace Synthium {
    class Token {
        public: enum class Type : u8 {
            UnknownErr,
            CharErr,
            EscapeErr,
            InvalidIdentifierErr,

            Int,
            Float,
            Null,
            Ident,

            Let,
            If,
            Else,
            Import,
            Fn,
            While,
            New,
            Delete,
            Return,
            Type,
            Struct,
            Enum,
            Union,
            As,
            Extern,
            Export,
            Sizeof,
            Typeof,
            For,
            Undefined,
            Continue,
            Break,

            Semi,
            Comma,
            Colon,
            Dot,
            TripleDot,
            Bang,
            BangEq,
            DoubleEq,

            Eq,
            PlusEq,
            MinusEq,
            StarEq,
            SlashEq,
            PercentEq,
            AmpersandEq,
            PipeEq,
            CaretEq,

            Smaller,
            SmallerEq,
            Greater,
            GreaterEq,

            Ampersand,
            DoubleAmpersand,
            Pipe,
            DoublePipe,
            Caret,
            Tilde,

            SmallerSmaller,
            GreaterGreater,

            LParen,
            RParen,
            LBrace,
            RBrace,
            LBracket,
            RBracket,
            String,
            Char,

            Eof
        };

        private: Span _span;
        private: Type _type;

        public: Token() : _type(Type::UnknownErr), _span(Synthium::Span()) {}

        public: inline Span get_span() {
            return this->_span;
        }

        public: inline Type get_type() {
            return this->_type;
        }

        public: string get_lexeme() {
            return this->_span.get_start();
        }

        public: inline bool is_err() {
            return this->_type <= Type::InvalidIdentifierErr;
        }

        public: inline usize size() {
            return this->_span.size();
        }
    };
}
