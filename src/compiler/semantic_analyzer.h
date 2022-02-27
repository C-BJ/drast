//
//  semantic_analyzer.h
//  drast
//
//  Created by Ashwin Paudel on 2022-02-05.
//

#ifndef __DRAST_SEMANTIC_ANALYZER_H__
#define __DRAST_SEMANTIC_ANALYZER_H__

#include "ast.h"
#include "ast_print.h"
#include "../utils/unmap.h"

typedef struct {
    char *declaration_name;
    ASTType declaration_type;
    AST *declaration_value;
} SemanticAnalyzerDeclarationTable;

typedef struct {
    AST **items;
    uintptr_t item_size;
} SemanticAnalyzerASTItems;

void semantic_analyzer_run_analysis(UNMap *table);

void semantic_analyzer_check_for_duplicate_declarations(UNMap *table);

void semantic_analyzer_check_function_declarations(UNMap *table);

void semantic_analyzer_check_function_declaration_argument(UNMap *table, AST **arguments, uintptr_t argument_size);

UNMap *semantic_analyzer_create_declaration_table(SemanticAnalyzerASTItems *ast_items);

char *semantic_analyzer_declaration_name(AST *ast);

#endif //__DRAST_SEMANTIC_ANALYZER_H__