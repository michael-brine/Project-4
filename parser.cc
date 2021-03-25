#include "parser.h"

#include <iostream>

struct InstructionNode* Parser::parse_generate_intermediate_representation() {
    parse_program();
}
void Parser::parse_program() {
    parse_var_section();
    parse_body();
    parse_inputs();
    expect(END_OF_FILE);
}
void Parser::parse_var_section() {
    parse_id_list();
    expect(ID);
}
void Parser::parse_id_list() {
    expect(ID);
    if (lexer.peek(1).token_type == COMMA) {
        expect(COMMA);
        parse_id_list();
    }
}
void Parser::parse_body() {
    expect(LBRACE);
    parse_stmt_list();
    expect(RBRACE);
}
void Parser::parse_stmt_list() {
    parse_stmt();
    if (lexer.peek(1).token_type != RBRACE) {
        parse_stmt_list();
    }
}
void Parser::parse_stmt() {
    switch (lexer.peek(1).token_type) {
        case ID:
            parse_assign_stmt();
            break;
        case WHILE:
            parse_while_stmt();
            break;
        case IF:
            parse_if_stmt();
            break;
        case SWITCH:
            parse_switch_stmt();
            break;
        case FOR:
            parse_for_stmt();
            break;
        case OUTPUT:
            parse_output_stmt();
            break;
        case INPUT:
            parse_input_stmt();
            break;
        default:
            std::cout << "DEAD" << std::endl;
            syntax_error();
    }
}
void Parser::parse_assign_stmt() {
    expect(ID);
    expect(EQUAL);
    if (lexer.peek(2).token_type != SEMICOLON) {
        parse_expr();
    } else {
        parse_primary();
    }
    expect(SEMICOLON);
}
void Parser::parse_expr() {
    parse_primary();
    parse_op();
    parse_primary();
}
void Parser::parse_primary() {
    switch (lexer.peek(1).token_type) {
        case ID:
            lexer.GetToken();
            break;
        case NUM:
            lexer.GetToken();
            break;
        default:
            syntax_error();
    }
}
void Parser::parse_op() {
    switch (lexer.peek(1).token_type) {
        case PLUS:
            lexer.GetToken();
            break;
        case MINUS:
            lexer.GetToken();
            break;
        case MULT:
            lexer.GetToken();
            break;
        case DIV:
            lexer.GetToken();
            break;
        default:
            syntax_error();
    }
}
void Parser::parse_output_stmt() {
    expect(OUTPUT);
    expect(ID);
    expect(SEMICOLON);
}
void Parser::parse_input_stmt() {
    expect(INPUT);
    expect(ID);
    expect(SEMICOLON);
}
void Parser::parse_while_stmt() {
    expect(WHILE);
    parse_condidtion();
    parse_body();
}
void Parser::parse_if_stmt() {
    expect(IF);
    parse_condidtion();
    parse_body();
}
void Parser::parse_condidtion() {
    parse_primary();
    parse_relop();
    parse_primary();
}
void Parser::parse_relop() {
    switch(lexer.peek(1).token_type) {
        case GREATER:
            lexer.GetToken();
            break;
        case LESS:
            lexer.GetToken();
            break;
        case NOTEQUAL:
            lexer.GetToken();
            break;
        default:
            syntax_error();
    }
}
void Parser::parse_switch_stmt() {
    expect(SWITCH);
    expect(ID);
    expect(LBRACE);
    parse_case_list();
    if(lexer.peek(1).token_type == DEFAULT) {
        parse_defualt_case();
    }
    expect(RBRACE);
}
void Parser::parse_for_stmt() {
    expect(FOR);
    expect(LPAREN);
    parse_assign_stmt();
    parse_condidtion();
    expect(SEMICOLON);
    parse_assign_stmt();
    expect(RPAREN);
    parse_body();
}
void Parser::parse_case_list() {
    parse_case();
    if(lexer.peek(1).token_type == CASE) {
        parse_case_list();
    }
}
void Parser::parse_case() {
    expect(CASE);
    expect(NUM);
    expect(COLON);
    parse_body();
}
void Parser::parse_defualt_case() {
    expect(DEFAULT);
    expect(COLON);
    parse_body();
}
void Parser::parse_inputs() {
    parse_num_list();
}
void Parser::parse_num_list() {
    expect(NUM);
    if(lexer.peek(1).token_type == NUM) {
        parse_num_list();
    }
}
void Parser::syntax_error() {
    std::cout << "SYNTAX ERROR\n";
    exit(1);
}
Token Parser::expect(TokenType expected_type) {
    Token t = lexer.GetToken();
    if (t.token_type != expected_type) syntax_error();
    return t;
}