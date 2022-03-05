//
//  ast.h
//  drast
//
//  Created by Ashwin Paudel on 2022-02-05.
//

#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "token.h"
#include "../utils/mxDynamicArray.h"

typedef enum {
    AST_TYPE_IMPORT,

    AST_TYPE_FUNCTION_DECLARATION,
    AST_TYPE_FUNCTION_ARGUMENT,

    AST_TYPE_FUNCTION_CALL,

    AST_TYPE_VARIABLE_DEFINITION,

    AST_TYPE_VALUE_KEYWORD,

    AST_TYPE_STRUCT_OR_UNION_DECLARATION,
    AST_TYPE_STRUCT_OR_UNION_MEMBER_CALL,
    AST_TYPE_STRUCT_INITIALIZER,

    AST_TYPE_ENUM_DECLARATION,
    AST_TYPE_ENUM_ITEM,

    AST_TYPE_BINARY,
    AST_TYPE_LITERAL,
    AST_TYPE_GROUPING,
    AST_TYPE_UNARY,

    AST_TYPE_RETURN,

    AST_TYPE_IF_ELSE_STATEMENT,

    AST_TYPE_WHILE_STATEMENT,
    AST_TYPE_FOR_LOOP,

    AST_TYPE_INLINE_ASSEMBLY,

    AST_TYPE_SWITCH_STATEMENT,
    AST_TYPE_SWITCH_CASE,

    AST_TYPE_DO_CATCH,
    AST_TYPE_TRY_STATEMENT,

    AST_TYPE_BODY,

    AST_TYPE_ALIAS,
} ASTType;

typedef union {
    struct {
        char *file;
        bool is_library;
    } Import;

    struct {
        char *identifier;
        bool is_initialized;
        bool is_volatile;
        bool is_private;
        struct AST *value;
        struct AST *type;
    } VariableDeclaration;

    struct {
        Token *token;
        bool is_array;
        bool is_optional;
        bool is_pointer;
    } ValueKeyword;

    struct {
        char *function_name;

        struct AST *return_type;

        bool is_private;
        bool is_struct_initializer;

        mxDynamicArray *arguments;
        mxDynamicArray *body;
    } FunctionDeclaration;

    struct {
        char *argument_name;
        struct AST *argument_type;
    } FunctionDeclarationArgument;

    struct {
        char *function_call_name;
        mxDynamicArray *arguments;
    } FunctionCall;

    struct {
        char *name;
        bool is_union;

        mxDynamicArray *members;

        bool is_private;
    } StructOrUnionDeclaration;

    struct {
        struct AST *struct_or_variable_name;
        struct AST *expression;
    } StructOrUnionMemberCall;

    struct {
        mxDynamicArray *arguments;
        mxDynamicArray *body;
    } StructInitializer;

    struct {
        char *enum_name;
        mxDynamicArray *cases;
        bool is_private;
    } EnumDeclaration;

    struct {
        char *case_name;
        int case_value;
    } EnumItem;

    //
    struct {
        struct AST *left;
        Token *operator;
        struct AST *right;
        char *from;
    } Binary;

    struct {
        Token *literal_value;
    } Literal;

    struct {
        struct AST *expression;
    } Grouping;

    struct {
        Token *operator;
        struct AST *right;
    } Unary;

    struct {
        struct AST *return_expression;
    } Return;

    struct {
        struct AST *if_condition;
        struct AST *if_body;

        mxDynamicArray *else_if_conditions;
        mxDynamicArray *else_if_bodies;

        bool has_else_statement;
        mxDynamicArray *else_body;
    } IfElseStatement;

    struct {
        struct AST *expression;
        struct AST *body;
    } WhileStatement;

    struct {
        struct AST *variable;
        struct AST *condition;
        struct AST *condition2;

        mxDynamicArray *body;
    } ForLoop;

    struct {
        mxDynamicArray *instructions;
    } InlineAssembly;

    struct {
        struct AST *expression;
        mxDynamicArray *switch_cases;
        bool is_matches_statement;
    } SwitchStatement;

    struct {
        bool is_default;

        struct AST *expression;
        mxDynamicArray *body;
    } SwitchCase;

    struct {
        // Do Statement
        mxDynamicArray *do_body;

        // Catch Or While Statement
        mxDynamicArray *second_body;

        bool is_while_statement;
        struct AST *while_statement_expression;
    } DoCatchOrWhileStatement;

    struct {
        struct AST *expression;
    } TryStatement;

    struct {
        struct AST **body;
        uintptr_t body_size;
    } Body;

    struct {
        char *alias_name;
        struct AST *alias_value;
    } Alias;
} ASTValue;

typedef struct AST {
    ASTType type;
    ASTValue value;
} AST;

AST *ast_init(void);

AST *ast_init_with_type(ASTType type);