#include "parser.h"

#include <iostream>

struct InstructionNode* parse_generate_intermediate_representation() {
    Parser parser;
    parser.parse_program();
}
void Parser::parse_program() {
    std::cout << "PROGRAM" << std::endl;
    parse_var_section();
    parse_body();
    parse_inputs();
    expect(END_OF_FILE);
    std::cout << "DONE" << std::endl;
}
void Parser::parse_var_section() {
    std::cout << "VAR SECTION" << std::endl;
    parse_id_list();
    expect(SEMICOLON);
}
void Parser::parse_id_list() {
    std::cout << "ID LIST" << std::endl;
    expect(ID);
    if (lexer.peek(1).token_type == COMMA) {
        expect(COMMA);
        parse_id_list();
    }
}
void Parser::parse_body() {
    std::cout << "BODY" << std::endl;
    expect(LBRACE);
    parse_stmt_list();
    expect(RBRACE);
}
void Parser::parse_stmt_list() {
    std::cout << "STMT LIST" << std::endl;
    parse_stmt();
    if (lexer.peek(1).token_type != RBRACE) {
        parse_stmt_list();
    }
}
void Parser::parse_stmt() {
    std::cout << "STMT" << std::endl;
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
    std::cout << "ASSIGN" << std::endl;
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
    std::cout << "EXPR" << std::endl;
    parse_primary();
    parse_op();
    parse_primary();
}
void Parser::parse_primary() {
    std::cout << "PRIMARY" << std::endl;
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
    std::cout << "OP" << std::endl;
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
    std::cout << "OUTPUT" << std::endl;
    expect(OUTPUT);
    expect(ID);
    expect(SEMICOLON);
}
void Parser::parse_input_stmt() {
    std::cout << "INPUT" << std::endl;
    expect(INPUT);
    expect(ID);
    expect(SEMICOLON);
}
void Parser::parse_while_stmt() {
    std::cout << "WHILE" << std::endl;
    expect(WHILE);
    parse_condidtion();
    parse_body();
}
void Parser::parse_if_stmt() {
    std::cout << "IF" << std::endl;
    expect(IF);
    parse_condidtion();
    parse_body();
}
void Parser::parse_condidtion() {
    std::cout << "CONDITION" << std::endl;
    parse_primary();
    parse_relop();
    parse_primary();
}
void Parser::parse_relop() {
    std::cout << "RELOP" << std::endl;
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
    std::cout << "SWITCH" << std::endl;
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
    std::cout << "FOR" << std::endl;
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
    std::cout << "CASE LIST" << std::endl;
    parse_case();
    if(lexer.peek(1).token_type == CASE) {
        parse_case_list();
    }
}
void Parser::parse_case() {
    std::cout << "CASE" << std::endl;
    expect(CASE);
    expect(NUM);
    expect(COLON);
    parse_body();
}
void Parser::parse_defualt_case() {
    std::cout << "DEFUALT" << std::endl;
    expect(DEFAULT);
    expect(COLON);
    parse_body();
}
void Parser::parse_inputs() {
    std::cout << "INPUTS" << std::endl;
    parse_num_list();
}
void Parser::parse_num_list() {
    std::cout << "NUM LIST" << std::endl;
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