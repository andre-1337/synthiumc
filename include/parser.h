#ifndef SYNTHIUMC_PARSER_H
#define SYNTHIUMC_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "ast.h"
#include "vec.h"
#include "mod.h"
#include "tyid.h"
#include "span.h"
#include "lexer.h"
#include "param.h"
#include "source.h"
#include "precedence.h"

#define CONSUME_OR_NULL(ty) if (!parser_consume(p, ty)) { return NULL; }
#define CHECK_EXPR_OR_NULL(name, expr) Expr *name = expr; if (name == NULL) { return NULL; }
#define CHECK_ASSIGN_OR_NULL(name, expr) name = expr; if (name == NULL) { return NULL; }

typedef struct ParseError {
    Span span;
    const char *text;
} ParseError;

typedef struct Parser {
    bool in_panic_mode;
    Lexer lexer;
    Vec errors;
} Parser;

ParseError parser_empty_err();
bool parser_err_is_empty(ParseError *err);

Parser parser_create(SourceFile source, SpanInterner *si, int32_t ctx);
void parser_free_p(Parser *p);
void parser_free_errs_from(Parser *p, int32_t start);
int32_t parser_num_errs(Parser *p);
ParseError *parser_get_err(Parser *p, int32_t idx);
void parser_push_err(Parser *p, ParseError error);
bool parser_advance(Parser *p, Token *dest);
bool parser_consume(Parser *p, TokenType ty);
bool parser_consume_ident(Parser *p, Token *ident, bool allow_access_expr);
bool parser_consume_token(Parser *p, TokenType ty, Token *dest);
bool parser_consume_type_ident(Parser *p, Token *ident, ParseError *err);
bool parser_consume_type(Parser *p, Type *dest);
Token parser_peek(Parser *p);

Module *parser_parse(Parser *p);

Stmt *parser_statement(Parser *p);
bool parser_parse_statement(Parser *p, Stmt **dest);
Stmt *parser_parse_delete_stmt(Parser *p);
Stmt *parser_parse_return_stmt(Parser *p);
Stmt *parser_parse_import_stmt(Parser *p);
Stmt *parser_parse_let_stmt(Parser *p);
Stmt *parser_parse_type_stmt(Parser *p);
Stmt *parser_parse_func_def(Parser *p, bool is_extern);
Stmt *parser_parse_while_stmt(Parser *p);
Stmt *parser_parse_if_stmt(Parser *p);
Stmt *parser_parse_block(Parser *p);
void parser_free_statements(Ptrvec *statements);

Vec parser_parse_field_list(Parser *p);
Vec parser_parse_param_list(Parser *p, bool allow_varargs);
ArgList parser_parse_arg_list(Parser *p);
InitList parser_parse_init_list(Parser *p);

Expr *parser_expression(Parser *p, bool no_struct);
Expr *parser_parse_expression(Parser *p, Precedence precedence, bool no_struct);
bool parser_next_higher_precedence(Parser *p, Precedence precedence, bool no_struct);
Expr *parser_prefix(Parser *p, bool no_struct);
Expr *parser_infix(Parser *p, Token *token, Expr *left, bool no_struct);

int32_t parser_sync(Parser *p);
int32_t parser_inner_sync(Parser *p);

ParseError parser_create_error(Span span, const char *text);
ParseError parser_create_type_ident_error(Parser *p, Span span, const char *text, const char *reason);
ParseError parser_create_statement_error(Parser *p, int32_t start, int32_t end);
ParseError parser_create_lex_error(Parser *p, Token *next);
ParseError parser_create_consume_error(Parser *p, Token *peek, TokenType expected_ty);
ParseError parser_create_consume_error_text(Parser *p, Token *peek, const char *text);

#endif
