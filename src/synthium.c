#define DEBUG_MODE

#include "../include/ty.h"
#include "../include/ast.h"
#include "../include/mod.h"
#include "../include/span.h"
#include "../include/path.h"
#include "../include/tyid.h"
#include "../include/reader.h"
#include "../include/ptrvec.h"
#include "../include/source.h"
#include "../include/record.h"
#include "../include/parser.h"
#include "../include/typecheck.h"

void synthium_print_debug_stmt_info(Stmt *s, SpanInterner *si);
void synthium_print_debug_mod_info(Module *mod, SpanInterner *si);
void synthium_print_parse_errors(Parser *p, SpanInterner *si, FileMap *fm, Path *abs_path);
void synthium_print_type_errors(TypeChecker *tc, SpanInterner *si, FileMap *fm, Path *abs_path);
void synthium_print_error(const char *err_text, BigSpan *span, SourceFile *file, Path *abs_path);

int main(int argc, char **argv) {
    if (argc <= 1) {
        printf("[error] no input files\n");
        return -1;
    }

    Path rel_compiler_path = path_empty();
    path_from_str(*argv, &rel_compiler_path);

    PathBuf abs_compiler_path = path_buf_from(path_empty());
    int32_t error = path_canonicalize(&rel_compiler_path, &abs_compiler_path);

    if (error != 0) {
        printf("[error] %s\n", strerror(error));
        return -1;
    }

    Path compiler_path = path_parent(&abs_compiler_path.inner);
    int32_t num_total_errs = 0;
    SpanInterner span_interner = span_create_interner();
    FileMap file_map = reader_create();
    FileAddResult res = reader_add_std_lib(&file_map, &compiler_path);

    if (res.err_code != 0) {
        const char *err_msg = error_err2str(res.err_code, res.file_name);
        printf("[error] %s\n", err_msg);
        free((void *) err_msg);

        reader_free_fm(&file_map);
        path_free(&abs_compiler_path);

        return -2;
    }

    res = reader_add_all(&file_map, &compiler_path, argc - 1, (const char **) argv + 1);
    if (res.err_code != 0) {
        const char *err_msg = error_err2str(res.err_code, res.file_name);
        printf("[error] %s\n", err_msg);
        free((void *) err_msg);

        reader_free_fm(&file_map);
        path_free(&abs_compiler_path);

        return -2;
    }

    ModuleMap mm = mod_map_with_cap(reader_num_files(&file_map));
    int32_t i = 0;

    while (i < reader_num_files(&file_map)) {
        SourceFile src = reader_get_by_idx(&file_map, i);
        Parser p = parser_create(src, &span_interner, i);
        Module *mod = parser_parse(&p);

        int32_t num_errs = parser_num_errs(&p);
        num_total_errs += num_errs;

        if (num_errs > 0) {
            printf("%d parse errors found\n", num_errs);
            synthium_print_parse_errors(&p, &span_interner, &file_map, &compiler_path);
        }

        mod_add_mod(&mm, mod);

        #ifdef DEBUG_MODE
            synthium_print_debug_mod_info(mod, &span_interner);
        #endif

        Stmt *s = mod_get_stmt_at(mod, 0);
        if (s != NULL) {
            #ifdef DEBUG_MODE
                synthium_print_debug_stmt_info(s, &span_interner);
            #endif
        } else {
            printf("[error] no valid statements in file\n");
        }

        parser_free_p(&p);

        i++;
    }

    TypeChecker tc = typecheck_create(&span_interner, &mm);
    typecheck_check(&tc);

    int32_t num_errs = typecheck_num_errs(&tc);
    num_total_errs += num_errs;

    if (num_errs > 0) {
        printf("%d type errors found\n", num_errs);
        synthium_print_type_errors(&tc, &span_interner, &file_map, &compiler_path);
    }

    typecheck_free_tc(&tc);
    reader_free_fm(&file_map);
    mod_free_map(&mm);
    path_free(&abs_compiler_path);

    return num_total_errs;
}

