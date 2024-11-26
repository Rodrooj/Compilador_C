//
// Created by rodri on 17/11/2024.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "tokens.h"
#include "parser.h"
#include "symbol_table.h"

static void advance(Parser *parser) {
    if (parser->current_token) {
        parser->current_token = parser->current_token->next;
    }
}

static bool expect(Parser *parser, TokenType type) {
    if (parser->current_token && parser->current_token->token.type == type) {
        advance(parser);
        return true;
    }
    fprintf(parser->output_file, "Syntax Error: Expected token type %d at line %d, column %d\n",
            type, parser->current_token ? parser->current_token->token.line : -1,
            parser->current_token ? parser->current_token->token.column : -1);
    parser->error_count++;
    return false;
}

// Function to check if the current token is an expression
static bool isExpression(Parser *parser) {
    if (!parser->current_token) return false;

    TokenType type = parser->current_token->token.type;
    // Expande para suportar mais casos se necessário
    return (type == TOKEN_IDENTIFIER ||
            type == TOKEN_INTEGER_LITERAL ||
            type == TOKEN_REAL_LITERAL ||
            type == TOKEN_LPAREN);
}


// Parse a single expression (simplified)
static bool parseExpression(Parser *parser) {
    // Simple expression parsing: identifier or literal
    if (!parser->current_token) {
        fprintf(parser->output_file, "Syntax Error: Unexpected end of input during expression parsing\n");
        parser->error_count++;
        return false;
    }

    TokenType type = parser->current_token->token.type;

    // Check for valid expression types
    if (type == TOKEN_IDENTIFIER ||
        type == TOKEN_INTEGER_LITERAL ||
        type == TOKEN_REAL_LITERAL) {
        advance(parser);
        return true;
        }

    // Support simple arithmetic expressions
    if (type == TOKEN_PLUS || type == TOKEN_MINUS) {
        advance(parser);
        return isExpression(parser);
    }

    fprintf(parser->output_file, "Syntax Error: Invalid expression at line %d, column %d\n",
            parser->current_token->token.line,
            parser->current_token->token.column);
    parser->error_count++;
    return false;
}

static bool parseVariableDeclaration(Parser *parser) {
    while (parser->current_token && parser->current_token->token.type == TOKEN_IDENTIFIER) {
        char *var_name = strdup(parser->current_token->token.lexeme);
        advance(parser);

        // Espera ':' após o identificador
        if (!expect(parser, TOKEN_COLON)) {
            free(var_name);
            return false;
        }

        DataType var_type = TYPE_UNKNOWN;

        // Identifica o tipo da variável
        if (parser->current_token) {
            if (parser->current_token->token.type == TOKEN_INTEGER) {
                var_type = TYPE_INTEGER;
            } else if (parser->current_token->token.type == TOKEN_REAL) {
                var_type = TYPE_REAL;
            } else if (parser->current_token->token.type == TOKEN_BOOLEAN) {
                var_type = TYPE_BOOLEAN;
            }
        }

        // Erro se o tipo for inválido ou ausente
        if (var_type == TYPE_UNKNOWN) {
            fprintf(parser->output_file, "Semantic Error: Invalid variable type at line %d\n",
                    parser->current_token ? parser->current_token->token.line : -1);
            parser->error_count++;
            free(var_name);
            return false;
        }

        // Adiciona à tabela de símbolos
        if (!addSymbol(parser->symbol_table, var_name, var_type)) {
            fprintf(parser->output_file, "Semantic Error: Variable %s already declared at line %d\n",
                    var_name, parser->current_token ? parser->current_token->token.line : -1);
            parser->error_count++;
        }

        free(var_name);
        advance(parser); // Avança após o tipo

        // Verifica se termina com ponto e vírgula
        if (parser->current_token && parser->current_token->token.type == TOKEN_SEMICOLON) {
            advance(parser);
        } else {
            return false; // Erro se não houver ';' ou outro token esperado
        }
    }
    return true;
}


