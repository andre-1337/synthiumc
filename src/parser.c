#include "../include/parser.h"

ParseError parser_empty_err() {
    ParseError error = {
        .span = span_empty(),
        .text = NULL
    };

    return error;
}

bool parser_err_is_empty(ParseError *err) {
    return err->text == NULL;
}

Parser parser_create(SourceFile source, SpanInterner *si, int32_t ctx) {
    Parser parser = {
        .in_panic_mode = false,
        .lexer = lexer_create(source, si, ctx),
        .errors = vec_create(sizeof(ParseError))
    };

    return parser;
}

void parser_free_p(Parser *p) {
    parser_free_errs_from(p, 0);
    vec_free(&p->errors);
}

void parser_free_errs_from(Parser *p, int32_t start) {
    int32_t i = start;
    ParseError elem = parser_empty_err();

    while (i < p->errors.len) {
        vec_get(&p->errors, i, (void *) &elem);
        free((void *) elem.text);
        i++;
    }

    p->errors.len -= (p->errors.len - start);
}

int32_t parser_num_errs(Parser *p) {
    return p->errors.len;
}

ParseError *parser_get_err(Parser *p, int32_t idx) {
    return (ParseError *) vec_get_ptr(&p->errors, idx);
}

void parser_push_err(Parser *p, ParseError error) {
    vec_push(&p->errors, (void *) &error);
}

bool parser_advance(Parser *p, Token *dest) {
    Token next = lexer_next_token(&p->lexer);

    if (next.ty == TOKEN_EOF) {
        if (!p->in_panic_mode) {
            ParseError error = parser_create_error(next.span, error_err2str(ERROR_UNEXPECTED_EOF));
            vec_push(&p->errors, (void *) &error);
        }

        return false;
    } else if (lexer_is_err(&next) && !p->in_panic_mode) {
        ParseError error = parser_create_lex_error(p, &next);
        parser_push_err(p, error);

        return false;
    }

    if (dest != NULL) {
        *dest = next;
    }

    return true;
}

bool parser_consume(Parser *p, TokenType ty) {
    return parser_consume_token(p, ty, NULL);
}

bool parser_consume_ident(Parser *p, Token *ident, bool allow_access_expr) {
    if (!parser_consume_token(p, TOKEN_IDENT, ident)) {
        return false;
    }

    if (allow_access_expr) {
        while (parser_peek(p).ty == TOKEN_DOT) {
            parser_consume(p, TOKEN_DOT);

            Token first_part = *ident;
            if (!parser_consume_token(p, TOKEN_IDENT, ident)) {
                return false;
            }

            Span span = span_merge(p->lexer.span_interner, first_part.span, ident->span);
            *ident = lexer_create_token_from_span(&p->lexer, TOKEN_IDENT, first_part.lexeme, span);
        }
    }

    return true;
}

bool parser_consume_token(Parser *p, TokenType ty, Token *dest) {
    Token next = lexer_empty_token();
    if (!parser_advance(p, &next)) {
        return false;
    }

    if (dest != NULL) {
        *dest = next;
    }

    if (next.ty == ty) {
        return true;
    }

    ParseError error = parser_create_consume_error(p, &next, ty);
    parser_push_err(p, error);

    return false;
}

bool parser_consume_type_ident(Parser *p, Token *ident, ParseError *err) {
    if (!parser_consume_token(p, TOKEN_IDENT, ident)) {
        return false;
    }

    int32_t dots = 0;
    while (parser_peek(p).ty == TOKEN_DOT) {
        parser_consume(p, TOKEN_DOT);

        Token first_part = *ident;
        if (!parser_consume_token(p, TOKEN_IDENT, ident)) {
            return false;
        }

        Span span = span_merge(p->lexer.span_interner, first_part.span, ident->span);
        *ident = lexer_create_token_from_span(&p->lexer, TOKEN_IDENT, first_part.lexeme, span);

        dots++;
    }

    if (dots > 1) {
        *err = parser_create_type_ident_error(p, ident->span, ident->lexeme, "type identifiers cannot contain more than one '.'");
        return false;
    }

    return true;
}

bool parser_consume_type(Parser *p, Type *dest) {
    int32_t num_errs = parser_num_errs(p);
    int32_t pointer_count = 0;

    while (parser_peek(p).ty == TOKEN_STAR) {
        parser_consume(p, TOKEN_STAR);
        pointer_count++;
    }

    Token ty = lexer_empty_token();
    ParseError illegal_type_err = parser_empty_err();

    if (!parser_consume_type_ident(p, &ty, &illegal_type_err)) {
        if (parser_num_errs(p) > num_errs) {
            parser_free_errs_from(p, num_errs);
            ParseError error = parser_create_consume_error_text(p, &ty, "type");
            parser_push_err(p, error);
        }

        if (!parser_err_is_empty(&illegal_type_err)) {
            parser_push_err(p, illegal_type_err);
        }

        return false;
    }

    *dest = type_create_ptr(ident_create(ty), pointer_count);
    return true;
}

Token parser_peek(Parser *p) {
    return lexer_peek(&p->lexer);
}

Module *parser_parse(Parser *p) {
    Path path = p->lexer.source.file.path.inner;
    Module *mod = mod_create(path);
    
    while (parser_peek(p).ty != TOKEN_EOF) {
        Stmt *s = parser_statement(p);
        if (s != NULL) {
            mod_push_stmt(mod, s);
        }
    }

    return mod;
}

Stmt *parser_statement(Parser *p) {
    int32_t num_errs = parser_num_errs(p);
    int32_t start = lexer_current_pos(&p->lexer);
    Stmt *s = NULL;
    bool consume_semi = parser_parse_statement(p, &s);

    if (s == NULL) {
        int32_t end = parser_sync(p);

        if (num_errs == parser_num_errs(p)) {
            ParseError error = parser_create_statement_error(p, start, end);
            parser_push_err(p, error);
        }

        return NULL;
    }

    if (consume_semi) {
        if (!parser_consume(p, TOKEN_SEMI)) {
            parser_sync(p);
            ast_stmt_free(s);

            return NULL;
        }
    }

    return s;
}