void synthium_print_debug_stmt_info(Stmt *s, SpanInterner *si) {
    bool is_expr = ast_is_expr_stmt(s);
    printf("is expr? %d\n", is_expr);

    if (is_expr) {
        Expr *e = ast_as_expr_stmt(s)->expr;
        
        const char *s = ast_expr_to_string(e, si);
        printf("%s\n", s);
        free((void *) s);

        if (ast_is_int_expr(e)) {
            printf("[error] todo\n");
        }
    } else if (ast_is_struct_decl_stmt(s)) {
        StructDeclStmt *decl_s = ast_as_struct_decl_stmt(s);
        
        const char *s = record_to_string(&decl_s->decl, si);
        printf("%s\n", s);
        free((void *) s);
    } else if (ast_is_let_stmt(s)) {
        LetStmt *ls = ast_as_let_stmt(s);
        const char *ident = ls->ident.ident;
        uint16_t ident_len = span_get(si, ls->ident.ident_span).len;
        const char *value = ast_expr_to_string(ls->value, si);
        char *ty = NULL;

        if (ast_has_type_decl(ls)) {
            free((void *) ty);
            
            const char *tys = type_to_string(&ls->ty, si);

            ty = (char *) fmt_str(": %s", tys);
            free((void *) tys);
        }

        printf("let %.*s%s = %s;\n", ident_len, ident, ty, value);
        free((void *) value);
        free((void *) ty);
    }
}

void synthium_print_debug_mod_info(Module *mod, SpanInterner *si) {
    int32_t num_imports = mod_num_imports(mod);
    int32_t j = 0;

    while (j < num_imports) {
        ImportStmt *is = mod_get_import_at(mod, j);
        
        const char *s = ident_to_string(&is->mod, si);
        printf("import '%s';\n", s);
        free((void *) s);

        j++;
    }

    printf("%d functions\n", mod_num_functions(mod));
    if (mod_num_functions(mod) > 0) {
        FuncDeclStmt *first = mod_get_function_at(mod, 0);

        const char *s = type_to_string(&first->decl.ret_ty, si);
        const char *name = ident_to_string(&first->decl.name, si);
        printf("fn %s: %s\n", name, s);
        free((void *) s);
        free((void *) name);
    }

    printf("%d structs\n", mod_num_structs(mod));
    if (mod_num_structs(mod) > 0) {
        StructDeclStmt *first = mod_get_struct_at(mod, 0);

        const char *s = record_to_string(&first->decl, si);
        printf("%s\n", s);
        free((void *) s);
    }
}

void synthium_print_parse_errors(Parser *p, SpanInterner *si, FileMap *fm, Path *abs_path) {
    int32_t i = 0;
    while (i < parser_num_errs(p)) {
        ParseError *err = parser_get_err(p, i);
        BigSpan span = span_get(si, err->span);
        SourceFile src = reader_get_by_idx(fm, span.ctx);

        synthium_print_error(err->text, &span, &src, abs_path);

        i++;
    }
}

void synthium_print_type_errors(TypeChecker *tc, SpanInterner *si, FileMap *fm, Path *abs_path) {
    int32_t i = 0;
    while (i < typecheck_num_errs(tc)) {
        TypeError *err = typecheck_get_err(tc, i);
        BigSpan span = span_get(si, err->span);
        SourceFile src = reader_get_by_idx(fm, span.ctx);

        synthium_print_error(err->text, &span, &src, abs_path);

        i++;
    }
}

void synthium_print_error(const char *err_text, BigSpan *span, SourceFile *file, Path *abs_path) {
    uint32_t last_nl = 0;
    int32_t line = source_find_line(span, file, &last_nl);
    const char *name = source_file_name_dup(file);

    printf("[error] %s\n--> %s:%d:%d\n\n", err_text, name + abs_path->len + 1, line, span->start - last_nl);

    free((void *) name);
}