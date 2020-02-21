#ifndef __PARSER_H__
#define __PARSER_H__

#include <istream>
#include <map>
#include <string>

#include "ast.h"
#include "scanner.h"

class parser
{
private:
    scanner scanner;

public:
    parser(std::istream& in): scanner(in) {
    }

    const ast_program* parse_program();

protected:
	std::map<std::string, expression_type> parse_parameters();

	const ast_expression* parse_expression();

	const ast_expression* parse_operand1();

	const ast_expression* parse_separated_operands1(const ast_expression* left);

	const ast_expression* parse_operand2();

	const ast_expression* parse_separated_operands2(const ast_expression* left);

	const ast_expression* parse_operand3();

	const ast_expression* parse_operand4();

	const ast_logical_expression* parse_logical_expression();

	const ast_logical_expression* parse_logical_operand1();

	const ast_logical_expression* parse_separated_logical_operands1(const ast_logical_expression* left);

	const ast_logical_expression* parse_logical_operand2();

	const ast_logical_expression* parse_separated_logical_operands2(const ast_logical_expression* left);

	const ast_logical_expression* parse_logical_operand3();

	const ast_logical_expression* parse_condition();
	
	void expect(lexeme lexeme);

    void expect(lexeme lexeme, std::string* buffer);

    bool skip(lexeme lexeme);

    bool skip(lexeme lexeme, std::string* buffer);
};

#endif