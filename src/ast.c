#include "../include/ast.h"

static inline Stmt create_stmt_tag(StmtType type) {
    Stmt stmt = {
        .tag = type
    };

    return stmt;
}

Stmt *ast_new_expr_stmt(Expr *e) {
    ExprStmt *expr_stmt = (ExprStmt *) malloc(sizeof(ExprStmt));
    expr_stmt->s = create_stmt_tag(STMT_EXPR);
    expr_stmt->expr = e;

    Stmt *stmt = (Stmt *) expr_stmt;

    return stmt;
}

bool ast_is_expr_stmt(Stmt *s) {
    return s->tag == STMT_EXPR;
}

ExprStmt *ast_as_expr_stmt(Stmt *s) {
    return (ExprStmt *) s;
}

Stmt *ast_new_delete_stmt(Expr *e) {
    DeleteStmt *delete_stmt = (DeleteStmt *) malloc(sizeof(DeleteStmt));
    delete_stmt->s = create_stmt_tag(STMT_DELETE);
    delete_stmt->expr = e;

    Stmt *stmt = (Stmt *) delete_stmt;

    return stmt;
}

bool ast_is_delete_stmt(Stmt *s) {
    return s->tag == STMT_DELETE;
}

DeleteStmt *ast_as_delete_stmt(Stmt *s) {
    return (DeleteStmt *) s;
}

Stmt *ast_new_return_stmt(Expr *e) {
    ReturnStmt *return_stmt = (ReturnStmt *) malloc(sizeof(ReturnStmt));
    return_stmt->s = create_stmt_tag(STMT_RETURN);
    return_stmt->expr = e;

    Stmt *stmt = (Stmt *) return_stmt;

    return stmt;
}

bool ast_is_return_stmt(Stmt *s) {
    return s->tag == STMT_RETURN;
}

ReturnStmt *ast_as_return_stmt(Stmt *s) {
    return (ReturnStmt *) s;
}

Stmt *ast_new_while_stmt(Expr *condition, BlockStmt *block) {
    WhileStmt *while_stmt = (WhileStmt *) malloc(sizeof(WhileStmt));
    while_stmt->s = create_stmt_tag(STMT_WHILE);
    while_stmt->cond = condition;
    while_stmt->block = block;

    Stmt *stmt = (Stmt *) while_stmt;

    return stmt;
}

bool ast_is_while_stmt(Stmt *s) {
    return s->tag == STMT_WHILE;
}

WhileStmt *ast_as_while_stmt(Stmt *s) {
    return (WhileStmt *) s;
}

Stmt *ast_new_if_stmt(Expr *condition, BlockStmt *block, Stmt *else_stmt) {
    IfStmt *if_stmt = (IfStmt *) malloc(sizeof(IfStmt));
    if_stmt->s = create_stmt_tag(STMT_IF);
    if_stmt->condition = condition;
    if_stmt->block = block;
    if_stmt->else_stmt = else_stmt;

    Stmt *stmt = (Stmt *) if_stmt;

    return stmt;
}

bool ast_is_if_stmt(Stmt *s) {
    return s->tag == STMT_IF;
}

IfStmt *ast_as_if_stmt(Stmt *s) {
    return (IfStmt *) s;
}

ElseType ast_else_type(IfStmt *s) {
    if (s->else_stmt == NULL) {
        return ELSE_TYPE_NO_ELSE;
    }

    if (ast_is_block_stmt(s->else_stmt)) {
        return ELSE_TYPE_BLOCK;
    }

    if (ast_is_if_stmt(s->else_stmt)) {
        return ELSE_TYPE_ELSE_IF;
    }

    return ELSE_TYPE_NO_ELSE;
}

Stmt *ast_new_block_stmt(Ptrvec statements) {
    BlockStmt *block_stmt = (BlockStmt *) malloc(sizeof(BlockStmt));
    block_stmt->s = create_stmt_tag(STMT_BLOCK);
    block_stmt->stmts = statements;

    Stmt *stmt = (Stmt *) block_stmt;

    return stmt;
}

bool ast_is_block_stmt(Stmt *s) {
    return s->tag == STMT_BLOCK;
}

BlockStmt *ast_as_block_stmt(Stmt *s) {
    return (BlockStmt *) s;
}

Stmt *ast_new_func_decl_stmt(Token ident, ParamList params, Type ret_ty, bool is_extern, BlockStmt *block) {
    FuncDeclStmt *func_decl_stmt = (FuncDeclStmt *) malloc(sizeof(FuncDeclStmt));
    func_decl_stmt->s = create_stmt_tag(STMT_FUNC_DECL);
    func_decl_stmt->decl = func_create(ident, params, ret_ty, is_extern);
    func_decl_stmt->block = block;

    Stmt *stmt = (Stmt *) func_decl_stmt;

    return stmt;
}

bool ast_is_func_decl_stmt(Stmt *s) {
    return s->tag == STMT_FUNC_DECL;
}

