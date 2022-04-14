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

Stmt *parser_parse_type_stmt(Parser *p) {
    CONSUME_OR_NULL(TOKEN_TYPE);

    Token ident = lexer_empty_token();
    if (!parser_consume_ident(p, &ident, false)) {
        return NULL;
    }

    CONSUME_OR_NULL(TOKEN_STRUCT);
    CONSUME_OR_NULL(TOKEN_LBRACE);

    Vec fields = parser_parse_field_list(p);
    if (fields.elem_size == 0) {
        vec_free(&fields);
        return NULL;
    }

    return ast_new_struct_decl_stmt(p->lexer.span_interner, ident_create(ident), fields);
}

Stmt *parser_parse_delete_stmt(Parser *p) {
    CONSUME_OR_NULL(TOKEN_DELETE);
    CHECK_EXPR_OR_NULL(expr, parser_expression(p, false));

    return ast_new_delete_stmt(expr);
}

Stmt *parser_parse_return_stmt(Parser *p) {
    CONSUME_OR_NULL(TOKEN_DELETE);
    
    if (parser_peek(p).ty == TOKEN_SEMI) {
        return ast_new_return_stmt(NULL);
    }

    CHECK_EXPR_OR_NULL(expr, parser_expression(p, false));

    return ast_new_return_stmt(expr);
}

Stmt *parser_parse_let_stmt(Parser *p) {
    CONSUME_OR_NULL(TOKEN_LET);

    Token ident = lexer_empty_token();
    if (!parser_consume_ident(p, &ident, false)) {
        return NULL;
    }

    Type ty = type_empty();
    if (parser_peek(p).ty == TOKEN_COLON) {
        CONSUME_OR_NULL(TOKEN_COLON);

        if (!parser_consume_type(p, &ty)) {
            return NULL;
        }
    }

    CONSUME_OR_NULL(TOKEN_EQ);
    CHECK_EXPR_OR_NULL(expr, parser_expression(p, false));

    return ast_new_let_stmt(ident, ty, expr);
}

Stmt *parser_parse_import_stmt(Parser *p) {
    CONSUME_OR_NULL(TOKEN_IMPORT);

    Token mod_path = lexer_empty_token();
    if (!parser_consume_token(p, TOKEN_STRING, &mod_path)) {
        return NULL;
    }

    return ast_new_import_stmt(mod_path.span, mod_path.lexeme);
}

Stmt *parser_parse_block(Parser *p) {
    CONSUME_OR_NULL(TOKEN_LBRACE);

    Ptrvec statements = ptrvec_create();
    Token peek = parser_peek(p);

    while (peek.ty != TOKEN_EOF && peek.ty != TOKEN_RBRACE) {
        Stmt *s = parser_statement(p);
        if (s == NULL) {
            parser_free_statements(&statements);
            return NULL;
        }

        ptrvec_push_ptr(&statements, (void *) s);
        peek = parser_peek(p);
    }

    if (!parser_consume(p, TOKEN_RBRACE)) {
        parser_free_statements(&statements);
        return NULL;
    }

    return ast_new_block_stmt(statements);
}

Vec parser_parse_field_list(Parser *p) {
    #define BAIL() vec_free(&fields); return vec_create(0)

    Vec fields = vec_create(sizeof(Fields));
    Token peek = parser_peek(p);

    while (peek.ty != TOKEN_EOF && peek.ty != TOKEN_RBRACE) {
        Token ident1 = lexer_empty_token();
        if (!parser_consume_ident(p, &ident1, false)) {
            BAIL();
        }

        Ident ident2 = ident_create(ident1);

        if (!parser_consume(p, TOKEN_COLON)) {
            BAIL();
        }

        Type ty = type_empty();
        if (!parser_consume_type(p, &ty)) {
            BAIL();
        }

        Field field = record_field_create(ident2, ty);
        vec_push(&fields, (void *) &field);

        peek = parser_peek(p);

        if (peek.ty == TOKEN_COMMA) {
            parser_consume(p, TOKEN_COMMA);
        }
    }

    if (!parser_consume(p, TOKEN_RBRACE)) {
        BAIL();
    }

    return fields;

    #undef BAIL
}

