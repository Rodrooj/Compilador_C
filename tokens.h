#ifndef TOKEN_H
#define TOKEN_H

// Definição de tipos de tokens
typedef enum {
    // Palavras-chave
    TOKEN_PROGRAM,         // "program"
    TOKEN_VAR,             // "var"
    TOKEN_INTEGER,         // "integer"
    TOKEN_REAL,            // "real"
    TOKEN_BOOLEAN,         // "boolean"
    TOKEN_PROCEDURE,       // "procedure"
    TOKEN_BEGIN,           // "begin"
    TOKEN_END,             // "end"
    TOKEN_IF,              // "if"
    TOKEN_THEN,            // "then"
    TOKEN_ELSE,            // "else"
    TOKEN_WHILE,           // "while"
    TOKEN_DO,              // "do"
    TOKEN_READ,            // "read"
    TOKEN_WRITE,           // "write"
    TOKEN_WRITELN,         // "writeln"

    // Operadores
    TOKEN_ASSIGN,          // ":="
    TOKEN_PLUS,            // "+"
    TOKEN_MINUS,           // "-"
    TOKEN_MULTIPLY,        // "*"
    TOKEN_DIVIDE,          // "/"
    TOKEN_EQ,              // "="
    TOKEN_NEQ,             // "<>"
    TOKEN_LT,              // "<"
    TOKEN_GT,              // ">"
    TOKEN_LTE,             // "<="
    TOKEN_GTE,             // ">="

    // Pontuação
    TOKEN_SEMICOLON,       // ";"
    TOKEN_COLON,           // ":"
    TOKEN_COMMA,           // ","
    TOKEN_DOT,             // "."
    TOKEN_LPAREN,          // "("
    TOKEN_RPAREN,          // ")"

    // Literais
    TOKEN_IDENTIFIER,      // Identificador (ex.: nomes de variáveis)
    TOKEN_INTEGER_LITERAL, // Literal inteiro
    TOKEN_REAL_LITERAL,    // Literal real
    TOKEN_BOOLEAN_LITERAL, // Literal booleano
    TOKEN_STRING_LITERAL,  // Literal de string

    // Outros
    TOKEN_EOF,             // Fim de arquivo
    TOKEN_ERROR            // Erro de tokenização
} TokenType;

// Estrutura de um token
typedef struct {
    TokenType type;    // Tipo do token
    char *lexeme;      // Lexema associado ao token
    int line;          // Linha onde o token foi encontrado
    int column;        // Coluna onde o token foi encontrado
} Token;

// Protótipos de funções
Token *create_token(TokenType type, const char *lexeme, int line, int column);
void free_token(Token *token);

typedef struct tokenNode {
    Token token;              // O token em si
    struct tokenNode *next;   // Ponteiro para o próximo nó
} tokenNode;

// Lista de tokens
typedef struct {
    tokenNode *head;          // Cabeça da lista
    tokenNode *tail;          // Cauda da lista
} TokenList;

// Linked list node for tokens
typedef struct TokenNode {
    Token token;
    struct TokenNode *next;
} TokenNode;

#endif // TOKEN_H