FuncDeclStmt *ast_as_func_decl_stmt(Stmt *s) {
    return (FuncDeclStmt *) s;
}

Stmt *ast_new_struct_decl_stmt(SpanInterner *si, Ident name, Vec fields) {
    StructDeclStmt *struct_decl_stmt = (StructDeclStmt *) malloc(sizeof(StructDeclStmt));
    struct_decl_stmt->s = create_stmt_tag(STMT_STRUCT_DECL);
    struct_decl_stmt->decl = record_struct_create(si, name, fields);

    Stmt *stmt = (Stmt *) struct_decl_stmt;

    return stmt;
}

bool ast_is_struct_decl_stmt(Stmt *s) {
    return s->tag == STMT_STRUCT_DECL;
}

StructDeclStmt *ast_as_struct_decl_stmt(Stmt *s) {
    return (StructDeclStmt *) s;
}

Stmt *ast_new_import_stmt(Span span, const char *path) {
    ImportStmt *import_stmt = (ImportStmt *) malloc(sizeof(ImportStmt));
    import_stmt->s = create_stmt_tag(STMT_IMPORT);
    import_stmt->mod = ident_from_str(span, path);

    Stmt *stmt = (Stmt *) import_stmt;

    return stmt;
}

bool ast_is_import_stmt(Stmt *s) {
    return s->tag == STMT_IMPORT;
}

ImportStmt *ast_as_import_stmt(Stmt *s) {
    return (ImportStmt *) s;
}

Stmt *ast_new_let_stmt(Token ident, Type ty, Expr *value) {
    LetStmt *let_stmt = (LetStmt *) malloc(sizeof(LetStmt));
    let_stmt->s = create_stmt_tag(STMT_LET);
    let_stmt->value = value;
    let_stmt->ident = ident_create(ident);
    let_stmt->ty = ty;

    Stmt *stmt = (Stmt *) let_stmt;

    return stmt;
}

bool ast_is_let_stmt(Stmt *s) {
    return s->tag == STMT_LET;
}

LetStmt *ast_as_let_stmt(Stmt *s) {
    return (LetStmt *) s;
}

bool ast_has_type_decl(LetStmt *s) {
    return !type_is_empty(&s->ty);
}

void ast_stmt_free(Stmt *s) {
    if (s == NULL) {
        return;
    }

    if (ast_is_expr_stmt(s)) {
        ast_expr_free(ast_as_expr_stmt(s)->expr);
    } else if (ast_is_let_stmt(s)) {
        ast_expr_free(ast_as_let_stmt(s)->value);
    } else if (ast_is_delete_stmt(s)) {
        ast_expr_free(ast_as_delete_stmt(s)->expr);
    } else if (ast_as_return_stmt(s)) {
        ast_expr_free(ast_as_return_stmt(s)->expr);
    } else if (ast_is_func_decl_stmt(s)) {
        FuncDeclStmt *func_decl_stmt = ast_as_func_decl_stmt(s);
        FuncDef func_def = func_decl_stmt->decl;
        
        func_free(&func_def);
        ast_stmt_free((Stmt *) func_decl_stmt->block);
    } else if (ast_is_block_stmt(s)) {
        Ptrvec stmts = ast_as_block_stmt(s)->stmts;
        int32_t i = 0;

        while (i < stmts.len) {
            void *s = ptrvec_get(&stmts, i);
            if (s != NULL) {
                ast_stmt_free((Stmt *) s);
            }

            i++;
        }

        ptrvec_free(&stmts);
    } else if (ast_is_if_stmt(s)) {
        IfStmt *if_stmt = ast_as_if_stmt(s);
        ast_expr_free(if_stmt->condition);
        ast_stmt_free((Stmt *) if_stmt->block);
        ast_stmt_free(if_stmt->else_stmt);
    } else if (ast_is_while_stmt(s)) {
        WhileStmt *while_stmt = ast_as_while_stmt(s);
        ast_expr_free(while_stmt->cond);
        ast_stmt_free((Stmt *) while_stmt->block);
    } else if (ast_is_struct_decl_stmt(s)) {
        StructDeclStmt *struct_decl_stmt = ast_as_struct_decl_stmt(s);
        record_struct_free(&struct_decl_stmt->decl);
    }

    free((void *) s);
}

static inline Expr create_expr_tag(ExprType type, Span span) {
    Expr expr = {
        .tag = type,
        .ty = NULL,
        .span = span
    };

    return expr;
}

Expr *ast_new_access_expr(Span span, Expr *left, Expr *right) {
    AccessExpr *access_expr = (AccessExpr *) malloc(sizeof(AccessExpr));
    access_expr->e = create_expr_tag(EXPR_ACCESS, span);
    access_expr->left = left;
    access_expr->right = right;

    Expr *expr = (Expr *) access_expr;

    return expr;
}

bool ast_is_access_expr(Expr *e) {
    return e->tag == EXPR_ACCESS;
}

AccessExpr *ast_as_access_expr(Expr *e) {
    return (AccessExpr *) e;
}