Vec parser_parse_param_list(Parser *p, bool allow_varargs) {
    #define BAIL() vec_free(&params); return vec_create(0)

    Vec params = vec_create(sizeof(Param));
    Token peek = parser_peek(p);

    while (peek.ty != TOKEN_EOF && peek.ty != TOKEN_RPAREN) {
        Token ident1 = lexer_empty_token();
        Type ty = type_empty();

        if (allow_varargs && parser_peek(p).ty == TOKEN_TRIPLE_DOT) {
            parser_consume_token(p, TOKEN_TRIPLE_DOT, &ident1);
        } else {
            ident1 = lexer_empty_token();
            if (!parser_consume_ident(p, &ident1, false)) {
                BAIL();
            }

            if (!parser_consume(p, TOKEN_COLON)) {
                BAIL();
            }

            if (!parser_consume_type(p, &ty)) {
                BAIL();
            }
        }

        Ident ident2 = ident_create(ident1);
        Param param = param_create(ident2, ty);

        vec_push(&params, (void *) &param);
        peek = parser_peek(p);

        if (peek.ty == TOKEN_COMMA) {
            parser_consume(p, TOKEN_COMMA);
        }
    }

    if (!parser_consume(p, TOKEN_RPAREN)) {
        BAIL();
    }

    return params;

    #undef BAIL
}

ArgList parser_parse_arg_list(Parser *p) {
    #define BAIL() ast_free_al(&args); return ast_create_arg_list()

    ArgList args = ast_create_arg_list();
    Token peek = parser_peek(p);

    while (peek.ty != TOKEN_EOF && peek.ty != TOKEN_RPAREN) {
        Expr *arg = parser_expression(p, false);
        if (arg == NULL) {
            BAIL();
        }

        ast_push_arg(&args, arg);
        peek = parser_peek(p);


        if (peek.ty == TOKEN_COMMA) {
            parser_consume(p, TOKEN_COMMA);
        }
    }

    return args;

    #undef BAIL
}

InitList parser_parse_init_list(Parser *p) {
    #define BAIL() ast_free_il(&inits); return ast_create_init_list()

    InitList inits = ast_create_init_list();
    Token peek = parser_peek(p);

    while (peek.ty != TOKEN_EOF && peek.ty != TOKEN_RBRACE) {
        Token ident = lexer_empty_token();
        if (!parser_consume_ident(p, &ident, false)) {
            BAIL();
        }

        if (!parser_consume(p, TOKEN_COLON)) {
            BAIL();
        }

        Expr *val = parser_expression(p, false);
        if (val == NULL) {
            BAIL();
        }

        Init init = ast_create_init(ident_create(ident), val);
        
        ast_push_init(&inits, init);
        peek = parser_peek(p);

        if (peek.ty == TOKEN_COMMA) {
            parser_consume(p, TOKEN_COMMA);
        }
    }

    return inits;

    #undef BAIL
}

void parser_free_statements(Ptrvec *statements) {
    int32_t i = 0;
    while (i < statements->len) {
        Stmt *s = (Stmt *) ptrvec_get(statements, i);
        ast_stmt_free(s);

        i++;
    }

    ptrvec_free(statements);
}

Expr *parser_expression(Parser *p, bool no_struct) {
    CHECK_EXPR_OR_NULL(left, parser_parse_expression(p, PRECEDENCE_ASSIGN, no_struct));

    while (parser_peek(p).ty == TOKEN_EQ) {
        parser_consume(p, TOKEN_EQ);
        
        Expr *value = parser_expression(p, no_struct);
        if (value == NULL) {
            ast_expr_free(left);
            return NULL;
        }

        Span span = span_merge(p->lexer.span_interner, left->span, value->span);
        left = ast_new_assign_expr(span, left, value);
    }

    return left;
}

Expr *parser_parse_expression(Parser *p, Precedence precedence, bool no_struct) {
    CHECK_EXPR_OR_NULL(left, parser_prefix(p, no_struct));

    while (parser_next_higher_precedence(p, precedence, no_struct)) {
        Token token = lexer_empty_token();
        if (!parser_advance(p, &token)) {
            ast_expr_free(left);
            return NULL;
        }

        Expr *infix = parser_infix(p, &token, left, no_struct);
        if (infix == NULL) {
            ast_expr_free(left);
            return NULL;
        }

        left = infix;
    }

    return left;
}

