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

typedef enum ExprType {
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

typedef enum BinaryType {
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

typedef enum UnaryType {
    UNARY_REF,
    UNARY_DEREF,
    UNARY_NEG_BOOL,
    UNARY_NEG_NUM
} UnaryType;

typedef enum StmtType {
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

typedef struct Stmt {
    int32_t tag;
} Stmt;

typedef struct Expr {
    int32_t tag;
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
    char *const ptr;
} IntExpr;

typedef struct StringExpr {
    Expr e;
    char *const ptr;
} StringExpr;

typedef struct CharExpr {
    Expr e;
    char *const ptr;
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

