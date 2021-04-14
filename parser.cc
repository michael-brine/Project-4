#include "parser.h"

#include <iostream>
#include <unordered_map>
int list_count = 0;
// int brace_count = 0;
InstructionNode* head = nullptr;
std::vector<insrct_list_node*> test_list;  // debugging fun
std::unordered_map<std::string, int> var_index;

void Parser::debug() {
    std::cout << "size: " << test_list.size() << std::endl;
    for (int i = 0; i < test_list.size(); i++) {
        std::cout << "type " << test_list.at(i)->node->type;
        std::cout << " num: " << test_list.at(i)->num << " ["
                  << test_list.at(i)->debug << "] ";
        if (test_list.at(i)->target != -1) {
            std::cout << " --> " << test_list.at(i)->target << " ["
                      << test_list.at(test_list.at(i)->target)->debug << "]";
        }
        std::cout << std::endl;
    }
}
struct InstructionNode* parse_generate_intermediate_representation() {
    Parser parser;
    parser.parse_program();
    std::vector<insrct_list_node*> cjmps;
    //add NOOP
    test_list.push_back(new insrct_list_node(test_list.size(), 0, "NOOP", new InstructionNode));
    test_list.at(test_list.size() - 1)->node->type = NOOP;
    test_list.at(test_list.size() - 1)->node->next = NULL;

