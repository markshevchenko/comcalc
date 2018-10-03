#ifndef __PARSER_H__
#define __PARSER_H__

#include <istream>
#include <set>
#include <vector>

#include "ast.h"
#include "scanner.h"

class parser
{
private:
    scanner scanner;

public:
    parser(std::istream& in): scanner(in) {
    }

    ast_program* parse();

protected:
    void expect(lexem lexem);

    void expect(lexem lexem, std::string* token);

    bool skip(lexem lexem);

    bool skip(lexem lexem, std::string* token);

    std::set<std::string> get_parameters();

    const ast_expression* get_expression();

    const ast_expression* get_operand1();

    const ast_expression* get_operator_separated_operands1(const ast_expression* left);

    const ast_expression* get_operand2();

    const ast_expression* get_operator_separated_operands2(const ast_expression* left);

    const ast_expression* get_operand3();

    const ast_expression* get_operand4();

	const ast_logical_expression* get_logical_expression();

	const ast_logical_expression* get_logical_operand1();

	const ast_logical_expression* get_operator_separated_logical_operands1(const ast_logical_expression* left);

	const ast_logical_expression* get_logical_operand2();

	const ast_logical_expression* get_operator_separated_logical_operands2(const ast_logical_expression* left);

	const ast_logical_expression* get_logical_operand3();

	const ast_logical_expression* get_condition();
};

#endif