bool parser_next_higher_precedence(Parser *p, Precedence precedence, bool no_struct) {
    Token t = parser_peek(p);

    if (lexer_is_err(&t) || t.ty == TOKEN_EOF) {
        return false;
    }

    if (t.ty == TOKEN_LBRACE) {
        return !no_struct && precedence_get(t.ty) > precedence;
    }

    return precedence_get(t.ty) > precedence;
}

Expr *parser_prefix(Parser *p, bool no_struct) {
    #define UNARY(typ)                                                                     \
        CHECK_EXPR_OR_NULL(expr, parser_parse_expression(p, PRECEDENCE_UNARY, no_struct)); \
        Span span = span_merge(p->lexer.span_interner, token.span, expr->span);            \
        return ast_new_unary_expr(span, (typ), expr)

    Token token = lexer_empty_token();
    if (!parser_advance(p, &token)) {
        return NULL;
    }

    switch (token.ty) {
        case TOKEN_INT: {
            return ast_new_int_expr(token.span, token.lexeme);
        }

        case TOKEN_STRING: {
            return ast_new_string_expr(token.span, token.lexeme);
        }

        case TOKEN_CHAR: {
            return ast_new_char_expr(token.span, token.lexeme);
        }

        case TOKEN_IDENT: {
            return ast_new_ident_expr(token);
        }

        case TOKEN_NEW: {
            CHECK_EXPR_OR_NULL(e, parser_expression(p, no_struct));
            Span span = span_merge(p->lexer.span_interner, token.span, e->span);

            return ast_new_new_expr(span, e);
        }

        case TOKEN_LPAREN: {
            CHECK_EXPR_OR_NULL(e, parser_expression(p, false));

            if (!parser_consume(p, TOKEN_RPAREN)) {
                ast_expr_free(e);
                return NULL;
            }

            e->span.start--;
            e->span.len_or_tag++;

            return e;
        }

        case TOKEN_AMPERSAND: {
            UNARY(UNARY_REF);
        }

        case TOKEN_STAR: {
            UNARY(UNARY_DEREF);
        }

        case TOKEN_BANG: {
            UNARY(UNARY_NEG_BOOL);
        }

        case TOKEN_MINUS: {
            UNARY(UNARY_NEG_NUM);
        }

        default: {
            break;
        }
    }

    return NULL;

    #undef UNARY
}

Expr *parser_infix(Parser *p, Token *token, Expr *left, bool no_struct) {
    #define BINARY(typ)                                                          \
        Precedence prec = precedence_get(token->ty);                             \
        CHECK_EXPR_OR_NULL(right, parser_parse_expression(p, prec, no_struct));  \
        Span span = span_merge(p->lexer.span_interner, left->span, right->span); \
        return ast_new_binary_expr(span, typ, left, right)

    switch (token->ty) {
        case TOKEN_DOT: {
            CHECK_EXPR_OR_NULL(right, parser_expression(p, no_struct));
            Span span = span_merge(p->lexer.span_interner, left->span, right->span);

            return ast_new_access_expr(span, left, right);
        }

        case TOKEN_LPAREN: {
            ArgList args = parser_parse_arg_list(p);
            Token closing_paren = lexer_empty_token();

            if (!parser_consume_token(p, TOKEN_RPAREN, &closing_paren)) {
                ast_free_al(&args);
                return NULL;
            }

            Span span = span_merge(p->lexer.span_interner, left->span, closing_paren.span);

            return ast_new_call_expr(span, left, args);
        }

        case TOKEN_LBRACE: {
            InitList inits = parser_parse_init_list(p);
            Token closing_brace = lexer_empty_token();

            if (!parser_consume_token(p, TOKEN_RBRACE, &closing_brace)) {
                ast_free_il(&inits);
                return NULL;
            }

            Span span = span_merge(p->lexer.span_interner, left->span, closing_brace.span);

            return ast_new_init_expr(span, left, inits);
        }

        case TOKEN_AS: {
            Type ty = type_empty();
            if (!parser_consume_type(p, &ty)) {
                return NULL;
            }

            Span span = span_merge(p->lexer.span_interner, left->span, type_span(&ty));

            return ast_new_as_expr(span, left, ty);
        }

        case TOKEN_PLUS: {
            BINARY(BINARY_ADD);
        }

        case TOKEN_MINUS: {
            BINARY(BINARY_SUB);
        }

        case TOKEN_STAR: {
            BINARY(BINARY_MUL);         
        }

        case TOKEN_SLASH: {
            BINARY(BINARY_DIV);
        }

        case TOKEN_PERCENT: {
            BINARY(BINARY_MOD);
        }

        case TOKEN_SMALLER: {
            BINARY(BINARY_ST);
        }

        case TOKEN_SMALLER_EQ: {
            BINARY(BINARY_SE);
        }

        case TOKEN_GREATER: {
            BINARY(BINARY_GT);
        }

        case TOKEN_GREATER_EQ: {
            BINARY(BINARY_GE);
        }

        case TOKEN_DOUBLE_AMPERSAND: {
            BINARY(BINARY_LOG_AND);
        }

        case TOKEN_DOUBLE_PIPE: {
            BINARY(BINARY_LOG_OR);
        }

        case TOKEN_DOUBLE_EQ: {
            BINARY(BINARY_EQ);
        }

        case TOKEN_BANG_EQ: {
            BINARY(BINARY_NE);
        }

        default: {
            break;
        }
    }

    return NULL;

    #undef BINARY
}

