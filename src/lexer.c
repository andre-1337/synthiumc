#include "../include/lexer.h"

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

Token lexer_empty_token() {
    Token token = {
        .ty = TOKEN_UNKNOWN_ERR,
        .span = span_empty(),
        .lexeme = NULL
    };

    return token;
}

bool lexer_is_err(Token *token) {
    return (token->ty == TOKEN_UNKNOWN_ERR || token->ty == TOKEN_CHAR_ERR);
}

const char *lexer_token_ty_to_static_string(TokenType ty) {
    return lexer_tok2str(ty);
}

int32_t lexer_token_len(Token *token, SpanInterner *si) {
    return span_get(si, token->span).len;
}

bool lexer_token_to_string(Token *token, SpanInterner *si, char **dest) {
    if (token->ty >= TOKEN_INT && token->ty <= TOKEN_IDENT) {
        int32_t len = lexer_token_len(token, si);
        *dest = strndup(token->lexeme, len);
        return true;
    }

    if (token->ty == TOKEN_STRING || token->ty == TOKEN_CHAR) {
        int32_t len = lexer_token_len(token, si);
        *dest = strndup(token->lexeme - 1, len + 2);
        return true;
    }

    *dest = (char *) lexer_tok2str(token->ty);
    return false;
}

Lexer lexer_create(SourceFile src, SpanInterner *si, int32_t ctx) {
    Span span = span_create(si, 0, 0, ctx);

    Token init_peek = {
        .ty = TOKEN_UNKNOWN_ERR,
        .span = span,
        .lexeme = NULL
    };

    Lexer lexer = {
        .has_peek = false,
        .ctx = ctx,
        .source_len = strlen(src.code),
        .peek = init_peek,
        .source = src,
        .start = src.code,
        .current = src.code,
        .span_interner = si
    };

    return lexer;
}

int32_t lexer_current_pos(Lexer *l) {
    if (l->has_peek) {
        return l->peek.span.start;
    }

    return l->current - l->source.code;
}

int32_t lexer_end_pos(Lexer *l) {
    return l->source_len - 1;
}

Token lexer_peek(Lexer *l) {
    if (l->has_peek) {
        return l->peek;
    }

    l->peek = lexer_next_token(l);
    l->has_peek = true;

    return l->peek;
}

Token lexer_next_token(Lexer *l) {
    if (l->has_peek) {
        l->has_peek = false;
        return l->peek;
    }

    lexer_skip_ws(l);
    return lexer_get_next_token(l);
}

int32_t lexer_current(Lexer *l) {
    int32_t c = 0;
    int32_t read_bytes = lexer_current_pos(l);

    read_char(l->current, l->source_len - read_bytes, &c);

    return c;
}

bool lexer_is_num(int32_t ch) {
    return (ch >= chr2int('0') && ch <= chr2int('9'));
}

bool lexer_is_letter(int32_t ch) {
    return (ch >= chr2int('a') && ch <= chr2int('z')) || (ch >= chr2int('A') && ch <= chr2int('Z'));
}

TokenType lexer_check_keyword(Lexer *l, int32_t start, int32_t rest_len, const char *rest, TokenType ty) {
    if (l->current - l->start == start + rest_len) {
        if (memcmp(l->start + start, rest, rest_len) == 0) {
            return ty;
        }
    }

    return TOKEN_IDENT;
}

