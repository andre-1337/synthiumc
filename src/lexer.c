#include "../include/lexer.h"

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

    *dest = lexer_tok2str(token->ty);
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
        if (memcmp((void *) l->start + start, (void *) rest, rest_len) == 0) {
            return ty;
        }
    }

    return TOKEN_IDENT;
}

int32_t lexer_ident_type(Lexer *l) {
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
    } // TODO
}