int32_t parser_sync(Parser *p) {
    p->in_panic_mode = true;
    int32_t pos = parser_inner_sync(p);
    p->in_panic_mode = false;

    return pos;
}

int32_t parser_inner_sync(Parser *p) {
    Token previous = lexer_empty_token();
    if (!parser_advance(p, &previous)) {
        return lexer_end_pos(&p->lexer);
    }

    Token peek = parser_peek(p);
    if (peek.ty != TOKEN_EOF) {
        BigSpan span = span_get(p->lexer.span_interner, previous.span);
        uint32_t pos = span.start + span.len;

        if (previous.ty == TOKEN_SEMI && parser_peek(p).ty != TOKEN_RBRACE) {
            return pos;
        }

        if (peek.ty == TOKEN_LET || peek.ty == TOKEN_FN) {
            return pos;
        }

        if (!parser_advance(p, &previous)) {
            return pos;
        }

        if (previous.ty == TOKEN_RBRACE) {
            BigSpan span = span_get(p->lexer.span_interner, previous.span);
            return span.start + span.len;
        }

        peek = parser_peek(p);
    }

    BigSpan span = span_get(p->lexer.span_interner, previous.span);
    return span.start + span.len;
}

ParseError parser_create_error(Span span, const char *text) {
    ParseError error = {
        .span = span,
        .text = text
    };

    return error;
}

ParseError parser_create_type_ident_error(Parser *p, Span span, const char *text, const char *reason) {
    uint16_t len = span_get(p->lexer.span_interner, span).len;
    return parser_create_error(span, error_err2str(ERROR_INVALID_TYPE_IDENT, len, text, reason));
}

ParseError parser_create_statement_error(Parser *p, int32_t start, int32_t end) {
    Span span = lexer_create_span(&p->lexer, start, end);
    uint16_t len = span_get(p->lexer.span_interner, span).len;

    return parser_create_error(span, error_err2str(ERROR_COULD_NOT_PARSE_STMT, len, source_code(&p->lexer.source) + start));
}

ParseError parser_create_lex_error(Parser *p, Token *next) {
    int32_t len = lexer_token_len(next, p->lexer.span_interner);
    ErrorCode err_ty = ERROR_UNKNOWN_SYMBOL;

    if (next->ty == TOKEN_CHAR_ERR) {
        err_ty = ERROR_CHAR_LIT_LEN;
    }

    return parser_create_error(next->span, error_err2str(err_ty, len, next->lexeme));
}

ParseError parser_create_consume_error(Parser *p, Token *peek, TokenType expected_ty) {
    return parser_create_consume_error_text(p, peek, lexer_token_ty_to_static_string(expected_ty));
}

ParseError parser_create_consume_error_text(Parser *p, Token *peek, const char *text) {
    char *peek_str = NULL;
    bool needs_free = lexer_token_to_string(peek, p->lexer.span_interner, &peek_str);

    const char *err_str = error_err2str(ERROR_EXPECTED_BUT_GOT, text, peek_str);

    if (needs_free) {
        free((void *) peek_str);
    }

    return parser_create_error(peek->span, err_str);
}