TokenType lexer_ident_type(Lexer *l) {
    int64_t bytes_until = l->start - l->source.code;
    int32_t start = 0;
    int32_t len = read_char(l->start, l->source_len - bytes_until, &start);

    if (start == chr2int('l')) {
        return lexer_check_keyword(l, 1, 2, "et", TOKEN_LET);
    } else if (start == chr2int('i') && l->current - l->start > 1) {
        int32_t next = 0;
        read_char(l->start + len, l->source_len - bytes_until - len, &next);

        if (next == chr2int('f')) {
            return lexer_check_keyword(l, 2, 0, "", TOKEN_IF);
        } else if (next == chr2int('m')) {
            return lexer_check_keyword(l, 2, 4, "port", TOKEN_IMPORT);
        }
    } else if (start == chr2int('d')) {
        return lexer_check_keyword(l, 1, 5, "elete", TOKEN_DELETE);
    } else if (start == chr2int('f')) {
        return lexer_check_keyword(l, 1, 1, "n", TOKEN_FN);
    } else if (start == chr2int('e') && l->current - l->start > 3) {
        int32_t next = 0;
        read_char(l->start + len, l->source_len - bytes_until - len, &next);

        if (next == chr2int('x')) {
            return lexer_check_keyword(l, 1, 5, "xtern", TOKEN_EXTERN);
        } else if (next == chr2int('l')) {
            return lexer_check_keyword(l, 1, 3, "lse", TOKEN_ELSE);
        }
    } else if (start == chr2int('w')) {
        return lexer_check_keyword(l, 1, 4, "hile", TOKEN_WHILE);
    } else if (start == chr2int('n')) {
        return lexer_check_keyword(l, 1, 2, "ew", TOKEN_NEW);
    } else if (start == chr2int('s')) {
        return lexer_check_keyword(l, 1, 5, "truct", TOKEN_STRUCT);
    } else if (start == chr2int('t')) {
        return lexer_check_keyword(l, 1, 3, "ype", TOKEN_TYPE);
    } else if (start == chr2int('r')) {
        return lexer_check_keyword(l, 1, 5, "eturn", TOKEN_RETURN);
    } else if (start == chr2int('a')) {
        return lexer_check_keyword(l, 1, 1, "s", TOKEN_AS);
    }

    return TOKEN_IDENT;
}

Token lexer_lex_ident(Lexer *l) {
    while (lexer_is_letter(lexer_current(l)) || lexer_is_num(lexer_current(l)) || lexer_current(l) == chr2int('_')) {
        lexer_advance(l);
    }

    return lexer_token_from_start(l, lexer_ident_type(l));
}

Token lexer_lex_num(Lexer *l) {
    while (lexer_is_num(lexer_current(l))) {
        lexer_advance(l);
    }

    return lexer_token_from_start(l, TOKEN_INT);
}

Token lexer_lex_string(Lexer *l) {
    while (!lexer_at_end(l) && (lexer_current(l) != chr2int('"'))) {
        lexer_advance(l);
    }

    lexer_advance(l);

    return lexer_create_token(l, TOKEN_STRING, l->start + 1, l->current - 1);
}

Token lexer_lex_char(Lexer *l) {
    while (!lexer_at_end(l) && (lexer_current(l) != chr2int(*"'"))) {
        lexer_advance(l);
    }

    lexer_advance(l);

    int32_t len = l->current - l->start;
    if (len == 4 && (l->start[1] == '\\')) {
        return lexer_create_token(l, TOKEN_CHAR, l->start + 1, l->current - 1);
    }

    if (len != 3) {
        return lexer_create_token(l, TOKEN_CHAR_ERR, l->start + 1, l->current - 1);
    }

    return lexer_create_token(l, TOKEN_CHAR_ERR, l->start + 1, l->current - 1);
}

bool lexer_is_ws(int32_t ch) {
    return ch == chr2int(' ') || ch == chr2int('\r') || ch == chr2int('\n') || ch == chr2int('\t') || ch == chr2int('\0');
}

void lexer_skip_until(Lexer *l, int32_t ch) {
    while (!lexer_at_end(l)) {
        lexer_advance(l);

        if (lexer_current(l) == ch) {
            return;
        }
    }
}

void lexer_skip_ws(Lexer *l) {
    while (!lexer_at_end(l)) {
        int32_t c = lexer_current(l);
        if (lexer_is_ws(c)) {
            lexer_advance(l);
            continue;
        }
    }
}

bool lexer_at_end(Lexer *l) {
    return *l->current == '\0';
}

int32_t lexer_advance(Lexer *l) {
    int32_t read_bytes = lexer_current_pos(l);
    int32_t c = 0;
    int32_t len = read_char(l->current, l->source_len - read_bytes, &c);

    l->current += len;

    return c;
}

Span lexer_create_span(Lexer *l, int32_t start, int32_t end) {
    return span_create(l->span_interner, start, end, l->ctx);
}

Token lexer_create_token(Lexer *l, TokenType ty, const char *start, const char *end) {
    Token token = {
        .ty = ty,
        .span = lexer_create_span(l, start - l->source.code, end - l->source.code),
        .lexeme = start
    };

    return token;
}

Token lexer_create_token_from_span(Lexer *l, TokenType ty, const char *start, Span span) {
    Token token = {
        .ty = ty,
        .span = span,
        .lexeme = start
    };

    return token;
}

