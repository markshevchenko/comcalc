#include <set>
#include <stdexcept>
#include <vector>

#include "parser.h"

const ast_program* parser::parse_program() {
    std::vector<const ast_function*> functions;
    std::vector<const ast_assignment*> assignments;

    do {
        std::string name;
        expect(lexeme::Identifier, &name);


		if (skip(lexeme::LParen)) {
			std::map<std::string, expression_type> parameters = parse_parameters();

			expect(lexeme::RParen);
			expect(lexeme::Eq);

			const ast_expression* expression = parse_expression();

			ast_function* function = new ast_function(name, parameters, expression);
			functions.push_back(function);
		}
        else {
            expect(lexeme::Eq);

            const ast_expression* expression = parse_expression();

            ast_assignment* assignment = new ast_assignment(name, expression);
			assignments.push_back(assignment);
        }

		while (skip(lexeme::NewLine))
			;
        
    } while (scanner.lexeme() != lexeme::Eof);

    return new ast_program(functions, assignments);
}

static expression_type get_type_by_first_letter(char first_letter) {
	if (std::strchr("ijklmnIJKLMN", first_letter) == NULL)
		return expression_type::Double;

	return expression_type::Long;
}

std::map<std::string, expression_type> parser::parse_parameters() {
    std::map<std::string, expression_type> parameters;

    do {
        std::string name;

        expect(lexeme::Identifier, &name);

		if (parameters.find(name) != parameters.end())
			throw new std::runtime_error("Dublicate parameter `" + name + "`.");

		expression_type type;
		if (skip(lexeme::Colon)) {
			if (skip(lexeme::Long))
				type = expression_type::Long;
			else if (skip(lexeme::Double))
				type = expression_type::Double;
			else
				throw new std::runtime_error("Unknown type. Must be 'long' or 'double'.");
		}
		else
			type = get_type_by_first_letter(name[0]);

        parameters[name] = type;
    } while (skip(lexeme::Comma));

    return parameters;
}

const ast_expression* parser::parse_expression() {
    const ast_expression* left = parse_operand1();

    return parse_separated_operands1(left);
}

const ast_expression* parser::parse_separated_operands1(const ast_expression* left) {
    binary_operation operation;
    if (skip(lexeme::Plus))
        operation = binary_operation::Add;
    else if (skip(lexeme::Minus))
        operation = binary_operation::Subtract;
    else
        return left;

    const ast_expression* right = parse_operand1();
    ast_expression* binary_operator = new ast_binary_operator(operation, left, right);

    return parse_separated_operands1(binary_operator);
}

const ast_expression* parser::parse_operand1() {
    const ast_expression* left = parse_operand2();

    return parse_separated_operands2(left);
}

const ast_expression* parser::parse_separated_operands2(const ast_expression* left) {
	binary_operation operation;
	if (skip(lexeme::Star))
        operation = binary_operation::Multiply;
    else if (skip(lexeme::Slash))
        operation = binary_operation::Divide;
    else if (skip(lexeme::Percent))
        operation = binary_operation::Reminder;
    else
        return left;

    const ast_expression* right = parse_operand2();
    ast_expression* binary_operator = new ast_binary_operator(operation, left, right);

    return parse_separated_operands2(binary_operator);
}

const ast_expression* parser::parse_operand2() {
    const ast_expression* left = parse_operand3();

    while (skip(lexeme::Caret)) {
        const ast_expression* right = parse_operand2();

        left = new ast_binary_operator(binary_operation::Pow, left, right);
    }

    return left;
}

const ast_expression* parser::parse_operand3() {
    if (skip(lexeme::Minus))
        return new ast_unary_operator(unary_operation::Negative, parse_operand4());

    if (skip(lexeme::Plus))
        return new ast_unary_operator(unary_operation::Positive, parse_operand4());
    
    return parse_operand4();
}

