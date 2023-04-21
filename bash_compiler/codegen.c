#include "codegen.h"
#include "error.h"
#include "symboltable.h"
#include <stdlib.h>

// Forward declarations
static void generate_ast(AST* ast);
static void generate_command(AST* ast);
static void generate_pipeline(AST* ast);
static void generate_redirection(AST* ast);
static void generate_simple_command(AST* ast);
static void generate_word_list(AST* ast);
static void generate_word(AST* ast);
static void generate_variable(AST* ast);
static void generate_string(AST* ast);
static void generate_command_substitution(AST* ast);

// Codegen state
static SymbolTable* symbol_table;
static int next_label;

// Generate machine code for an AST
void generate_code(AST* ast) {
    symbol_table = create_symbol_table();
    next_label = 0;

    generate_ast(ast);

    free_symbol_table(symbol_table);
}

// Generate machine code for an AST node
static void generate_ast(AST* ast) {
    switch (ast->type) {
        case AST_COMMAND_SEQUENCE:
            generate_command_sequence(ast);
            break;
        case AST_PIPELINE:
            generate_pipeline(ast);
            break;
        case AST_COMMAND:
            generate_command(ast);
            break;
        case AST_REDIRECTION:
            generate_redirection(ast);
            break;
        case AST_SIMPLE_COMMAND:
            generate_simple_command(ast);
            break;
        case AST_WORD_LIST:
            generate_word_list(ast);
            break;
        case AST_WORD:
            generate_word(ast);
            break;
        case AST_VARIABLE:
            generate_variable(ast);
            break;
        case AST_STRING:
            generate_string(ast);
            break;
        case AST_COMMAND_SUBSTITUTION:
            generate_command_substitution(ast);
            break;
        default:
            // Should never happen
            break;
    }
}

// Generates code for a pipeline
static void codegen_pipeline(AST* node) {
    codegen(node->left);
    emit_instr(INSTR_PIPE);
    codegen(node->right);
}

// Generates code for a redirection
static void codegen_redirection(AST* node) {
    if (node->type == AST_INPUT_REDIRECTION) {
        emit_instr_arg(INSTR_INPUT_REDIRECTION, node->str_value);
    } else if (node->type == AST_OUTPUT_REDIRECTION) {
        emit_instr_arg(INSTR_OUTPUT_REDIRECTION, node->str_value);
    } else if (node->type == AST_APPEND_REDIRECTION) {
        emit_instr_arg(INSTR_APPEND_REDIRECTION, node->str_value);
    }
    codegen(node->left);
}

// Generates code for a simple command
static void codegen_simple_command(AST* node) {
    // Generate code for the arguments
    AST* arg_list = node->left;
    while (arg_list) {
        codegen(arg_list->left);
        arg_list = arg_list->right;
    }
    // Generate code for the command name
    codegen(node->right);
    emit_instr_arg(INSTR_CALL_FUNCTION, arg_list_length(node->left));
}

// Generates code for a variable
static void codegen_variable(AST* node) {
    emit_instr_arg(INSTR_LOAD_VARIABLE, add_string_to_pool(node->str_value));
}

// Generates code for a string
static void codegen_string(AST* node) {
    emit_instr_arg(INSTR_LOAD_STRING, add_string_to_pool(node->str_value));
}

// Generates code for a command substitution
static void codegen_command_substitution(AST* node) {
    // Generate code for the command
    codegen(node->left);
    // Call the function with no arguments
    emit_instr_arg(INSTR_CALL_FUNCTION, 0);
}

// Generates code for an AST node
void codegen(AST* node) {
    if (!node) {
        return;
    }
    switch (node->type) {
        case AST_COMMAND_SEQUENCE:
            codegen(node->left);
            codegen(node->right);
            break;
        case AST_PIPELINE:
            codegen_pipeline(node);
            break;
        case AST_REDIRECTION:
            codegen_redirection(node);
            break;
        case AST_SIMPLE_COMMAND:
            codegen_simple_command(node);
            break;
        case AST_WORD:
            codegen(node->left);
            break;
        case AST_VARIABLE:
            codegen_variable(node);
            break;
        case AST_STRING:
            codegen_string(node);
            break;
        case AST_COMMAND_SUBSTITUTION:
            codegen_command_substitution(node);
            break;
        default:
            fprintf(stderr, "Error: invalid AST node type: %d\n", node->type);
            exit(1);
    }
}
