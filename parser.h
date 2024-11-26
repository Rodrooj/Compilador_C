//
// Created by rodri on 17/11/2024.
//

#ifndef PARSER_H
#define PARSER_H

#include "tokens.h"
#include "symbol_table.h"

typedef struct {
    TokenList *tokens;
    TokenNode *current_token;
    SymbolTable *symbol_table;
    FILE *output_file;
    int error_count;
} Parser;

void initParser(Parser *parser, TokenList *tokens, FILE *output_file);
bool parse(Parser *parser);
void freeParser(const Parser *parser);

#endif