#include "../include/ast.h"
#include "../include/lexer.h"

struct Token;

static inline Stmt create_stmt_tag(StmtType type) {
    Stmt stmt = {
        .tag = type
    };

    return stmt;
}

// < Expression Statement
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
// Expression Statement >

// < Delete Statement
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
// Delete Statement >

// < Return Statement
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
// Return Statement >

// < While Statement
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
// While Statement >

// < If Statement
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
// If Statement >

// < Block Statement
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
// Block Statement >

// < Function Declaration Statement
Stmt *ast_new_func_decl_stmt(struct Token ident, ParamList params, Type ret_ty, bool is_extern, BlockStmt *block) {
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
// Function Declaration Statement >

// < Struct Declaration Statement
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
// Struct Declaration Statement >

// < Import Statement
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
// Import Statement >

// < Let Statement
Stmt *ast_new_let_stmt(struct Token ident, Type ty, Expr *value) {
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
// Let Statement >

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

// < Access Expression
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
// Access Expression >

// < As Expression
Expr *ast_new_as_expr(Span span, Expr *e, Type ty) {
    AsExpr *as_expr = (AsExpr *) malloc(sizeof(AsExpr));
    as_expr->e = create_expr_tag(EXPR_AS, span);
    as_expr->expr = e;
    as_expr->ty = ty;

    Expr *expr = (Expr *) as_expr;

    return expr;
}

bool ast_is_as_expr(Expr *e) {
    return e->tag == EXPR_AS;
}

AsExpr *ast_as_as_expr(Expr *e) {
    return (AsExpr *) e;
}
// As Expression >

// < New Expression
Expr *ast_new_new_expr(Span span, Expr *e) {
    NewExpr *new_expr = (NewExpr *) malloc(sizeof(NewExpr));
    new_expr->e = create_expr_tag(EXPR_NEW, span);
    new_expr->expr = e;

    Expr *expr = (Expr *) new_expr;

    return expr;
}

bool ast_is_new_expr(Expr *e) {
    return e->tag == EXPR_NEW;
}

NewExpr *ast_as_new_expr(Expr *e) {
    return (NewExpr *) e;
}
// New Expression >

// < Call Expression
Expr *ast_new_call_expr(Span span, Expr *ident, ArgList args) {
    if (!ast_is_ident_expr(ident) && !ast_is_access_expr(ident)) {
        ast_expr_free(ident);
        return NULL;
    }

    CallExpr *call_expr = (CallExpr *) malloc(sizeof(CallExpr));
    call_expr->e = create_expr_tag(EXPR_CALL, span);
    call_expr->ident = ident;
    call_expr->args = args;

    Expr *expr = (Expr *) call_expr;

    return expr;
}

bool ast_is_call_expr(Expr *e) {
    return e->tag == EXPR_CALL;
}

CallExpr *ast_as_call_expr(Expr *e) {
    return (CallExpr *) e;
}
// Call Expression >

// < Initializer Expression
Expr *ast_new_init_expr(Span span, Expr *ident, InitList inits) {
    if (!ast_is_ident_expr(ident) && !ast_is_access_expr(ident)) {
        ast_expr_free(ident);
        return NULL;
    }

    InitExpr *init_expr = (InitExpr *) malloc(sizeof(InitExpr));
    init_expr->e = create_expr_tag(EXPR_INIT, span);
    init_expr->ident = ident;
    init_expr->inits = inits;

    Expr *expr = (Expr *) init_expr;

    return expr;
}

bool ast_is_init_expr(Expr *e) {
    return e->tag == EXPR_INIT;
}

InitExpr *ast_as_init_expr(Expr *e) {
    return (InitExpr *) e;
}
// Initializer Expression >

// < Assignment Expression
Expr *ast_new_assign_expr(Span span, Expr *left, Expr *right) {
    AssignExpr *assign_expr = (AssignExpr *) malloc(sizeof(AssignExpr));
    assign_expr->e = create_expr_tag(EXPR_ASSIGN, span);
    assign_expr->left = left;
    assign_expr->right = right;

    Expr *expr = (Expr *) assign_expr;

    return expr;
}

bool ast_is_assign_expr(Expr *e) {
    return e->tag == EXPR_ASSIGN;
}

AssignExpr *ast_as_assign_expr(Expr *e) {
    return (AssignExpr *) e;
}
// Assignment Expression >

// < Identifier Expression
Expr *ast_new_ident_expr(Token ident) {
    IdentExpr *ident_expr = (IdentExpr *) malloc(sizeof(IdentExpr));
    ident_expr->e = create_expr_tag(EXPR_IDENT, ident.span);
    ident_expr->ident = ident_create(ident);

    Expr *expr = (Expr *) ident_expr;

    return expr;
}

bool ast_is_ident_expr(Expr *e) {
    return e->tag == EXPR_IDENT;
}

IdentExpr *ast_as_ident_expr(Expr *e) {
    return (IdentExpr *) e;
}
// Identifier Expression >

// < Integer Expression
Expr *ast_new_int_expr(Span span, const char *ptr) {
    IntExpr *int_expr = (IntExpr *) malloc(sizeof(IntExpr));
    int_expr->e = create_expr_tag(EXPR_INT, span);
    int_expr->ptr = ptr;

    Expr *expr = (Expr *) int_expr;

    return expr;
}

bool ast_is_int_expr(Expr *e) {
    return e->tag == EXPR_INT;
}

IntExpr *ast_as_int_expr(Expr *e) {
    return (IntExpr *) e;
}
// Integer Expression >

// < String Expression
Expr *ast_new_string_expr(Span span, const char *ptr) {
    StringExpr *string_expr = (StringExpr *) malloc(sizeof(StringExpr));
    string_expr->e = create_expr_tag(EXPR_STRING, span);
    string_expr->ptr = ptr;

    Expr *expr = (Expr *) string_expr;

    return expr;
}

bool ast_is_string_expr(Expr *e) {
    return e->tag == EXPR_STRING;
}

StringExpr *ast_as_string_expr(Expr *e) {
    return (StringExpr *) e;
}
// String Expression >

// < Character Expression
Expr *ast_new_char_expr(Span span, const char *ptr) {
    CharExpr *char_expr = (CharExpr *) malloc(sizeof(CharExpr));
    char_expr->e = create_expr_tag(EXPR_CHAR, span);
    char_expr->ptr = ptr;

    Expr *expr = (Expr *) char_expr;

    return expr;
}

bool ast_is_char_expr(Expr *e) {
    return e->tag == EXPR_CHAR;
}

CharExpr *ast_as_char_expr(Expr *e) {
    return (CharExpr *) e;
}
// Character Expression >

// < Binary Expression
Expr *ast_new_binary_expr(Span span, BinaryType ty, Expr *left, Expr *right) {
    BinaryExpr *binary_expr = (BinaryExpr *) malloc(sizeof(BinaryExpr));
    binary_expr->e = create_expr_tag(EXPR_BINARY, span);
    binary_expr->left = left;
    binary_expr->right = right;
    binary_expr->ty = ty;

    Expr *expr = (Expr *) binary_expr;

    return expr;
}

bool ast_is_binary_expr(Expr *e) {
    return e->tag == EXPR_BINARY;
}

BinaryExpr *ast_as_binary_expr(Expr *e) {
    return (BinaryExpr *) e;
}
// Binary Expression >

// < Unary Expression
Expr *ast_new_unary_expr(Span span, UnaryType kind, Expr *right) {
    UnaryExpr *unary_expr = (UnaryExpr *) malloc(sizeof(UnaryExpr));
    unary_expr->e = create_expr_tag(EXPR_UNARY, span);
    unary_expr->ty = kind;
    unary_expr->right = right;

    Expr *expr = (Expr *) unary_expr;

    return expr;
}

bool ast_is_unary_expr(Expr *e) {
    return e->tag == EXPR_UNARY;
}

UnaryExpr *ast_as_unary_expr(Expr *e) {
    return (UnaryExpr *) e;
}
// Unary Expression >

void ast_expr_free(Expr *e) {
    if (e == NULL) {
        return;
    }

    if (ast_is_binary_expr(e)) {
        BinaryExpr *binary_expr = ast_as_binary_expr(e);
        ast_expr_free(binary_expr->left);
        ast_expr_free(binary_expr->right);
    } else if (ast_is_assign_expr(e)) {
        AssignExpr *assign_expr = ast_as_assign_expr(e);
        ast_expr_free(assign_expr->left);
        ast_expr_free(assign_expr->right);
    } else if (ast_is_access_expr(e)) {
        AccessExpr *access_expr = ast_as_access_expr(e);
        ast_expr_free(access_expr->left);
        ast_expr_free(access_expr->right);
    } else if (ast_is_access_expr(e)) {
        CallExpr *call_expr = ast_as_call_expr(e);
        ast_expr_free((Expr *) call_expr->ident);
        ast_free_al(&call_expr->args);
    } else if (ast_is_init_expr(e)) {
        InitExpr *init_expr = ast_as_init_expr(e);
        ast_expr_free((Expr *) init_expr->ident);
        ast_free_il(&init_expr->inits);
    } else if (ast_is_unary_expr(e)) {
        ast_expr_free(ast_as_unary_expr(e)->right);
    } else if (ast_is_as_expr(e)) {
        ast_expr_free(ast_as_as_expr(e)->expr);
    } else if (ast_is_new_expr(e)) {
        ast_expr_free(ast_as_new_expr(e)->expr);
    }

    free((void *) e);
}

const char *ast_expr_to_string(Expr *e, SpanInterner *si) {
    if (e == NULL) {
        return strdup("(null)");
    }

    if (ast_is_int_expr(e)) {
        IntExpr *int_expr = ast_as_int_expr(e);
        int32_t len = span_get(si, e->span).len;
        return strndup(int_expr->ptr, len);
    }

    if (ast_is_string_expr(e)) {
        StringExpr *string_expr = ast_as_string_expr(e);
        int32_t len = span_get(si, e->span).len;
        return strndup(string_expr->ptr - 1, len + 2);
    }

    if (ast_is_char_expr(e)) {
        CharExpr *char_expr = ast_as_char_expr(e);
        int32_t len = span_get(si, e->span).len;
        return strndup(char_expr->ptr - 1, len + 2);
    }

    if (ast_is_ident_expr(e)) {
        IdentExpr *ident_expr = ast_as_ident_expr(e);
        return ident_to_string(&ident_expr->ident, si);
    }

    if (ast_is_new_expr(e)) {
        NewExpr *new_expr = ast_as_new_expr(e);
        const char *expr_s = ast_expr_to_string(new_expr->expr, si);
        const char *s = fmt_str("new %s", expr_s);

        free((void *) expr_s);

        return s;
    }

    if (ast_is_as_expr(e)) {
        AsExpr *as_expr = ast_as_as_expr(e);
        const char *expr_s = ast_expr_to_string(as_expr->expr, si);
        const char *ty_s = type_to_string(&as_expr->ty, si);
        const char *s = fmt_str("%s as %s", expr_s, ty_s);

        free((void *) expr_s);
        free((void *) ty_s);

        return s;
    }

    if (ast_is_assign_expr(e)) {
        AssignExpr *assign_expr = ast_as_assign_expr(e);
        const char *left = ast_expr_to_string(assign_expr->left, si);
        const char *right = ast_expr_to_string(assign_expr->right, si);
        const char *s = fmt_str("%s = %s", left, right);

        free((void *) left);
        free((void *) right);

        return s;
    }

    if (ast_is_access_expr(e)) {
        AccessExpr *access_expr = ast_as_access_expr(e);
        const char *left = ast_expr_to_string(access_expr->left, si);
        const char *right = ast_expr_to_string(access_expr->right, si);
        const char *s = fmt_str("%s.%s", left, right);

        free((void *) left);
        free((void *) right);

        return s;
    }

    if (ast_is_unary_expr(e)) {
        UnaryExpr *unary_expr = ast_as_unary_expr(e);
        const char *right = ast_expr_to_string(unary_expr->right, si);
        const char *token_str = lexer_unary2str(unary_expr->ty);
        const char *s = fmt_str("(%s%s)", token_str, right);

        free((void *) right);

        return s;
    }

    if (ast_is_binary_expr(e)) {
        BinaryExpr *binary_expr = ast_as_binary_expr(e);
        const char *left = ast_expr_to_string(binary_expr->left, si);
        const char *right = ast_expr_to_string(binary_expr->right, si);
        const char *token_str = lexer_binary2str(binary_expr->ty);
        const char *s = fmt_str("(%s%s%s)", left, token_str, right);

        free((void *) left);
        free((void *) right);

        return s;
    }

    if (ast_is_call_expr(e)) {
        CallExpr *call_expr = ast_as_call_expr(e);
        const char *ident = ast_expr_to_string((Expr *) call_expr->ident, si);
        const char *args = ast_arg_list_to_string(&call_expr->args, si);
        const char *s = fmt_str("%s(%s)", ident, args);

        free((void *) ident);
        free((void *) args);

        return s;
    }

    if (ast_is_init_expr(e)) {
        InitExpr *init_expr = ast_as_init_expr(e);
        const char *ident = ast_expr_to_string((Expr *) init_expr->ident, si);
        const char *inits = ast_init_list_to_string(&init_expr->inits, si);
        const char *s = fmt_str("%s {%s}", ident, inits);

        free((void *) ident);
        free((void *) inits);

        return s;
    }

    return NULL;
}

ArgList ast_create_arg_list() {
    ArgList arg_list = {
        .args = ptrvec_create()
    };

    return arg_list;
}

int32_t ast_num_args(ArgList *al) {
    return al->args.len;
}

Expr *ast_get_arg_at(ArgList *al, int32_t i) {
    return (Expr *) ptrvec_get(&al->args, i);
}

void ast_push_arg(ArgList *al, Expr *arg) {
    ptrvec_push_ptr(&al->args, (void *) arg);
}

void ast_free_al(ArgList *al) {
    int32_t i = 0;
    while (i < al->args.len) {
        Expr *e = (Expr *) ptrvec_get(&al->args, i);
        ast_expr_free(e);

        i++;
    }

    ptrvec_free(&al->args);
}

const char *ast_arg_list_to_string(ArgList *al, SpanInterner *si) {
    char *args = NULL;
    int32_t i = 0;

    while (i < al->args.len) {
        Expr *a = ast_get_arg_at(al, i);
        const char *s = ast_expr_to_string(a, si);
        int32_t comma_len = 0;

        if (i > 0) {
            comma_len = 2;
        }

        int32_t new_len = strlen(args) + strlen(s) + 1 + comma_len;
        char *new_args = (char *) malloc(new_len);
        char *old_args = args;

        args = strcpy(new_args, args);
        free((void *) old_args);

        if (comma_len > 0) {
            args = strcat(args, ", ");
        }

        args = strcat(args, s);
        free((void *) s);

        i++;
    }

    args[strlen(args) - 1] = '\0';

    return args;
}

Init ast_create_init(Ident ident, Expr *expr) {
    Init init = {
        .ident = ident,
        .expr = expr
    };

    return init;
}

Init ast_empty_init() {
    Init init = {
        .ident = ident_empty(),
        .expr = NULL
    };

    return init;
}

InitList ast_create_init_list() {
    InitList init_list = {
        .inits = vec_create(sizeof(Init))
    };

    return init_list;
}

int32_t ast_num_inits(InitList *il) {
    return il->inits.len;
}

Expr *ast_get_init_expr_at(InitList *il, int32_t i) {
    Init init = ast_empty_init();

    if (!ast_get_init_at(il, i, &init)) {
        return NULL;
    }

    return init.expr;
}

bool ast_get_init_at(InitList *il, int32_t i, Init *dest) {
    return vec_get(&il->inits, i, (void *) dest);
}

void ast_push_init(InitList *il, Init init) {
    vec_push(&il->inits, (void *) &init);
}

void ast_free_il(InitList *il) {
    int32_t i = 0;
    while (i < il->inits.len) {
        Init init = ast_empty_init();
        ast_get_init_at(il, i, &init);
        ast_expr_free(init.expr);

        i++;
    }

    vec_free(&il->inits);
}

const char *ast_init_list_to_string(InitList *il, SpanInterner *si) {
    char *inits = NULL;
    int32_t i = 0;

    while (i < il->inits.len) {
        Init init = ast_empty_init();
        if (!ast_get_init_at(il, i, &init)) {
            free((void *) inits);
            return NULL;
        }

        const char *n = ident_to_string(&init.ident, si);
        const char *e = ast_expr_to_string(init.expr, si);
        int32_t extra_len = 2;

        if (i > 0) {
            extra_len = 4;
        }

        int32_t new_len = strlen(inits) + strlen(n) + strlen(e) + 1 + extra_len;
        char *new_inits = (char *) malloc(new_len);
        char *old_inits = inits;

        inits = strcpy(new_inits, inits);
        free((void *) old_inits);

        if (extra_len > 2) {
            inits = strcat(inits, ", ");
        }

        inits = strcat(inits, n);
        inits = strcat(inits, ": ");
        inits = strcat(inits, e);

        free((void *) n);
        free((void *) e);

        i++;
    }

    inits[strlen(inits) - 1] = '\0';

    return inits;
}