    for (int i = 0; i < test_list.size() - 1; i++) {
        if(test_list.at(i)->goto_flag == 0) {
            test_list.at(i)->node->next = test_list.at(i + 1)->node;
        }
        if (test_list.at(i)->node->type == CJMP) {
            test_list.at(i)->node->cjmp_inst.target = test_list.at(test_list.at(i)->target)->node;
        }
    }
    //parser.debug();
    return test_list.at(0)->node;
}
void Parser::parse_program() {
    parse_var_section();
    parse_body();
    parse_inputs();
}
void Parser::parse_var_section() {
    parse_id_list();
    expect(SEMICOLON);
}
void Parser::parse_id_list() {
    var_index[expect(ID).lexeme] = next_available;
    mem[next_available] = 0;
    next_available++;
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
            syntax_error();
    }
}
void Parser::parse_assign_stmt() {
    InstructionNode* n = new InstructionNode;
    std::string debug, temp;
    n->type = ASSIGN;
    temp = expect(ID).lexeme;
    n->assign_inst.left_hand_side_index = var_index[temp];
    debug += temp;
    expect(EQUAL);
    debug += " = ";
    switch (lexer.peek(2).token_type) {
        case SEMICOLON:
            n->assign_inst.op = OPERATOR_NONE;
            temp = parse_primary().lexeme;
            n->assign_inst.operand1_index = var_index[temp];
            debug += temp;
            break;
        default:
            temp = parse_primary().lexeme;
            n->assign_inst.operand1_index = var_index[temp];
            debug += temp;
            int t = parse_op();
            switch (t) {
                case 124:
                    debug += " + ";
                    break;
                case 125:
                    debug += " - ";
                    break;
                case 126:
                    debug += " * ";
                    break;
                case 127:
                    debug += " / ";
                    break;
            }
            n->assign_inst.op = (ArithmeticOperatorType)t;
            temp = parse_primary().lexeme;
            n->assign_inst.operand2_index = var_index[temp];
            debug += temp;
    }
    expect(SEMICOLON);
    test_list.push_back(new insrct_list_node(list_count++, 0, debug, n));
}
void Parser::parse_output_stmt() {
    InstructionNode* n = new InstructionNode;
    std::string temp, debug;
    n->type = OUT;
    debug += expect(OUTPUT).lexeme;
    temp = expect(ID).lexeme;
    n->input_inst.var_index = var_index[temp];
    expect(SEMICOLON);
    debug += " " + temp;
    test_list.push_back(new insrct_list_node(list_count++, 0, debug, n));
}
void Parser::parse_input_stmt() {
    InstructionNode* n = new InstructionNode;
    std::string temp, debug;
    n->type = IN;
    debug += expect(INPUT).lexeme;
    temp = expect(ID).lexeme;
    n->input_inst.var_index = var_index[temp];
    expect(SEMICOLON);
    debug += " " + temp;
    test_list.push_back(new insrct_list_node(list_count++, 0, debug, n));
}
void Parser::parse_expr() {
    // std::cout << "EXPR" << std::endl;
    parse_primary();
    parse_op();
    parse_primary();
}
Token Parser::parse_primary() {
    // std::cout << "PRIMARY" << std::endl;
    Token t;
    switch (lexer.peek(1).token_type) {
        case ID:
            return lexer.GetToken();
        case NUM:
            t = lexer.GetToken();
            var_index[t.lexeme] = next_available;
            mem[next_available] = stoi(t.lexeme);
            next_available++;
            return t;
        default:
            syntax_error();
    }
}
int Parser::parse_op() {
    // std::cout << "OP" << std::endl;
    switch (lexer.peek(1).token_type) {
        case PLUS:
            lexer.GetToken();
            return 124;
        case MINUS:
            lexer.GetToken();
            return 125;
        case MULT:
            lexer.GetToken();
            return 126;
        case DIV:
            lexer.GetToken();
            return 127;
        default:
            syntax_error();
    }
    return -1;
}
void Parser::parse_while_stmt() {
    InstructionNode* n = new InstructionNode;
    std::string debug, temp;
    n->type = CJMP;
    debug += expect(WHILE).lexeme + " ";
    //parse_condidtion();
    temp = parse_primary().lexeme;
    n->cjmp_inst.operand1_index = var_index[temp];
    debug += temp;
    int t = parse_relop();
    switch (t) {
        case 345:
            debug += " > ";
            break;
        case 346:
            debug += " < ";
            break;
        case 347:
            debug += " != ";
            break;
    }
    n->cjmp_inst.condition_op = (ConditionalOperatorType)t;
    temp = parse_primary().lexeme;
    n->cjmp_inst.operand2_index = var_index[temp];
    debug += temp;
    test_list.push_back(new insrct_list_node(list_count++, 0, debug, n));
    t = test_list.size() - 1;
    parse_body();
    test_list.at(test_list.size() - 1)->goto_flag = 1;
    test_list.at(test_list.size() - 1)->debug += " -> " + test_list.at(t)->debug + " {GOTO}";
    test_list.at(test_list.size() - 1)->node->next = test_list.at(t)->node;
    test_list.at(t)->target = list_count;
}
void Parser::parse_if_stmt() {
    InstructionNode* n = new InstructionNode;
    std::string debug, temp;
    n->type = CJMP;
    debug += expect(IF).lexeme + " ";
    //parse_condidtion();
    temp = parse_primary().lexeme;
    n->cjmp_inst.operand1_index = var_index[temp];
    debug += temp;
    int t = parse_relop();
    switch (t) {
        case 345:
            debug += " > ";
            break;
        case 346:
            debug += " < ";
            break;
        case 347:
            debug += " != ";
            break;
    }
    n->cjmp_inst.condition_op = (ConditionalOperatorType)t;
    temp = parse_primary().lexeme;
    n->cjmp_inst.operand2_index = var_index[temp];
    debug += temp;
    test_list.push_back(new insrct_list_node(list_count++, 0, debug, n));
    t = test_list.size() - 1;
    parse_body();
    test_list.at(t)->target = list_count;
}
void Parser::parse_condidtion() {
    // std::cout << "CONDITION" << std::endl;
    parse_primary();
    parse_relop();
    parse_primary();
}
int Parser::parse_relop() {
    // std::cout << "RELOP" << std::endl;
    switch (lexer.peek(1).token_type) {
        case GREATER:
            lexer.GetToken();
            return 345;
        case LESS:
            lexer.GetToken();
            return 346;
        case NOTEQUAL:
            lexer.GetToken();
            return 347;
        default:
            syntax_error();
    }
    return -1;
}
void Parser::parse_switch_stmt() {
    // std::cout << "SWITCH" << std::endl;
    expect(SWITCH);
    expect(ID);
    expect(LBRACE);
    parse_case_list();
    if (lexer.peek(1).token_type == DEFAULT) {
        parse_defualt_case();
    }
    expect(RBRACE);
}
void Parser::parse_for_stmt() {
    // std::cout << "FOR" << std::endl;
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
    // std::cout << "CASE LIST" << std::endl;
    parse_case();
    if (lexer.peek(1).token_type == CASE) {
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
    // std::cout << "DEFUALT" << std::endl;
    expect(DEFAULT);
    expect(COLON);
    parse_body();
}
void Parser::parse_inputs() {
    // std::cout << "INPUTS" << std::endl;
    parse_num_list();
}
void Parser::parse_num_list() {
    // std::cout << "NUM LIST" << std::endl;
    Token t = expect(NUM);
    inputs.push_back(stoi(t.lexeme));
    if (lexer.peek(1).token_type == NUM) {
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