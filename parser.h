#ifndef __PARSER_H__
#define __PARSER_H__
#include "compiler.h"
#include "lexer.h"

struct insrct_list_node {
    int num;
    int target;
    std::string debug;
    struct InstructionNode* node;
    insrct_list_node(int n, std::string d, InstructionNode* in_node) {
        num = n;
        debug = d;
        node = in_node;
        target = -1;
    }
};
class Parser {
   public:
    void parse_program();
    void debug();
   private:

    LexicalAnalyzer lexer;
    void parse_var_section();
    void parse_id_list();
    void parse_body();
    void parse_stmt_list();
    void parse_stmt();
    void parse_assign_stmt();
    void parse_expr();
    Token parse_primary();
    int parse_op();
    void parse_output_stmt();
    void parse_input_stmt();
    void parse_while_stmt();
    void parse_if_stmt();
    void parse_condidtion();
    int parse_relop();
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