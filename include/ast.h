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
    int32_t ty;
} BinaryExpr;

typedef struct UnaryExpr {
    Expr e;
    int32_t kind;
    Expr *right;
} UnaryExpr;

