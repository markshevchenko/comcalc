#include <set>
#include <stdexcept>
#include <string>
#include <vector>

#include "ast.h"
#include "parser.h"
#include "scanner.h"

ast_program* parser::parse() {
    std::vector<const ast_function*> functions;
    std::vector<const ast_assignment*> assignments;

    do {
        std::string name;
        expect(lexeme::Identifier, &name);


		if (skip(lexeme::LParen)) {
			std::set<std::string> parameters = get_parameters();

			expect(lexeme::RParen);
			expect(lexeme::Eq);

			const ast_expression* expression = get_expression();

			ast_function* function = new ast_function(name, parameters, expression);
			functions.push_back(function);
		}
        else {
            expect(lexeme::Eq);

            const ast_expression* expression = get_expression();

            ast_assignment* assignment = new ast_assignment(name, expression);
			assignments.push_back(assignment);
        }

		while (skip(lexeme::NewLine))
			;
        
    } while (scanner.lexeme() != lexeme::Eof);

    return new ast_program(functions, assignments);
}

void parser::expect(lexeme lexem) {
    if (scanner.lexeme() != lexem)
        throw new std::runtime_error("Lexem " + scanner::lexeme_str(lexem) + " expected, but " + scanner::lexeme_str(scanner.lexeme()) + " read.");

    scanner.next();
}

void parser::expect(lexeme lexem, std::string* token) {
    if (scanner.lexeme() != lexem)
        throw new std::runtime_error("Lexem " + scanner::lexeme_str(lexem) + " expected, but " + scanner::lexeme_str(scanner.lexeme()) + " read.");

    *token = scanner.token();

    scanner.next();
}

bool parser::skip(lexeme lexem) {
    if (scanner.lexeme() == lexem) {
        scanner.next();

        return true;
    }

    return false;
}

bool parser::skip(lexeme lexem, std::string* token) {
    if (scanner.lexeme() == lexem) {
        *token = scanner.token();

        scanner.next();

        return true;
    }

    return false;
    
}

std::set<std::string> parser::get_parameters() {
    std::set<std::string> result;

    do {
        std::string identifier;

        expect(lexeme::Identifier, &identifier);

		if (result.find(identifier) != result.end())
			throw new std::runtime_error("Dublicate parameter `" + identifier + "`.");

        result.insert(identifier);
    } while (skip(lexeme::Comma));

    return result;
}

const ast_expression* parser::get_expression() {
    const ast_expression* left = get_operand1();

    return get_operator_separated_operands1(left);
}

const ast_expression* parser::get_operator_separated_operands1(const ast_expression* left) {
    std::string operation;
    if (skip(lexeme::Plus))
        operation = '+';
    else if (skip(lexeme::Minus))
        operation = '-';
    else
        return left;

    const ast_expression* right = get_operand1();
    ast_expression* binary_operator = new ast_binary_operator(operation, left, right);

    return get_operator_separated_operands1(binary_operator);
}

const ast_expression* parser::get_operand1() {
    const ast_expression* left = get_operand2();

    return get_operator_separated_operands2(left);
}

const ast_expression* parser::get_operator_separated_operands2(const ast_expression* left) {
    std::string operation;
    if (skip(lexeme::Star))
        operation = '*';
    else if (skip(lexeme::Slash))
        operation = '/';
    else if (skip(lexeme::Percent))
        operation = '%';
    else
        return left;

    const ast_expression* right = get_operand2();
    ast_expression* binary_operator = new ast_binary_operator(operation, left, right);

    return get_operator_separated_operands2(binary_operator);
}

const ast_expression* parser::get_operand2() {
    const ast_expression* left = get_operand3();

    while (skip(lexeme::Caret)) {
        const ast_expression* right = get_operand2();

        left = new ast_binary_operator("^", left, right);
    }

    return left;
}

const ast_expression* parser::get_operand3() {
    if (skip(lexeme::Minus))
        return new ast_unary_operator("-", get_operand4());

    if (skip(lexeme::Plus))
        return new ast_unary_operator("+", get_operand4());
    
    return get_operand4();
}

const ast_expression* parser::get_operand4() {
    std::string token;
    if (skip(lexeme::Identifier, &token)) {
        if (skip(lexeme::LParen)) {
            std::vector<const ast_expression*> parameters;
            parameters.push_back(get_expression());

            while (skip(lexeme::Comma))
                parameters.push_back(get_expression());

            expect(lexeme::RParen);

            return new ast_call(token, parameters);
        }

        return new ast_variable(token);
    }
    else if (skip(lexeme::Integer, &token)) {
        int value = std::stoi(token);

        return new ast_integer(value);
    }
    else if (skip(lexeme::Double, &token)) {
        double value = std::stod(token);
        
        return new ast_double(value);
    }
    else if (skip(lexeme::If)) {
		const ast_logical_expression* logical_expression = get_logical_expression();

		expect(lexeme::Then);

		const ast_expression* then_expression = get_expression();

		expect(lexeme::Else);

		const ast_expression* else_expression = get_expression();

		return new ast_if_then_else(logical_expression, then_expression, else_expression);
    }
    else if (skip(lexeme::LParen)) {
        const ast_expression* expression = get_expression();
		expect(lexeme::RParen);

		return expression;
    }
    
    throw new std::runtime_error("Operand expected.");
}

const ast_logical_expression* parser::get_logical_expression() {
	const ast_logical_expression* left = get_logical_operand1();

	return get_operator_separated_logical_operands1(left);
}

const ast_logical_expression* parser::get_logical_operand1() {
	const ast_logical_expression* left = get_logical_operand2();

	return get_operator_separated_logical_operands2(left);
}

const ast_logical_expression* parser::get_operator_separated_logical_operands1(const ast_logical_expression* left) {
	if (skip(lexeme::Or)) {
		const ast_logical_expression* right = get_logical_operand1();
		ast_logical_expression* binary_operator = new ast_logical_binary_operator("or", left, right);

		return get_operator_separated_logical_operands1(binary_operator);
	}

	return left;
}

const ast_logical_expression* parser::get_logical_operand2() {
	if (skip(lexeme::Not)) {
		const ast_logical_expression* operand = get_logical_operand3();

		return new ast_logical_not_operator(operand);
	}

	return get_logical_operand3();
}

const ast_logical_expression* parser::get_operator_separated_logical_operands2(const ast_logical_expression* left) {
	if (skip(lexeme::And)) {
		const ast_logical_expression* right = get_logical_operand1();
		ast_logical_expression* binary_operator = new ast_logical_binary_operator("and", left, right);

		return get_operator_separated_logical_operands2(binary_operator);
	}

	return left;
}

const ast_logical_expression* parser::get_logical_operand3() {
	if (skip(lexeme::LParen)) {
		const ast_logical_expression* logical_expression = get_logical_expression();

		expect(lexeme::RParen);

		return logical_expression;
	}

	return get_condition();
}

const ast_logical_expression* parser::get_condition() {
	std::string operation;
	const ast_expression* left = get_expression();

	if (skip(lexeme::Gt))
		operation = ">";
	else if (skip(lexeme::Ge))
		operation = ">=";
	else if (skip(lexeme::Lt))
		operation = "<";
	else if (skip(lexeme::Le))
		operation = "<=";
	else if (skip(lexeme::Ne))
		operation = "<>";
	else if (skip(lexeme::Eq))
		operation = "=";
	else
		throw new std::runtime_error("Comparison operator expected.");

	const ast_expression* right = get_expression();

	return new ast_condition(operation, left, right);
}
