#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "tyid.h"
#include "func.h"
#include "span.h"
#include "ident.h"
#include "utils.h"
#include "record.h"
#include "ptrvec.h"

struct Ty;

typedef enum ExprType : int32_t {
    EXPR_INT = 1,
    EXPR_STRING,
    EXPR_CHAR,
    EXPR_BINARY,
    EXPR_UNARY,
    EXPR_IDENT,
    EXPR_ASSIGN,
    EXPR_CALL,
    EXPR_INIT,
    EXPR_ACCESS,
    EXPR_AS,
    EXPR_NEW
} ExprType;

typedef enum BinaryType : int32_t {
    BINARY_ADD = 1,
    BINARY_SUB,
    BINARY_MUL,
    BINARY_DIV,
    BINARY_MOD,
    BINARY_ST,
    BINARY_SE,
    BINARY_GT,
    BINARY_GE,
    BINARY_LOG_AND,
    BINARY_LOG_OR,
    BINARY_EQ,
    BINARY_NE
} BinaryType;

typedef enum UnaryType : int32_t {
    UNARY_REF,
    UNARY_DEREF,
    UNARY_NEG_BOOL,
    UNARY_NEG_NUM
} UnaryType;

typedef enum StmtType : int32_t {
    STMT_EXPR = 1,
    STMT_LET,
    STMT_IMPORT,
    STMT_FUNC_DECL,
    STMT_STRUCT_DECL,
    STMT_BLOCK,
    STMT_IF,
    STMT_WHILE,
    STMT_DELETE,
    STMT_RETURN
} StmtType;

typedef enum ElseType : int32_t {
    ELSE_TYPE_NO_ELSE,
    ELSE_TYPE_BLOCK,
    ELSE_TYPE_ELSE_IF
} ElseType;

typedef struct Stmt {
    int32_t tag;
} Stmt;

typedef struct Expr {
    ExprType tag;
    Ty *ty;
    Span span;
} Expr;

typedef struct ExprStmt {
    Stmt s;
    Expr *expr;
} ExprStmt;

typedef struct DeleteStmt {
    Stmt s;
    Expr *expr;
} DeleteStmt;

typedef struct ReturnStmt {
    Stmt s;
    Expr *expr;
} ReturnStmt;

typedef struct BlockStmt {
    Stmt s;
    Ptrvec stmts;
} BlockStmt;

typedef struct WhileStmt {
    Stmt s;
    Expr *cond;
    BlockStmt *block;
} WhileStmt;

typedef struct IfStmt {
    Stmt s;
    Expr *condition;
    BlockStmt *block;
    Stmt *else_stmt;
} IfStmt;

typedef struct FuncDeclStmt {
    Stmt s;
    FuncDef decl;
    BlockStmt *block;
} FuncDeclStmt;

typedef struct StructDeclStmt {
    Stmt s;
    StructDecl decl;
} StructDeclStmt;

typedef struct ImportStmt {
    Stmt s;
    Ident mod;
} ImportStmt;

typedef struct LetStmt {
    Stmt s;
    Expr *value;
    Ident ident;
    Type ty;
} LetStmt;

typedef struct AccessExpr {
    Expr e;
    Expr *left;
    Expr *right;
} AccessExpr;

typedef struct AsExpr {
    Expr e;
    Expr *expr;
    Type ty;
} AsExpr;

typedef struct NewExpr {
    Expr e;
    Expr *expr;
} NewExpr;

typedef struct ArgList {
    Ptrvec args;
} ArgList;

typedef struct Init {
    Ident ident;
    Expr *expr;
} Init;

typedef struct InitList {
    Vec inits;
} InitList;

typedef struct CallExpr {
    Expr e;
    Expr *ident;
    ArgList args;
} CallExpr;

typedef struct InitExpr {
    Expr e;
    Expr *ident;
    InitList inits;
} InitExpr;

typedef struct AssignExpr {
    Expr e;
    Expr *left;
    Expr *right;
} AssignExpr;

typedef struct IdentExpr {
    Expr e;
    Ident ident;
} IdentExpr;

typedef struct IntExpr {
    Expr e;
    const char *ptr;
} IntExpr;

typedef struct StringExpr {
    Expr e;
    const char *ptr;
} StringExpr;

typedef struct CharExpr {
    Expr e;
    const char *ptr;
} CharExpr;

typedef struct BinaryExpr {
    Expr e;
    Expr *left;
    Expr *right;
    BinaryType ty;
} BinaryExpr;

typedef struct UnaryExpr {
    Expr e;
    Expr *right;
    UnaryType ty;
} UnaryExpr;

Stmt *ast_new_expr_stmt(Expr *e);
bool ast_is_expr_stmt(Stmt *s);
ExprStmt *ast_as_expr_stmt(Stmt *s);

Stmt *ast_new_delete_stmt(Expr *e);
bool ast_is_delete_stmt(Stmt *s);
DeleteStmt *ast_as_delete_stmt(Stmt *s);