const ast_expression* parser::parse_operand4() {
    std::string token;
    if (skip(lexeme::Identifier, &token)) {
        if (skip(lexeme::LParen)) {
			std::vector<const ast_expression*> parameters;

			if (skip(lexeme::RParen))
				return new ast_call(token, parameters);

			do {
				auto expression = parse_expression();
				parameters.push_back(expression);
			} while (skip(lexeme::Comma));

            expect(lexeme::RParen);

            return new ast_call(token, parameters);
        }
		else if (skip(lexeme::Colon)) {
			if (skip(lexeme::Long))
				return new ast_variable(token, expression_type::Long);

			if (skip(lexeme::Double))
				return new ast_variable(token, expression_type::Double);

			throw new std::runtime_error("Unknown type. Must be 'long' or 'double'.");
		}

		bool is_long_variable_by_default = std::strchr("ijklmnIJKLMN", token[0]) != NULL;
		if (is_long_variable_by_default)
			return new ast_variable(token, expression_type::Long);

		return new ast_variable(token, expression_type::Double);
    }
    else if (skip(lexeme::LongConstant, &token)) {
        int value = std::stoi(token);

        return new ast_long(value);
    }
    else if (skip(lexeme::DoubleConstant, &token)) {
        double value = std::stod(token);
        
        return new ast_double(value);
    }
    else if (skip(lexeme::If)) {
		const ast_logical_expression* logical_expression = parse_logical_expression();

		expect(lexeme::Then);

		const ast_expression* then_expression = parse_expression();

		expect(lexeme::Else);

		const ast_expression* else_expression = parse_expression();

		return new ast_if_then_else(logical_expression, then_expression, else_expression);
    }
    else if (skip(lexeme::LParen)) {
        const ast_expression* expression = parse_expression();
		expect(lexeme::RParen);

		return expression;
    }
    
    throw new std::runtime_error("Operand expected.");
}

const ast_logical_expression* parser::parse_logical_expression() {
	const ast_logical_expression* left = parse_logical_operand1();

	return parse_separated_logical_operands1(left);
}

const ast_logical_expression* parser::parse_logical_operand1() {
	const ast_logical_expression* left = parse_logical_operand2();

	return parse_separated_logical_operands2(left);
}

const ast_logical_expression* parser::parse_separated_logical_operands1(const ast_logical_expression* left) {
	if (skip(lexeme::Or)) {
		const ast_logical_expression* right = parse_logical_operand1();
		ast_logical_expression* binary_operator = new ast_logical_binary_operator("or", left, right);

		return parse_separated_logical_operands1(binary_operator);
	}

	return left;
}

const ast_logical_expression* parser::parse_logical_operand2() {
	if (skip(lexeme::Not)) {
		const ast_logical_expression* operand = parse_logical_operand3();

		return new ast_logical_not_operator(operand);
	}

	return parse_logical_operand3();
}

const ast_logical_expression* parser::parse_separated_logical_operands2(const ast_logical_expression* left) {
	if (skip(lexeme::And)) {
		const ast_logical_expression* right = parse_logical_operand1();
		ast_logical_expression* binary_operator = new ast_logical_binary_operator("and", left, right);

		return parse_separated_logical_operands2(binary_operator);
	}

	return left;
}

const ast_logical_expression* parser::parse_logical_operand3() {
	if (skip(lexeme::LParen)) {
		const ast_logical_expression* logical_expression = parse_logical_expression();

		expect(lexeme::RParen);

		return logical_expression;
	}

	return parse_condition();
}

const ast_logical_expression* parser::parse_condition() {
	std::string operation;
	const ast_expression* left = parse_expression();

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

	const ast_expression* right = parse_expression();

	return new ast_condition(operation, left, right);
}

void throw_if_unexpected_lexeme(lexeme expected_lexeme, lexeme actual_lexeme) {
	if (expected_lexeme != actual_lexeme)
		throw new std::runtime_error("Lexem " + scanner::name(expected_lexeme)
			+ " expected, but " + scanner::name(actual_lexeme) + " read.");
}

void parser::expect(lexeme lexeme) {
	throw_if_unexpected_lexeme(lexeme, scanner.lexeme());

	scanner.next();
}

void parser::expect(lexeme lexeme, std::string* buffer) {
	throw_if_unexpected_lexeme(lexeme, scanner.lexeme());

	*buffer = scanner.buffer();

	scanner.next();
}

bool parser::skip(lexeme lexeme) {
	if (scanner.lexeme() == lexeme) {
		scanner.next();

		return true;
	}

	return false;
}

bool parser::skip(lexeme lexeme, std::string* buffer) {
	if (scanner.lexeme() == lexeme) {
		*buffer = scanner.buffer();

		scanner.next();

		return true;
	}

	return false;
}

