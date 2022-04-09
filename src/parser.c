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

bool parser_parse_statement(Parser *p, Stmt **dest) {
    Token peek = parser_peek(p);

    switch (peek.ty) {
        case TOKEN_FN: {
            *dest = parser_parse_func_def(p, false);
            return false;
        }

        case TOKEN_IF: {
            *dest = parser_parse_if_stmt(p);
            return false;
        }

        case TOKEN_WHILE: {
            *dest = parser_parse_while_stmt(p);
            return false;
        }

        case TOKEN_TYPE: {
            *dest = parser_parse_type_stmt(p);
            return false;
        }

        case TOKEN_EXTERN: {
            parser_consume(p, TOKEN_EXTERN);
            *dest = parser_parse_func_def(p, true);
            return true;
        }

        case TOKEN_DELETE: {
            *dest = parser_parse_delete_stmt(p);
            return true;
        }

        case TOKEN_RETURN: {
            *dest = parser_parse_return_stmt(p);
            return true;
        }

        case TOKEN_LET: {
            *dest = parser_parse_let_stmt(p);
            return true;
        }

        case TOKEN_IMPORT: {
            *dest = parser_parse_import_stmt(p);
            return true;
        }

        default: {
            Expr *expr = parser_expression(p, false);
            if (expr == NULL) {
                *dest = NULL;
                return true;
            }

            *dest = ast_new_expr_stmt(expr);
            return true;
        }
    }
}

Stmt *parser_parse_func_def(Parser *p, bool is_extern) {
    CONSUME_OR_NULL(TOKEN_FN);

    Token ident = lexer_empty_token();
    if (!parser_consume_ident(p, &ident, false)) {
        return NULL;
    }

    CONSUME_OR_NULL(TOKEN_LPAREN);

    Vec params = parser_parse_param_list(p, is_extern);
    ParamList param_list = func_pl_from_vec(params);

    if (!parser_consume(p, TOKEN_COLON)) {
        func_pl_free(&param_list);
        return NULL;
    }

    Type ret_ty = type_empty();
    if (!parser_consume_type(p, &ret_ty)) {
        func_pl_free(&param_list);
        return NULL;
    }

    Stmt *block = NULL;
    if (!is_extern) {
        block = parser_parse_block(p);

        if (block == NULL) {
            func_pl_free(&param_list);
            return NULL;
        }
    }

    return ast_new_func_decl_stmt(ident, param_list, ret_ty, is_extern, ast_as_block_stmt(block));
}

Stmt *parser_parse_if_stmt(Parser *p) {
    CONSUME_OR_NULL(TOKEN_IF);

    Token peek = parser_peek(p);
    Expr *condition = parser_expression(p, true);

    if (condition == NULL) {
        ParseError error = parser_create_consume_error_text(p, &peek, "condition");
        parser_push_err(p, error);

        return NULL;
    }

    Stmt *block = parser_parse_block(p);
    if (block == NULL) {
        ast_expr_free(condition);
        return NULL;
    }

    Stmt *if_stmt = ast_new_if_stmt(condition, ast_as_block_stmt(block), NULL);
    if (parser_peek(p).ty != TOKEN_ELSE) {
        return if_stmt;
    }

    parser_consume(p, TOKEN_ELSE);

    Stmt *else_stmt = NULL;
    peek = parser_peek(p);

    switch (peek.ty) {
        case TOKEN_IF: {
            else_stmt = parser_parse_if_stmt(p);
        }

        case TOKEN_LBRACE: {
            else_stmt = parser_parse_block(p);
        }

        default: {
            ParseError error = parser_create_consume_error_text(p, &peek, "either 'if' or '{' after 'else'");
            parser_push_err(p, error);
        }
    }

    if (else_stmt == NULL) {
        ast_stmt_free(if_stmt);
        return NULL;
    }

    ast_as_if_stmt(if_stmt)->else_stmt = else_stmt;
    return if_stmt;
}

Stmt *parser_parse_while_stmt(Parser *p) {
    CONSUME_OR_NULL(TOKEN_WHILE);

    Token peek = parser_peek(p);
    Expr *condition = parser_expression(p, true);

    if (condition == NULL) {
        ParseError error = parser_create_consume_error_text(p, &peek, "condition");
        parser_push_err(p, error);

        return NULL;
    }

    Stmt *block = parser_parse_block(p);
    if (block == NULL) {
        ast_expr_free(condition);
        return NULL;
    }

    return ast_new_while_stmt(condition, ast_as_block_stmt(block));
}