#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "span.h"
#include "utils.h"
#include "source.h"

typedef enum TokenType : int32_t {
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

typedef struct Token {
    TokenType ty;
    Span span;
    const char *lexeme;
} Token;

typedef struct Lexer {
    bool has_peek;
    int32_t ctx;
    int32_t source_len;
    Token peek;
    SourceFile source;
    const char *start;
    const char *current;
    SpanInterner *span_interner;
} Lexer;

const char *lexer_get_str(int32_t index, size_t len, const char **array);
const char *lexer_tok2str(TokenType type);
const char *lexer_unary2str(int32_t op);
const char *lexer_binary2str(int32_t type);
Token lexer_empty_token();
bool lexer_is_err(Token *token);
Lexer lexer_create(SourceFile src, SpanInterner *si, int32_t ctx);
int32_t lexer_current_pos(Lexer *l);
int32_t lexer_end_pos(Lexer *l);
int32_t lexer_current(Lexer *l);
Token lexer_peek(Lexer *l);
void lexer_skip_until(Lexer *l, int32_t ch);
Token lexer_next_token(Lexer *l);
Token lexer_get_next_token(Lexer *l);
bool lexer_is_num(int32_t ch);
bool lexer_is_ws(int32_t ch);
bool lexer_is_letter(int32_t ch);
Token lexer_lex_ident(Lexer *l);
Token lexer_lex_num(Lexer *l);
Token lexer_lex_string(Lexer *l);
Token lexer_lex_char(Lexer *l);
TokenType lexer_check_keyword(Lexer *l, int32_t start, int32_t rest_len, const char *rest, TokenType ty);
TokenType lexer_ident_type(Lexer *l);
void lexer_skip_ws(Lexer *l);
bool lexer_at_end(Lexer *l);
int32_t lexer_advance(Lexer *l);
Span lexer_create_span(Lexer *l, int32_t start, int32_t end);
Token lexer_create_token(Lexer *l, TokenType ty, const char *start, const char *end);
Token lexer_create_token_from_span(Lexer *l, TokenType ty, const char *start, Span span);
Token lexer_token_from_start(Lexer *l, TokenType ty);
const char *lexer_token_ty_to_static_string(TokenType ty);
bool lexer_token_to_string(Token *token, SpanInterner *si, char **dest);
int32_t lexer_token_len(Token *token, SpanInterner *si);