#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variables
static Token current_token;
static Token previous_token;
static int has_error;

// Forward declarations
static void error(const char* message);
static void advance();
static void consume(TokenType type, const char* message);
static AST* program();
static AST* pipeline();
static AST* command();
static AST* redirection();
static AST* simple_command();
static AST* word_list();
static AST* word();
static AST* variable();
static AST* string();
static AST* command_substitution();

// Parses a command sequence
// Returns an AST node representing the command sequence
struct ast_node* parse_command_sequence() {
    struct ast_node* node = NULL;
    struct ast_node* prev_node = NULL;
    struct token* tok = NULL;
    int is_pipeline = 0;

    while (1) {
        // Parse the next command in the sequence
        struct ast_node* cmd_node = parse_command();
        if (!cmd_node) {
            // Error occurred while parsing command
            if (node) {
                free_ast(node);
            }
            return NULL;
        }

        if (is_pipeline) {
            // Add the command to the pipeline
            prev_node->right = cmd_node;
            prev_node = cmd_node;
        } else {
            // Create a new sequence or pipeline node
            if (!node) {
                node = create_ast_node(AST_COMMAND_SEQUENCE);
                node->left = cmd_node;
                prev_node = node->left;
            } else {
                // Check if this should be a pipeline node
                tok = get_current_token();
                if (tok && tok->type == TOKEN_PIPE) {
                    is_pipeline = 1;
                    prev_node->right = cmd_node;
                    prev_node = cmd_node;
                } else {
                    // Create a new sequence node
                    struct ast_node* new_node = create_ast_node(AST_COMMAND_SEQUENCE);
                    new_node->left = node;
                    new_node->right = cmd_node;
                    node = new_node;
                    prev_node = node->right;
                }
            }
        }

        // Check for end of command sequence
        tok = get_current_token();
        if (!tok || tok->type != TOKEN_SEMICOLON) {
            break;
        }
        next_token();
    }

    return node;
}

