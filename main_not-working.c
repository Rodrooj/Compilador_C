#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tokens.h"
/*
// Linked list node for tokens
typedef struct tokenNode {
    Token token;
    struct tokenNode *next;
} tokenNode;

// Linked list for managing tokens
typedef struct {
    tokenNode *head;
    tokenNode *tail;
    tokenNode *current;  // For parsing
} TokenList;

// Function to initialize the token list
void initTokenList(TokenList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->current = NULL;
}

// Function to create a new token node
tokenNode *createTokenNode(TokenType type, const char *lexeme, int line, int column) {
    tokenNode *newNode = (tokenNode *)malloc(sizeof(tokenNode));
    if (!newNode) {
        fprintf(stderr, "Memory allocation error\n");
        exit(EXIT_FAILURE);
    }
    newNode->token.type = type;
    newNode->token.lexeme = strdup(lexeme);
    if (!newNode->token.lexeme) {
        fprintf(stderr, "Memory allocation error for lexeme\n");
        free(newNode);
        exit(EXIT_FAILURE);
    }
    newNode->token.line = line;
    newNode->token.column = column;
    newNode->next = NULL;
    printf("Debug: Created token: Type = %d, Lexeme = %s, Line = %d, Column = %d\\n",
           type, lexeme, line, column);
    return newNode;
}

// Function to add a token to the list
void addToken(TokenList *list, TokenType type, const char *lexeme, int line, int column) {
    tokenNode *node = createTokenNode(type, lexeme, line, column);
    if (!list->head) {
        list->head = list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
}

// Function to free the token list
void freeTokenList(TokenList *list) {
    tokenNode *current = list->head;
    while (current) {
        tokenNode *temp = current;
        current = current->next;
        free(temp->token.lexeme);
        free(temp);
    }
}

// Function to advance to the next token during parsing
Token *getNextToken(TokenList *list) {
    if (!list->current) {
        list->current = list->head;
    } else {
        list->current = list->current->next;
    }
    return list->current ? &list->current->token : NULL;
}

// Function to perform lexical analysis
void tokenizeSource(const char *source, TokenList *list) {
    int line = 1, column = 1;
    char buffer[256];
    int bufferIndex = 0;

    for (const char *c = source; *c; ++c) {
        if (*c == ' ' || *c == '\t' || *c == '\n' || *c == ';' || *c == ':' || *c == ',' || *c == '(' || *c == ')' || *c == '.') {
            // If buffer has content, process it
            if (bufferIndex > 0) {
                buffer[bufferIndex] = '\0';  // Null-terminate the buffer

                // Check if the buffer is a keyword, identifier, or literal
                if (strcmp(buffer, "program") == 0) {
                    addToken(list, TOKEN_PROGRAM, buffer, line, column - bufferIndex);
                } else if (strcmp(buffer, "var") == 0) {
                    addToken(list, TOKEN_VAR, buffer, line, column - bufferIndex);
                } else if (strcmp(buffer, "begin") == 0) {
                    addToken(list, TOKEN_BEGIN, buffer, line, column - bufferIndex);
                } else if (strcmp(buffer, "end") == 0) {
                    addToken(list, TOKEN_END, buffer, line, column - bufferIndex);
                } else if (strcmp(buffer, "integer") == 0) {
                    addToken(list, TOKEN_INTEGER, buffer, line, column - bufferIndex);
                } else {
                    // Default to identifier
                    addToken(list, TOKEN_IDENTIFIER, buffer, line, column - bufferIndex);
                }

                bufferIndex = 0;  // Reset buffer
            }

            // Process the current character as a separate token if necessary
            if (*c == ';') {
                addToken(list, TOKEN_SEMICOLON, ";", line, column);
            } else if (*c == ':') {
                addToken(list, TOKEN_COLON, ":", line, column);
            } else if (*c == ',') {
                addToken(list, TOKEN_COMMA, ",", line, column);
            } else if (*c == '(') {
                addToken(list, TOKEN_LPAREN, "(", line, column);
            } else if (*c == ')') {
                addToken(list, TOKEN_RPAREN, ")", line, column);
            } else if (*c == '.') {
                addToken(list, TOKEN_DOT, ".", line, column);
            }

            if (*c == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
        } else {
            // Accumulate character into buffer
            if (bufferIndex < sizeof(buffer) - 1) {
                buffer[bufferIndex++] = *c;
            } else {
                fprintf(stderr, "Token buffer overflow at line %d, column %d\n", line, column);
                exit(EXIT_FAILURE);
            }
            column++;
        }
    }

    // Add EOF token
    addToken(list, TOKEN_EOF, "EOF", line, column);
}

// Function to perform syntactic analysis
int parseProgram(TokenList *list, FILE *logFile) {
    Token *token = getNextToken(list);
    if (!token || token->type != TOKEN_PROGRAM) {
        fprintf(logFile, "Syntax Error: Expected 'program' at line %d, column %d\n", token->line, token->column);
        return 0;
    }
    fprintf(logFile, "Parsed 'program'\n");

    // Further parsing logic here...
    return 1;
}

// Function to perform semantic analysis (example implementation)
int semanticAnalysis(TokenList *list, FILE *logFile) {
    // Placeholder: Extend with real semantic analysis logic
    fprintf(logFile, "Semantic analysis: Not yet implemented\\n");
    return 1;
}

// Main function
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <source_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Open source file
    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    // Read file content into a buffer
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = (char *)malloc(length + 1);
    if (!buffer) {
        fprintf(stderr, "Memory allocation error\\n");
        fclose(file);
        return EXIT_FAILURE;
    }
    fread(buffer, 1, length, file);
    buffer[length] = '\0';
    fclose(file);

    // Initialize token list
    TokenList tokenList;
    initTokenList(&tokenList);

    // Tokenize the source code
    tokenizeSource(buffer, &tokenList);
    free(buffer);

    // Open .lex file for logging
    FILE *logFile = fopen("output.lex", "w");
    if (!logFile) {
        perror("Error opening log file");
        freeTokenList(&tokenList);
        return EXIT_FAILURE;
    }

    // Perform syntactic and semantic analysis
    int syntaxResult = parseProgram(&tokenList, logFile);
    int semanticResult = syntaxResult ? semanticAnalysis(&tokenList, logFile) : 0;

    // Print results
    if (syntaxResult && semanticResult) {
        fprintf(logFile, "Analysis completed successfully.\n");
        printf("Analysis completed successfully.\n");
    } else {
        fprintf(logFile, "Analysis failed.\n");
        printf("Analysis failed.\n");
    }

    // Close log file and free token list
    fclose(logFile);
    freeTokenList(&tokenList);

    return EXIT_SUCCESS;
}
*/