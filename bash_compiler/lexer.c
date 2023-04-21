#include <stdio.h>
#include <ctype.h>

#define MAX_TOKEN_LEN 100

// Token types
enum token_type {
    TOKEN_NONE,
    TOKEN_WORD,
    TOKEN_NEWLINE,
    TOKEN_PIPE,
    TOKEN_AND,
    TOKEN_SEMICOLON
};

// Token structure
struct token {
    enum token_type type;
    char value[MAX_TOKEN_LEN + 1];
};

// Function to get the next character from the input stream
int get_char() {
    static int c = '\n'; // Initialize with newline to start on a new line
    return c = getchar();
}

// Function to put a character back into the input stream
void unget_char(int c) {
    ungetc(c, stdin);
}

// Function to skip whitespace characters
void skip_whitespace() {
    int c;
    while ((c = get_char()) != EOF && isspace(c));
    unget_char(c);
}

// Function to read a word token
void read_word(struct token *tok) {
    int i = 0;
    int c;
    while ((c = get_char()) != EOF && !isspace(c) && c != '|' && c != '&' && c != ';' && c != '\n') {
        if (i < MAX_TOKEN_LEN) {
            tok->value[i++] = c;
        }
    }
    unget_char(c);
    tok->value[i] = '\0';
    tok->type = TOKEN_WORD;
}

// Function to read a newline token
void read_newline(struct token *tok) {
    tok->type = TOKEN_NEWLINE;
}

// Function to read a pipe token
void read_pipe(struct token *tok) {
    tok->type = TOKEN_PIPE;
}

// Function to read an and token
void read_and(struct token *tok) {
    int c = get_char();
    if (c == '&') {
        tok->type = TOKEN_AND;
    } else {
        unget_char(c);
        tok->type = TOKEN_WORD;
        tok->value[0] = '&';
        tok->value[1] = '\0';
    }
}

// Function to read a semicolon token
void read_semicolon(struct token *tok) {
    tok->type = TOKEN_SEMICOLON;
}

// Function to get the next token from the input stream
void get_token(struct token *tok) {
    skip_whitespace();
    int c = get_char();
    switch (c) {
        case EOF:
            tok->type = TOKEN_NONE;
            break;
        case '\n':
            read_newline(tok);
            break;
        case '|':
            read_pipe(tok);
            break;
        case '&':
            read_and(tok);
            break;
        case ';':
            read_semicolon(tok);
            break;
        default:
            unget_char(c);
            read_word(tok);
            break;
    }
}

// Main function for testing
int main() {
    struct token tok;
    while (tok.type != TOKEN_NONE) {
        get_token(&tok);
        printf("%s\n", tok.value);
    }
    return 0;
}