// Parse assignment statement
static bool parseAssignmentStatement(Parser *parser) {
    // Expect an identifier (variable name)
    if (!parser->current_token || parser->current_token->token.type != TOKEN_IDENTIFIER) {
        fprintf(parser->output_file, "Syntax Error: Expected variable name in assignment\n");
        parser->error_count++;
        return false;
    }

    // Store variable name for potential semantic checks
    char *var_name = strdup(parser->current_token->token.lexeme);
    advance(parser);

    // Expect assignment operator
    if (!expect(parser, TOKEN_ASSIGN)) {
        free(var_name);
        return false;
    }

    // Expect a valid expression after assignment
    if (!isExpression(parser)) {
        fprintf(parser->output_file, "Syntax Error: Expected an expression after ':=' at line %d, column %d\n",
                parser->current_token ? parser->current_token->token.line : -1,
                parser->current_token ? parser->current_token->token.column : -1);
        parser->error_count++;
        free(var_name);
        return false;
    }

    // Parse the expression
    if (!parseExpression(parser)) {
        free(var_name);
        return false;
    }

    free(var_name);
    return true;
}


static bool parseStatementBlock(Parser *parser) {
    while (parser->current_token && parser->current_token->token.type != TOKEN_END) {
        // Handle different types of statements
        if (parser->current_token->token.type == TOKEN_IDENTIFIER) {
            // Assignment statement
            if (!parseAssignmentStatement(parser)) {
                return false;
            }

            // Expect semicolon between statements
            if (!expect(parser, TOKEN_SEMICOLON)) {
                return false;
            }
        }
        // Add support for other statement types like writeln, read, etc.
        else if (parser->current_token->token.type == TOKEN_WRITELN) {
            advance(parser);
            if (!expect(parser, TOKEN_LPAREN)) return false;

            // Parse múltiplos argumentos (simplificado)
            while (parser->current_token && parser->current_token->token.type != TOKEN_RPAREN) {
                if (!parseExpression(parser)) return false;

                // Opção: vírgula entre argumentos
                if (parser->current_token && parser->current_token->token.type == TOKEN_COMMA) {
                    advance(parser);
                }
            }

            if (!expect(parser, TOKEN_RPAREN)) return false;
            if (!expect(parser, TOKEN_SEMICOLON)) return false;
        }
        else {
            fprintf(parser->output_file, "Syntax Error: Unexpected token in statement block\n");
            parser->error_count++;
            return false;
        }
    }
    return true;
}

// static bool parseProgram(Parser *parser) {
//     // Parse program header
//     if (!expect(parser, TOKEN_PROGRAM)) return false;
//     if (!expect(parser, TOKEN_IDENTIFIER)) return false;
//     if (!expect(parser, TOKEN_SEMICOLON)) return false;
//
//     // Parse variable declarations if present
//     if (parser->current_token && parser->current_token->token.type == TOKEN_VAR) {
//         advance(parser);
//         if (!parseVariableDeclaration(parser)) return false;
//     }
//
//     // Parse main program block
//     if (!expect(parser, TOKEN_BEGIN)) return false;
//
//     // Parse statements
//     if (!parseStatementBlock(parser)) return false;
//
//     if (!expect(parser, TOKEN_END)) return false;
//     if (!expect(parser, TOKEN_DOT)) return false;
//
//     return true;
// }

void initParser(Parser *parser, TokenList *tokens, FILE *output_file) {
    parser->tokens = tokens;
    parser->current_token = tokens->head;
    parser->output_file = output_file;
    parser->error_count = 0;
    parser->symbol_table = (SymbolTable *)malloc(sizeof(SymbolTable));
    initSymbolTable(parser->symbol_table);
}

// bool parse(Parser *parser) {
//     return parseProgram(parser);
// }

void freeParser(const Parser *parser) {
    freeSymbolTable(parser->symbol_table);
    free(parser->symbol_table);
}
