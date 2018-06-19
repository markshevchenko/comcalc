#include <stdexcept>
#include <string>
#include <vector>

#include "ast.h"
#include "parser.h"
#include "scanner.h"

ast_program* parser::parse()
{
    std::vector<const ast_function*> functions;
    std::vector<const ast_assignment*> assignments;

    do {
        std::string name;
        expect(lexem::Identifier, &name);


		if (skip(lexem::LParen)) {
			std::vector<std::string> parameters = get_parameters();

			expect(lexem::RParen);
			expect(lexem::Eq);

			const ast_expression* expression = get_expression();

			ast_function* function = new ast_function(name, parameters, expression);
			functions.push_back(function);
		}
        else {
            expect(lexem::Eq);

            const ast_expression* expression = get_expression();

            ast_assignment* assignment = new ast_assignment(name, expression);
			assignments.push_back(assignment);
        }

		while (skip(lexem::NewLine))
			;
        
    } while (scanner.lexem() != lexem::Eof);

    return new ast_program(functions, assignments);
}

void parser::expect(lexem lexem)
{
    if (scanner.lexem() != lexem)
        throw new std::runtime_error("Lexem " + scanner::lexem_str(lexem) + " expected, but " + scanner::lexem_str(scanner.lexem()) + " read.");

    scanner.next();
}

void parser::expect(lexem lexem, std::string* token) {
    if (scanner.lexem() != lexem)
        throw new std::runtime_error("Lexem " + scanner::lexem_str(lexem) + " expected, but " + scanner::lexem_str(scanner.lexem()) + " read.");

    *token = scanner.token();

    scanner.next();
}

bool parser::skip(lexem lexem)
{
    if (scanner.lexem() == lexem) {
        scanner.next();

        return true;
    }

    return false;
}

bool parser::skip(lexem lexem, std::string* token)
{
    if (scanner.lexem() == lexem) {
        *token = scanner.token();

        scanner.next();

        return true;
    }

    return false;
    
}

std::vector<std::string> parser::get_parameters()
{
    std::vector<std::string> result;

    do {
        std::string identifier;

        expect(lexem::Identifier, &identifier);
        
        result.push_back(identifier);
    } while (skip(lexem::Comma));

    return result;
}

const ast_expression* parser::get_expression()
{
    const ast_expression* left = get_operand1();

    return get_operator_separated_operands1(left);
}

const ast_expression* parser::get_operator_separated_operands1(const ast_expression* left)
{
    std::string operation;
    if (skip(lexem::Plus))
        operation = '+';
    else if (skip(lexem::Minus))
        operation = '-';
    else
        return left;

    const ast_expression* right = get_operand1();
    ast_expression* binary_operator = new ast_binary_operator(operation, left, right);

    return get_operator_separated_operands1(binary_operator);
}

const ast_expression* parser::get_operand1()
{
    const ast_expression* left = get_operand2();

    return get_operator_separated_operands2(left);
}

const ast_expression* parser::get_operator_separated_operands2(const ast_expression* left)
{
    std::string operation;
    if (skip(lexem::Star))
        operation = '*';
    else if (skip(lexem::Slash))
        operation = '/';
    else if (skip(lexem::Percent))
        operation = '%';
    else
        return left;

    const ast_expression* right = get_operand2();
    ast_expression* binary_operator = new ast_binary_operator(operation, left, right);

    return get_operator_separated_operands2(binary_operator);
}

const ast_expression* parser::get_operand2()
{
    const ast_expression* left = get_operand3();

    while (skip(lexem::Caret)) {
        const ast_expression* right = get_operand2();

        left = new ast_binary_operator("^", left, right);
    }

    return left;
}

const ast_expression* parser::get_operand3()
{
    if (skip(lexem::Minus))
        return new ast_unary_operator("-", get_operand4());

    if (skip(lexem::Plus))
        return new ast_unary_operator("+", get_operand4());
    
    return get_operand4();
}

const ast_expression* parser::get_operand4()
{
    std::string token;
    if (skip(lexem::Identifier, &token)) {
        if (skip(lexem::LParen)) {
            std::vector<const ast_expression*> parameters;
            parameters.push_back(get_expression());

            while (skip(lexem::Comma))
                parameters.push_back(get_expression());

            expect(lexem::RParen);

            return new ast_call(token, parameters);
        }

        return new ast_variable(token);
    }
    else if (skip(lexem::Integer, &token)) {
        int value = std::stoi(token);

        return new ast_integer(value);
    }
    else if (skip(lexem::Double, &token)) {
        double value = std::stod(token);
        
        return new ast_double(value);
    }
    else if (skip(lexem::If)) {
		const ast_logical_expression* logical_expression = get_logical_expression();

		expect(lexem::Then);

		const ast_expression* then_expression = get_expression();

		expect(lexem::Else);

		const ast_expression* else_expression = get_expression();

		return new ast_if_then_else(logical_expression, then_expression, else_expression);
    }
    else if (skip(lexem::LParen)) {
        const ast_expression* expression = get_expression();
		expect(lexem::RParen);

		return expression;
    }
    
    throw new std::runtime_error("Operand expected.");
}

const ast_logical_expression* parser::get_logical_expression()
{
	const ast_logical_expression* left = get_logical_operand1();

	return get_operator_separated_logical_operands1(left);
}

const ast_logical_expression* parser::get_logical_operand1()
{
	const ast_logical_expression* left = get_logical_operand2();

	return get_operator_separated_logical_operands2(left);
}

const ast_logical_expression* parser::get_operator_separated_logical_operands1(const ast_logical_expression* left)
{
	if (skip(lexem::Or)) {
		const ast_logical_expression* right = get_logical_operand1();
		ast_logical_expression* binary_operator = new ast_logical_binary_operator("or", left, right);

		return get_operator_separated_logical_operands1(binary_operator);
	}

	return left;
}

const ast_logical_expression* parser::get_logical_operand2()
{
	if (skip(lexem::Not)) {
		const ast_logical_expression* operand = get_logical_operand3();

		return new ast_logical_not_operator(operand);
	}

	return get_logical_operand3();
}

const ast_logical_expression* parser::get_operator_separated_logical_operands2(const ast_logical_expression* left)
{
	if (skip(lexem::And)) {
		const ast_logical_expression* right = get_logical_operand1();
		ast_logical_expression* binary_operator = new ast_logical_binary_operator("and", left, right);

		return get_operator_separated_logical_operands2(binary_operator);
	}

	return left;
}

const ast_logical_expression* parser::get_logical_operand3()
{
	if (skip(lexem::LParen)) {
		const ast_logical_expression* logical_expression = get_logical_expression();

		expect(lexem::RParen);

		return logical_expression;
	}

	return get_condition();
}

const ast_logical_expression* parser::get_condition()
{
	std::string operation;
	const ast_expression* left = get_expression();

	if (skip(lexem::Gt))
		operation = ">";
	else if (skip(lexem::Ge))
		operation = ">=";
	else if (skip(lexem::Lt))
		operation = "<";
	else if (skip(lexem::Le))
		operation = "<=";
	else if (skip(lexem::Ne))
		operation = "<>";
	else if (skip(lexem::Eq))
		operation = "=";
	else
		throw new std::runtime_error("Comparison operator expected.");

	const ast_expression* right = get_expression();

	return new ast_condition(operation, left, right);
}
