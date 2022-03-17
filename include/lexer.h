#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum TokenType {
    TOKEN_UNKNOWN_ERR,
    TOKEN_CHAR_ERR,
    TOKEN_INT,
    TOKEN_IDENT,
    TOKEN_LET,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_IMPORT,
    TOKEN_FN,
    TOKEN_WHILE,
    TOKEN_NEW,
    TOKEN_DELETE,
    TOKEN_RETURN,
    TOKEN_TYPE,
    TOKEN_STRUCT,
    TOKEN_AS,
    TOKEN_EXTERN,
    TOKEN_SEMI,
    TOKEN_COMMA,
    TOKEN_COLON,
    TOKEN_DOT,
    TOKEN_TRIPLE_DOT,
    TOKEN_BANG,
    TOKEN_BANG_EQ,
    TOKEN_EQ,
    TOKEN_DOUBLE_EQ,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_PERCENT,
    TOKEN_SMALLER,
    TOKEN_SMALLER_EQ,
    TOKEN_GREATER,
    TOKEN_GREATER_EQ,
    TOKEN_AMPERSAND,
    TOKEN_DOUBLE_AMPERSAND,
    TOKEN_PIPE,
    TOKEN_DOUBLE_PIPE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_EOF
} TokenType;

static char const *const token_strings[] = {
    "unknown token",
    "invalid character literal",
    "integer",
    "identifier",
    "let",
    "if",
    "else",
    "import",
    "fn",
    "while",
    "new",
    "delete",
    "return",
    "type",
    "struct",
    "as",
    "extern",
    ";",
    ",",
    ":",
    ".",
    "...",
    "!",
    "!=",
    "=",
    "==",
    "+",
    "-",
    "*",
    "/",
    "%",
    "<",
    "<=",
    ">",
    ">=",
    "&",
    "&&",
    "|",
    "||",
    "(",
    ")",
    "{",
    "}",
    "string",
    "char",
    "eof"
};

static char const *const unary_type_ops[] = {
    "error",
    "&",
    "*",
    "!",
    "-"
};

static char const *const binary_type_ops[] = {
    "error",
    "+",
    "-",
    "*",
    "/",
    "%",
    "<",
    "<=",
    ">",
    ">=",
    "&&",
    "||",
    "==",
    "!="
};

size_t const len_token_strings = sizeof(token_strings) / sizeof(char *);
size_t const len_unary_strings = sizeof(unary_type_ops) / sizeof(char *);
size_t const len_binary_strings = sizeof(binary_type_ops) / sizeof(char *);

char const *const get_str(int32_t index, size_t len, char const *const *array);
char const *const tok2str(TokenType type);
char const *const unary2str(TokenType op);
char const *const binary2str(TokenType type);