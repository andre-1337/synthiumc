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

        public: inline Token() : _span(Synthium::Span()), _type(Type::UnknownErr) {}
        public: inline Token(Type type, string start, string end) : _span(Synthium::Span(start, end)), _type(type) {}
        public: inline Token(Type type, Synthium::Span span) : _span(span), _type(type) {}

        public: inline Span get_span() {
            return this->_span;
        }

        public: inline Type get_type() {
            return this->_type;
        }

        public: inline string get_lexeme() {
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
