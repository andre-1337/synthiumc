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

// TODO : continue porting the lexer