Token lexer_token_from_start(Lexer *l, TokenType ty) {
    return lexer_create_token(l, ty, l->start, l->current);
}

Token lexer_get_next_token(Lexer *l) {
    if (lexer_at_end(l)) {
        int32_t pos = lexer_current_pos(l);
        Token token = {
            .ty = TOKEN_EOF,
            .span = lexer_create_span(l, pos, pos),
            .lexeme = NULL
        };

        return token;
    }

    l->start = l->current;
    int32_t c = lexer_advance(l);

    if (lexer_is_num(c)) {
        return lexer_lex_num(l);
    }

    if (lexer_is_letter(c)) {
        return lexer_lex_ident(l);
    }

    if (c == chr2int('"')) {
        return lexer_lex_string(l);
    }

    if (c == chr2int(*"'")) {
        return lexer_lex_char(l);
    }

    if (c == chr2int(';')) {
        return lexer_token_from_start(l, TOKEN_SEMI);
    }

    if (c == chr2int(',')) {
        return lexer_token_from_start(l, TOKEN_COMMA);
    }

    if (c == chr2int(':')) {
        return lexer_token_from_start(l, TOKEN_COLON);
    }

    if (c == chr2int('+')) {
        return lexer_token_from_start(l, TOKEN_PLUS);
    }

    if (c == chr2int('-')) {
        return lexer_token_from_start(l, TOKEN_MINUS);
    }

    if (c == chr2int('*')) {
        return lexer_token_from_start(l, TOKEN_STAR);
    }

    if (c == chr2int('(')) {
        return lexer_token_from_start(l, TOKEN_LPAREN);
    }
    
    if (c == chr2int(')')) {
        return lexer_token_from_start(l, TOKEN_RPAREN);
    }

    if (c == chr2int('{')) {
        return lexer_token_from_start(l, TOKEN_LBRACE);
    }

    if (c == chr2int('}')) {
        return lexer_token_from_start(l, TOKEN_RBRACE);
    }

    if (c == chr2int('%')) {
        return lexer_token_from_start(l, TOKEN_PERCENT);
    }

    if (c == chr2int('.')) {
        if (memcmp((void *) l->start, (void *) "...", 3) == 0) {
            lexer_advance(l);
            lexer_advance(l);

            return lexer_token_from_start(l, TOKEN_TRIPLE_DOT);
        }

        return lexer_token_from_start(l, TOKEN_DOT);
    }

    if (c == chr2int('/')) {
        if (lexer_current(l) == chr2int('/')) {
            lexer_skip_until(l, chr2int('\n'));
            return lexer_next_token(l);
        }

        return lexer_token_from_start(l, TOKEN_SLASH);
    }

    if (c == chr2int('<')) {
        if (lexer_current(l) == chr2int('=')) {
            lexer_advance(l);
            return lexer_token_from_start(l, TOKEN_SMALLER_EQ);
        }

        return lexer_token_from_start(l, TOKEN_SMALLER);
    }

    if (c == chr2int('>')) {
        if (lexer_current(l) == chr2int('=')) {
            lexer_advance(l);
            return lexer_token_from_start(l, TOKEN_GREATER_EQ);
        }

        return lexer_token_from_start(l, TOKEN_GREATER);
    }

    if (c == chr2int('&')) {
        if (lexer_current(l) == chr2int('&')) {
            lexer_advance(l);
            return lexer_token_from_start(l, TOKEN_DOUBLE_AMPERSAND);
        }

        return lexer_token_from_start(l, TOKEN_AMPERSAND);
    }

    if (c == chr2int('|')) {
        if (lexer_current(l) == chr2int('|')) {
            lexer_advance(l);
            return lexer_token_from_start(l, TOKEN_DOUBLE_PIPE);
        }

        return lexer_token_from_start(l, TOKEN_PIPE);
    }

    if (c == chr2int('=')) {
        if (lexer_current(l) == chr2int('=')) {
            lexer_advance(l);
            return lexer_token_from_start(l, TOKEN_DOUBLE_EQ);
        }

        return lexer_token_from_start(l, TOKEN_EQ);
    }

    if (c == chr2int('!')) {
        if (lexer_current(l) == chr2int('=')) {
            lexer_advance(l);
            return lexer_token_from_start(l, TOKEN_BANG_EQ);
        }

        return lexer_token_from_start(l, TOKEN_BANG);
    }

    return lexer_token_from_start(l, TOKEN_UNKNOWN_ERR);
}