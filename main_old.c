//
// Created by rodri on 17/11/2024.
//

// main.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "tokens.h"
/*
#define BUFFER_SIZE 1024
#define MAX_IDENTIFIER_LENGTH 255
#define MAX_ERROR_MSG_LENGTH 100

typedef struct {
    FILE* source;
    char currentChar;
    int line;
    int column;
    char buffer[BUFFER_SIZE];
    int bufferPos;
    int bufferSize;
} Scanner;

// Forward declarations
static void initScanner(Scanner* scanner, const char* filename);
static void advance(Scanner* scanner);
static void skipWhitespace(Scanner* scanner);
static void skipComment(Scanner* scanner);
static Token* handleIdentifierOrKeyword(Scanner* scanner);
static Token* handleNumber(Scanner* scanner);
static Token* handleString(Scanner* scanner);
static Token* handleOperator(Scanner* scanner);
static Token* createToken(TokenType type, char* lexeme, int line, int column);

// Keywords hash table
static ht_hash_table* keywords;

// Initialize keywords table
static void initKeywords() {
    keywords = ht_new();
    ht_insert(keywords, "program", "TOKEN_PROGRAM");
    ht_insert(keywords, "var", "TOKEN_VAR");
    ht_insert(keywords, "integer", "TOKEN_INTEGER");
    ht_insert(keywords, "real", "TOKEN_REAL");
    ht_insert(keywords, "boolean", "TOKEN_BOOLEAN");
    ht_insert(keywords, "procedure", "TOKEN_PROCEDURE");
    ht_insert(keywords, "begin", "TOKEN_BEGIN");
    ht_insert(keywords, "end", "TOKEN_END");
    ht_insert(keywords, "if", "TOKEN_IF");
    ht_insert(keywords, "then", "TOKEN_THEN");
    ht_insert(keywords, "else", "TOKEN_ELSE");
    ht_insert(keywords, "while", "TOKEN_WHILE");
    ht_insert(keywords, "do", "TOKEN_DO");
    ht_insert(keywords, "read", "TOKEN_READ");
    ht_insert(keywords, "write", "TOKEN_WRITE");
}

// Scanner initialization
static void initScanner(Scanner* scanner, const char* filename) {
    scanner->source = fopen(filename, "r");
    if (!scanner->source) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(1);
    }
    scanner->line = 1;
    scanner->column = 0;
    scanner->bufferPos = 0;
    scanner->bufferSize = 0;
    advance(scanner);
}

// Advance to next character
static void advance(Scanner* scanner) {
    if (scanner->bufferPos >= scanner->bufferSize) {
        scanner->bufferSize = (int)fread(scanner->buffer, 1, BUFFER_SIZE, scanner->source);
        scanner->bufferPos = 0;
        if (scanner->bufferSize == 0) {
            scanner->currentChar = EOF;
            return;
        }
    }

    scanner->currentChar = scanner->buffer[scanner->bufferPos++];
    scanner->column++;

    if (scanner->currentChar == '\n') {
        scanner->line++;
        scanner->column = 0;
    }
}

// Skip whitespace characters
static void skipWhitespace(Scanner* scanner) {
    while (isspace(scanner->currentChar)) {
        advance(scanner);
    }
}

// Skip comments
static void skipComment(Scanner* scanner) {
    if (scanner->currentChar == '{') {
        advance(scanner);
        while (scanner->currentChar != '}' && scanner->currentChar != EOF) {
            advance(scanner);
        }
        if (scanner->currentChar == '}') {
            advance(scanner);
        }
    }
}

// Handle identifiers and keywords
static Token* handleIdentifierOrKeyword(Scanner* scanner) {
    char lexeme[MAX_IDENTIFIER_LENGTH + 1];
    int i = 0;
    int startColumn = scanner->column;

    while (isalnum(scanner->currentChar) || scanner->currentChar == '_') {
        if (i < MAX_IDENTIFIER_LENGTH) {
            lexeme[i++] = (char)tolower(scanner->currentChar);
        }
        advance(scanner);
    }
    lexeme[i] = '\0';

    char* tokenType = ht_search(keywords, lexeme);
    if (tokenType != NULL) {
        // It's a keyword
        return createToken(strtol(tokenType), strdup(lexeme), scanner->line, startColumn);
    }

    // It's an identifier
    return createToken(TOKEN_IDENTIFIER, strdup(lexeme), scanner->line, startColumn);
}

// Handle numeric literals
static Token* handleNumber(Scanner* scanner) {
    char number[MAX_IDENTIFIER_LENGTH + 1];
    int i = 0;
    int startColumn = scanner->column;
    TokenType type = TOKEN_INTEGER_LITERAL;

    while (isdigit(scanner->currentChar)) {
        number[i++] = scanner->currentChar;
        advance(scanner);
    }

    if (scanner->currentChar == '.') {
        type = TOKEN_REAL_LITERAL;
        number[i++] = scanner->currentChar;
        advance(scanner);

        while (isdigit(scanner->currentChar)) {
            number[i++] = scanner->currentChar;
            advance(scanner);
        }
    }

    number[i] = '\0';
    return createToken(type, strdup(number), scanner->line, startColumn);
}

// Create a new token
static Token* createToken(TokenType type, char* lexeme, int line, int column) {
    Token* token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->lexeme = lexeme;
    token->line = line;
    token->column = column;
    return token;
}

// Main lexical analysis function
Token* getNextToken(Scanner* scanner) {
    while (scanner->currentChar != EOF) {
        // Skip whitespace and comments
        skipWhitespace(scanner);
        skipComment(scanner);

        // Store starting position for error reporting
        int startLine = scanner->line;
        int startColumn = scanner->column;

        if (scanner->currentChar == EOF) {
            return createToken(TOKEN_EOF, strdup("EOF"), startLine, startColumn);
        }

        // Identify token type
        if (isalpha(scanner->currentChar)) {
            return handleIdentifierOrKeyword(scanner);
        }

        if (isdigit(scanner->currentChar)) {
            return handleNumber(scanner);
        }

        // Handle operators and punctuation
        switch (scanner->currentChar) {
            case ':':
                advance(scanner);
                if (scanner->currentChar == '=') {
                    advance(scanner);
                    return createToken(TOKEN_ASSIGN, strdup(":="), startLine, startColumn);
                }
                return createToken(TOKEN_COLON, strdup(":"), startLine, startColumn);

            case '+': advance(scanner); return createToken(TOKEN_PLUS, strdup("+"), startLine, startColumn);
            case '-': advance(scanner); return createToken(TOKEN_MINUS, strdup("-"), startLine, startColumn);
            case '*': advance(scanner); return createToken(TOKEN_MULTIPLY, strdup("*"), startLine, startColumn);
            case '/': advance(scanner); return createToken(TOKEN_DIVIDE, strdup("/"), startLine, startColumn);

            // Add other operators and punctuation...

            default: {
                char errorMsg[MAX_ERROR_MSG_LENGTH];
                snprintf(errorMsg, MAX_ERROR_MSG_LENGTH, "Unexpected character: %c", scanner->currentChar);
                advance(scanner);
                return createToken(TOKEN_ERROR, strdup(errorMsg), startLine, startColumn);
            }
        }
    }

    return createToken(TOKEN_EOF, strdup("EOF"), scanner->line, scanner->column);
}

// Main function for testing
int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    // Initialize scanner and keywords table
    Scanner scanner;
    initKeywords();
    initScanner(&scanner, argv[1]);

    // Test the lexical analyzer
    Token* token;
    do {
        token = getNextToken(&scanner);
        printf("Token: %d, Lexeme: %s, Line: %d, Column: %d\n",
               token->type, token->lexeme, token->line, token->column);

        free(token->lexeme);
        free(token);
    } while (token->type != TOKEN_EOF);

    // Cleanup
    fclose(scanner.source);
    ht_del_hash_table(keywords);

    return 0;
}*/