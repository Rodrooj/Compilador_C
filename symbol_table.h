//
// Created by rodri on 17/11/2024.
//

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>
#include "tokens.h"

typedef enum {
    TYPE_INTEGER,
    TYPE_REAL,
    TYPE_BOOLEAN,
    TYPE_PROCEDURE,
    TYPE_UNKNOWN
} DataType;

typedef struct Symbol {
    char *name;
    DataType type;
    int scope;
    struct Symbol *next;
} Symbol;

typedef struct {
    Symbol *head;
    int current_scope;
} SymbolTable;

void initSymbolTable(SymbolTable *table);
bool addSymbol(SymbolTable *table, const char *name, DataType type);
Symbol* findSymbol(SymbolTable *table, const char *name);
void freeSymbolTable(SymbolTable *table);

#endif