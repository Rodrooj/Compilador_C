#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Para verificar caracteres
#include "tokens.h"
#include "parser.h"

static void processDelimiter(char c, int line, int column, TokenList *list);

// Inicializa a lista de tokens
void initTokenList(TokenList *list) {
    list->head = NULL;
    list->tail = NULL;
}

// Cria um novo nó de token
tokenNode *createTokenNode(TokenType type, const char *lexeme, int line, int column) {
    tokenNode *newNode = (tokenNode *)malloc(sizeof(tokenNode));
    if (!newNode) {
        fprintf(stderr, "Erro de alocação de memória ao criar token\n");
        exit(EXIT_FAILURE);
    }
    newNode->token.type = type;
    newNode->token.lexeme = strdup(lexeme);
    newNode->token.line = line;
    newNode->token.column = column;
    newNode->next = NULL;
    return newNode;
}

// Adiciona um token à lista
void addToken(TokenList *list, TokenType type, const char *lexeme, int line, int column) {
    tokenNode *node = createTokenNode(type, lexeme, line, column);
    if (!list->head) {
        list->head = list->tail = node;
    } else {
        list->tail->next = node;
        list->tail = node;
    }
}

// Libera a memória da lista de tokens
void freeTokenList(TokenList *list) {
    tokenNode *current = list->head;
    while (current) {
        tokenNode *temp = current;
        current = current->next;
        free(temp->token.lexeme);
        free(temp);
    }
}

// Processa literais e identificadores
static void processLiteral(char *buffer, const int bufferIndex, int line, int column, TokenList *list) {
    buffer[bufferIndex] = '\0'; // Finaliza o buffer

    // Verifica palavras-chave e identificadores
    if (strcmp(buffer, "program") == 0) {
        addToken(list, TOKEN_PROGRAM, buffer, line, column - bufferIndex);
    } else if (strcmp(buffer, "var") == 0) {
        addToken(list, TOKEN_VAR, buffer, line, column - bufferIndex);
    } else if (strcmp(buffer, "begin") == 0) {
        addToken(list, TOKEN_BEGIN, buffer, line, column - bufferIndex);
    } else if (strcmp(buffer, "end") == 0) {
        addToken(list, TOKEN_END, buffer, line, column - bufferIndex);
    } else if (isdigit(buffer[0])) {
        // Diferencia literais numéricos
        addToken(list, TOKEN_INTEGER_LITERAL, buffer, line, column - bufferIndex);
    } else if (strcmp(buffer, "integer") == 0) {
        addToken(list, TOKEN_INTEGER, buffer, line, column - bufferIndex);
    } else if (strcmp(buffer, "real") == 0) {
        addToken(list, TOKEN_REAL, buffer, line, column - bufferIndex);
    } else if (strcmp(buffer, "boolean") == 0) {
        addToken(list, TOKEN_BOOLEAN, buffer, line, column - bufferIndex);
    } else {
        // Identificadores genéricos
        addToken(list, TOKEN_IDENTIFIER, buffer, line, column - bufferIndex);
    }
}

// Processa delimitadores e operadores
static void processDelimiter(char c, int line, int column, TokenList *list) {
    switch (c) {
        case ';': addToken(list, TOKEN_SEMICOLON, ";", line, column); break;
        case ':': addToken(list, TOKEN_COLON, ":", line, column); break;
        case '+': addToken(list, TOKEN_PLUS, "+", line, column); break;
        case '.': addToken(list, TOKEN_DOT, ".", line, column); break;
        case '(': addToken(list, TOKEN_LPAREN, "(", line, column); break;
        case ')': addToken(list, TOKEN_RPAREN, ")", line, column); break;
        default:
            fprintf(stderr, "Erro: delimitador inesperado '%c' na linha %d, coluna %d\n", c, line, column);
            break;
    }
}

// Tokeniza o código-fonte
void tokenizeSource(const char *source, TokenList *list) {
    int line = 1, column = 1;
    char buffer[256];
    int bufferIndex = 0;

    for (const char *c = source; *c; ++c) {
        if (isspace(*c) || strchr(";,.:()+", *c)) {
            // Processa o buffer acumulado
            if (bufferIndex > 0) {
                processLiteral(buffer, bufferIndex, line, column, list);
                bufferIndex = 0;
            }
            // Processa delimitadores
            if (strchr(";,.:()+", *c)) {
                processDelimiter(*c, line, column, list);
            }
            if (*c == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
        } else {
            // Adiciona ao buffer
            if (bufferIndex < sizeof(buffer) - 1) {
                buffer[bufferIndex++] = *c;
            } else {
                fprintf(stderr, "Erro: buffer excedido na linha %d, coluna %d\n", line, column);
                exit(EXIT_FAILURE);
            }
            column++;
        }
    }
    // Processa o buffer restante
    if (bufferIndex > 0) {
        processLiteral(buffer, bufferIndex, line, column, list);
    }
    addToken(list, TOKEN_EOF, "EOF", line, column);
}

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
        fprintf(stderr, "Memory allocation error\n");
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

    FILE *output_file = fopen("output.lex", "w");
    if (!output_file) {
        perror("Error opening output file");
        freeTokenList(&tokenList);
        return EXIT_FAILURE;
    }

    // Print lexical analysis results
    fprintf(output_file, "Lexical Analysis Results:\n");
    for (tokenNode *node = tokenList.head; node; node = node->next) {
        fprintf(output_file, "Type: %-5d, Lexeme: %-30s, Line: %-5d, Column: %-5d\n",
                node->token.type, node->token.lexeme, node->token.line, node->token.column);
    }

    // Perform syntactic and semantic analysis
    //Parser parser;
    //initParser(&parser, &tokenList, output_file);

    fprintf(output_file, "\nSyntactic and Semantic Analysis Results:\n");
    // if (parse(&parser)) {
    //     if (parser.error_count == 0) {
    //         fprintf(output_file, "Analysis completed successfully with no errors.\n");
    //     } else {
    //         fprintf(output_file, "Analysis completed with %d errors.\n", parser.error_count);
    //     }
    // } else {
    //     fprintf(output_file, "Analysis failed with fatal errors.\n");
    // }

    //freeParser(&parser);
    fclose(output_file);

    // Print tokens
    printf("Tokens:\n");
    for (tokenNode *node = tokenList.head; node; node = node->next) {
        printf("Type: %-5d, Lexeme: %-30s, Line: %-5d, Column: %-5d\n",
               node->token.type, node->token.lexeme, node->token.line, node->token.column);
    }

    // Free token list
    freeTokenList(&tokenList);

    return EXIT_SUCCESS;
}
