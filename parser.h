#ifndef __PARSER_H__
#define __PARSER_H__
#include "compiler.h"
#include "lexer.h"

class Parser {
   public:
    void parse_program();
   private:
    LexicalAnalyzer lexer;
    void parse_var_section();
    void parse_id_list();
    void parse_body();
    void parse_stmt_list();
    void parse_stmt();
    void parse_assign_stmt();
    void parse_expr();
    void parse_primary();
    void parse_op();
    void parse_output_stmt();
    void parse_input_stmt();
    void parse_while_stmt();
    void parse_if_stmt();
    void parse_condidtion();
    void parse_relop();
    void parse_switch_stmt();
    void parse_for_stmt();
    void parse_case_list();
    void parse_case();
    void parse_defualt_case();
    void parse_inputs();
    void parse_num_list();
    void syntax_error();
    Token expect(TokenType expected_type);
};
#endif