Stmt *ast_new_return_stmt(Expr *e);
bool ast_is_return_stmt(Stmt *s);
ReturnStmt *ast_as_return_stmt(Stmt *s);

Stmt *ast_new_while_stmt(Expr *condition, BlockStmt *block);
bool ast_is_while_stmt(Stmt *s);
WhileStmt *ast_as_while_stmt(Stmt *s);

Stmt *ast_new_if_stmt(Expr *condition, BlockStmt *block, Stmt *else_stmt);
bool ast_is_if_stmt(Stmt *s);
IfStmt *ast_as_if_stmt(Stmt *s);
ElseType ast_else_type(IfStmt *s);

Stmt *ast_new_block_stmt(Ptrvec statements);
bool ast_is_block_stmt(Stmt *s);
BlockStmt *ast_as_block_stmt(Stmt *s);

Stmt *ast_new_func_decl_stmt(Token ident, ParamList params, Type ret_ty, bool is_extern, BlockStmt *block);
bool ast_is_func_decl_stmt(Stmt *s);
FuncDeclStmt *ast_as_func_decl_stmt(Stmt *s);

Stmt *ast_new_struct_decl_stmt(SpanInterner *si, Ident name, Vec fields);
bool ast_is_struct_decl_stmt(Stmt *s);
StructDeclStmt *ast_as_struct_decl_stmt(Stmt *s);

Stmt *ast_new_import_stmt(Span span, const char *path);
bool ast_is_import_stmt(Stmt *s);
ImportStmt *ast_as_import_stmt(Stmt *s);

Stmt *ast_new_let_stmt(Token ident, Type ty, Expr *value);
bool ast_is_let_stmt(Stmt *s);
LetStmt *ast_as_let_stmt(Stmt *s);

bool ast_has_type_decl(LetStmt *s);
void ast_stmt_free(Stmt *s);

Expr *ast_new_access_expr(Span span, Expr *left, Expr *right);
bool ast_is_access_expr(Expr *e);
AccessExpr *ast_as_access_expr(Expr *e);

Expr *ast_new_as_expr(Span span, Expr *e, Type ty);
bool ast_is_as_expr(Expr *e);
AsExpr *ast_as_as_expr(Expr *e);

Expr *ast_new_new_expr(Span span, Expr *e);
bool ast_is_new_expr(Expr *e);
NewExpr *ast_as_new_expr(Expr *e);

Expr *ast_new_call_expr(Span span, Expr *ident, ArgList args);
bool ast_is_call_expr(Expr *e);
CallExpr *ast_as_call_expr(Expr *e);

Expr *ast_new_init_expr(Span span, Expr *ident, InitList inits);
bool ast_is_init_expr(Expr *e);
InitExpr *ast_as_init_expr(Expr *e);

Expr *ast_new_assign_expr(Span span, Expr *left, Expr *right);
bool ast_is_assign_expr(Expr *e);
AssignExpr *ast_as_assign_expr(Expr *e);

Expr *ast_new_ident_expr(Token ident);
bool ast_is_ident_expr(Expr *e);
IdentExpr *ast_as_ident_expr(Expr *e);

Expr *ast_new_int_expr(Span span, const char *ptr);
bool ast_is_int_expr(Expr *e);
IntExpr *ast_as_int_expr(Expr *e);

Expr *ast_new_string_expr(Span span, const char *ptr);
bool ast_is_string_expr(Expr *e);
StringExpr *ast_as_string_expr(Expr *e);

Expr *ast_new_char_expr(Span span, const char *ptr);
bool ast_is_char_expr(Expr *e);
CharExpr *ast_as_char_expr(Expr *e);

Expr *ast_new_binary_expr(Span span, BinaryType ty, Expr *left, Expr *right);
bool ast_is_binary_expr(Expr *e);
BinaryExpr *ast_as_binary_expr(Expr *e);

Expr *ast_new_unary_expr(Span span, UnaryType kind, Expr *right);
bool ast_is_unary_expr(Expr *e);
UnaryExpr *ast_as_unary_expr(Expr *e);

Expr ast_create_expr(ExprType tag, Span span);
void ast_expr_free(Expr *e);
const char *ast_expr_to_string(Expr *e, SpanInterner *si);

ArgList ast_create_arg_list();
int32_t ast_num_args(ArgList *al);
Expr *ast_get_arg_al(ArgList *al, int32_t i);
void ast_push_arg(ArgList *al, Expr *arg);
void ast_free_al(ArgList *al);
const char *ast_arg_list_to_string(ArgList *al, SpanInterner *si);

Init ast_create_init(Ident ident, Expr *expr);
Init ast_empty_init();
InitList ast_create_init_list();
int32_t ast_num_inits(InitList *il);
Expr *ast_get_init_expr(InitList *il, int32_t i);
bool ast_get_init_at(InitList *il, int32_t i, Init *dest);
void ast_push_init(InitList *il, Init init);
void ast_free_il(InitList *il);
const char *ast_init_list_to_string(InitList *il, SpanInterner *si);