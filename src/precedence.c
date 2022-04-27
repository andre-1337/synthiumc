#include "../include/precedence.h"

Precedence precedence_get(TokenType ty) {
    switch (ty) {
        case TOKEN_EQ: return PRECEDENCE_ASSIGN;
        case TOKEN_DOUBLE_AMPERSAND: case TOKEN_DOUBLE_PIPE: return PRECEDENCE_AND;
        case TOKEN_DOUBLE_EQ: case TOKEN_BANG_EQ: return PRECEDENCE_EQUALITY;
        case TOKEN_SMALLER: case TOKEN_SMALLER_EQ: case TOKEN_GREATER: case TOKEN_GREATER_EQ: return PRECEDENCE_COMPARISON;
        case TOKEN_PLUS: case TOKEN_MINUS: return PRECEDENCE_SUM;
        case TOKEN_STAR: case TOKEN_SLASH: case TOKEN_PERCENT: case TOKEN_AS: return PRECEDENCE_PRODUCT;
        case TOKEN_LPAREN: case TOKEN_LBRACE: return PRECEDENCE_CALL;
        case TOKEN_DOT: return PRECEDENCE_ACCESS;

        default: return PRECEDENCE_NONE;
    }
}
